cmake_minimum_required(VERSION 3.0)
project(game-engine VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(vendor)
add_subdirectory(src)

option(GAME_ENGINE_BUILD_EDITOR "Build the game engine editor" ON)

if(GAME_ENGINE_BUILD_EDITOR)
  add_subdirectory(editor)
endif()

#add_subdirectory(examples)
