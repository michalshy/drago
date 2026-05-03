cmake_minimum_required(VERSION 4.3)
project(drago VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/FetchDependencies.cmake)

add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(tests)