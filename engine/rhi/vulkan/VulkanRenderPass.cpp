#include "VulkanRenderPass.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "vulkan/vulkan.hpp"

namespace drago::rhi
{

VulkanRenderPass::VulkanRenderPass(
    VulkanDevice* device,
    VulkanSwapchain* swapchain
)
    : swapchain(swapchain)
{
    auto color_att = vk::AttachmentDescription{}
        .setFormat(swapchain->get_format().format)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    auto color_att_ref = vk::AttachmentReference{}
        .setAttachment(0)
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);    

    auto subpass = vk::SubpassDescription{}
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachmentCount(1)
        .setColorAttachments(color_att_ref);

    auto dependency = vk::SubpassDependency{}
        .setSrcSubpass(vk::SubpassExternal)
        .setDstSubpass(0)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlags{})
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);


    auto renderpass_info = vk::RenderPassCreateInfo{}
        .setAttachmentCount(1)
        .setAttachments(color_att)
        .setSubpassCount(1)
        .setSubpasses(subpass)
        .setDependencyCount(1)
        .setPDependencies(&dependency);

    renderpass = device->get().createRenderPass(renderpass_info);
    
    if(!renderpass)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

VulkanRenderPass::~VulkanRenderPass()
{
    device->get().destroyRenderPass(renderpass);
}

}