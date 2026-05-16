#include "VulkanInstance.h"
#include "spdlog/spdlog.h"
#include "GLFW/glfw3.h"
#include "stdexcept"
#include "vulkan/vulkan.hpp"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace drago::rhi {

VulkanInstance::VulkanInstance(bool enable_validation)
    : validation(enable_validation)
{
    create_instance();
    if (validation) setup_debug();
}

VulkanInstance::~VulkanInstance() {
    if (validation && debug_messanger) {
        instance.destroyDebugUtilsMessengerEXT(debug_messanger);
    }
    instance.destroy();
}

void VulkanInstance::create_instance() {
    VULKAN_HPP_DEFAULT_DISPATCHER.init();
    if (validation && !check_validation_support()) {
        throw std::runtime_error("Validation layers requested but not available");
    }

    auto app_info = vk::ApplicationInfo{}
        .setPApplicationName("drago")
        .setApplicationVersion(VK_MAKE_VERSION(0, 1, 0))
        .setPEngineName("drago engine")
        .setEngineVersion(VK_MAKE_VERSION(0, 1, 0))
        .setApiVersion(VK_API_VERSION_1_3);

    // extensions wymagane przez GLFW
    uint32_t ext_count = 0;
    const char** exts = glfwGetRequiredInstanceExtensions(&ext_count);
    std::vector<const char*> extensions(exts, exts + ext_count);

    if (validation) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

#ifdef __APPLE__
#if DRAGO_VULKAN
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
#endif

    auto create_info = vk::InstanceCreateInfo{}
        .setPApplicationInfo(&app_info)
        .setEnabledExtensionCount((uint32_t)extensions.size())
        .setPpEnabledExtensionNames(extensions.data());

#ifdef __APPLE__
#if DRAGO_VULKAN
    create_info.setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR);
#endif
#endif

    if (validation) {
        create_info.enabledLayerCount   = (uint32_t)validation_layers.size();
        create_info.ppEnabledLayerNames = validation_layers.data();
    }

    instance = vk::createInstance(create_info);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
    spdlog::info("Vulkan instance created (API 1.3)");
}

void VulkanInstance::setup_debug() {
    auto create_info = vk::DebugUtilsMessengerCreateInfoEXT{}
        .setMessageSeverity(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
        .setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
        .setPfnUserCallback(debug_callback);

    debug_messanger = instance.createDebugUtilsMessengerEXT(create_info);
}

bool VulkanInstance::check_validation_support() {
    auto layers = vk::enumerateInstanceLayerProperties();
    for (const char* name : validation_layers) {
        bool found = false;
        for (const auto& layer : layers) {
            if (strcmp(name, layer.layerName) == 0) { found = true; break; }
        }
        if (!found) return false;
    }
    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::debug_callback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT* data,
    void* user_data)
{
    if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        spdlog::warn("{}", std::string(data->pMessage));
    }
    return VK_FALSE;
}

}