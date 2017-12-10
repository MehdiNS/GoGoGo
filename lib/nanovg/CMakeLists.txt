cmake_minimum_required(VERSION 3.0)

project(libnanovg)

set(_src_root_path "${CMAKE_CURRENT_SOURCE_DIR}")

file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_root_path}/*.c*"
    "${_src_root_path}/*.h*"
)

add_library(nanovg ${_source_list})

make_group_path(${_src_root_path} "${_source_list}")