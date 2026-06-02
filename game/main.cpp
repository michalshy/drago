#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <cstdint>
#include <spdlog/spdlog.h>
#include "core/Platform.h"
#include "glm/ext/matrix_transform.hpp"
#include "renderer/Image.h"
#include "renderer/Types.h"
#include "rhi/vulkan/VulkanDescriptorSet.h"
#include "rhi/vulkan/VulkanIndexBuffer.h"
#include "rhi/vulkan/VulkanTexture.h"
#include "rhi/vulkan/VulkanUniformBuffer.h"

#ifdef DRAGO_METAL

#elif DRAGO_VULKAN
#include <vulkan/vulkan.hpp>
#include "rhi/vulkan/VulkanCommandBuffer.h"
#include "rhi/vulkan/VulkanFramebuffer.h"
#include "rhi/vulkan/VulkanInstance.h"
#include "rhi/vulkan/VulkanDevice.h"
#include "rhi/vulkan/VulkanRenderPass.h"
#include "rhi/vulkan/VulkanSurface.h"
#include "rhi/vulkan/VulkanSwapchain.h"
#include "rhi/vulkan/VulkanPipeline.h"
#include "rhi/vulkan/VulkanVertexBuffer.h"
#include "rhi/vulkan/VulkanUtils.h"
#endif
#include "renderer/Vertex.h"

const std::vector<drago::renderer::Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

static void on_framebuffer_resize(GLFWwindow* window, int width, int height) {
    auto* flag = reinterpret_cast<bool*>(glfwGetWindowUserPointer(window));
    *flag = true;
}

#if defined(DRAGO_VULKAN)

void update_ubo(uint32_t img_idx, drago::rhi::VulkanUniformBuffer& buff, drago::rhi::VulkanSwapchain* swapchain)
{
    static auto start_time = std::chrono::high_resolution_clock::now();

    auto current = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(current - start_time).count();

    drago::renderer::UniformBufferObject ubo{};
    ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    ubo.proj = glm::perspective(glm::radians(45.0f), 
        swapchain->get_extent().width / (float)swapchain->get_extent().height, 
        0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    buff.update(img_idx, ubo);
}

#endif

int main() {
    spdlog::info("Platform: {} | RHI: {}", DRAGO_PLATFORM_NAME, DRAGO_RHI_NAME);

    bool resized = false;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "drago", nullptr, nullptr);

    glfwSetWindowUserPointer(window, &resized);
    glfwSetFramebufferSizeCallback(window, on_framebuffer_resize);

    auto img = drago::renderer::Image(ASSETS_DIR "/textures/texture.jpg");

#if defined(DRAGO_VULKAN)
    drago::rhi::VulkanInstance instance(true);
    spdlog::info("Init OK");

    drago::rhi::VulkanSurface surface(&instance, window);
    spdlog::info("Surface OK");

    drago::rhi::VulkanDevice device(&instance, &surface);
    spdlog::info("Device OK");

    drago::rhi::VulkanSwapchain swapchain(&surface, &device, window);
    spdlog::info("Swapchain OK");

    drago::rhi::VulkanRenderPass renderpass(&device, &swapchain);
    spdlog::info("Renderpass OK");

    drago::rhi::VulkanVertexBuffer vertexbuffer(vertices, &device);
    spdlog::info("Vertices OK");

    drago::rhi::VulkanIndexBuffer indexbuffer(indices, &device);
    spdlog::info("Indices OK");

    drago::rhi::VulkanUniformBuffer uniformbuffer(&device);
    spdlog::info("Uniform OK");

    drago::rhi::VulkanTexture tex(img, &device);
    drago::rhi::VulkanDescriptorSet descriptor_set(&device, &uniformbuffer, &tex);
    spdlog::info("Descriptor Set OK");

    drago::rhi::VulkanPipeline pipeline(&device, &renderpass, &swapchain, &descriptor_set);
    spdlog::info("Pipeline OK");

    drago::rhi::VulkanFramebuffer framebuffer(&device, &swapchain, &renderpass);
    spdlog::info("Framebuffer OK");

    drago::rhi::VulkanCommandBuffer cmd(&device, &vertexbuffer, &indexbuffer, &framebuffer, &descriptor_set, &swapchain, &renderpass, &pipeline);
    spdlog::info("CommandBuffer OK");

#endif

    uint32_t current_frame = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        #if defined(DRAGO_VULKAN)
        cmd.wait_for_fence(current_frame);
        
        uint32_t idx;
        try {
            idx = device.get().acquireNextImageKHR(
                swapchain.get(), UINT64_MAX,
                cmd.image_semaphore(current_frame)
            ).value;
        } catch (vk::OutOfDateKHRError&) {
            resized = false;
            cmd.recreate();
            continue;
        }
        
        cmd.reset_fence(current_frame);
        cmd.reset(current_frame);
        
        update_ubo(current_frame, uniformbuffer, &swapchain);
        cmd.record(idx, current_frame);
        cmd.submit(idx, current_frame);
        
        try {
            swapchain.present(idx, cmd.render_semaphore(idx));
        } catch (vk::OutOfDateKHRError&) {
            resized = false;
            cmd.recreate();
        }

        if(resized)
        {
            resized = false;
            cmd.recreate();
        }
    
        current_frame = (current_frame + 1) % drago::rhi::MAX_FRAMES_IN_FLIGHT;

        #endif
    }

    #if defined(DRAGO_VULKAN)
    device.wait();
    #endif

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}