include(FetchContent)

FetchContent_Declare(
  dear-imgui
  GIT_REPOSITORY https://github.com/jeremytieman/imgui.git
  GIT_TAG docking
  PREFIX vendor
)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/jeremytieman/glfw.git
  GIT_TAG master
  PREFIX vendor
)

FetchContent_MakeAvailable(glfw dear-imgui)
