# this file contains a list of tools that can be activated and downloaded on-demand each tool is
# enabled during configuration by passing an additional `-DUSE_<TOOL>=<VALUE>` argument to CMake

# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

# enables sanitizers support using the the `USE_SANITIZER` flag available values are: Address,
# Memory, MemoryWithOrigins, Undefined, Thread, Leak, 'Address;Undefined'
if(USE_SANITIZER OR USE_STATIC_ANALYZER)
  CPMAddPackage("gh:StableCoder/cmake-scripts#1f822d1fc87c8d7720c074cde8a278b44963c354")

  if(USE_SANITIZER)
    include(${cmake-scripts_SOURCE_DIR}/sanitizers.cmake)
  endif()

  if(USE_STATIC_ANALYZER)
    if("clang-tidy" IN_LIST USE_STATIC_ANALYZER)
      set(CLANG_TIDY
          ON
          CACHE INTERNAL ""
      )
    else()
      set(CLANG_TIDY
          OFF
          CACHE INTERNAL ""
      )
    endif()
    if("iwyu" IN_LIST USE_STATIC_ANALYZER)
      set(IWYU
          ON
          CACHE INTERNAL ""
      )
    else()
      set(IWYU
          OFF
          CACHE INTERNAL ""
      )
    endif()
    if("cppcheck" IN_LIST USE_STATIC_ANALYZER)
      set(CPPCHECK
          ON
          CACHE INTERNAL ""
      )
    else()
      set(CPPCHECK
          OFF
          CACHE INTERNAL ""
      )
    endif()

    include(${cmake-scripts_SOURCE_DIR}/tools.cmake)

    clang_tidy(${CLANG_TIDY_ARGS})
    include_what_you_use(${IWYU_ARGS})
    cppcheck(${CPPCHECK_ARGS})
  endif()
endif()

# enables CCACHE support through the USE_CCACHE flag possible values are: YES, NO or equivalent
if(USE_CCACHE)
  CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.3")
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

