project(editor VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_MakeAvailable(glfw dear-imgui imgui-file-browser stb imgui-memory-editor chip-emulators)

set(SOURCE_FILES
  ${PROJECT_BINARY_DIR}/../_deps/imgui-file-browser-src/FileBrowser/ImGuiFileBrowser.cpp
  main.cpp)
add_executable(editor ${SOURCE_FILES})

target_include_directories(editor
  PRIVATE ${PROJECT_BINARY_DIR}/../_deps/dear-imgui-src/
    ${PROJECT_BINARY_DIR}/../_deps/imgui-file-browser-src/FileBrowser/
    ${PROJECT_BINARY_DIR}/../_deps/stb-src/
    ${PROJECT_BINARY_DIR}/../_deps/imgui-memory-editor-src/)

if(CMAKE_CONFIGURATION_TYPES)
  add_compile_definitions($<$<NOT:$<CONFIG:Debug>>:NDEBUG>)
else()
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(NDEBUG)
  endif()
endif()

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

target_link_libraries(editor PRIVATE glfw game-library ChipEmulators)
set_target_properties(editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../bin)
