#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "rhi/vulkan/VulkanInstance.h"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "drago", nullptr, nullptr);

    drago::rhi::VulkanInstance instance(true);
    spdlog::info("Init OK");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}