#pragma once

#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan.hpp>

namespace drago::rhi
{

class VulkanRenderPass
{
public:
    VulkanRenderPass(
        VulkanDevice* device,
        VulkanSwapchain* swapchain
    );
    ~VulkanRenderPass();

    vk::RenderPass get() { return renderpass; }
private:
    VulkanDevice* device;
    VulkanSwapchain* swapchain;

    vk::RenderPass renderpass;
};

}