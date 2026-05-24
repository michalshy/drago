#include "VulkanDescriptorSet.h"
#include "renderer/Types.h"
#include "rhi/vulkan/VulkanUtils.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
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
    // layout
    auto ubo_layout = vk::DescriptorSetLayoutBinding{}
        .setBinding(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(1)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex);

    auto layout_info = vk::DescriptorSetLayoutCreateInfo{}
        .setBindingCount(1)
        .setBindings(ubo_layout);

    descriptor_layout = device->get().createDescriptorSetLayout(layout_info);

    //pools
    auto pool_size = vk::DescriptorPoolSize{}
        .setType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(MAX_FRAMES_IN_FLIGHT);

    auto pool_info = vk::DescriptorPoolCreateInfo{}
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
        .setMaxSets(MAX_FRAMES_IN_FLIGHT)
        .setPoolSizeCount(1)
        .setPoolSizes(pool_size);

    pool = device->get().createDescriptorPool(pool_info);
    
    // sets
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptor_layout);
    auto info = vk::DescriptorSetAllocateInfo{}
        .setDescriptorPool(pool)
        .setDescriptorSetCount(static_cast<uint32_t>(layouts.size()))
        .setPSetLayouts(layouts.data());

    sets = device->get().allocateDescriptorSets(info);

    // config
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto buffer_info = vk::DescriptorBufferInfo{}
            .setBuffer(ubo_buffer->get()[i])
            .setOffset(0)
            .setRange(sizeof(renderer::UniformBufferObject));

        auto desc_write = vk::WriteDescriptorSet{}
            .setDstSet(sets[i])
            .setDstBinding(0)
            .setDstArrayElement(0)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setPBufferInfo(&buffer_info);

        device->get().updateDescriptorSets(desc_write, {});
    }

}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    device->get().destroyDescriptorSetLayout(descriptor_layout);
    device->get().destroyDescriptorPool(pool);
}

}