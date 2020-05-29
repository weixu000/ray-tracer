file(GLOB_RECURSE RAYTRACER_SOURCES CONFIGURE_DEPENDS *.cpp *.hpp *.inc)

find_package(Threads REQUIRED)

add_executable(raytracer ${RAYTRACER_SOURCES})
target_link_libraries(raytracer PRIVATE glm stb_image_write Threads::Threads embree tinyobjloader)
target_compile_definitions(raytracer PRIVATE MULTITHREADING)