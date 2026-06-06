#pragma once

#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <utility>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

namespace drago::rhi
{
    class VulkanDevice;

    struct FrameData
    {
        vk::CommandPool pool;
        vk::CommandBuffer buffer;
        vk::Semaphore render_semaphore, swapchain_semaphore;
        vk::Fence render_fence;
    };

    const int MAX_FRAMES_IN_FLIGHT = 2;

    vk::ImageSubresourceRange image_subresource_range(vk::ImageAspectFlags aspect_mask);

    vk::SemaphoreSubmitInfo semaphore_submit_info(vk::PipelineStageFlags2 stage_mask, vk::Semaphore semaphore);
    vk::CommandBufferSubmitInfo command_buffer_submit_info(vk::CommandBuffer cmd);
    vk::SubmitInfo2 submit_info(vk::CommandBufferSubmitInfo* cmd, vk::SemaphoreSubmitInfo* signal_semaphore_info,
        vk::SemaphoreSubmitInfo* wait_semaphore_info);

    struct SingleCommand
    {
        vk::CommandBuffer buffer;
        vk::CommandPool pool;
    };
 
    void create_buffer(
        VulkanDevice* device,
        vk::DeviceSize size,
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags properties,
        vk::Buffer& buffer,
        vk::DeviceMemory& buffer_memory 
    );
    
    void copy_buffer(
        VulkanDevice* device,
        vk::Buffer src,
        vk::Buffer dst,
        vk::DeviceSize size
    );
    
    std::pair<vk::Image, vk::DeviceMemory> create_image(
        VulkanDevice* device,
        uint32_t width,
        uint32_t height,
        vk::Format format,
        vk::ImageTiling tiling,
        vk::ImageUsageFlags usage,
        vk::MemoryPropertyFlags props
    );

    void copy_buffer_to_image(
        vk::CommandBuffer& cmd,
        const vk::Buffer& buffer,
        vk::Image& img,
        uint32_t width,
        uint32_t height
    );

    SingleCommand begin_single_command(VulkanDevice* device);
    void end_single_command(VulkanDevice* device, SingleCommand cmd);

    void transition_img_layout(
        vk::CommandBuffer buffer,
        const vk::Image& img,
        vk::ImageLayout old_layout,
        vk::ImageLayout new_layout
    );

    vk::ImageView create_image_view(
        VulkanDevice* device,
        vk::Image const& img,
        vk::Format format
    );
}