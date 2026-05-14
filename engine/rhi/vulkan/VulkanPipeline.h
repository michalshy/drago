#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>
#include "VulkanDevice.h"
#include "rhi/vulkan/VulkanRenderPass.h"
#include "rhi/vulkan/VulkanSwapchain.h"

namespace drago::rhi
{

class VulkanPipeline
{
public:
    explicit VulkanPipeline(
        VulkanDevice* device,
        VulkanRenderPass* renderpass,
        VulkanSwapchain* swapchain
    );
    ~VulkanPipeline();

    vk::Pipeline get_pipeline() { return graphics_pipeline; }
private:
    VulkanDevice* device;
    VulkanRenderPass* renderpass;
    VulkanSwapchain* swapchain;

    vk::PipelineLayout pipeline_layout;

    vk::Pipeline graphics_pipeline;

    vk::ShaderModule create_shader_module(const std::vector<char>& code);
};

}