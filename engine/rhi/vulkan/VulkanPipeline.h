#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "VulkanDevice.h"
#include "rhi/vulkan/VulkanRenderPass.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "vulkan/vulkan.hpp"

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

private:
    VulkanDevice* device;
    VulkanRenderPass* renderpass;
    VulkanSwapchain* swapchain;

    vk::PipelineLayout pipeline_layout;

    vk::Pipeline graphics_pipeline;

    vk::ShaderModule create_shader_module(const std::vector<char>& code);
};

}