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

const int MAX_FRAMES_IN_FLIGHT = 2;

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

    void submit(uint32_t img_idx, uint32_t frame_idx);
    void record(uint32_t img_idx, uint32_t frame_idx);
    void reset(uint32_t frame_idx);

    void wait_for_fence(uint32_t frame_idx);
    void reset_fence(uint32_t frame_idx);

    vk::Semaphore image_semaphore(uint32_t frame_idx) { return image_available_sems[frame_idx]; }
    vk::Semaphore render_semaphore(uint32_t frame_idx) { return render_finished_sems[frame_idx]; }
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
    std::vector<vk::Semaphore> image_available_sems;
    std::vector<vk::Semaphore> render_finished_sems;
    std::vector<vk::Fence> in_flight_fens;
};

}