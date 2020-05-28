set(SOURCE_FILES
  glad.c
  init.cpp
  util.cpp
  3dmath.cpp
  ogl.cpp
  ${PROJECT_BINARY_DIR}/_deps/dear-imgui-src/imgui.cpp
  ${PROJECT_BINARY_DIR}/_deps/dear-imgui-src/imgui_draw.cpp
  ${PROJECT_BINARY_DIR}/_deps/dear-imgui-src/imgui_widgets.cpp
  ${PROJECT_BINARY_DIR}/_deps/dear-imgui-src/examples/imgui_impl_opengl3.cpp
  ${PROJECT_BINARY_DIR}/_deps/dear-imgui-src/examples/imgui_impl_glfw.cpp)

add_library(game_engine ${SOURCE_FILES})

target_include_directories(game_engine
  PRIVATE ${CMAKE_SOURCE_DIR}/inc/
  PRIVATE ${PROJECT_BINARY_DIR}/_deps/dear-imgui-src/)

if(CMAKE_CONFIGURATION_TYPES)
  add_compile_definitions($<$<CONFIG:Debug>:NDEBUG>)
else()
  if(CMAKE_BUILD_TYPE == "Debug")
    add_compile_definitions(NDEBUG)
  endif()
endif()

if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  add_compile_options(/W4 /WX)
endif()

target_link_libraries(game_engine PRIVATE glfw)
set_target_properties(game_engine PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/)
