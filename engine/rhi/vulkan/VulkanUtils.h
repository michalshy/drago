#pragma once

#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan.hpp>

namespace drago::rhi
{
    class VulkanDevice;

    const int MAX_FRAMES_IN_FLIGHT = 2;

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
}