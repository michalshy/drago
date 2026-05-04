#include "VulkanDevice.h"
#include "rhi/vulkan/VulkanInstance.h"
#include "rhi/vulkan/VulkanSurface.h"
#include "vulkan/vulkan.hpp"
#include <stdexcept>
#include <vector>
#include <set>

namespace drago::rhi
{

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
        .setPEnabledFeatures(&device_features);

    dev = physical_dev.createDevice(device_info);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(dev);
    graphics_queue = dev.getQueue(indices.graphics_family.value(), 0);
    present_queue  = dev.getQueue(indices.present_family.value(), 0);
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

bool VulkanDevice::is_device_suitable(vk::PhysicalDevice dev)
{
    QueueFamilyIndices indices = find_queue_families(dev);
    return indices.is_complete();
}

}