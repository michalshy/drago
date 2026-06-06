#pragma once

#include "GLFW/glfw3.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "vulkan/vulkan.hpp"
#include <cstdint>
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
            VulkanDevice* device,
            GLFWwindow* window  
        );
        ~VulkanSwapchain();

        vk::SwapchainKHR get() { return swapchain; }
        vk::Extent2D get_extent() { return extent; }
        vk::SurfaceFormatKHR get_format() { return format; }
        std::vector<vk::ImageView>& get_views() { return image_views; }
        std::vector<vk::Image>& get_images() { return images; }
        uint32_t get_image_count() { return images.size(); }

        void present(uint32_t img_idx, vk::Semaphore wait_semaphore);
        void recreate();    
    private:
        VulkanDevice* device;
        GLFWwindow* window;

        vk::SwapchainKHR swapchain;
        std::vector<vk::Image> images;
        vk::SurfaceFormatKHR format;
        vk::Extent2D extent;

        // views
        std::vector<vk::ImageView> image_views;

        void create_swapchain();
        void create_image_views();
        void cleanup();

        vk::SurfaceFormatKHR choose_surface_fmt(const std::vector<vk::SurfaceFormatKHR>& formats);
        vk::PresentModeKHR choose_present_mode(const std::vector<vk::PresentModeKHR>& modes);
        vk::Extent2D choose_extent(const vk::SurfaceCapabilitiesKHR& capabilities);
    };
}