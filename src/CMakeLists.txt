file(GLOB_RECURSE RAYTRACER_SOURCES CONFIGURE_DEPENDS *.cpp *.hpp *.inc)

add_executable(raytracer ${RAYTRACER_SOURCES})
target_link_libraries(raytracer PRIVATE ${GLM_LIB} stb_image_write Threads::Threads)
target_compile_definitions(raytracer PRIVATE MULTITHREADING)