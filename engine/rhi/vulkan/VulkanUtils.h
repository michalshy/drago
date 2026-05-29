#pragma once

#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <utility>
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
    std::pair<vk::Image, vk::DeviceMemory> create_image(
        VulkanDevice* device,
        uint32_t width,
        uint32_t height,
        vk::Format format,
        vk::ImageTiling tiling,
        vk::ImageUsageFlags usage,
        vk::MemoryPropertyFlags props
    );
}