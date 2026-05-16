#include "VulkanVertexBuffer.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanUtils.h"
#include "vulkan/vulkan.hpp"

#include <cstddef>
#include <cstring>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace drago::rhi
{

VulkanVertexBuffer::VulkanVertexBuffer(
    const std::vector<renderer::Vertex>& vertices, 
    VulkanDevice* device
)
    : device(device)
{
    vk::DeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

    vk::Buffer stagingbuffer;
    vk::DeviceMemory staging_mem;

    create_buffer(
        device, 
        buffer_size, 
        vk::BufferUsageFlagBits::eTransferSrc, 
        vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, 
        stagingbuffer, 
        staging_mem);

    
    void* data = device->get().mapMemory(staging_mem, 0, buffer_size);
    memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
    device->get().unmapMemory(staging_mem);

    create_buffer(
        device, 
        buffer_size, 
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, 
        vk::MemoryPropertyFlagBits::eDeviceLocal, 
        vertexbuffer, 
        vertexbuffer_mem
    );

    copy_buffer(device, stagingbuffer, vertexbuffer, buffer_size);

    device->get().destroyBuffer(stagingbuffer);
    device->get().freeMemory(staging_mem);
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
    device->get().destroyBuffer(vertexbuffer);
    device->get().freeMemory(vertexbuffer_mem);
}

}