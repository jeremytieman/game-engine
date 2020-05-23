set(SOURCE_FILES
  ${PROJECT_BINARY_DIR}/_deps/imgui-file-browser-src/FileBrowser/ImGuiFileBrowser.cpp
  main.cpp)
add_executable(editor ${SOURCE_FILES})

target_include_directories(editor
  PRIVATE ${CMAKE_SOURCE_DIR}/inc/
  PRIVATE ${PROJECT_BINARY_DIR}/_deps/dear-imgui-src/
  PRIVATE ${PROJECT_BINARY_DIR}/_deps/imgui-file-browser-src/FileBrowser/)

target_link_libraries(editor PRIVATE glfw PRIVATE game_engine)
set_target_properties(editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
