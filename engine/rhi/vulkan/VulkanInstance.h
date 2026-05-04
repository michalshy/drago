#pragma once
#include "vulkan/vulkan.hpp"
#include <vector>
#include <string>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

namespace drago::rhi
{
    class VulkanInstance {
    public:
        explicit VulkanInstance(bool enable_validation = true);
        ~VulkanInstance();

        vk::Instance get() const { return instance; }
        bool validation_enabled() const { return validation; }

    private:
        void create_instance();
        void setup_debug();
        bool check_validation_support();

        vk::Instance instance;
        vk::DebugUtilsMessengerEXT debug_messanger;
        bool validation;

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
            vk::DebugUtilsMessageTypeFlagsEXT type,
            const vk::DebugUtilsMessengerCallbackDataEXT* data,
            void* user_data
        );
    };
} // namespace drago::rhi