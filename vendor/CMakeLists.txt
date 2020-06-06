include(FetchContent)

FetchContent_Declare(
  dear-imgui
  GIT_REPOSITORY https://github.com/jeremytieman/imgui.git
  GIT_TAG docking
)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/jeremytieman/glfw.git
  GIT_TAG master
)

FetchContent_Declare(
  imgui-file-browser
  GIT_REPOSITORY https://github.com/jeremytieman/ImGui-Addons.git
  GIT_TAG imgui-v1.77-fix
)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/jeremytieman/stb.git
  GIT_TAG master
)

FetchContent_Declare(
  imgui-memory-editor
  GIT_REPOSITORY https://github.com/jeremytieman/imgui_club.git
  GIT_TAG master
)

FetchContent_Declare(
  chip-emulators
  GIT_REPOSITORY https://github.com/jeremytieman/chip-emulators.git
  GIT_TAG master
)
