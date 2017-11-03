[
[Build Status](https://travis-ci.org/graphicsfuzz/get-image-glsl.svg?branch=master)](https://travis-ci.org/graphicsfuzz/get-image-glsl)

[![Build status](https://ci.appveyor.com/api/projects/status/4mbuu9dkx83xbwii/branch/master?svg=true)](https://ci.appveyor.com/project/hevrard/get-image-glsl/branch/master)

# get-image-glsl

This program renders a fragment shader, it can be compiled with
different context provider, currently EGL or GLFW. See build
instructions below.

# Prerequisites

The following are required for building and running the tool:
* The OpenGL headers (in Ubuntu, provided by `mesa-common-dev`)
* GLFW (in Ubuntu, provided by `libglfw-3-dev`)
* CMake

# Usage

Note: For an up-to-date option overview, just run one of the executables
without any arguments, it should list its options.

```
Usage: ./get_image_<egl/glfw> [options] shader.frag

The program will look for a JSON whose name is derived from the
shader as '<shader>.json'. This JSON file can contain uniforms
initialisations. If no JSON file is found, the program uses default
values for some uniforms.

Options are:
  --delay                            number of frames before PNG capture
  --exit-compile                     exit after compilation
  --exit-linking                     exit after linking
  --output file.png                  set PNG output file name
  --resolution <width> <height>      set viewport resolution, in Pixels
  --vertex shader.vert               use a specific vertex shader
  --dump_bin <file>                  dump binary output to given file

Return values:
  0    Successful rendering
  1    Error
  101  Shader compilation error (either fragment or vertex)
  102  Shader linking error
```

# Build

The EGL version requires that you copy libEGL and libGLESv2 into `deps/`.
See below for skipping the EGL version.

## Linux

Using CMake manually:

```cmd
mkdir b
cd b
cmake -G "Unix Makefiles" .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cmake -DCMAKE_INSTALL_PREFIX=../install -DBUILD_TYPE=Release -P cmake_install.cmake
```

Or, use the script.

```
cd build
./build-linux
```

Alternatively, if you already have glfw3 and egl installed, you can use
the Makefile (just call `make`).

## Windows

Using CMake manually:

```cmd
mkdir b
cd b
cmake -G "Visual Studio 14 2015 Win64" .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
cmake -DCMAKE_INSTALL_PREFIX=../install -DBUILD_TYPE=Debug -P cmake_install.cmake
```

Or, use this cross-compilation script from Linux:

```
cd build
./build-x86_64-w64-mingw32
```

## Skipping version

Skip building certain versions as follows. E.g.

```bash
cmake -G "Unix Makefiles" .. -DBUILD_GLFW_VERSION=OFF -DBUILD_EGL_VERSION=OFF
                               ^ (skip GLFW version)    ^ (skip EGL version)
```

# Developer notes

## TODO

- add option to force a particular opengl api version (with glfw)

## Crash early!

For any error, use the `crash()` macro defined in `common.h`. Do not
hesitate to crash as soon as something goes wrong, rather than passing
around return values.

## CI

 - Windows: See `appveyor.yml` and the corresponding scripts under
   `buildscripts/`

There is a dependency on:

    # see buildscripts/1-install-deps-appveyor.sh
    https://github.com/paulthomson/github-release
