#include "VulkanCommandBuffer.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanIndexBuffer.h"
#include "rhi/vulkan/VulkanPipeline.h"
#include "rhi/vulkan/VulkanRenderPass.h"
#include "rhi/vulkan/VulkanVertexBuffer.h"
#include "vulkan/vulkan.hpp"
#include <cstddef>
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace drago::rhi
{

VulkanCommandBuffer::VulkanCommandBuffer(
    VulkanDevice* device,
    VulkanVertexBuffer* vertexbuffer,
    VulkanIndexBuffer* indexbuffer,
    VulkanFramebuffer* framebuffer,
    VulkanSwapchain* swapchain,
    VulkanRenderPass* renderpass,
    VulkanPipeline* pipeline
)
    : device(device)
    , vertexbuffer(vertexbuffer)
    , indexbuffer(indexbuffer)
    , framebuffer(framebuffer)
    , swapchain(swapchain)
    , renderpass(renderpass)
    , pipeline(pipeline)
{
    // pool
    QueueFamilyIndices indices = device->find_queue_families(device->get_physical());

    auto pool_info = vk::CommandPoolCreateInfo{}
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(indices.graphics_family.value());

    pool = device->get().createCommandPool(pool_info);
    if(!pool)
    {
        throw std::runtime_error("failed to create command pool!");
    }

    // buffer
    auto alloc_info = vk::CommandBufferAllocateInfo{}
        .setCommandPool(pool)
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(swapchain->get_image_count());

    buffers = device->get().allocateCommandBuffers(alloc_info);

    create_sync();
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    for(size_t i = 0; i < swapchain->get_image_count(); i++)
    {
        device->get().destroySemaphore(render_finished_sems[i]);
    }
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        device->get().destroySemaphore(image_available_sems[i]);
        device->get().destroyFence(in_flight_fens[i]);
    }
    device->get().destroyCommandPool(pool);
}

void VulkanCommandBuffer::submit(uint32_t img_idx, uint32_t frame_idx)
{
    auto submit_info = vk::SubmitInfo{};

    vk::Semaphore wait[] = {image_available_sems[frame_idx]};
    vk::PipelineStageFlags stages[] = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };

    submit_info.setWaitSemaphoreCount(1);
    submit_info.setPWaitSemaphores(wait);
    submit_info.setWaitDstStageMask(stages);

    vk::CommandBuffer cmd_buffers[] = { buffers[frame_idx] };
    submit_info.setCommandBufferCount(1);
    submit_info.setPCommandBuffers(cmd_buffers);

    vk::Semaphore signal_semaphores[] = { render_finished_sems[img_idx] };
    submit_info.setSignalSemaphoreCount(1);
    submit_info.setPSignalSemaphores(signal_semaphores);

    device->get_graphics().submit(submit_info, in_flight_fens[frame_idx]);
}

void VulkanCommandBuffer::record(uint32_t img_idx, uint32_t frame_idx)
{
    auto& cmd = buffers[frame_idx];

    auto begin_info = vk::CommandBufferBeginInfo{};
    cmd.begin(begin_info);

    vk::ClearValue clear_value{vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f}};
    auto renderpass_info = vk::RenderPassBeginInfo{}
        .setRenderPass(renderpass->get())
        .setFramebuffer(framebuffer->get()[img_idx])
        .setRenderArea(
            vk::Rect2D{}
                .setExtent(swapchain->get_extent())
                .setOffset({{0,0}})
        )
        .setClearValueCount(1)
        .setClearValues(clear_value);

    cmd.beginRenderPass(renderpass_info, vk::SubpassContents::eInline);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->get_pipeline());

    cmd.bindVertexBuffers(0, vertexbuffer->get(), {0});
    cmd.bindIndexBuffer(indexbuffer->get(), 0, vk::IndexType::eUint16);

    auto viewport = vk::Viewport{}
        .setX(0.0f)
        .setY(0.0f)
        .setHeight(static_cast<float>(swapchain->get_extent().height))
        .setWidth(static_cast<float>(swapchain->get_extent().width))
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);

    cmd.setViewport(0, viewport);

    auto scissor = vk::Rect2D{}
        .setOffset({0,0})
        .setExtent(swapchain->get_extent());

    cmd.setScissor(0, scissor);

    cmd.drawIndexed(6, 1, 0, 0, 0);

    cmd.endRenderPass();
    cmd.end();
}

void VulkanCommandBuffer::reset(uint32_t frame_idx)
{
    buffers[frame_idx].reset();
}

void VulkanCommandBuffer::recreate()
{
    device->wait();

    swapchain->recreate();
    framebuffer->recreate();
}

void VulkanCommandBuffer::wait_for_fence(uint32_t frame_idx)
{
    [[maybe_unused]] auto result = device->get().waitForFences(in_flight_fens[frame_idx], vk::True, UINT64_MAX);
}

void VulkanCommandBuffer::reset_fence(uint32_t frame_idx)
{
    device->get().resetFences(in_flight_fens[frame_idx]);
}

void VulkanCommandBuffer::create_sync()
{
    auto sem_info = vk::SemaphoreCreateInfo{};
    auto fen_info = vk::FenceCreateInfo{}
        .setFlags(vk::FenceCreateFlagBits::eSignaled);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        image_available_sems.push_back(device->get().createSemaphore(sem_info));
        in_flight_fens.push_back(device->get().createFence(fen_info));
    }
    for(size_t i = 0; i < swapchain->get_image_count(); i++)
    {
        render_finished_sems.push_back(device->get().createSemaphore(sem_info));
    }
}

}