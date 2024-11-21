cmake_minimum_required(VERSION 3.30)
cmake_policy(SET CMP0091 NEW)

#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
#        "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
#
#if (Clang)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fbuiltin-headers-in-system-modules -fmodules -fcxx-modules")
#endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

project(Doti LANGUAGES CXX)

# Make sure to add -DCMAKE_TOOLCHAIN_FILE=<path/to/vcpkg>/scripts/buildsystems/vcpkg.cmake when calling CMake.

find_package(Eigen3 CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
#find_package(glad CONFIG REQUIRED)
find_package(glbinding CONFIG REQUIRED)
find_package(quill CONFIG REQUIRED)
#find_package(Boost REQUIRED COMPONENTS signals2)
#find_package(spdlog CONFIG REQUIRED)

# Build Doti library
add_library(Doti STATIC "")
set_target_properties(Doti PROPERTIES OUTPUT_NAME "Doti")
set_target_properties(Doti PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/windows/x64/$<CONFIG>")

#target_include_directories(Doti SYSTEM PRIVATE
#        ${EIGEN3_INCLUDE_DIR}
#        ${GLFW3_INCLUDE_DIR}
#        ${GLBINDING_INCLUDE_DIR}
#        ${BOOST_}
#)

target_compile_definitions(Doti PRIVATE
        GLFW_INCLUDE_NONE
)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if (MSVC)
    target_compile_options(Doti PRIVATE /EHsc)
elseif (Clang)
    target_compile_options(Doti PRIVATE -fexceptions -fcxx-exceptions -Wall -Wextra -Wpedantic)
elseif (Gcc)
    target_compile_options(Doti PRIVATE -fexceptions)
endif ()

set_target_properties(Doti PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(Doti PRIVATE cxx_std_23)

if (MSVC)
    target_compile_options(Doti PRIVATE $<$<CONFIG:Debug>:-Od> $<$<CONFIG:Release>:-O2>)
    target_compile_options(Doti PRIVATE $<$<CONFIG:Debug>:-Zi> $<$<CONFIG:Release>:-DNDEBUG>)
else ()
    target_compile_options(Doti PRIVATE $<$<CONFIG:Debug>:-O0> $<$<CONFIG:Release>:-O3>)
    target_compile_options(Doti PRIVATE $<$<CONFIG:Debug>:-g> $<$<CONFIG:Release>:-DNDEBUG>)
endif ()

if (MSVC)
    set_property(TARGET Doti PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

target_sources(Doti
        PUBLIC
        FILE_SET CXX_MODULES FILES
        doti/Utils/utils.cppm
        doti/Utils/ray.cppm
        doti/Math/math.cppm
        doti/Utils/color.cppm
        doti/Utils/Event/event.cppm
        doti/Window/window.cppm
        doti/Debug/logger.cppm
        doti/Graphics/Shader/shader.cppm
        doti/Graphics/Render/Drawable/drawable.cppm
        doti/Graphics/Render/Drawable/mesh.cppm
        doti/Graphics/Render/Drawable/triangle.cppm
        doti/Graphics/Render/Model/model.cppm
        wrapper/test.cppm
        wrapper/eigen.cppm
        wrapper/glfw.cppm
        wrapper/gl.cppm
        wrapper/std.mock.cppm
        wrapper/assimp.cppm
        #        wrapper/boost-signals2.cppm
)

target_link_libraries(Doti PRIVATE
        glfw
        #        Boost::signals2
        glbinding::glbinding
        Eigen3::Eigen
        opengl32
        user32
        shell32
        gdi32
)

# Build Editor
add_executable(Editor "")
set_target_properties(Editor PROPERTIES OUTPUT_NAME "Editor")
set_target_properties(Editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/windows/x64/$<CONFIG>")

if (MSVC)
    target_compile_options(Editor PRIVATE /EHsc)
elseif (Clang)
    target_compile_options(Doti PRIVATE -fexceptions -fcxx-exceptions -Wall -Wextra -Wpedantic)
elseif (Gcc)
    target_compile_options(Editor PRIVATE -fexceptions)
endif ()

set_target_properties(Editor PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(Editor PRIVATE cxx_std_23)

if (MSVC)
    target_compile_options(Editor PRIVATE $<$<CONFIG:Debug>:-Od> $<$<CONFIG:Release>:-O2>)
    target_compile_options(Editor PRIVATE $<$<CONFIG:Debug>:-Zi> $<$<CONFIG:Release>:-DNDEBUG>)
else ()
    target_compile_options(Editor PRIVATE $<$<CONFIG:Debug>:-O0> $<$<CONFIG:Release>:-O3>)
    target_compile_options(Editor PRIVATE $<$<CONFIG:Debug>:-g> $<$<CONFIG:Release>:-DNDEBUG>)
endif ()

if (MSVC)
    set_property(TARGET Editor PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

target_sources(Editor PRIVATE
        editor/main.cpp
)

target_link_libraries(Editor PRIVATE Doti)

# Build Demo
add_executable(Demo "")
set_target_properties(Demo PROPERTIES OUTPUT_NAME "Demo")
set_target_properties(Demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/windows/x64/$<CONFIG>")

if (MSVC)
    target_compile_options(Demo PRIVATE /EHsc)
elseif (Clang)
    target_compile_options(Doti PRIVATE -fexceptions -fcxx-exceptions -Wall -Wextra -Wpedantic)
elseif (Gcc)
    target_compile_options(Demo PRIVATE -fexceptions)
endif ()

set_target_properties(Demo PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(Demo PRIVATE cxx_std_23)

if (MSVC)
    target_compile_options(Demo PRIVATE $<$<CONFIG:Debug>:-Od> $<$<CONFIG:Release>:-O2>)
    target_compile_options(Demo PRIVATE $<$<CONFIG:Debug>:-Zi> $<$<CONFIG:Release>:-DNDEBUG>)
else ()
    target_compile_options(Demo PRIVATE $<$<CONFIG:Debug>:-O0> $<$<CONFIG:Release>:-O3>)
    target_compile_options(Demo PRIVATE $<$<CONFIG:Debug>:-g> $<$<CONFIG:Release>:-DNDEBUG>)
endif ()

if (MSVC)
    set_property(TARGET Demo PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

target_sources(Demo PRIVATE
        demo/main.cpp
)

target_link_libraries(Demo PRIVATE Doti)