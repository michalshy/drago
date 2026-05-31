#include "VulkanTexture.h"
#include "rhi/vulkan/VulkanUtils.h"
#include <cstdint>
#include <tuple>
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

    std::tie(tex_img, tex_mem) = create_image(
        device, 
        img.width,
        img.height, 
        vk::Format::eR8G8B8A8Srgb, 
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, 
        vk::MemoryPropertyFlagBits::eDeviceLocal);

    SingleCommand command = begin_single_command(device);
    transition_img_layout(device, command.buffer, tex_img, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    copy_buffer_to_image(command.buffer, staging_buffer, tex_img, static_cast<uint32_t>(img.width), static_cast<uint32_t>(img.height));

    transition_img_layout(device, command.buffer, tex_img, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    end_single_command(device, std::move(command));

    device->get().destroyBuffer(staging_buffer);
    device->get().freeMemory(staging_mem);
}

VulkanTexture::~VulkanTexture()
{
    device->get().destroyImage(tex_img);
    device->get().freeMemory(tex_mem);
}

}