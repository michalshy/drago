#include <GLFW/glfw3.h>
#include <cstdint>
#include <spdlog/spdlog.h>
#include "rhi/vulkan/VulkanCommandBuffer.h"
#include "rhi/vulkan/VulkanFramebuffer.h"
#include "rhi/vulkan/VulkanInstance.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanRenderPass.h"
#include "rhi/vulkan/VulkanSurface.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "rhi/vulkan/VulkanPipeline.h"

void draw_frame(
    drago::rhi::VulkanCommandBuffer* cmd,
    drago::rhi::VulkanDevice* dev
)
{
    
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "drago", nullptr, nullptr);

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

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        cmd.wait_for_fence();
        cmd.reset_fence();

        uint32_t idx = device.get().acquireNextImageKHR(swapchain.get(), UINT64_MAX, cmd.image_semaphore()).value;
        cmd.reset(idx);
        cmd.record(idx);
        cmd.submit(idx);
        
        swapchain.present(idx, cmd.render_semaphore());
    }

    device.wait();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}