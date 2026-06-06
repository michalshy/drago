#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdint>
#include <spdlog/spdlog.h>
#include "core/Platform.h"
#include "renderer/Image.h"
#include "rhi/vulkan/VulkanRenderer.h"

static void on_framebuffer_resize(GLFWwindow* window, int width, int height) {
    auto* flag = reinterpret_cast<bool*>(glfwGetWindowUserPointer(window));
    *flag = true;
}

int main() {
    spdlog::info("Platform: {} | RHI: {}", DRAGO_PLATFORM_NAME, DRAGO_RHI_NAME);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "drago", nullptr, nullptr);

    glfwSetFramebufferSizeCallback(window, on_framebuffer_resize);

    auto img = drago::renderer::Image(ASSETS_DIR "/textures/texture.jpg");

#if defined(DRAGO_VULKAN)

    drago::rhi::VulkanRenderer renderer(window, true);
    glfwSetWindowUserPointer(window, &renderer.is_resized());

#endif

    uint32_t current_frame = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        renderer.draw();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}