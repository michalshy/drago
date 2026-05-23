#pragma once

#include "rhi/vulkan/VulkanDevice.h"
#include "vulkan/vulkan.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>

namespace drago::rhi
{

class VulkanUniformBuffer
{
public:
    VulkanUniformBuffer(
        VulkanDevice* device
    );
    ~VulkanUniformBuffer();

    std::vector<vk::Buffer>& get() { return buffers; }
private:
    VulkanDevice* device;

    std::vector<vk::Buffer> buffers;
    std::vector<vk::DeviceMemory> buffers_mem;
    std::vector<void*> buffers_mapped; 
};

}