cmake_minimum_required(VERSION 3.0)

project(gogame)

set(_src_root_path "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_root_path}/*.c*"
    "${_src_root_path}/*.h*"
)

add_executable(gogame ${_source_list} ${_header_list})

make_group_path(${_src_root_path} "${_source_list}")

include_directories(gogame src)

file(
    GLOB_RECURSE _font_list 
    LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/font/*.ttf"
)

# add files to project binary dir, as VS uses it as debug working dir
file(COPY ${_font_list} DESTINATION "${PROJECT_BINARY_DIR}/font")

# add files to output binary dir (at build time)
add_custom_command(
    TARGET gogame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/font"
    COMMAND ${CMAKE_COMMAND} -E copy ${_font_list} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/font"
    )

target_link_libraries(gogame glfw nanovg glew ${OPENGL_gl_LIBRARY})