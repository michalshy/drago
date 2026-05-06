cmake_minimum_required(VERSION 3.28)
project(drago VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(cmake/FetchDependencies.cmake)

add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(tests)

include(${CMAKE_SOURCE_DIR}/cmake/Shaders.cmake)
add_shader_targets(shaders
    SOURCES 
        ${CMAKE_SOURCE_DIR}/assets/shaders/base.slang
    ENTRIES vertMain fragMain
)
add_dependencies(drago shaders)