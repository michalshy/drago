#include <GLFW/glfw3.h>
#include <cstdint>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>
#include "rhi/vulkan/VulkanCommandBuffer.h"
#include "rhi/vulkan/VulkanFramebuffer.h"
#include "rhi/vulkan/VulkanInstance.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanRenderPass.h"
#include "rhi/vulkan/VulkanSurface.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "rhi/vulkan/VulkanPipeline.h"

static void on_framebuffer_resize(GLFWwindow* window, int width, int height) {
    auto* flag = reinterpret_cast<bool*>(glfwGetWindowUserPointer(window));
    *flag = true;
}

int main() {
    bool resized = false;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "drago", nullptr, nullptr);

    glfwSetWindowUserPointer(window, &resized);
    glfwSetFramebufferSizeCallback(window, on_framebuffer_resize);

    drago::rhi::VulkanInstance instance(true);
    spdlog::info("Init OK");

    drago::rhi::VulkanSurface surface(&instance, window);
    spdlog::info("Surface OK");

    drago::rhi::VulkanDevice device(&instance, &surface);
    spdlog::info("Device OK");

    drago::rhi::VulkanSwapchain swapchain(&surface, &device, window);
    spdlog::info("Swapchain OK");

    drago::rhi::VulkanRenderPass renderpass(&device, &swapchain);
    spdlog::info("Renderpass OK");

    drago::rhi::VulkanPipeline pipeline(&device, &renderpass, &swapchain);
    spdlog::info("Pipeline OK");

    drago::rhi::VulkanFramebuffer framebuffer(&device, &swapchain, &renderpass);

    drago::rhi::VulkanCommandBuffer cmd(&device, &framebuffer, &swapchain, &renderpass, &pipeline);

    uint32_t current_frame = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        cmd.wait_for_fence(current_frame);
        
        uint32_t idx;
        try {
            idx = device.get().acquireNextImageKHR(
                swapchain.get(), UINT64_MAX,
                cmd.image_semaphore(current_frame)
            ).value;
        } catch (vk::OutOfDateKHRError&) {
            resized = false;
            cmd.recreate();
            continue;
        }
        
        cmd.reset_fence(current_frame);
        cmd.reset(current_frame);
        cmd.record(idx, current_frame);
        cmd.submit(idx, current_frame);
        
        try {
            swapchain.present(idx, cmd.render_semaphore(idx));
        } catch (vk::OutOfDateKHRError&) {
            resized = false;
            cmd.recreate();
        }

        if(resized)
        {
            resized = false;
            cmd.recreate();
        }
    
        current_frame = (current_frame + 1) % drago::rhi::MAX_FRAMES_IN_FLIGHT;
    }

    device.wait();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}