cmake_minimum_required(VERSION 2.8)

project(cpp)

# -- Build dependencies -- #
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/deps)
execute_process(
COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} ${CMAKE_SOURCE_DIR}/deps -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/deps
)
execute_process(
COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE}
WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/deps
)
execute_process(
COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=../install -DBUILD_TYPE=${CMAKE_BUILD_TYPE} -P cmake_install.cmake
WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/deps
)
# -- End build dependencies -- #

# Make sure CMake can find built dependencies
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/deps/install ${CMAKE_PREFIX_PATH})

find_package(glfw3 REQUIRED)

add_executable(get_image_glfw main.cpp context_glfw.cpp lodepng.cpp glad.c)
target_link_libraries(get_image_glfw glfw)
target_compile_definitions(get_image_glfw PUBLIC -DGETIMAGE_CONTEXT=CONTEXT_GLFW)
target_include_directories(get_image_glfw PUBLIC include)

install(TARGETS get_image_glfw
    DESTINATION bin
)


if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wall -std=c++11 ${CMAKE_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()
