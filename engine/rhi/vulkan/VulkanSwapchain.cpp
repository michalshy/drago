#include "VulkanSwapchain.h"
#include "GLFW/glfw3.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanSurface.h"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace drago::rhi
{
    VulkanSwapchain::VulkanSwapchain(
        VulkanSurface* surface,
        VulkanDevice* device,
        GLFWwindow* window
    ) 
        : surface(surface)
        , device(device)
        , window(window)
        , swapchain(nullptr)
    {
        SwapChainSupportDetails details = device->query_support(device->get_physical());
        
        vk::SurfaceFormatKHR format = choose_surface_fmt(details.formats);
        vk::PresentModeKHR mode = choose_present_mode(details.present_modes);
        vk::Extent2D extent = choose_extent(details.capabilities);

        uint32_t image_count = details.capabilities.minImageCount + 1;
        if (details.capabilities.maxImageCount > 0 &&
            image_count > details.capabilities.maxImageCount) {
                image_count = details.capabilities.maxImageCount;
            }

        auto swapchain_info = vk::SwapchainCreateInfoKHR{}
            .setSurface(surface->get())
            .setMinImageCount(image_count)
            .setImageFormat(format.format)
            .setImageColorSpace(format.colorSpace)
            .setImageExtent(extent)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        QueueFamilyIndices indices = device->find_queue_families(device->get_physical());
        uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};
        
        if (indices.graphics_family != indices.present_family) {
            swapchain_info.setImageSharingMode(vk::SharingMode::eConcurrent);
            swapchain_info.setQueueFamilyIndexCount(2);
            swapchain_info.setPQueueFamilyIndices(queue_family_indices);
        } else {
            swapchain_info.setImageSharingMode(vk::SharingMode::eExclusive);
            swapchain_info.setQueueFamilyIndexCount(0);
            swapchain_info.setPQueueFamilyIndices(nullptr);
        }

        swapchain_info.setPreTransform(details.capabilities.currentTransform);
        swapchain_info.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        swapchain_info.setPresentMode(mode);
        swapchain_info.setClipped(vk::True);

        swapchain_info.setOldSwapchain(nullptr);

        swapchain = device->get().createSwapchainKHR(swapchain_info);
        if(!swapchain) {
            throw std::runtime_error("failed to create swap chain!");
        }

        images = device->get().getSwapchainImagesKHR(swapchain);
        extent = extent;
        format = format;
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        device->get().destroySwapchainKHR(swapchain);
    }

    vk::SurfaceFormatKHR VulkanSwapchain::choose_surface_fmt(
        const std::vector<vk::SurfaceFormatKHR>& formats
    ) {
        for (const auto& format : formats) {
            if (format.format == vk::Format::eB8G8R8A8Srgb &&
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return format;   
            } 
        }
        return formats[0];
    }

    vk::PresentModeKHR VulkanSwapchain::choose_present_mode(
        const std::vector<vk::PresentModeKHR>& modes
    ) {
        for(const auto& mode : modes) {
            if(mode == vk::PresentModeKHR::eMailbox) {
                return mode;
            }
        }

        return vk::PresentModeKHR::eFifo;
    }
    
    vk::Extent2D VulkanSwapchain::choose_extent(const vk::SurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            vk::Extent2D actual = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actual.width = std::clamp(
                actual.width, 
                capabilities.minImageExtent.width,
                capabilities.maxImageExtent.width
            );
            actual.height = std::clamp(
                actual.height, 
                capabilities.minImageExtent.height, 
                capabilities.maxImageExtent.height
            );

            return actual;
        }
    }

}