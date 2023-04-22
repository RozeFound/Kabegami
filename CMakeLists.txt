cmake_minimum_required(VERSION 3.19...3.22)

# ---- Project ----

project(
  Kabegami
  VERSION 0.0.1
  LANGUAGES CXX
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

# Enable precompiled headers
target_precompile_headers(${PROJECT_NAME} PRIVATE "source/logging.hpp")

# ---- Enable debug info ----

target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:DEBUG>")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/settings.json DESTINATION ${PROJECT_BINARY_DIR})

# ---- Link libraries ----

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt glaze::glaze)
target_link_libraries(${PROJECT_NAME} PRIVATE lz4 glfw spdlog)