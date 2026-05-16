#pragma once

#include "renderer/Vertex.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "vulkan/vulkan.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
namespace drago::rhi
{

class VulkanVertexBuffer
{

public:
    VulkanVertexBuffer(
        const std::vector<renderer::Vertex>& vertices,
        VulkanDevice* device
    );
    ~VulkanVertexBuffer();

    vk::Buffer get() { return vertexbuffer; }

private:
    VulkanDevice* device;

    vk::Buffer vertexbuffer;
    vk::DeviceMemory vertexbuffer_mem;
};

}