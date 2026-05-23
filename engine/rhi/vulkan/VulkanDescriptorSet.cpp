#include "VulkanDescriptorSet.h"
#include "rhi/vulkan/VulkanUtils.h"
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace drago::rhi
{

VulkanDescriptorSet::VulkanDescriptorSet(
    VulkanDevice* device,
    VulkanUniformBuffer* ubo_buffer
)
    : device(device)
    , ubo_buffer(ubo_buffer)
{
    auto pool_size = vk::DescriptorPoolSize{}
        .setType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(MAX_FRAMES_IN_FLIGHT);

    auto pool_info = vk::DescriptorPoolCreateInfo{}
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
        .setMaxSets(MAX_FRAMES_IN_FLIGHT)
        .setPoolSizeCount(1)
        .setPoolSizes(pool_size);

    pool = device->get().createDescriptorPool(pool_info);
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    device->get().destroyDescriptorPool(pool);
}

}