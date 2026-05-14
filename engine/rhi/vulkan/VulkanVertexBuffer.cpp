#include "VulkanVertexBuffer.h"
#include "rhi/vulkan/VulkanDevice.h"

#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

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

VulkanVertexBuffer::VulkanVertexBuffer(
    const std::vector<renderer::Vertex>& vertices, 
    VulkanDevice* device
)
    : device(device)
{
    auto buffer_info = vk::BufferCreateInfo{}
        .setSize(sizeof(vertices[0]) * vertices.size())
        .setUsage(vk::BufferUsageFlagBits::eVertexBuffer)
        .setSharingMode(vk::SharingMode::eExclusive);

    vertexbuffer = device->get().createBuffer(buffer_info);

    auto mem_req = device->get().getBufferMemoryRequirements(vertexbuffer);
    auto mem_alloc_info = vk::MemoryAllocateInfo{}
        .setAllocationSize(mem_req.size)
        .setMemoryTypeIndex(details::find_memory_type(
            mem_req.memoryTypeBits, 
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, 
            device
        ));

    vertexbuffer_mem = device->get().allocateMemory(mem_alloc_info);
    device->get().bindBufferMemory(vertexbuffer, vertexbuffer_mem, 0);

    void* data = device->get().mapMemory(vertexbuffer_mem, 0, buffer_info.size);
    memcpy(data, vertices.data(), buffer_info.size);
    device->get().unmapMemory(vertexbuffer_mem);
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
    device->get().destroyBuffer(vertexbuffer);
    device->get().freeMemory(vertexbuffer_mem);
}

}