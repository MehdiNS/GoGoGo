cmake_minimum_required(VERSION 3.0)

set(_extension_filters "cpp\\.cpp" "cpp\\.c" "h\\.h")

function(make_group_path _src_root _sources)
    foreach(_source IN ITEMS ${_sources})
        get_filename_component(_source_path ${_source} PATH)
        file(RELATIVE_PATH _source_path_rel ${_src_root} ${_source_path})
        string(REPLACE "/" "\\" _group_dir "${_source_path_rel}")
        
        get_filename_component(_extension ${_source} EXT)
        foreach(_filter IN ITEMS ${_extension_filters})
            get_filename_component(_filtered_ext ${_filter} EXT)
            if(_extension STREQUAL _filtered_ext)
                get_filename_component(_filtered_dir ${_filter} DIRECTORY)
                set(_group_path "${_group_dir}\\${_filtered_dir}")
            endif()
        endforeach()
        
        source_group("${_group_path}" FILES "${_source}")
    endforeach()
endfunction(make_group_path)

function(setTargetFolder _sub_directory _target_folder)
    get_property(_targets DIRECTORY ${_sub_directory} PROPERTY BUILDSYSTEM_TARGETS)
    foreach(_target IN ITEMS ${_targets})
        set_target_properties(${_target} PROPERTIES FOLDER ${_target_folder})
        #message(STATUS "Adding target ${_target} to ${_target_folder}")
    endforeach()
endfunction(setTargetFolder)

project(gogame)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIGURATION>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>)

set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

# Dependencies
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Enable GLFW Examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Enable GLFW Tests" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Enable GLFW Doc" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Enable GLFW Install" FORCE)
set(BUILD_UTILS OFF CACHE BOOL "glew utilities" FORCE)

add_subdirectory(lib/glfw-3.2.1)
include_directories(lib/glfw-3.2.1/include)
setTargetFolder("lib/glfw-3.2.1" "lib/glfw")
setTargetFolder("lib/glfw-3.2.1/src" "lib/glfw")
#setTargetFolder("lib/glfw-3.2.1/tests" "lib/glfw/tests")

add_subdirectory(lib/glew-2.1.0/build/cmake)
include_directories(lib/glew-2.1.0/include)
setTargetFolder("lib/glew-2.1.0/build/cmake" "lib/glew")

add_subdirectory(lib/nanovg)
include_directories(lib/nanovg)
setTargetFolder("lib/nanovg" "lib/nanovg")
            
find_package(OpenGL REQUIRED)

add_subdirectory(gogame)
