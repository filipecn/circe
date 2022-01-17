# Building Circe

\tableofcontents

Circe is in [github](https://github.com/filipecn/circe), so you can download directly in the website or you can
just `clone` the repository like this:

```shell
git clone https://github.com/filipecn/circe.git
```

Regardless the method you just chose, jump into the circe folder containing all the source code:

```shell
cd circe
```

The folder should contain something like this

```shell
ls 
CMake CMakeLists.txt docs Doxyfile examples ext circe LICENSE README.md tests
```

Now you are all set to build circe!

## cmake

Since there are no raw makefiles or anything, the easy way to go is with [cmake](https://cmake.org/). `cmake` 
is a tool that generates all the makefiles for you in a very portable manner, so you can just call 
`make` later. Usually, people create a separate directory for all the generated files and binaries:

```shell
mkdir build
cd build
```
> From now on, all the commands must be called from this build folder you just created

then you can invoke `cmake` from the newly created folder:
```shell
cmake ..
```
This will tell `cmake` that the source code is in the parent folder (if you created `build` inside `circe`) 
and `cmake` will generate all its files, include the makefiles, in the current (`build`) folder. Calling
`cmake` with no configuration options (just the `..` argument) will configure `circe` on its default setting: 
which is just the circe library. But of course there are other things you may want to compile along with
the core library.

## Build Options

We set compilation options in the `cmake` command. For example, `circe` provides unit tests, `CUDA` and 
`Vulkan` support. 
You need to tell `cmake` if you want these extra features. An option can _turned_ `ON` or `OFF`, depending on 
whether it is set or not - so options with value `ON` are options you want. Here is a list of all the 
options you can use when configuring `cmake` for `circe`:

| option | description | default  |
|--------------|--------|-----|
| BUILD_WITH_CUDA  | build `circe` with support to CUDA | OFF |
| USE_VULKAN  | compiles with support to Vulkan | OFF |
| BUILD_TESTS  | build unit-tests | OFF |
| BUILD_EXAMPLES  | build examples | OFF |
| BUILD_DOCS  | generates documentation | OFF |
| BUILD_ALL  | set all options above to ON | OFF |

You set an option by using the `-D` argument in the command line. Suppose you would like to 
use `Vulkan` (USE_VULKAN option) and also perform build the examples (BUILD_EXAMPLES option), 
your `cmake` command then will look like this:

````shell
cmake .. -DUSE_VULKAN=ON -DBUILD_EXAMPLES=ON
````

## Dependencies

Here is the list of all external libraries that `circe` uses:

| option | used for  |
|--------------|--------|
| [hermes](https://github.com/filipecn/hermes) | all the basics|
| [GLFW](http://www.glfw.org) | window management and user input|
| [imgui](https://github.com/ocornut/imgui) | user interface|
| [stb](https://github.com/nothings/stb) | image files and text fonts|
| [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) | reading/writing obj files|
| [glad](https://github.com/Dav1dde/glad) | handling OpenGL extension functions|
| [catch2](https://github.com/catchorg/Catch2) | unit-tests |

All the libraries listed above are handled automatically by `cmake`, so there is no need to get or 
install any of them. `hermes` and `GLFW` are downloaded and compiled in the process, and the 
rest are header-only libraries, which are already included in the repository.

The only cases where you will need to take extra actions are when use the options USE_VULKAN or 
BUILD_WITH_CUDA. Both cases require you to manually install the `CUDA` toolkit and the `Vulkan` 
[SDK](https://vulkan.lunarg.com/) in your machine, respectively.

### CUDA

You will need to have `CUDA` runtime libraries installed in your machine
and let `cmake` know where `nvcc`  (`CUDA` compiler) is located passing the `CMAKE_CUDA_COMPILER` 
variable. For example, in my machine `nvcc` is located at `/usr/local/cuda-11.0/bin/nvcc`,
so my `cmake` command would look like this:
```shell
cmake .. -DBUILD_WITH_CUDA=ON -DCMAKE_CUDA_COMPILER=/usr/local/cuda-11.0/bin/nvcc
```

Right now, my `cmake` file is very simple regarding `CUDA`, I just call the `enable_language(CUDA)`
function inside it and include CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES (which is defined automatically
by `cmake`).

>I'm currently using Ubuntu 20.04 with no special setting. Hopefully `cmake` will be able to 
> handle `CUDA` in your system as well :)

### Vulkan

Circe is also handling `Vulkan` in a very simple fashion. It just links against `vulkan dl` libraries.
So it is currently just expecting `Vulkan` libraries to be installed in the usual system folders that
`gcc` looks into.

## Install

You can also install `circe` headers and library wherever you want. By default, the installation 
folder is the `build` folder from where you ran the `cmake` command. In order to install in 
another location use the INSTALL_PATH variable when running the `cmake` command like this:

```shell
cmake .. -DINSTALL_PATH=/my/install/location
```

> **Important:** Circe will also put in the installation folder all the dependencies for you as well 
> (but neither CUDA nor Vulkan)

## Build

If the `cmake` command runs successfully, then you can proceed to compilation:

```shell
make -j8
```
and install
```shell
make install
```

Hopefully you will not encounter any errors! Now you are free include and link `circe` to
your project :)

## Summary
So putting all together, we get something like this:
```shell
git clone https://github.com/filipecn/circe.git
cd circe
mkdir build
cd build
cmake .. -DBUILD_ALL=ON -DINSTALL_PATH=/my/install/location
make -j8 install
```
---
Check the [next](2_linking.md) page to see how use `circe` in your project.

## Troubleshooting

_TODO_