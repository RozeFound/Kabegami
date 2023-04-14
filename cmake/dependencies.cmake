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