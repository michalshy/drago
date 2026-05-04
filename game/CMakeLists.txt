add_executable(drago
main.cpp
)
target_compile_definitions(engine PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
target_include_directories(drago PRIVATE ${CMAKE_SOURCE_DIR}/engine)
target_link_libraries(drago PRIVATE engine glfw spdlog::spdlog)