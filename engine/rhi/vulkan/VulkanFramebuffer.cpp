#include "VulkanFramebuffer.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanRenderPass.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "vulkan/vulkan.hpp"

namespace drago::rhi
{

VulkanFramebuffer::VulkanFramebuffer(
    VulkanDevice* device,
    VulkanSwapchain* swapchain,
    VulkanRenderPass* renderpass    
)
    : device(device)
    , swapchain(swapchain)
    , renderpass(renderpass)
{
    create();
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    for(auto& framebuffer : framebuffers) 
    {
        device->get().destroyFramebuffer(framebuffer);   
    }
}

void VulkanFramebuffer::create()
{
    for(auto& view : swapchain->get_views())
    {
        auto framebuffer_info = vk::FramebufferCreateInfo{}
            .setRenderPass(renderpass->get())
            .setAttachmentCount(1)
            .setAttachments(view)
            .setWidth(swapchain->get_extent().width)
            .setHeight(swapchain->get_extent().height)
            .setLayers(1);
        
        framebuffers.push_back(
            device->get().createFramebuffer(framebuffer_info)
        );
    }
}

void VulkanFramebuffer::recreate()
{
    for(auto& fb: framebuffers)
    {
        device->get().destroyFramebuffer(fb);
    }
    framebuffers.clear();
    create();
}

}