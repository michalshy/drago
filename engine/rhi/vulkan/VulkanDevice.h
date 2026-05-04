#pragma once

#include "rhi/vulkan/VulkanInstance.h"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <optional>

namespace drago::rhi
{
    struct QueueFamilyIndices {
        std::optional<uint32_t> family;

        bool is_complete() {
            return family.has_value();
        }
    };

    class VulkanDevice
    {
    public:
        explicit VulkanDevice(VulkanInstance* instance);
        ~VulkanDevice();
        void pick_physical_device();
    
    private:
        vk::Device dev;
        vk::PhysicalDevice physical_dev;
        vk::Queue graphics_queue;

        void pick_physical_device(VulkanInstance* instance);
        void create_logical_device(VulkanInstance* instance);
        QueueFamilyIndices find_queue_families(vk::PhysicalDevice dev);

        bool is_device_suitable(vk::PhysicalDevice dev);
    };
}