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

void VulkanDevice::wait()
{
    dev.waitIdle();
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

    auto device_extensions = get_device_extensions(physical_dev);
    auto device_info = vk::DeviceCreateInfo{}
        .setQueueCreateInfos(queue_create_infos)
        .setPEnabledFeatures(&device_features)
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

    return indices.is_complete() && ex_support && swapchain_fit;
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