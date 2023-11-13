file(GLOB_RECURSE src_list "**.cpp")
add_executable(raylib ${src_list})
target_include_directories(raylib PRIVATE ../include)
target_link_libraries(raylib PRIVATE project_options project_warnings)
