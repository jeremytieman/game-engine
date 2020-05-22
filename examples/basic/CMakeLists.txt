set(SOURCE_FILES
  main.cpp)
add_executable(basic ${SOURCE_FILES})

target_include_directories(basic
  PRIVATE ${CMAKE_SOURCE_DIR}/inc/
  PRIVATE ${PROJECT_BINARY_DIR}/vendor/glfw-src/include/
  PRIVATE ${PROJECT_BINARY_DIR}/vendor/dear-imgui-src/)

target_link_libraries(basic PRIVATE glfw PRIVATE game_engine)
set_target_properties(basic PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/examples)

add_custom_command(TARGET basic POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/examples/basic/shaders $<TARGET_FILE_DIR:basic>/shaders)
