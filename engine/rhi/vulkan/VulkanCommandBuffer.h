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

    void submit(uint32_t idx);
    void record(uint32_t img_idx);
    void reset(uint32_t frame_idx);

    void wait_for_fence();
    void reset_fence();

    vk::Semaphore image_semaphore() { return image_available_sem; }
    vk::Semaphore render_semaphore() { return render_finished_sem; }
private:
    // refs
    VulkanDevice* device;
    VulkanFramebuffer* framebuffer;
    VulkanSwapchain* swapchain;
    VulkanRenderPass* renderpass;
    VulkanPipeline* pipeline;
    // owned
    vk::CommandPool pool;
    std::vector<vk::CommandBuffer> buffers;
    // sync
    void create_sync();
    vk::Semaphore image_available_sem;
    vk::Semaphore render_finished_sem;
    vk::Fence in_flight_fen;
};

}