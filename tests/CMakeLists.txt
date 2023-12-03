file(GLOB_RECURSE test_list "*_test.cpp")
file(GLOB_RECURSE src_list "../src/mraylib/*.cpp")
add_executable(mraylib-tests ${src_list} ${test_list} main.cpp)
target_include_directories(mraylib-tests SYSTEM PRIVATE ../lib)
target_include_directories(mraylib-tests PRIVATE ../include)
target_link_libraries(mraylib-tests PRIVATE
  project_options
  project_warnings
  doctest::doctest
)
