cmake_minimum_required(VERSION 3.20)
project(mraylib CXX)
include(cmake/StandardProjectSettings.cmake)
# include(cmake/StaticAnalyzers.cmake)
include(cmake/CompilerWarnings.cmake)

add_compile_options(-std=c++2b)
# TODO: clang-tidy creating problem with the same
# set(CMAKE_CXX_STANDARD 23)

# CPM
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Packages
CPMAddPackage(
  NAME doctest
  GITHUB_REPOSITORY doctest/doctest
  GIT_TAG v2.4.11
)
CPMAddPackage("gh:NVIDIA/stdexec#main")

add_library(project_options INTERFACE)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

option(ENABLE_TESTING "Enable Test Builds" ON)

if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

add_subdirectory(src)
