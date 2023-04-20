# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

set(CPM_USE_LOCAL_PACKAGES)
include(cmake/CPM.cmake)

CPMAddPackage(
    NAME fmt
    GIT_TAG 9.1.0
    GITHUB_REPOSITORY fmtlib/fmt
)

CPMAddPackage(
    NAME glaze
    VERSION 1.2.0
    GITHUB_REPOSITORY stephenberry/glaze
)

CPMAddPackage(
    NAME lz4
    VERSION 1.9.4
    GITHUB_REPOSITORY lz4/lz4
    DOWNLOAD_ONLY
)

if (lz4_ADDED)
    add_subdirectory("${lz4_SOURCE_DIR}/build/cmake" EXCLUDE_FROM_ALL)
    set(LZ4_BUILD_CLI OFF)
    set(LZ4_BUILD_LEGACY_LZ4C OFF)
endif()