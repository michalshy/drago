#include "VulkanUtils.h"
#include "VulkanDevice.h"
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan_core.h>

namespace drago::rhi
{

namespace details
{
    uint32_t find_memory_type(uint32_t filter, vk::MemoryPropertyFlags properties, VulkanDevice* dev) {
        auto mem_properties = dev->get_physical().getMemoryProperties();

        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        {
            if ((filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("no suitable memory type!");
    }
}

vk::ImageSubresourceRange image_subresource_range(vk::ImageAspectFlags aspect_mask)
{
    vk::ImageSubresourceRange sub_img {};
    sub_img.aspectMask = aspect_mask;
    sub_img.baseMipLevel = 0;
    sub_img.levelCount = VK_REMAINING_MIP_LEVELS;
    sub_img.baseArrayLayer = 0;
    sub_img.layerCount = VK_REMAINING_ARRAY_LAYERS;

    return sub_img;
}

vk::SemaphoreSubmitInfo semaphore_submit_info(vk::PipelineStageFlags2 stage_mask, vk::Semaphore semaphore)
{
	vk::SemaphoreSubmitInfo submit_info{};
	submit_info.semaphore = semaphore;
	submit_info.stageMask = stage_mask;
	submit_info.deviceIndex = 0;
	submit_info.value = 1;

	return submit_info;
}

vk::CommandBufferSubmitInfo command_buffer_submit_info(vk::CommandBuffer cmd)
{
	VkCommandBufferSubmitInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	info.pNext = nullptr;
	info.commandBuffer = cmd;
	info.deviceMask = 0;

	return info;
}

vk::SubmitInfo2 submit_info(vk::CommandBufferSubmitInfo* cmd, vk::SemaphoreSubmitInfo* signal_semaphore_info,
    vk::SemaphoreSubmitInfo* wait_semaphore_info)
{
    vk::SubmitInfo2 info = {};
    info.pNext = nullptr;

    info.waitSemaphoreInfoCount = wait_semaphore_info == nullptr ? 0 : 1;
    info.pWaitSemaphoreInfos = wait_semaphore_info;

    info.signalSemaphoreInfoCount = signal_semaphore_info == nullptr ? 0 : 1;
    info.pSignalSemaphoreInfos = signal_semaphore_info;

    info.commandBufferInfoCount = 1;
    info.pCommandBufferInfos = cmd;

    return info;
}

void create_buffer(
    VulkanDevice* device,
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    vk::Buffer& buffer,
    vk::DeviceMemory& buffer_memory 
) {
        vk::BufferCreateInfo buffer_info = vk::BufferCreateInfo{}
    .setSize(size)
    .setUsage(usage)
    .setSharingMode(vk::SharingMode::eExclusive);

    buffer = device->get().createBuffer(buffer_info);
    
    vk::MemoryRequirements mem_req = device->get().getBufferMemoryRequirements(buffer);
    vk::MemoryAllocateInfo alloc_info = vk::MemoryAllocateInfo{}
        .setAllocationSize(mem_req.size)
        .setMemoryTypeIndex(details::find_memory_type(
            mem_req.memoryTypeBits, properties,
            device)
        );

    buffer_memory = device->get().allocateMemory(alloc_info);
    device->get().bindBufferMemory(buffer, buffer_memory, 0);
}

void copy_buffer(
    VulkanDevice* device,
    vk::Buffer src,
    vk::Buffer dst,
    vk::DeviceSize size
) {
    SingleCommand cmd = begin_single_command(device);
    vk::BufferCopy region = vk::BufferCopy{}
        .setSrcOffset(0)
        .setDstOffset(0)
        .setSize(size);

    cmd.buffer.copyBuffer(src, dst, 1, &region);
    end_single_command(device, std::move(cmd));
}

std::pair<vk::Image, vk::DeviceMemory> create_image(
    VulkanDevice* device,
    uint32_t width,
    uint32_t height,
    vk::Format format,
    vk::ImageTiling tiling,
    vk::ImageUsageFlags usage,
    vk::MemoryPropertyFlags props
) {
    auto img_info = vk::ImageCreateInfo{}
        .setImageType(vk::ImageType::e2D)
        .setFormat(format)
        .setExtent({static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1})
        .setMipLevels(1)
        .setArrayLayers(1)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setTiling(tiling)
        .setUsage(usage)
        .setSharingMode(vk::SharingMode::eExclusive);

    vk::Image img = device->get().createImage(img_info);

    vk::MemoryRequirements reqs = device->get().getImageMemoryRequirements(img);
    auto alloc_info = vk::MemoryAllocateInfo{}
        .setAllocationSize(reqs.size)
        .setMemoryTypeIndex(details::find_memory_type(reqs.memoryTypeBits, props, device));
        
    vk::DeviceMemory image_memory = device->get().allocateMemory(alloc_info);
    device->get().bindImageMemory(img, image_memory, 0);

    return {std::move(img), std::move(image_memory)};
}

void copy_buffer_to_image(
    vk::CommandBuffer& cmd,
    const vk::Buffer& buffer,
    vk::Image& img,
    uint32_t width,
    uint32_t height
) {
    auto region = vk::BufferImageCopy{}
        .setBufferOffset(0)
        .setBufferRowLength(0)
        .setBufferImageHeight(0)
        .setImageSubresource(
            vk::ImageSubresourceLayers{}
                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseArrayLayer(0)
                .setLayerCount(1)
                .setMipLevel(0)
        )
        .setImageOffset({0,0,0})
        .setImageExtent({width, height, 1});

    cmd.copyBufferToImage(buffer, img, vk::ImageLayout::eTransferDstOptimal, region);
}

SingleCommand begin_single_command(VulkanDevice *device)
{
    QueueFamilyIndices indices = device->find_queue_families(device->get_physical());

    auto pool_info = vk::CommandPoolCreateInfo{}
        .setFlags(vk::CommandPoolCreateFlagBits::eTransient)
        .setQueueFamilyIndex(indices.graphics_family.value());

    vk::CommandPool pool = device->get().createCommandPool(pool_info);

    vk::CommandBufferAllocateInfo alloc_info = vk::CommandBufferAllocateInfo{}
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(pool)
        .setCommandBufferCount(1);

    vk::CommandBuffer command_buffer = device->get().allocateCommandBuffers(alloc_info).front();
    
    auto begin_info = vk::CommandBufferBeginInfo{}
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    command_buffer.begin(begin_info);
    return {std::move(command_buffer), std::move(pool)};
}

void end_single_command(VulkanDevice *device, SingleCommand cmd)
{
    cmd.buffer.end();

    vk::SubmitInfo submit_info = vk::SubmitInfo{}
        .setCommandBufferCount(1)
        .setCommandBuffers(cmd.buffer);

    device->get_graphics().submit(submit_info);
    device->get_graphics().waitIdle();

    device->get().freeCommandBuffers(cmd.pool, cmd.buffer);
    device->get().destroyCommandPool(cmd.pool);
}

void transition_img_layout(
    vk::CommandBuffer buffer,
    const vk::Image& img,
    vk::ImageLayout old_layout,
    vk::ImageLayout new_layout
) {
    vk::ImageAspectFlags aspect_mask = (new_layout == vk::ImageLayout::eDepthAttachmentOptimal) 
        ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;

    auto barrier = vk::ImageMemoryBarrier2{}
        .setOldLayout(old_layout)
        .setNewLayout(new_layout)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands)
        .setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite)
        .setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands)
        .setSrcAccessMask(
            vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead
        )
        .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
        .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
        .setImage(img)
        .setSubresourceRange(
            image_subresource_range(aspect_mask)  
        );

    vk::DependencyInfo dep_info = vk::DependencyInfo{}
        .setImageMemoryBarrierCount(1)
        .setImageMemoryBarriers(barrier);

    buffer.pipelineBarrier2(dep_info);
}

vk::ImageView create_image_view(
    VulkanDevice* device,
    vk::Image const& img,
    vk::Format format
) {
    auto view_info = vk::ImageViewCreateInfo{}
        .setImage(img)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(format)
        .setSubresourceRange(
            vk::ImageSubresourceRange{}
                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseArrayLayer(0)
                .setLayerCount(1)
                .setBaseMipLevel(0)
                .setLevelCount(1)
        );
    return device->get().createImageView(view_info);
}

}