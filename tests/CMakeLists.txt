file(GLOB_RECURSE test_list "*_test.cpp")
file(GLOB_RECURSE src_list "../src/raylib/*.cpp")
add_executable(raylib-tests ${src_list} ${test_list} main.cpp)
target_include_directories(raylib-tests PRIVATE ../include)
target_link_libraries(raylib-tests PRIVATE
  project_options
  project_warnings
  doctest::doctest
)
