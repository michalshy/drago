#include "VulkanDevice.h"
#include "rhi/vulkan/VulkanInstance.h"
#include "vulkan/vulkan.hpp"
#include <stdexcept>
#include <vector>

namespace drago::rhi
{

VulkanDevice::VulkanDevice(VulkanInstance* instance)
    : physical_dev(nullptr)
    , dev(nullptr)
{
    pick_physical_device(instance);
    create_logical_device(instance);
}

VulkanDevice::~VulkanDevice()
{
    dev.destroy();
}

void VulkanDevice::pick_physical_device(VulkanInstance* instance) 
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

void VulkanDevice::create_logical_device(VulkanInstance* instance)
{
    QueueFamilyIndices indices = find_queue_families(physical_dev);

    float priority = 1.0f;
    auto queue_create_info = vk::DeviceQueueCreateInfo{}
        .setQueueFamilyIndex(indices.family.value())
        .setQueueCount(1)
        .setPQueuePriorities(&priority);

    auto device_features = vk::PhysicalDeviceFeatures{};

    auto device_info = vk::DeviceCreateInfo{}
        .setQueueCreateInfos(queue_create_info)
        .setPEnabledFeatures(&device_features);

    dev = physical_dev.createDevice(device_info);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(dev);
    graphics_queue = dev.getQueue(indices.family.value(), 0);
}

QueueFamilyIndices VulkanDevice::find_queue_families(vk::PhysicalDevice dev)
{
    QueueFamilyIndices indices;

    auto families = dev.getQueueFamilyProperties();
    int i = 0;
    for (const auto& queueFamily : families) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.family = i;
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