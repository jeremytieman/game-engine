set(SOURCE_FILES
  ${PROJECT_BINARY_DIR}/_deps/imgui-file-browser-src/FileBrowser/ImGuiFileBrowser.cpp
  main.cpp)
add_executable(editor ${SOURCE_FILES})

target_include_directories(editor
  PRIVATE ${CMAKE_SOURCE_DIR}/inc/
  PRIVATE ${PROJECT_BINARY_DIR}/_deps/dear-imgui-src/
  PRIVATE ${PROJECT_BINARY_DIR}/_deps/imgui-file-browser-src/FileBrowser/
  PRIVATE ${PROJECT_BINARY_DIR}/_deps/stb-src/)

if (MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  add_compile_options(/W4 /WX)
endif()

add_compile_definitions(STB_IMAGE_IMPLEMENTATION)
add_compile_definitions(STBI_ONLY_JPEG)
add_compile_definitions(STBI_ONLY_PNG)
add_compile_definitions(STBI_ONLY_BMP)
add_compile_definitions(STBI_ONLY_TGA)
add_compile_definitions(STBI_ONLY_GIF)

target_link_libraries(editor PRIVATE glfw PRIVATE game_engine)
set_target_properties(editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
