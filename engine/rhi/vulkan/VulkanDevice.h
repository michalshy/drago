#pragma once

#include "rhi/vulkan/VulkanInstance.h"
#include "rhi/vulkan/VulkanSurface.h"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <optional>

namespace drago::rhi
{
    struct SwapChainSupportDetails;
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool is_complete() {
            return graphics_family.has_value() && present_family.has_value();
        }
    };

    class VulkanDevice
    {
    public:
        explicit VulkanDevice(
            VulkanInstance* instance,
            VulkanSurface* surface
        );
        ~VulkanDevice();

        vk::PhysicalDevice get_physical() { return physical_dev; }
        vk::Device get() { return dev; }
        vk::Queue get_graphics() { return graphics_queue; }
        vk::Queue get_present() { return present_queue; }

        void wait();

        SwapChainSupportDetails query_support(vk::PhysicalDevice dev);
        QueueFamilyIndices find_queue_families(vk::PhysicalDevice dev);
    private:
        vk::PhysicalDevice physical_dev;
        vk::Device dev;
        vk::Queue graphics_queue;
        vk::Queue present_queue;

        VulkanInstance* instance;
        VulkanSurface* surface;

        void pick_physical_device();
        void create_logical_device();

        bool is_device_suitable(vk::PhysicalDevice dev);
        bool check_extension_support(vk::PhysicalDevice dev);
    };
}