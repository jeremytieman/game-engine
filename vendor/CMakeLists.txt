# Download and unpack Dear ImGui at configure time
configure_file(DearImGui/CMakeLists.txt.in dear-imgui-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dear-imgui-download )
if(result)
  message(FATAL_ERROR "CMake step for DearImGui failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dear-imgui-download )
if(result)
  message(FATAL_ERROR "Build step for DearImGui failed: ${result}")
endif()

# Download and unpack glfw at configure time
configure_file(GLFW/CMakeLists.txt.in glfw-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/glfw-download )
if(result)
  message(FATAL_ERROR "CMake step for glfw failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/glfw-download )
if(result)
  message(FATAL_ERROR "Build step for glfw failed: ${result}")
endif()

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glfw-src
                 ${CMAKE_CURRENT_BINARY_DIR}/glfw-build
                 EXCLUDE_FROM_ALL)
