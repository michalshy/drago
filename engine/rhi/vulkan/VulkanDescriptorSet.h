#pragma once

#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanUniformBuffer.h"
#include <vector>
namespace drago::rhi
{

class VulkanDescriptorSet
{

public:
    VulkanDescriptorSet(
        VulkanDevice* device,
        VulkanUniformBuffer* ubo_buffer
    );
    ~VulkanDescriptorSet();

    vk::DescriptorSetLayout get_layout() { return descriptor_layout; }
    std::vector<vk::DescriptorSet>& get_sets() { return sets; } 
private:
    VulkanDevice* device;
    VulkanUniformBuffer* ubo_buffer;

    vk::DescriptorSetLayout descriptor_layout;
    vk::DescriptorPool pool;
    std::vector<vk::DescriptorSet> sets;
};

}