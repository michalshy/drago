add_library(engine STATIC
    core/Window.cpp
    core/EventLoop.cpp
    rhi/vulkan/VulkanInstance.cpp
    rhi/vulkan/VulkanRenderPass.cpp
    rhi/vulkan/VulkanPipeline.cpp
    rhi/vulkan/VulkanDevice.cpp
    rhi/vulkan/VulkanSwapchain.cpp
    rhi/vulkan/VulkanSurface.cpp
    rhi/vulkan/VulkanFramebuffer.cpp
    rhi/vulkan/VulkanCommandBuffer.cpp
    renderer/Renderer.cpp
)

target_compile_definitions(engine PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

target_compile_definitions(engine PRIVATE 
    ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets"
)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine PUBLIC glfw glm spdlog::spdlog Vulkan::Vulkan)