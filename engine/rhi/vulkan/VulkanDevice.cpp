#include "VulkanDevice.h"
#include "vulkan/vulkan.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <set>
#include "VulkanSwapchain.h"
#include "spdlog/spdlog.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace drago::rhi
{

std::vector<const char*> get_device_extensions(vk::PhysicalDevice dev) {
    std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef __APPLE__
#if DRAGO_VULKAN
    auto available = dev.enumerateDeviceExtensionProperties();
    for (const auto& ext : available) {
        if (strcmp(ext.extensionName, "VK_KHR_portability_subset") == 0) {
            extensions.push_back("VK_KHR_portability_subset");
            break;
        }

    }
#endif
#endif
    return extensions;
}

vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> features{
    vk::PhysicalDeviceFeatures2{}.setFeatures(
        vk::PhysicalDeviceFeatures{}.setSamplerAnisotropy(true)
    ),
    vk::PhysicalDeviceVulkan13Features{}.setSynchronization2(true).setDynamicRendering(true),
    vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT{}.setExtendedDynamicState(true)
};


VulkanDevice::VulkanDevice(
    GLFWwindow* window,
    bool validation    
)
    : window(window)
    , validation(validation)
{
    create_instance();
    if (validation) setup_debug();
    create_surface();
    pick_physical_device();
    create_logical_device();
}

VulkanDevice::~VulkanDevice()
{
    if (validation && debug_messanger) {
        instance.destroyDebugUtilsMessengerEXT(debug_messanger);
    }
    dev.destroy();
    instance.destroySurfaceKHR(surface);
    instance.destroy();
}

void VulkanDevice::wait()
{
    dev.waitIdle();
}

SwapChainSupportDetails VulkanDevice::query_support(vk::PhysicalDevice dev)
{
    SwapChainSupportDetails details;
    details.capabilities = dev.getSurfaceCapabilitiesKHR(surface);
    details.formats = dev.getSurfaceFormatsKHR(surface);
    details.present_modes = dev.getSurfacePresentModesKHR(surface);
    return details;
}

QueueFamilyIndices VulkanDevice::find_queue_families(vk::PhysicalDevice dev)
{
    QueueFamilyIndices indices;

    auto families = dev.getQueueFamilyProperties();
    int i = 0;
    for (const auto& queueFamily : families) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphics_family = i;
        }

        if (dev.getSurfaceSupportKHR(i, surface)) {
            indices.present_family = i;
        }

        if (indices.is_complete()) {
            break;
        }

        i++;
    }

    return indices;
}

void VulkanDevice::create_instance()
{
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

void VulkanDevice::setup_debug() {
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

bool VulkanDevice::check_validation_support() {
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

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDevice::debug_callback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT* data,
    void* user_data)
{
    if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        spdlog::warn(std::string(data->pMessage));
    }
    return VK_FALSE;
}

void VulkanDevice::create_surface()
{
    VkSurfaceKHR raw_surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &raw_surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface");
    }
    surface = raw_surface;
}

void VulkanDevice::pick_physical_device() 
{
    auto devices = instance.enumeratePhysicalDevices();

    if (devices.size() == 0) {
        throw std::runtime_error("failed to find gpu with vulkan support");
    }

    for (const auto& device: devices) {
        if (is_device_suitable(device)) {
            physical_dev = device;
            break;
        }
    }

    if(physical_dev == nullptr) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void VulkanDevice::create_logical_device()
{
    QueueFamilyIndices indices = find_queue_families(physical_dev);

    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_families = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };

    float priority = 1.0f;
    for (uint32_t family : unique_families) {
        queue_create_infos.push_back(
            vk::DeviceQueueCreateInfo{}
                .setQueueFamilyIndex(family)
                .setQueueCount(1)
                .setPQueuePriorities(&priority)
        );
    }

    auto device_extensions = get_device_extensions(physical_dev);
    auto device_info = vk::DeviceCreateInfo{}
        .setQueueCreateInfos(queue_create_infos)
        .setPNext(&features.get<vk::PhysicalDeviceFeatures2>())
        .setPEnabledExtensionNames(device_extensions);

    dev = physical_dev.createDevice(device_info);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(dev);
    graphics_queue = dev.getQueue(indices.graphics_family.value(), 0);
    present_queue  = dev.getQueue(indices.present_family.value(), 0);
}

bool VulkanDevice::is_device_suitable(vk::PhysicalDevice dev)
{
    QueueFamilyIndices indices = find_queue_families(dev);
    bool ex_support = check_extension_support(dev);

    bool swapchain_fit = false;
    if (ex_support) {
        SwapChainSupportDetails details = query_support(dev);
        swapchain_fit = !details.formats.empty() && !details.present_modes.empty();
    }

    auto chain = dev.getFeatures2<
        vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
        >();

    auto& base     = chain.get<vk::PhysicalDeviceFeatures2>();
    auto& vk13     = chain.get<vk::PhysicalDeviceVulkan13Features>();
    auto& dynstate = chain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

    bool features_ok = base.features.samplerAnisotropy
        && vk13.synchronization2
        && vk13.dynamicRendering
        && dynstate.extendedDynamicState;

    return indices.is_complete() && ex_support && swapchain_fit && features_ok;
}

bool VulkanDevice::check_extension_support(vk::PhysicalDevice dev) {
    auto extensions = dev.enumerateDeviceExtensionProperties();

    auto device_extensions = get_device_extensions(dev);
    std::set<std::string> required(device_extensions.begin(), device_extensions.end());

    for(const auto& extension: extensions) {
        required.erase(extension.extensionName);
    }

    return required.empty();
}

}