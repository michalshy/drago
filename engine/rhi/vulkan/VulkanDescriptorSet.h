#pragma once

#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanUniformBuffer.h"
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

private:
    VulkanDevice* device;
    VulkanUniformBuffer* ubo_buffer;

    vk::DescriptorPool pool;
};

}