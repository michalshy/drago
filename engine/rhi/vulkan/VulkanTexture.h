#pragma once

#include "renderer/Image.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "vulkan/vulkan.hpp"
namespace drago::rhi
{

class VulkanTexture
{

public:
    VulkanTexture(
        renderer::Image& img,
        VulkanDevice* device
    );
    ~VulkanTexture();

    vk::ImageView get_img_view() { return tex_view; }
    vk::Sampler get_sampler() { return sampler; }

private:
    VulkanDevice* device;
    renderer::Image& img;

    vk::Image tex_img;
    vk::DeviceMemory tex_mem;

    vk::ImageView tex_view;
    vk::Sampler sampler;
};

}