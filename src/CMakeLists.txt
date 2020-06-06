project(game-library VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_MakeAvailable(glfw dear-imgui)

set(SOURCE_FILES
  glad.c
  init.cpp
  util.cpp
  3dmath.cpp
  ogl.cpp
  ${PROJECT_BINARY_DIR}/../_deps/dear-imgui-src/imgui.cpp
  ${PROJECT_BINARY_DIR}/../_deps/dear-imgui-src/imgui_draw.cpp
  ${PROJECT_BINARY_DIR}/../_deps/dear-imgui-src/imgui_widgets.cpp
  ${PROJECT_BINARY_DIR}/../_deps/dear-imgui-src/examples/imgui_impl_opengl3.cpp
  ${PROJECT_BINARY_DIR}/../_deps/dear-imgui-src/examples/imgui_impl_glfw.cpp)

add_library(game-library ${SOURCE_FILES})

target_include_directories(game-library
  PUBLIC ../inc/
  PRIVATE ${PROJECT_BINARY_DIR}/../_deps/dear-imgui-src/)

if(CMAKE_CONFIGURATION_TYPES)
  add_compile_definitions($<$<NOT:$<CONFIG:Debug>>:NDEBUG>)
else()
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(NDEBUG)
  endif()
endif()

if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  add_compile_options(/W4 /WX)
endif()

target_link_libraries(game-library PRIVATE glfw)
set_target_properties(game-library PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../lib/)
