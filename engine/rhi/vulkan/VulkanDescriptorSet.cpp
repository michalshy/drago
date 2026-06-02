#include "VulkanDescriptorSet.h"
#include "renderer/Types.h"
#include "rhi/vulkan/VulkanUtils.h"
#include <array>
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
    VulkanUniformBuffer* ubo_buffer,
    VulkanTexture* tex
)
    : device(device)
    , ubo_buffer(ubo_buffer)
    , tex(tex)
{
    // layout
    std::array<vk::DescriptorSetLayoutBinding, 2> ubo_layout{
        vk::DescriptorSetLayoutBinding{}
            .setBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eVertex),
        vk::DescriptorSetLayoutBinding{}
            .setBinding(1)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eFragment)
    };

    auto layout_info = vk::DescriptorSetLayoutCreateInfo{}
        .setBindingCount(ubo_layout.size())
        .setBindings(ubo_layout);

    descriptor_layout = device->get().createDescriptorSetLayout(layout_info);

    //pools
    std::array<vk::DescriptorPoolSize, 2> pool_size{
        vk::DescriptorPoolSize{}
            .setType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(MAX_FRAMES_IN_FLIGHT),
        vk::DescriptorPoolSize{}
            .setType(vk::DescriptorType::eCombinedImageSampler)
            .setDescriptorCount(MAX_FRAMES_IN_FLIGHT)
    };

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

        auto image_info = vk::DescriptorImageInfo{}
            .setSampler(tex->get_sampler())
            .setImageView(tex->get_img_view())
            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

        std::array<vk::WriteDescriptorSet, 2> desc_writes{
            vk::WriteDescriptorSet{}
                .setDstSet(sets[i])
                .setDstBinding(0)
                .setDstArrayElement(0)
                .setDescriptorCount(1)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setPBufferInfo(&buffer_info),
            vk::WriteDescriptorSet{}
                .setDstSet(sets[i])
                .setDstBinding(1)
                .setDstArrayElement(0)
                .setDescriptorCount(1)
                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setPImageInfo(&image_info)
        };

        device->get().updateDescriptorSets(desc_writes, {});
    }

}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    device->get().destroyDescriptorSetLayout(descriptor_layout);
    device->get().destroyDescriptorPool(pool);
}

}