#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "VulkanDevice.h"

namespace drago::rhi
{

class VulkanPipeline
{
public:
    explicit VulkanPipeline(VulkanDevice* device);
    ~VulkanPipeline();

private:
    VulkanDevice* device;

    vk::ShaderModule create_shader_module(const std::vector<char>& code);
};

}