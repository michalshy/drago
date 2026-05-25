#include "VulkanTexture.h"

namespace drago::rhi
{

VulkanTexture::VulkanTexture(
    renderer::Image& img,
    VulkanDevice* device
)
    : img(img)
    , device(device)
{

}

VulkanTexture::~VulkanTexture()
{

}

}