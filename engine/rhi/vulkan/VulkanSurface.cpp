#include "VulkanSurface.h"
#include <vulkan/vulkan_core.h>

namespace drago::rhi
{
    VulkanSurface::VulkanSurface(
        VulkanInstance* instance,
        GLFWwindow* window
    )
        : instance(instance)
    {
        VkSurfaceKHR raw_surface;
        if (glfwCreateWindowSurface(instance->get(), window, nullptr, &raw_surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface");
        }
        surface = raw_surface;
    }

    VulkanSurface::~VulkanSurface()
    {
        instance->get().destroySurfaceKHR(surface);
    }
}