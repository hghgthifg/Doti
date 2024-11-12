cmake_minimum_required(VERSION 3.28.0)
cmake_policy(SET CMP0091 NEW)
project(Doti LANGUAGES CXX)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# 使用 vcpkg 的工具链文件
# 确保在调用 CMake 时添加 -DCMAKE_TOOLCHAIN_FILE=<vcpkg 安装路径>/scripts/buildsystems/vcpkg.cmake

find_package(Eigen3 CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

# 添加静态库目标
add_library(Doti STATIC "")
set_target_properties(Doti PROPERTIES OUTPUT_NAME "Doti")
set_target_properties(Doti PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/windows/x64/$<CONFIG>")

target_include_directories(Doti SYSTEM PRIVATE
        ${EIGEN3_INCLUDE_DIR}
        ${GLFW3_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR}
)

target_compile_definitions(Doti PRIVATE
        GLFW_INCLUDE_NONE
)

if(MSVC)
    target_compile_options(Doti PRIVATE /EHsc)
elseif(Clang)
    target_compile_options(Doti PRIVATE -fexceptions -fcxx-exceptions)
elseif(Gcc)
    target_compile_options(Doti PRIVATE -fexceptions)
endif()

set_target_properties(Doti PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(Doti PRIVATE cxx_std_23)

if(MSVC)
    target_compile_options(Doti PRIVATE $<$<CONFIG:Debug>:-Od> $<$<CONFIG:Release>:-O2>)
    target_compile_options(Doti PRIVATE $<$<CONFIG:Debug>:-Zi> $<$<CONFIG:Release>:-DNDEBUG>)
else()
    target_compile_options(Doti PRIVATE $<$<CONFIG:Debug>:-O0> $<$<CONFIG:Release>:-O3>)
    target_compile_options(Doti PRIVATE $<$<CONFIG:Debug>:-g> $<$<CONFIG:Release>:-DNDEBUG>)
endif()

if(MSVC)
    set_property(TARGET Doti PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

target_sources(Doti
        PUBLIC
        FILE_SET CXX_MODULES FILES
        doti/Utils/utils.cppm
        doti/Utils/ray.cppm
        doti/Math/math.cppm
        doti/Utils/color.cppm
        wrapper/test.cppm
        wrapper/eigen.cppm
        wrapper/glfw.cppm
)

target_link_libraries(Doti PRIVATE
        glfw
        glad::glad
        Eigen3::Eigen
        opengl32
        user32
        shell32
        gdi32
)

# 添加可执行文件目标
add_executable(Editor "")
set_target_properties(Editor PROPERTIES OUTPUT_NAME "Editor")
set_target_properties(Editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/windows/x64/$<CONFIG>")

if(MSVC)
    target_compile_options(Editor PRIVATE /EHsc)
elseif(Clang)
    target_compile_options(Editor PRIVATE -fexceptions -fcxx-exceptions)
elseif(Gcc)
    target_compile_options(Editor PRIVATE -fexceptions)
endif()

set_target_properties(Editor PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(Editor PRIVATE cxx_std_23)

if(MSVC)
    target_compile_options(Editor PRIVATE $<$<CONFIG:Debug>:-Od> $<$<CONFIG:Release>:-O2>)
    target_compile_options(Editor PRIVATE $<$<CONFIG:Debug>:-Zi> $<$<CONFIG:Release>:-DNDEBUG>)
else()
    target_compile_options(Editor PRIVATE $<$<CONFIG:Debug>:-O0> $<$<CONFIG:Release>:-O3>)
    target_compile_options(Editor PRIVATE $<$<CONFIG:Debug>:-g> $<$<CONFIG:Release>:-DNDEBUG>)
endif()

if(MSVC)
    set_property(TARGET Editor PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

target_sources(Editor PRIVATE
        editor/main.cpp
)

target_link_libraries(Editor PRIVATE Doti)