#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "VulkanDevice.h"
#include "rhi/vulkan/VulkanSwapchain.h"

namespace drago::rhi
{

class VulkanPipeline
{
public:
    explicit VulkanPipeline(
        VulkanDevice* device,
        VulkanSwapchain* swapchain
    );
    ~VulkanPipeline();

private:
    VulkanDevice* device;
    VulkanSwapchain* swapchain;

    vk::PipelineLayout pipeline_layout;

    vk::ShaderModule create_shader_module(const std::vector<char>& code);
};

}