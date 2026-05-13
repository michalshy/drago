#include "VulkanPipeline.h"
#include "renderer/Vertex.h"

#include <fstream>
#include <vector>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

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

static vk::VertexInputBindingDescription get_binding_description()
{
    return vk::VertexInputBindingDescription{}
        .setBinding(0)
        .setStride(sizeof(drago::renderer::Vertex))
        .setInputRate(vk::VertexInputRate::eVertex);
}

static std::array<vk::VertexInputAttributeDescription, 2> get_attribute_descriptions()
{
    return {{
        vk::VertexInputAttributeDescription{}
            .setLocation(0)
            .setBinding(0)
            .setFormat(vk::Format::eR32G32Sfloat)
            .setOffset(offsetof(drago::renderer::Vertex, pos)),
        vk::VertexInputAttributeDescription{}
            .setLocation(1)
            .setBinding(0)
            .setFormat(vk::Format::eR32G32B32Sfloat)
            .setOffset(offsetof(drago::renderer::Vertex, color)),
    }};
}

}

VulkanPipeline::VulkanPipeline(
    VulkanDevice* device,
    VulkanRenderPass* renderpass,
    VulkanSwapchain* swapchain
)
    : device(device)
    , renderpass(renderpass)
    , swapchain(swapchain)
{
    auto shader_module = create_shader_module(details::read_file(ASSETS_DIR "/shaders/compiled/base.spv"));

    auto vert_shader = vk::PipelineShaderStageCreateInfo{}
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(shader_module)
        .setPName("vertMain");

    auto frag_shader = vk::PipelineShaderStageCreateInfo{}
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(shader_module)
        .setPName("fragMain");

    auto shader_stages = { vert_shader, frag_shader };

    auto dynamic_states = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    auto dynamic_state = vk::PipelineDynamicStateCreateInfo{}
        .setDynamicStates(dynamic_states);

    auto binding_desc = details::get_binding_description();
    auto attribute_desc = details::get_attribute_descriptions();

    auto vertex_input = vk::PipelineVertexInputStateCreateInfo{}
      .setVertexBindingDescriptions(binding_desc)
      .setVertexAttributeDescriptions(attribute_desc);

    auto input_assembly = vk::PipelineInputAssemblyStateCreateInfo{}
        .setTopology(vk::PrimitiveTopology::eTriangleList)
        .setPrimitiveRestartEnable(vk::False);

    vk::Viewport viewport{0.0f, 0.0f, static_cast<float>(swapchain->get_extent().width), static_cast<float>(swapchain->get_extent().height), 0.0f, 1.0f};
    vk::Rect2D scissor{vk::Offset2D{ 0, 0 }, swapchain->get_extent()};

    auto viewport_state = vk::PipelineViewportStateCreateInfo{}
        .setViewportCount(1)
        .setScissorCount(1);

    auto rasterizer = vk::PipelineRasterizationStateCreateInfo{}
        .setDepthClampEnable(vk::False)
        .setRasterizerDiscardEnable(vk::False)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eClockwise)
        .setDepthBiasEnable(vk::False)
        .setLineWidth(1.0f);

    auto multisampling = vk::PipelineMultisampleStateCreateInfo{}
        .setRasterizationSamples(vk::SampleCountFlagBits::e1)
        .setSampleShadingEnable(vk::False);

    auto color_blend_attachment = vk::PipelineColorBlendAttachmentState{}
        .setBlendEnable(vk::True)
        .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
        .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
        .setColorBlendOp(vk::BlendOp::eAdd)
        .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
        .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
        .setAlphaBlendOp(vk::BlendOp::eAdd)
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA
        );

    auto color_blending = vk::PipelineColorBlendStateCreateInfo{}
        .setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachments(color_blend_attachment);

    auto pipeline_layout_info = vk::PipelineLayoutCreateInfo{}
    .setSetLayoutCount(0)
    .setPushConstantRangeCount(0);

    pipeline_layout = device->get().createPipelineLayout(pipeline_layout_info);

    // create pipeline

    auto pipeline_info = vk::GraphicsPipelineCreateInfo{}
        .setStageCount(2)
        .setStages(shader_stages)
        .setPVertexInputState(&vertex_input)
        .setPInputAssemblyState(&input_assembly)
        .setPViewportState(&viewport_state)
        .setPRasterizationState(&rasterizer)
        .setPMultisampleState(&multisampling)
        .setPDepthStencilState(nullptr)
        .setPColorBlendState(&color_blending)
        .setPDynamicState(&dynamic_state)
        .setLayout(pipeline_layout)
        .setRenderPass(renderpass->get())
        .setSubpass(0)
        .setBasePipelineHandle(nullptr)
        .setBasePipelineIndex(-1);

    graphics_pipeline = device->get().createGraphicsPipeline(nullptr, pipeline_info).value;

    if(!graphics_pipeline)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

VulkanPipeline::~VulkanPipeline()
{
    device->get().destroyPipeline(graphics_pipeline);
    device->get().destroyPipelineLayout(pipeline_layout);
}

[[nodiscard]] vk::ShaderModule VulkanPipeline::create_shader_module(const std::vector<char>& code)
{
    auto create_info = vk::ShaderModuleCreateInfo{}
        .setCodeSize(code.size() * sizeof(char))
        .setPCode(reinterpret_cast<const uint32_t*>(code.data()));

    return device->get().createShaderModule(create_info);    
}

}