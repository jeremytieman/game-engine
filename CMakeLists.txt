cmake_minimum_required (VERSION 2.6)
project (OpenGLDemo LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)

add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(editor)
add_subdirectory(examples)