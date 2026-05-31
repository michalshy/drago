#pragma once

#include "rhi/vulkan/VulkanCommandBuffer.h"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <utility>
#include <vulkan/vulkan.hpp>

namespace drago::rhi
{
    class VulkanDevice;

    const int MAX_FRAMES_IN_FLIGHT = 2;

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
        VulkanDevice* device,
        vk::CommandBuffer buffer,
        const vk::Image& img,
        vk::ImageLayout old_layout,
        vk::ImageLayout new_layout
    );
}