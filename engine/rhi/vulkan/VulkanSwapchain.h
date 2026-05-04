#pragma once

#include "GLFW/glfw3.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanSurface.h"
#include "vulkan/vulkan.hpp"
#include <vector>

namespace drago::rhi
{
    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> present_modes;
    };

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(
            VulkanSurface* surface,
            VulkanDevice* device,
            GLFWwindow* window  
        );
        ~VulkanSwapchain();
    private:
        VulkanSurface* surface;
        VulkanDevice* device;
        GLFWwindow* window;

        VkSwapchainKHR swapchain;
        std::vector<vk::Image> images;
        vk::SurfaceFormatKHR format;
        vk::Extent2D extent;

        vk::SurfaceFormatKHR choose_surface_fmt(const std::vector<vk::SurfaceFormatKHR>& formats);
        vk::PresentModeKHR choose_present_mode(const std::vector<vk::PresentModeKHR>& modes);
        vk::Extent2D choose_extent(const vk::SurfaceCapabilitiesKHR& capabilities);
    };
}