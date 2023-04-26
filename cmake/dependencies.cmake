# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

set(CPM_USE_LOCAL_PACKAGES)
include(cmake/CPM.cmake)

CPMAddPackage(
    GIT_TAG 9.1.0
    GITHUB_REPOSITORY fmtlib/fmt
)

CPMAddPackage(
    VERSION 1.2.1
    GITHUB_REPOSITORY stephenberry/glaze
)

CPMAddPackage(
    VERSION 1.3.246
    GITHUB_REPOSITORY khronosgroup/vulkan-hpp
)

# Temp solution for broken system 1.3.246 verson. Once driver is updated it can be removed
file(GLOB_RECURSE vulkan-headers "${vulkan-hpp_SOURCE_DIR}/vulkan/*.hpp")
add_library(vulkan-hpp INTERFACE EXCLUDE_FROM_ALL ${vulkan-headers})
target_include_directories(vulkan-hpp INTERFACE $<BUILD_INTERFACE:${vulkan-hpp_SOURCE_DIR}>)

CPMAddPackage(
    VERSION 3.3.9
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3fa2360720eeba1964df3c0ecf4b5df8648a8e52
    OPTIONS
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BULID_DOCS OFF"
        "GLFW_BUILD_WAYLAND ON"
        "GLFW_BUILD_X11 OFF"
)

CPMAddPackage(
    VERSION 1.11.0
    GITHUB_REPOSITORY gabime/spdlog
    OPTIONS "SPDLOG_FMT_EXTERNAL 1"
)

CPMAddPackage(
    VERSION 1.9.4
    GITHUB_REPOSITORY lz4/lz4
    DOWNLOAD_ONLY
)

add_library(lz4 STATIC ${lz4_SOURCE_DIR}/lib/lz4.c)
target_include_directories(lz4 PUBLIC ${lz4_SOURCE_DIR}/lib)