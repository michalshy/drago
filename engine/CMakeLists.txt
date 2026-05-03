add_library(engine STATIC
    core/Window.cpp
    core/EventLoop.cpp
    rhi/vulkan/VulkanInstance.cpp
    rhi/vulkan/VulkanDevice.cpp
    rhi/vulkan/VulkanSwapchain.cpp
    renderer/Renderer.cpp
)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine PUBLIC glfw glm spdlog::spdlog Vulkan::Vulkan)