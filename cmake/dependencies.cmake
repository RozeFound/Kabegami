# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

# Read the dependencies file
file(READ "dependencies.json" json_content)
string(JSON library_list LENGTH ${json_content})
math(EXPR library_list "${library_list} - 1")

foreach(IDX RANGE ${library_list})
    string(JSON library_name GET ${json_content} ${IDX} name)
    string(JSON library_tag GET ${json_content} ${IDX} tag)
    string(JSON library_repo GET ${json_content} ${IDX} github)
    string(JSON library_commit GET ${json_content} ${IDX} commit)

    set(${library_name}_repo ${library_repo})
    set(${library_name}_tag ${library_tag})
    set(${library_name}_commit ${library_commit})
endforeach()

include(cmake/CPM.cmake)

CPMAddPackage(
    GIT_TAG ${fmt_tag}
    GITHUB_REPOSITORY ${fmt_repo}
)

CPMAddPackage(
    GIT_TAG ${spdlog_tag}
    GITHUB_REPOSITORY ${spdlog_repo}
    OPTIONS "SPDLOG_FMT_EXTERNAL 1"
)

CPMAddPackage(
    GIT_TAG ${glaze_tag}
    GITHUB_REPOSITORY ${glaze_repo}
)

CPMAddPackage(
    GIT_TAG ${glfw_tag}
    GITHUB_REPOSITORY ${glfw_repo}
    OPTIONS
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BULID_DOCS OFF"
)

CPMAddPackage(
    GIT_TAG ${glm_tag}
    GITHUB_REPOSITORY ${glm_repo}
)

CPMAddPackage(
    GIT_TAG ${lz4_tag}
    GITHUB_REPOSITORY ${lz4_repo}
    DOWNLOAD_ONLY
)

add_library(lz4 STATIC ${lz4_SOURCE_DIR}/lib/lz4.c)
target_include_directories(lz4 PUBLIC ${lz4_SOURCE_DIR}/lib)

CPMAddHeaderOnly(
    NAME stb
    GITHUB_REPOSITORY ${stb_repo}
    GIT_TAG ${stb_commit}
    HEADER "stb_image.h"
)
#target_compile_definitions(stb INTERFACE "STB_IMAGE_IMPLEMENTATION")

CPMAddPackage(
  GITHUB_REPOSITORY ${SPIRV-Headers_repo}
  GIT_TAG ${SPIRV-Headers_commit}
  DOWNLOAD_ONLY YES
)

CPMAddPackage(
  GITHUB_REPOSITORY ${SPIRV-Tools_repo}
  GIT_TAG ${SPIRV-Tools_commit}
  OPTIONS
    "SPIRV_SKIP_TESTS ON"
    "SPIRV_SKIP_EXECUTABLES ON"
)

CPMAddPackage(
  GITHUB_REPOSITORY ${glslang_repo}
  GIT_TAG ${glslang_commit}
  OPTIONS
    "GLSLANG_TESTS OFF"
    "ENABLE_GLSLANG_BINARIES OFF"
    "BUILD_EXTERNAL OFF"
)

set(glslang-link-targets 
    glslang
    SPIRV
    glslang-default-resource-limits)

CPMAddPackage(
    GITHUB_REPOSITORY ${xxHash_repo}
    GIT_TAG ${xxHash_commit}
    OPTIONS 
        "XXHASH_BUILD_ENABLE_INLINE_API OFF"
        "XXHASH_BUILD_XXHSUM OFF"
    SOURCE_SUBDIR cmake_unofficial
)

