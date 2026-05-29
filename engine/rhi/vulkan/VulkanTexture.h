#pragma once

#include "renderer/Image.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanSwapchain.h"
namespace drago::rhi
{

class VulkanTexture
{

public:
    VulkanTexture(
        renderer::Image& img,
        VulkanDevice* device,
        VulkanSwapchain* swapchain
    );
    ~VulkanTexture();

private:
    VulkanDevice* device;
    renderer::Image& img;

    vk::Image tex_img;
    vk::DeviceMemory tex_mem;
};

}