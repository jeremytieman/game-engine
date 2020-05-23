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

FetchContent_MakeAvailable(glfw dear-imgui imgui-file-browser)
