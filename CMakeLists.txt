cmake_minimum_required(VERSION 3.10)
project(RayTracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glm REQUIRED)
find_package(Threads REQUIRED)

if (TARGET glm)
  set(GLM_LIB glm)
elseif (TARGET glm::glm)
  set(GLM_LIB glm::glm)
else ()
  message(FATAL_ERROR "Do not know how to refer glm.")
endif ()

add_subdirectory(extern)
add_subdirectory(src)
