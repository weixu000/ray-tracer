include(FetchContent)
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0.9.9.8)
FetchContent_Populate(glm)
add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})