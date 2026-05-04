#include "VulkanDevice.h"
#include "rhi/vulkan/VulkanInstance.h"
#include "rhi/vulkan/VulkanSurface.h"
#include "vulkan/vulkan.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <set>
#include "VulkanSwapchain.h"

namespace drago::rhi
{

const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VulkanDevice::VulkanDevice(
    VulkanInstance* instance,
    VulkanSurface* surface
)
    : physical_dev(nullptr)
    , dev(nullptr)
    , instance(instance)
    , surface(surface)
{
    pick_physical_device();
    create_logical_device();
}

VulkanDevice::~VulkanDevice()
{
    dev.destroy();
}

SwapChainSupportDetails VulkanDevice::query_support(vk::PhysicalDevice dev)
{
    SwapChainSupportDetails details;
    details.capabilities = dev.getSurfaceCapabilitiesKHR(surface->get());
    details.formats = dev.getSurfaceFormatsKHR(surface->get());
    details.present_modes = dev.getSurfacePresentModesKHR(surface->get());
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

        if (dev.getSurfaceSupportKHR(i, surface->get())) {
            indices.present_family = i;
        }

        if (indices.is_complete()) {
            break;
        }

        i++;
    }

    return indices;
}

void VulkanDevice::pick_physical_device() 
{
    auto devices = instance->get().enumeratePhysicalDevices();

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

    auto device_features = vk::PhysicalDeviceFeatures{};

    auto device_info = vk::DeviceCreateInfo{}
        .setQueueCreateInfos(queue_create_infos)
        .setPEnabledFeatures(&device_features)
        .setPEnabledExtensionNames(DEVICE_EXTENSIONS);

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

    return indices.is_complete() && ex_support && swapchain_fit;
}

bool VulkanDevice::check_extension_support(vk::PhysicalDevice dev) {
    auto extensions = dev.enumerateDeviceExtensionProperties();

    std::set<std::string> required(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

    for(const auto& extension: extensions) {
        required.erase(extension.extensionName);
    }

    return required.empty();
}

}