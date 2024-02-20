# this file contains a list of tools that can be activated and downloaded on-demand each tool is
# enabled during configuration by passing an additional `-DUSE_<TOOL>=<VALUE>` argument to CMake

# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

# enables CCACHE support through the USE_CCACHE flag possible values are: YES, NO or equivalent
if(USE_CCACHE)
  CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.5")
endif()

function(CPMAddHeaderOnly)

  set(oneValueArgs
    NAME
    VERSION
    GIT_TAG
    GITHUB_REPOSITORY
    HEADER
  )

  cmake_parse_arguments(ARGS "" "${oneValueArgs}" "" "${ARGN}")

  CPMAddPackage(
    NAME "_${ARGS_NAME}"
    VERSION "${ARGS_VERSION}"
    GITHUB_REPOSITORY "${ARGS_GITHUB_REPOSITORY}"
    GIT_TAG "${ARGS_GIT_TAG}"
    DOWNLOAD_ONLY
  )

  add_library(${ARGS_NAME} INTERFACE EXCLUDE_FROM_ALL ${_${ARGS_NAME}_SOURCE_DIR}/${ARGS_HEADER})
  target_include_directories(${ARGS_NAME} INTERFACE $<BUILD_INTERFACE:${_${ARGS_NAME}_SOURCE_DIR}>)

endfunction(CPMAddHeaderOnly)

