#pragma once

#include "renderer/Image.h"
#include "rhi/vulkan/VulkanDevice.h"
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

private:
    VulkanDevice* device;
    renderer::Image& img;
};

}