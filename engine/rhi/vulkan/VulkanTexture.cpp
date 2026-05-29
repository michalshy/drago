#include "VulkanTexture.h"
#include "rhi/vulkan/VulkanUtils.h"
#include <cstdint>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace drago::rhi
{

VulkanTexture::VulkanTexture(
    renderer::Image& img,
    VulkanDevice* device,
    VulkanSwapchain* swapchain
)
    : img(img)
    , device(device)
{
    vk::Buffer staging_buffer;
    vk::DeviceMemory staging_mem;

    auto img_size = img.data.size();

    create_buffer(
        device, 
        img_size, 
        vk::BufferUsageFlagBits::eTransferSrc, 
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, 
        staging_buffer, 
        staging_mem
    );

    void* data = device->get().mapMemory(staging_mem, 0, img_size);
    memcpy(data, img.data.data(), img_size);
    device->get().unmapMemory(staging_mem);

    auto img_info = vk::ImageCreateInfo{}
        .setImageType(vk::ImageType::e2D)
        .setFormat(swapchain->get_format().format)
        .setExtent({static_cast<uint32_t>(img.width), static_cast<uint32_t>(img.height), 1})
        .setMipLevels(1)
        .setArrayLayers(1)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setTiling(vk::ImageTiling::eOptimal)
        .setUsage(vk::ImageUsageFlagBits::eSampled)
        .setSharingMode(vk::SharingMode::eExclusive);

    device->get().destroyBuffer(staging_buffer);
    device->get().freeMemory(staging_mem);
}

VulkanTexture::~VulkanTexture()
{

}

}