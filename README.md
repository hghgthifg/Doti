# Doti

A simple game engine for learning purposes, also final project for the course "Computer Program Design" at University of
Science and Technology of China.

## Build

Environment:

- CMake 3.30 or later
- Clang 19 or later
- Ninja 1.12.1 or later
- Vcpkg

(This project uses C++23 features, so the requirements may be higher than usual)

Besides, you need to install the following packages via vcpkg:

- Eigen3
- glfw3
- glbinding
- imgui
- Freetype
- libjxl

Then you can build the project using CMake:

```shell
mkdir build
cd build
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake ..
```