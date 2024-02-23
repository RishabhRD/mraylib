cmake_minimum_required(VERSION 3.20)
project(mraylib CXX)
include(cmake/StandardProjectSettings.cmake)
# include(cmake/StaticAnalyzers.cmake)
include(cmake/CompilerWarnings.cmake)
include(package.cmake)

add_compile_options(-std=c++2b)
# TODO: clang-tidy creating problem with the same
# set(CMAKE_CXX_STANDARD 23)

add_library(project_options INTERFACE)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

option(ENABLE_TESTING "Enable Test Builds" ON)

if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

add_subdirectory(examples)
