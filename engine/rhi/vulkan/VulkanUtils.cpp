#include "VulkanUtils.h"
#include "VulkanDevice.h"
#include "vulkan/vulkan.hpp"

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

        vk::BufferCopy region = vk::BufferCopy{}
            .setSrcOffset(0)
            .setDstOffset(0)
            .setSize(size);

        command_buffer.copyBuffer(src, dst, 1, &region);
        command_buffer.end();

        vk::SubmitInfo submit_info = vk::SubmitInfo{}
            .setCommandBufferCount(1)
            .setCommandBuffers(command_buffer);

        device->get_graphics().submit(submit_info);
        device->get_graphics().waitIdle();

        device->get().freeCommandBuffers(pool, command_buffer);
        device->get().destroyCommandPool(pool);
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
}