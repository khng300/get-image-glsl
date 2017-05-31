cmake_minimum_required(VERSION 2.8)

project(cpp)

# Add "deps" to search prefix for convenience.
set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/deps" "${CMAKE_SOURCE_DIR}/deps" "${CMAKE_PREFIX_PATH}")
# The libraries have the "lib" prefix even on Windows.
set(CMAKE_FIND_LIBRARY_PREFIXES "lib" "${CMAKE_FIND_LIBRARY_PREFIXES}")
# Use c++11, this is compiler-agnostic (not GCC specific)
set (CMAKE_CXX_STANDARD 11)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")
endif()

# Set rpath for Mac and Linux.
if(APPLE)
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:@executable_path/../lib:@executable_path/" )
else(UNIX)
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib:$ORIGIN/" )
endif()

# EGL
find_library(LIB_EGL    NAMES EGL)
find_library(LIB_GLES   NAMES GLESv2)

if(NOT LIB_EGL)
    message(FATAL_ERROR "EGL library not found")
endif()

if(NOT LIB_GLES)
    message(FATAL_ERROR "GLES library not found")
endif()

add_executable(get_image_egl main.cpp lodepng.cpp context_egl.cpp)
target_compile_definitions(get_image_egl PUBLIC -DGETIMAGE_CONTEXT=EGL)
target_link_libraries(get_image_egl ${LIB_EGL} ${LIB_GLES})
target_include_directories(get_image_egl PUBLIC)

# GLFW
find_package(glfw3 3.2 REQUIRED)

add_executable(get_image_glfw main.cpp lodepng.cpp context_glfw.cpp glad.c)
target_compile_definitions(get_image_glfw PUBLIC -DGETIMAGE_CONTEXT=GLFW)
target_link_libraries(get_image_glfw glfw dl)
target_include_directories(get_image_glfw PUBLIC include)

# common
install(TARGETS get_image_egl get_image_glfw
    DESTINATION bin
)
