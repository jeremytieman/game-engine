# Overview

This is a combination game engine and emulator. The initial version will use OpenGL 4, GLFW, and Dear ImGui targeting a CHIP-8 emulator. It will also combine elements of a disassembler to support emulation.

# Build

Following CMake convention this is built out of source. On all platforms, run this command:

`mkdir build && cd build`

In order to generate the debug version of the project, run this command on *nix:

`cmake -DCMAKE_BUILD_TYPE=Debug ..`

For the release version on *nix, run this command:

`cmake -DCMAKE_BUILD_TYPE=Release ..`

On Windows for both versions, run this command:

`cmake ..`

In order to actually build the project, run this on *nix:

`cmake --build .`

On Windows, for the debug build, run this command:

`cmake --build . --config Debug`

On Windows, for the release build, run this command:

`cmake --build . --config Release`
