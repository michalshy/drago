#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>
#include "VulkanDevice.h"
#include "rhi/vulkan/VulkanDescriptorSet.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "vulkan/vulkan.hpp"

namespace drago::rhi
{

class VulkanPipeline
{
public:
    explicit VulkanPipeline(
        VulkanDevice* device,
        VulkanSwapchain* swapchain,
        VulkanDescriptorSet* descriptor_set
    );
    ~VulkanPipeline();

    vk::Pipeline get_pipeline() { return graphics_pipeline; }
    vk::PipelineLayout get_layout() { return pipeline_layout; }
private:
    VulkanDevice* device;
    VulkanSwapchain* swapchain;
    VulkanDescriptorSet* descriptor_set;

    vk::PipelineLayout pipeline_layout;

    vk::Pipeline graphics_pipeline;

    void create_graphics_pipeline();
    vk::ShaderModule create_shader_module(const std::vector<char>& code);
};

}