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

private:
    VulkanDevice* device;

    vk::Buffer index_buffer;
    vk::DeviceMemory index_memory;
};

}