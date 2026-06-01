#include "VulkanSwapchain.h"
#include "GLFW/glfw3.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanSurface.h"
#include "rhi/vulkan/VulkanUtils.h"
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
        create_swapchain();
        create_image_views();
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        cleanup();
    }

    void VulkanSwapchain::present(uint32_t img_idx, vk::Semaphore wait_semaphore)
    {
        auto present_info = vk::PresentInfoKHR{}
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&wait_semaphore);

        vk::SwapchainKHR swapchains[] = { swapchain };
        present_info.setSwapchainCount(1);
        present_info.setPSwapchains(swapchains);
        present_info.setPImageIndices(&img_idx);
        present_info.setPResults(nullptr);
        
        [[maybe_unused]] auto res = device->get_present().presentKHR(present_info);
    }

    void VulkanSwapchain::recreate()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        cleanup();
        create_swapchain();
        create_image_views();
    }

    void VulkanSwapchain::create_swapchain() {
        SwapChainSupportDetails details = device->query_support(device->get_physical());
        
        vk::SurfaceFormatKHR swapchain_format = choose_surface_fmt(details.formats);
        vk::PresentModeKHR swapchain_mode = choose_present_mode(details.present_modes);
        vk::Extent2D swapchain_extent = choose_extent(details.capabilities);

        uint32_t image_count = details.capabilities.minImageCount + 1;
        if (details.capabilities.maxImageCount > 0 &&
            image_count > details.capabilities.maxImageCount) {
                image_count = details.capabilities.maxImageCount;
            }

        auto swapchain_info = vk::SwapchainCreateInfoKHR{}
            .setSurface(surface->get())
            .setMinImageCount(image_count)
            .setImageFormat(swapchain_format.format)
            .setImageColorSpace(swapchain_format.colorSpace)
            .setImageExtent(swapchain_extent)
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
        swapchain_info.setPresentMode(swapchain_mode);
        swapchain_info.setClipped(vk::True);

        swapchain_info.setOldSwapchain(nullptr);

        swapchain = device->get().createSwapchainKHR(swapchain_info);
        if(!swapchain) {
            throw std::runtime_error("failed to create swap chain!");
        }

        images = device->get().getSwapchainImagesKHR(swapchain);
        extent = swapchain_extent;
        format = swapchain_format;
    }

    void VulkanSwapchain::create_image_views() 
    {
        image_views.resize(images.size());
        for (size_t i = 0; i < images.size(); ++i) {

            image_views[i] = create_image_view(device, images[i], format.format);
            if(!image_views[i]) {
                throw std::runtime_error("failed to create image view");
            }
        }
    }

    void VulkanSwapchain::cleanup()
    {
        for(auto view : image_views)
        {
            device->get().destroyImageView(view);
        }
        image_views.clear();
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