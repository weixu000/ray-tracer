cmake_minimum_required(VERSION 3.10)
project(RayTracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB EXTERN_LIBS LIST_DIRECTORIES true CONFIGURE_DEPENDS extern/*)
foreach (EXTERN_LIB ${EXTERN_LIBS})
    if (IS_DIRECTORY "${EXTERN_LIB}")
        add_subdirectory("${EXTERN_LIB}")
    else ()
        include("${EXTERN_LIB}")
    endif ()
endforeach ()

add_subdirectory(src)
