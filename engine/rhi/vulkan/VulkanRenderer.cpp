#include "VulkanRenderer.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "rhi/vulkan/VulkanUtils.h"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace drago::rhi
{

VulkanRenderer::VulkanRenderer(
    GLFWwindow* window,
    bool validate
) {
    device = std::make_unique<VulkanDevice>(window, validate);
    swapchain = std::make_unique<VulkanSwapchain>(device.get(), window);
    init_frame_data();
}
VulkanRenderer::~VulkanRenderer()
{
    device->wait();

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        device->get().destroySemaphore(frames[i].render_semaphore);
        device->get().destroySemaphore(frames[i].swapchain_semaphore);
        device->get().destroyFence(frames[i].render_fence);
        device->get().destroyCommandPool(frames[i].pool);
    }
}

void VulkanRenderer::draw()
{
    device->get().waitForFences(1, &get_frame().render_fence, true, 1000000000);
    device->get().resetFences(1, &get_frame().render_fence);

    uint32_t idx;
    try {
        idx = device->get().acquireNextImageKHR(
            swapchain->get(), UINT64_MAX,
            get_frame().swapchain_semaphore
        ).value;
    } catch (vk::OutOfDateKHRError&) {
        resized = false;
        recreate();
    }

    vk::CommandBuffer cmd = get_frame().buffer;
    cmd.reset();

    auto begin_info = vk::CommandBufferBeginInfo{}
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmd.begin(begin_info);

    transition_img_layout(cmd, swapchain->get_images()[idx], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

	float flash = std::abs(std::sin(frame_no / 120.f));
    vk::ClearColorValue clear_value{ 0.0f, 0.0f, flash, 1.0f };

	vk::ImageSubresourceRange clear_range = image_subresource_range(vk::ImageAspectFlagBits::eColor);

	cmd.clearColorImage(swapchain->get_images()[idx], vk::ImageLayout::eGeneral, &clear_value, 1, &clear_range);
	transition_img_layout(cmd, swapchain->get_images()[idx],vk::ImageLayout::eGeneral, vk::ImageLayout::ePresentSrcKHR);
    cmd.end();
	
    submit(cmd);
    present(idx);
}

void VulkanRenderer::submit(vk::CommandBuffer cmd)
{
    vk::CommandBufferSubmitInfo cmd_info = command_buffer_submit_info(cmd);

    vk::SemaphoreSubmitInfo wait_info = semaphore_submit_info(
        vk::PipelineStageFlagBits2::eColorAttachmentOutput, 
        get_frame().swapchain_semaphore
    );
    vk::SemaphoreSubmitInfo signal_info = semaphore_submit_info(
        vk::PipelineStageFlagBits2::eAllGraphics, 
        get_frame().render_semaphore
    );

    vk::SubmitInfo2 info = submit_info(&cmd_info, &signal_info, &wait_info);

    device->get_graphics().submit2(1, &info, get_frame().render_fence);
}

void VulkanRenderer::present(uint32_t idx)
{
    auto swapchains = swapchain->get();

    auto present_info = vk::PresentInfoKHR{}
        .setSwapchains(swapchains)
        .setSwapchainCount(1)
        .setWaitSemaphores(get_frame().render_semaphore)
        .setWaitSemaphoreCount(1)
        .setImageIndices(idx);

    device->get_graphics().presentKHR(present_info);

    frame_no++;
}

void VulkanRenderer::recreate()
{
    device->wait();
    swapchain->recreate();
}

void VulkanRenderer::init_frame_data()
{
    // pool
    QueueFamilyIndices indices = device->find_queue_families(device->get_physical());

    auto pool_info = vk::CommandPoolCreateInfo{}
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(indices.graphics_family.value());
    auto sem_info = vk::SemaphoreCreateInfo{};
    auto fen_info = vk::FenceCreateInfo{}
        .setFlags(vk::FenceCreateFlagBits::eSignaled);

    for (auto& data : frames)
    {
        data.pool = device->get().createCommandPool(pool_info);
    
        auto alloc_info = vk::CommandBufferAllocateInfo{}
            .setCommandPool(data.pool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);

        data.buffer = device->get().allocateCommandBuffers(alloc_info).front();
    
        data.render_semaphore = device->get().createSemaphore(sem_info);
        data.swapchain_semaphore = device->get().createSemaphore(sem_info);
        data.render_fence = device->get().createFence(fen_info);
    }
}

}