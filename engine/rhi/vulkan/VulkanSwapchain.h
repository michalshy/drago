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

        vk::Extent2D get_extent() { return extent; }
        vk::SurfaceFormatKHR get_format() { return format; }
    private:
        VulkanSurface* surface;
        VulkanDevice* device;
        GLFWwindow* window;

        VkSwapchainKHR swapchain;
        std::vector<vk::Image> images;
        vk::SurfaceFormatKHR format;
        vk::Extent2D extent;

        // views
        std::vector<vk::ImageView> image_views;

        void create_swapchain();
        void create_image_views();

        vk::SurfaceFormatKHR choose_surface_fmt(const std::vector<vk::SurfaceFormatKHR>& formats);
        vk::PresentModeKHR choose_present_mode(const std::vector<vk::PresentModeKHR>& modes);
        vk::Extent2D choose_extent(const vk::SurfaceCapabilitiesKHR& capabilities);
    };
}