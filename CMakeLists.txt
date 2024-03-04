cmake_minimum_required(VERSION 3.19...3.22)

# ---- Project ----

project(
  Kabegami
  VERSION 0.0.1
  LANGUAGES C CXX
)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Import tools ----

set(USE_CCACHE YES)
include(cmake/tools.cmake)

# ---- Import dependencies ----

include(cmake/dependencies.cmake)

# ---- Add source files ----

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

# ---- Create executable ----

add_executable(${PROJECT_NAME} ${sources})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)

set_property(TARGET ${PROJECT_NAME} PROPERTY "$<$<CONFIG:RELEASE>:INTERPROCEDURAL_OPTIMIZATION>" TRUE)

# Enable precompiled headers
target_compile_definitions(${PROJECT_NAME} PRIVATE "VULKAN_HPP_NO_CONSTRUCTORS" "VULKAN_HPP_NO_TO_STRING")
target_precompile_headers(${PROJECT_NAME} PRIVATE <vulkan/vulkan_raii.hpp> "source/logging.hpp" <glm/glm.hpp> <GLFW/glfw3.h>)

# ---- Enable debug info ----

target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

# ---- Copy over resources ----

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/settings.json DESTINATION ${PROJECT_BINARY_DIR})

file(GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS "resources/*")

foreach(FILE ${RESOURCE_FILES})

  file(RELATIVE_PATH REL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/resources ${FILE})

  cmake_path(GET REL_PATH PARENT_PATH REL_DIRS)

  cmake_path(GET FILE FILENAME NAME)
  cmake_path(GET FILE EXTENSION EXT)

  set(OUT_FILE ${PROJECT_BINARY_DIR}/${REL_DIRS}/${NAME})
  configure_file(${FILE} ${OUT_FILE} COPYONLY)

endforeach()


# ---- Link libraries ----

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt spdlog glaze::glaze)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm::glm vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE ${glslang-link-targets})
target_link_libraries(${PROJECT_NAME} PRIVATE lz4 stb xxHash::xxhash)