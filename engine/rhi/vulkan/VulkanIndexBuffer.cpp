#include "VulkanIndexBuffer.h"
#include "VulkanUtils.h"
#include <cstdint>

namespace drago::rhi
{

VulkanIndexBuffer::VulkanIndexBuffer(
    const std::vector<uint16_t>& indices,
    VulkanDevice* device
)
    : device(device)
    , index_count(static_cast<uint32_t>(indices.size()))
{
    vk::DeviceSize buffer_size = sizeof(indices[0]) * indices.size();

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
    memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
    device->get().unmapMemory(staging_mem);

    create_buffer(
        device, 
        buffer_size, 
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, 
        vk::MemoryPropertyFlagBits::eDeviceLocal, 
        index_buffer, 
        index_memory
    );

    copy_buffer(device, stagingbuffer, index_buffer, buffer_size);

    device->get().destroyBuffer(stagingbuffer);
    device->get().freeMemory(staging_mem);
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
   device->get().destroyBuffer(index_buffer);
   device->get().freeMemory(index_memory); 
}

}