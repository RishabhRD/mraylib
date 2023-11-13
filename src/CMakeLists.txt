file(GLOB_RECURSE src_list "**.cpp")
add_executable(mraylib ${src_list})
target_include_directories(mraylib PRIVATE ../include)
target_link_libraries(mraylib PRIVATE project_options project_warnings)
