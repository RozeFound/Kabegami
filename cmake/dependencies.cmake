# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

set(CPM_USE_LOCAL_PACKAGES)
include(cmake/CPM.cmake)

CPMAddPackage(
    GIT_TAG 9.1.0
    GITHUB_REPOSITORY fmtlib/fmt
)

CPMAddPackage(
    VERSION 1.2.2
    GITHUB_REPOSITORY stephenberry/glaze
)

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
    GIT_TAG 0.9.9.8
    GITHUB_REPOSITORY g-truc/glm
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

CPMAddHeaderOnly(
    NAME stb
    VERSION stable
    GITHUB_REPOSITORY nothings/stb
    GIT_TAG 3ecc60f25ae1391cf6434578ece782afa1458b56
    HEADER "stb_image.h"
)
target_compile_definitions(stb INTERFACE "STB_IMAGE_IMPLEMENTATION")