#pragma once

#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanFramebuffer.h"
#include "rhi/vulkan/VulkanPipeline.h"
#include "rhi/vulkan/VulkanRenderPass.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace drago::rhi
{

class VulkanCommandBuffer
{
public:
    VulkanCommandBuffer(
        VulkanDevice* device,
        VulkanFramebuffer* framebuffer,
        VulkanSwapchain* swapchain,
        VulkanRenderPass* renderpass,
        VulkanPipeline* pipeline
    );
    ~VulkanCommandBuffer();
    void record(vk::CommandBuffer cmd, uint32_t img_idx);
private:
    VulkanDevice* device;
    VulkanFramebuffer* framebuffer;
    VulkanSwapchain* swapchain;
    VulkanRenderPass* renderpass;
    VulkanPipeline* pipeline;
    
    vk::CommandPool pool;
    std::vector<vk::CommandBuffer> buffers;
};

}