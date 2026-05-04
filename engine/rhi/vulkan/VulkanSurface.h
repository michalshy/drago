#pragma once

#include "rhi/vulkan/VulkanInstance.h"
#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"

namespace drago::rhi
{
    class VulkanSurface
    {
    public:
        VulkanSurface(
            VulkanInstance* instance,
            GLFWwindow* window
        );
        ~VulkanSurface();

        vk::SurfaceKHR get() { return surface; }

    private:
        VulkanInstance* instance;
        vk::SurfaceKHR surface;
    };
}