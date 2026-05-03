include(FetchContent)

# GLFW
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# spdlog
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG        v1.14.1
)
FetchContent_MakeAvailable(spdlog)

# Catch2
FetchContent_Declare(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG        v3.6.0
)
FetchContent_MakeAvailable(Catch2)

# Vulkan — systemic
find_package(Vulkan REQUIRED)