# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

set(CPM_USE_LOCAL_PACKAGES)
include(cmake/CPM.cmake)

CPMAddPackage(
    GIT_TAG 10.2.1
    GITHUB_REPOSITORY fmtlib/fmt
)

CPMAddPackage(
    VERSION 2.1.4
    GITHUB_REPOSITORY stephenberry/glaze
)

CPMAddPackage(
    VERSION 3.4
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 6f1ddf51a130f2dee6ade5fa4d8217e4071124e8
    OPTIONS
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BULID_DOCS OFF"
)

CPMAddPackage(
    VERSION 1.0.0
    GIT_TAG adf31f555e73e2bd6fda373bb5d8740f9c6c17c0
    GITHUB_REPOSITORY g-truc/glm
)

CPMAddPackage(
    VERSION 1.13.0
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
    GIT_TAG ae721c50eaf761660b4f90cc590453cdb0c2acd0
    HEADER "stb_image.h"
)
target_compile_definitions(stb INTERFACE "STB_IMAGE_IMPLEMENTATION")