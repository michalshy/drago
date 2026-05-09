#include "VulkanCommandBuffer.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanPipeline.h"
#include "rhi/vulkan/VulkanRenderPass.h"
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan_core.h>

namespace drago::rhi
{

VulkanCommandBuffer::VulkanCommandBuffer(
    VulkanDevice* device,
    VulkanFramebuffer* framebuffer,
    VulkanSwapchain* swapchain,
    VulkanRenderPass* renderpass,
    VulkanPipeline* pipeline
)
    : device(device)
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
        .setCommandBufferCount(1);

    buffers = device->get().allocateCommandBuffers(alloc_info);
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    device->get().destroyCommandPool(pool);
}

void VulkanCommandBuffer::record(vk::CommandBuffer cmd, uint32_t img_idx)
{
    auto begin_info = vk::CommandBufferBeginInfo{};
    cmd.begin(begin_info);

    vk::ClearColorValue color = {0.0f, 0.0f, 0.0f, 1.0f};
    auto renderpass_info = vk::RenderPassBeginInfo{}
        .setRenderPass(renderpass->get())
        .setFramebuffer(framebuffer->get()[img_idx])
        .setRenderArea(
            vk::Rect2D{}
                .setExtent(swapchain->get_extent())
                .setOffset({{0,0}})
        )
        .setClearValueCount(1)
        .setClearValues(color);

    cmd.beginRenderPass(renderpass_info, vk::SubpassContents::eInline);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->get_pipeline());

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

    cmd.draw(3, 1, 0, 0);

    cmd.endRenderPass();
    cmd.end();
}


}