#pragma once

#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <optional>
#include "GLFW/glfw3.h"

namespace drago::rhi
{
    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

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
            GLFWwindow* window,
            bool validation
        );
        ~VulkanDevice();

        vk::PhysicalDevice get_physical() { return physical_dev; }
        vk::Device get() { return dev; }
        vk::Queue get_graphics() { return graphics_queue; }
        vk::Queue get_present() { return present_queue; }
        vk::SurfaceKHR get_surface() { return surface; }

        void wait();

        SwapChainSupportDetails query_support(vk::PhysicalDevice dev);
        QueueFamilyIndices find_queue_families(vk::PhysicalDevice dev);
    private:
        // Instance
        vk::Instance instance;
        vk::DebugUtilsMessengerEXT debug_messanger;
        bool validation;
        void create_instance();
        void setup_debug();
        bool check_validation_support();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
            vk::DebugUtilsMessageTypeFlagsEXT type,
            const vk::DebugUtilsMessengerCallbackDataEXT* data,
            void* user_data
        );

        // Surface
        GLFWwindow* window;
        vk::SurfaceKHR surface;
        void create_surface();

        // Devices
        vk::PhysicalDevice physical_dev;
        vk::Device dev;

        void pick_physical_device();
        void create_logical_device();

        bool is_device_suitable(vk::PhysicalDevice dev);
        bool check_extension_support(vk::PhysicalDevice dev);

        // Queues
        vk::Queue graphics_queue;
        vk::Queue present_queue;
    };
}