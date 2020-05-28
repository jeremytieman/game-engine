set(SOURCE_FILES
  main.cpp)
add_executable(basic ${SOURCE_FILES})

target_include_directories(basic
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

target_link_libraries(basic PRIVATE glfw PRIVATE game_engine)
set_target_properties(basic PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/examples)

add_custom_command(TARGET basic POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/examples/basic/shaders $<TARGET_FILE_DIR:basic>/shaders)
