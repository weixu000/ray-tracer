include(FetchContent)
FetchContent_Declare(tinyobjloader
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader
        GIT_TAG v2.0-rc1)
FetchContent_Populate(tinyobjloader)
add_subdirectory(${tinyobjloader_SOURCE_DIR} ${tinyobjloader_BINARY_DIR})
