#include "VulkanUniformBuffer.h"
#include "VulkanUtils.h"
#include "renderer/Types.h"

namespace drago::rhi
{

VulkanUniformBuffer::VulkanUniformBuffer(
    VulkanDevice* device
)
    : device(device)
{
    buffers.resize(MAX_FRAMES_IN_FLIGHT);
    buffers_mem.resize(MAX_FRAMES_IN_FLIGHT);
    buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

    vk::DeviceSize buffer_size = sizeof(renderer::UniformBufferObject);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        create_buffer(
            device, 
            buffer_size, 
            vk::BufferUsageFlagBits::eUniformBuffer, 
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            buffers[i],
            buffers_mem[i]
        );

        buffers_mapped[i] = device->get().mapMemory(buffers_mem[i], 0, buffer_size);
    }
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
    
}

}