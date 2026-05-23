#pragma once

#include "renderer/Types.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "vulkan/vulkan.hpp"
#include <cstdint>
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
    void update(uint32_t img_idx, renderer::UniformBufferObject& obj);
private:
    VulkanDevice* device;

    std::vector<vk::Buffer> buffers;
    std::vector<vk::DeviceMemory> buffers_mem;
    std::vector<void*> buffers_mapped; 
};

}