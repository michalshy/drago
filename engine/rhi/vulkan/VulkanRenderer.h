#pragma once

#include "GLFW/glfw3.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "rhi/vulkan/VulkanUtils.h"
#include "vulkan/vulkan.hpp"
#include <array>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace drago::rhi
{

class VulkanRenderer
{
    std::array<FrameData, MAX_FRAMES_IN_FLIGHT> frames;
    int frame_no{0};
    bool resized{false};
public:
    VulkanRenderer(
        GLFWwindow* window,
        bool validate
    );
    ~VulkanRenderer();

    FrameData& get_frame(){ return frames[frame_no % MAX_FRAMES_IN_FLIGHT]; }
    void draw();

    bool& is_resized() { return resized; }

private:
    std::unique_ptr<VulkanDevice> device;
    std::unique_ptr<VulkanSwapchain> swapchain;

    void submit(vk::CommandBuffer cmd);
    void present(uint32_t idx);

    void recreate();
    void init_frame_data();
};

}