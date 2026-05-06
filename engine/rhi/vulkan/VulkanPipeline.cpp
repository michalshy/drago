#include "VulkanPipeline.h"

#include <fstream>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace drago::rhi
{

namespace details
{

static std::vector<char> read_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open()) {
        throw std::runtime_error("failed to open file");
    }

    std::vector<char> buffer(file.tellg());

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();

    return buffer;
}

}

VulkanPipeline::VulkanPipeline(VulkanDevice* device)
    : device(device)
{
    auto shader_code = details::read_file(ASSETS_DIR "/shaders/compiled/base.spv");
}

VulkanPipeline::~VulkanPipeline()
{
    
}

[[nodiscard]] vk::ShaderModule VulkanPipeline::create_shader_module(const std::vector<char>& code)
{
    auto create_info = vk::ShaderModuleCreateInfo{}
        .setCodeSize(code.size() * sizeof(char))
        .setPCode(reinterpret_cast<const uint32_t*>(code.data()));

    return device->get().createShaderModule(create_info);    
}

}