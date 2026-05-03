add_executable(drago
    main.cpp
)

target_link_libraries(drago PRIVATE engine glfw spdlog::spdlog)