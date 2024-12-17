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
set(DOTI_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

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
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(PkgConfig)
pkg_check_modules(Jxl REQUIRED IMPORTED_TARGET libjxl libjxl_cms libjxl_threads)
#find_package(Boost REQUIRED COMPONENTS signals2)
#find_package(spdlog CONFIG REQUIRED)

# Settings
set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)

# Build Shaders
set(SHADERS_GLSL
        #        doti/Graphics/Shader/raster_frag.frag
        doti/Graphics/Shader/raster_vert.vert
        #        doti/Graphics/Shader/raytracing.comp
)

find_program(SHADERC_GLSL_EXECUTABLE
        NAMES glslc
        PATHS ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/shaderc
        PATH_SUFFIXES .
)

if (NOT SHADERC_GLSL_EXECUTABLE)
    message(FATAL_ERROR "glslc not found. Please ensure shaderc is installed via vcpkg.")
endif ()

message(STATUS "Found shaderc_glsl: ${SHADERC_GLSL_EXECUTABLE}")

foreach (shader ${SHADERS_GLSL})
    get_filename_component(shader_name ${shader} NAME_WE)
    set(spv_output ${CMAKE_BINARY_DIR}/shaders_spv/${shader_name}.spv)
    add_custom_command(
            OUTPUT ${spv_output}
            COMMAND ${SHADERC_GLSL_EXECUTABLE}
            ${DOTI_ROOT_DIR}/${shader}
            -o ${spv_output}
            --target-env=opengl
            #            --platform desktop
            -I doti/Graphics/Shader/
            DEPENDS ${shader}
            COMMENT "Compiling ${shader} to SPIR-V"
    )
    list(APPEND SPIRV_BINARY ${spv_output})
endforeach ()

add_custom_target(Shaders ALL DEPENDS ${SPIRV_BINARY})

set(SHADER_MODULES_DIR ${CMAKE_SOURCE_DIR}/doti/Graphics/Shader/Modules)
file(MAKE_DIRECTORY ${SHADER_MODULES_DIR})

set(Python3_ROOT_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/python3)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

if (NOT Python3_Interpreter_FOUND)
    message(FATAL_ERROR "Python3 not found. Please ensure Python3 is installed via vcpkg.")
endif ()

message(STATUS "Found Python3: ${Python3_EXECUTABLE}")

foreach (spv ${SPIRV_BINARY})
    get_filename_component(spv_name ${spv} NAME_WE)
    set(module_file ${SHADER_MODULES_DIR}/${spv_name}.cppm)

    add_custom_command(
            OUTPUT ${module_file}
            COMMAND ${Python3_EXECUTABLE}
            ${DOTI_ROOT_DIR}/scripts/SpirvToModule.py
            ${spv}
            ${module_file}
            ${spv_name}
            DEPENDS ${spv}
            COMMENT "Generating module for ${spv_name}.spv, output: ${module_file}"
    )

    list(APPEND SPIRV_MODULES ${module_file})
endforeach ()

add_custom_target(ShaderModules ALL DEPENDS ${SPIRV_MODULES})
add_dependencies(ShaderModules Shaders)

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

add_dependencies(Doti ShaderModules)

target_sources(Doti
        PUBLIC
        FILE_SET CXX_MODULES FILES
        ${SPIRV_MODULES}
        doti/Graphics/Shader/Modules/Shaders.cppm
        doti/Core/Math/Math.cppm
        doti/Core/Event.cppm
        doti/Core/Random.cppm
        doti/Core/Logger.cppm
        doti/Core/Input/InputManager.cppm
        doti/Graphics/Buffer.cppm
        doti/Graphics/Camera/Camera.cppm
        doti/Graphics/Shader.cppm
        doti/Graphics/Color.cppm
        doti/Graphics/FrameCanvas.cppm
        doti/Graphics/Acceleration/AABB.cppm
        doti/Graphics/Acceleration/BVHTree.cppm
        doti/Graphics/Acceleration/Primitive/Primitive.cppm
        doti/Graphics/Acceleration/Primitive/PrimitiveBase.cppm
        doti/Graphics/Acceleration/Primitive/TrianglePrimitive.cppm
        doti/Graphics/Acceleration/Primitive/SpherePrimitive.cppm
        doti/Graphics/Render/Pipeline.cppm
        doti/Graphics/Render/RenderContext.cppm
        doti/Graphics/Render/Object/ObjectBase.cppm
        doti/Graphics/Render/Object/Object.cppm
        doti/Graphics/Render/Object/Mesh.cppm
        doti/Graphics/Render/Type/Light.cppm
        doti/Graphics/Render/Type/Material.cppm
        doti/Core/Loader/JxlLoader.cppm
        doti/Core/Loader/ImageLoader.cppm
        doti/Scene/Scene.cppm
        doti/Scene/SceneManager.cppm
        doti/Scene/SceneNode.cppm
        doti/Scene/SceneNodeFactory.cppm
        wrapper/test.cppm
        wrapper/eigen.cppm
        wrapper/glfw.cppm
        wrapper/gl.cppm
        wrapper/glm.cppm
        wrapper/std.mock.cppm
        wrapper/imgui.cppm
        wrapper/assimp.cppm
        wrapper/libjxl.cppm
        wrapper/json.cppm
)

target_link_libraries(Doti PRIVATE
        glfw
        nlohmann_json::nlohmann_json
        Freetype::Freetype
        PkgConfig::Jxl
        glbinding::glbinding
        Eigen3::Eigen
        imgui::imgui
        glm::glm
        assimp::assimp
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
        editor/Window/UI/UIComponent.cppm
        editor/Window/UI/Components/OpenGLView.cppm
        editor/Window/UI/Components/DockSpace.cppm
        editor/Window/UI/Components/MenuBar.cppm
        editor/Window/UI/Components/Console.cppm
)

#file(GLOB SCENE_MODULES "editor/Test/*.cppm")

#target_sources(Editor
#        PUBLIC
#        FILE_SET CXX_MODULES FILES
#        ${SCENE_MODULES}
#)

target_sources(Editor PRIVATE
        editor/App.cpp
)

target_link_libraries(Editor PRIVATE Doti)

add_custom_command(TARGET Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resource"
        "$<TARGET_FILE_DIR:Editor>/resource"
)