file(GLOB_RECURSE RAYTRACER_HEADERS CONFIGURE_DEPENDS ../include/raytracer/*.hpp)
file(GLOB_RECURSE RAYTRACER_SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(raytracer ${RAYTRACER_SOURCES} ${RAYTRACER_HEADERS})
target_include_directories(raytracer PRIVATE ../include)
target_link_libraries(raytracer PRIVATE glm stb_image_write)