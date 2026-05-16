set(ENGINE_COMMON_SOURCES
    core/Window.cpp
    core/EventLoop.cpp
    renderer/Renderer.cpp
)

if(RHI_BACKEND STREQUAL "metal")
    set(ENGINE_RHI_SOURCES
        rhi/metal/MetalInstance.mm
        rhi/metal/MetalDevice.mm
        rhi/metal/MetalSwapchain.mm
        rhi/metal/MetalCommandBuffer.mm
        rhi/metal/MetalPipeline.mm
        rhi/metal/MetalRenderPass.mm
        rhi/metal/MetalVertexBuffer.mm
    )
    set(ENGINE_RHI_DEFINES DRAGO_METAL)
    set(ENGINE_RHI_LIBS "-framework Metal" "-framework QuartzCore" "-framework Foundation")
else()
    set(ENGINE_RHI_SOURCES
        rhi/vulkan/VulkanInstance.cpp
        rhi/vulkan/VulkanRenderPass.cpp
        rhi/vulkan/VulkanPipeline.cpp
        rhi/vulkan/VulkanDevice.cpp
        rhi/vulkan/VulkanSwapchain.cpp
        rhi/vulkan/VulkanSurface.cpp
        rhi/vulkan/VulkanFramebuffer.cpp
        rhi/vulkan/VulkanCommandBuffer.cpp
        rhi/vulkan/VulkanVertexBuffer.cpp
        rhi/vulkan/VulkanUtils.cpp
        rhi/vulkan/VulkanIndexBuffer.cpp
    )
    set(ENGINE_RHI_DEFINES DRAGO_VULKAN VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
    set(ENGINE_RHI_LIBS Vulkan::Vulkan)
endif()

add_library(engine STATIC
    ${ENGINE_COMMON_SOURCES}
    ${ENGINE_RHI_SOURCES}
)

if(RHI_BACKEND STREQUAL "metal")
    target_compile_definitions(engine PUBLIC DRAGO_RHI_NAME="Metal")
elseif(RHI_BACKEND STREQUAL "vulkan")
    target_compile_definitions(engine PUBLIC DRAGO_RHI_NAME="Vulkan")
elseif(RHI_BACKEND STREQUAL "dx12")
    target_compile_definitions(engine PUBLIC DRAGO_RHI_NAME="DX12")
endif()

target_compile_definitions(engine PUBLIC ${ENGINE_RHI_DEFINES})
target_compile_definitions(engine PRIVATE
    ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets"
)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine PUBLIC glfw glm spdlog::spdlog ${ENGINE_RHI_LIBS})