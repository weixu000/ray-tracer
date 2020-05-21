cmake_minimum_required(VERSION 3.10)
project(RayTracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(embree 3.0 REQUIRED)

include(FetchContent)
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0.9.9.8)
FetchContent_Populate(glm)
add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})

add_subdirectory(extern)
add_subdirectory(src)
