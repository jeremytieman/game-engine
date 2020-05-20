set(SOURCE_FILES
  glad.c
  init.cpp
  util.cpp
  3dmath.cpp
  ogl.cpp
  ${PROJECT_BINARY_DIR}/external/dear-imgui-src/imgui.cpp
  ${PROJECT_BINARY_DIR}/external/dear-imgui-src/imgui_draw.cpp
  ${PROJECT_BINARY_DIR}/external/dear-imgui-src/imgui_widgets.cpp
  ${PROJECT_BINARY_DIR}/external/dear-imgui-src/examples/imgui_impl_opengl3.cpp
  ${PROJECT_BINARY_DIR}/external/dear-imgui-src/examples/imgui_impl_glfw.cpp)

add_library(game_engine ${SOURCE_FILES})

target_include_directories(game_engine
  PRIVATE ${CMAKE_SOURCE_DIR}/inc/
  PRIVATE ${PROJECT_BINARY_DIR}/external/glfw-src/include/
  PRIVATE ${PROJECT_BINARY_DIR}/external/dear-imgui-src/)

target_link_libraries(game_engine PRIVATE glfw)
set_target_properties(game_engine PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/)

#add_custom_command(TARGET Demo POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E copy_directory
#  ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:Demo>/shaders)
