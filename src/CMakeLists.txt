file(GLOB_RECURSE RAYTRACER_HEADERS CONFIGURE_DEPENDS ../include/RayTracer/*.h)
file(GLOB_RECURSE RAYTRACER_SOURCES CONFIGURE_DEPENDS *.cpp)

add_library(raytracer ${RAYTRACER_SOURCES} ${RAYTRACER_HEADERS})
target_include_directories(raytracer PUBLIC ../include)
target_link_libraries(raytracer PUBLIC glm)