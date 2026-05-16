#pragma once

#include "vulkan/vulkan.hpp"
#include "VulkanDevice.h"

namespace drago::rhi
{

class VulkanIndexBuffer
{

public:
    VulkanIndexBuffer(
        const std::vector<uint16_t>& indices,
        VulkanDevice* device
    );
    ~VulkanIndexBuffer();

    vk::Buffer get() { return index_buffer; }
    uint32_t count() const { return index_count; }

private:
    uint32_t index_count;
    VulkanDevice* device;

    vk::Buffer index_buffer;
    vk::DeviceMemory index_memory;
};

}