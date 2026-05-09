#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "rhi/vulkan/VulkanInstance.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanRenderPass.h"
#include "rhi/vulkan/VulkanSurface.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "rhi/vulkan/VulkanPipeline.h"

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

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}