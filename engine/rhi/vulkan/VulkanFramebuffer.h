#pragma once

#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanRenderPass.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "vulkan/vulkan.hpp"
#include <vector>

namespace drago::rhi
{

class VulkanFramebuffer
{
public:
    VulkanFramebuffer(
        VulkanDevice* device,
        VulkanSwapchain* swapchain,
        VulkanRenderPass* renderpass    
    );
    ~VulkanFramebuffer();

    std::vector<vk::Framebuffer>& get(){ return framebuffers; }

private:
    VulkanSwapchain* swapchain;
    VulkanRenderPass* renderpass;
    VulkanDevice* device;

    std::vector<vk::Framebuffer> framebuffers;
};

}