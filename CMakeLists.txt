cmake_minimum_required(VERSION 3.30)
cmake_policy(SET CMP0091 NEW)

#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
#        "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
#
#if (Clang)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
#endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

project(Doti LANGUAGES CXX)

# Make sure to add -DCMAKE_TOOLCHAIN_FILE=<path/to/vcpkg>/scripts/buildsystems/vcpkg.cmake when calling CMake.

#find_package(OpenMP)
find_package(assimp CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
#find_package(glad CONFIG REQUIRED)
find_package(glbinding CONFIG REQUIRED)
#find_package(quill CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Freetype CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(PkgConfig)
pkg_check_modules(Jxl REQUIRED IMPORTED_TARGET libjxl libjxl_cms libjxl_threads)
#find_package(Boost REQUIRED COMPONENTS signals2)
#find_package(spdlog CONFIG REQUIRED)

# Build Doti library
add_library(Doti STATIC "")
set_target_properties(Doti PROPERTIES OUTPUT_NAME "Doti")
set_target_properties(Doti PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/windows/x64/$<CONFIG>")

target_compile_definitions(Doti PRIVATE
        GLFW_INCLUDE_NONE
)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if (MSVC)
    # Enable C++ exceptions in MSVC
    target_compile_options(Doti PRIVATE /EHsc)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Enable C++ exceptions and common warnings in Clang
    target_compile_options(Doti PRIVATE -fexceptions -fcxx-exceptions -Wall -Wextra -Wpedantic -Xclang -fmodules -fopenmp)
    target_link_options(Doti PUBLIC -fopenmp -fuse-ld=lld)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Enable C++ exceptions in GCC
    target_compile_options(Doti PRIVATE -fexceptions)
endif ()

set_target_properties(Doti PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(Doti PRIVATE cxx_std_23)

if (MSVC)
    # MSVC
    target_compile_options(Doti PRIVATE
            $<$<CONFIG:Debug>:/Od>               # Disable optimizations in Debug
            $<$<CONFIG:Release>:/O2>             # Optimize for speed in Release
            $<$<CONFIG:Debug>:/Zi>               # Enable debugging information in Debug
    )
    target_compile_definitions(Doti PRIVATE
            $<$<CONFIG:Release>:NDEBUG>          # Define NDEBUG in Release
    )
    set_property(TARGET Doti PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
else ()
    # GCC or Clang
    target_compile_options(Doti PRIVATE
            $<$<CONFIG:Debug>:-O0>               # No optimization in Debug
            $<$<CONFIG:Release>:-O3>             # High optimization in Release
            $<$<CONFIG:Debug>:-g>                # Generate debug symbols in Debug
    )
    target_compile_definitions(Doti PRIVATE
            $<$<CONFIG:Release>:NDEBUG>          # Define NDEBUG in Release
    )
endif ()

target_sources(Doti
        PUBLIC
        FILE_SET CXX_MODULES FILES
        doti/Math/Math.cppm
        doti/Utils/Event.cppm
        doti/Utils/Random.cppm
        doti/Debug/Logger.cppm
        doti/Graphics/Color.cppm
        doti/Graphics/Texture.cppm
        doti/Graphics/Material.cppm
        doti/Graphics/Shader/Shader.cppm
        doti/Graphics/Camera/Camera.cppm
        doti/Graphics/Render/RenderContext.cppm
        doti/Graphics/Render/Illumination/ray.cppm
        doti/Graphics/Render/Drawable/Drawable.cppm
        doti/Graphics/Render/Drawable/Mesh.cppm
        doti/Graphics/Render/Drawable/Triangle.cppm
        doti/Graphics/Render/Drawable/Canvas.cppm
        doti/Graphics/Render/Drawable/Model/Model.cppm
        doti/Graphics/Loader/JxlLoader.cppm
        doti/Graphics/Loader/ImageLoader.cppm
        doti/Scene/SceneBase.cppm
        doti/Scene/SceneManager.cppm
        wrapper/test.cppm
        wrapper/eigen.cppm
        wrapper/glfw.cppm
        wrapper/gl.cppm
        wrapper/glm.cppm
        wrapper/std.mock.cppm
        wrapper/imgui.cppm
        wrapper/assimp.cppm
        wrapper/libjxl.cppm
)
#if (OpenMP_CXX_FOUND)
#    target_link_libraries(Doti PUBLIC OpenMP::OpenMP_CXX)
#endif ()

target_link_libraries(Doti PRIVATE
        glfw
        Freetype::Freetype
        PkgConfig::Jxl
        glbinding::glbinding
        Eigen3::Eigen
        imgui::imgui
        glm::glm
        assimp::assimp
        #        opengl32
        #        user32
        #        shell32
        #        gdi32
)

# Build Editor
add_executable(Editor "")
set_target_properties(Editor PROPERTIES OUTPUT_NAME "Editor")
set_target_properties(Editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/windows/x64/$<CONFIG>")

if (MSVC)
    # Enable C++ exceptions in MSVC
    target_compile_options(Editor PRIVATE /EHsc)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Enable C++ exceptions and common warnings in Clang
    target_compile_options(Editor PRIVATE -fexceptions -fcxx-exceptions -Wall -Wextra -Wpedantic -fopenmp)
    target_link_options(Doti PUBLIC -fopenmp -fuse-ld=lld)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Enable C++ exceptions in GCC
    target_compile_options(Editor PRIVATE -fexceptions)
endif ()

set_target_properties(Editor PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(Editor PRIVATE cxx_std_23)

if (MSVC)
    # MSVC
    target_compile_options(Editor PRIVATE
            $<$<CONFIG:Debug>:/Od>               # Disable optimizations in Debug
            $<$<CONFIG:Release>:/O2>             # Optimize for speed in Release
            $<$<CONFIG:Debug>:/Zi>               # Enable debugging information in Debug
    )
    target_compile_definitions(Editor PRIVATE
            $<$<CONFIG:Release>:NDEBUG>          # Define NDEBUG in Release
    )
    set_property(TARGET Editor PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
else ()
    # GCC or Clang
    target_compile_options(Editor PRIVATE
            $<$<CONFIG:Debug>:-O0>               # No optimization in Debug
            $<$<CONFIG:Release>:-O3>             # High optimization in Release
            $<$<CONFIG:Debug>:-g>                # Generate debug symbols in Debug
    )
    target_compile_definitions(Editor PRIVATE
            $<$<CONFIG:Release>:NDEBUG>          # Define NDEBUG in Release
    )
endif ()

target_sources(Editor
        PUBLIC
        FILE_SET CXX_MODULES FILES
        editor/Window/Window.cppm
        editor/Window/FrameCanvas.cppm
        editor/Window/UI/UIComponent.cppm
        editor/Window/UI/Components/OpenGLView.cppm
        editor/Window/UI/Components/DockSpace.cppm
        editor/Window/UI/Components/MenuBar.cppm
)

file(GLOB SCENE_MODULES "editor/Test/*.cppm")

target_sources(Editor
        PUBLIC
        FILE_SET CXX_MODULES FILES
        ${SCENE_MODULES}
)

target_sources(Editor PRIVATE
        editor/App.cpp
)

target_link_libraries(Editor PRIVATE Doti)

add_custom_command(TARGET Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resource"
        "$<TARGET_FILE_DIR:Editor>/resource"
)