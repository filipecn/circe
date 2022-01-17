# Linking Circe

\tableofcontents

Once `circe` is built and/or installed ([instructions](1_build_and_install.md))
you are free to use it in your project.

Linking `circe` is quite straightforward, but you will need to remember to link against all the 
dependency libraries as well. If you used `make install` though, then `circe` install put together
all the dependency libraries in the same folder as the `circe` library for you.

Before talking about `CUDA` or `Vulkan`, lets see what you need to do if you compiled `circe` **without**
`CUDA` or `Vulkan` support. You want to link your code against `circe` libraries, which in `UNIX` systems will
probably a file with extension `.a`.

If you ran `make install`, then all the files will be in the installation folder, which by default
is the folder you used to build the library itself. Both headers and library files will be 
separate, respectively, in two folders like this:
```shell
include/
 - circe/
   - # all headers and subdirectories of headers
lib/
 - libcirce.a
 - libglad.a
 - libglfw3.a
 - libtinyobjloader.a
```

Once you now exactly the location of these folders, you can easily include and link them 
in your project. For example, lets say `circe` is installed under `/my/location` directory.
So there you find both folders:
```shell
/my/location/include # containing circe headers
/my/location/lib     # containing libcirce.a libglad.a ... etc
```

Suppose you want compile the following program saved in a file `main.cpp`:
```cpp
#include <circe/circe.h>
int main() {
  // TODO
  return 0;
}
```

In order to compile and run this file using `gcc` you can do something like this:
```shell
# compilation command
g++ -I/my/location/include                \
    -l/my/location/lib/libcirce.a         \
    -l/my/location/lib/libglad.a          \
    -l/my/location/lib/libglfw3.a         \
    -l/my/location/lib/libtinyobjloader.a \
    -lpthread                             \
    -lX11                                 \
    -ldl                                  \
    main.cpp  
# run
./a.out
hello circe!
```

Please notice the last 3 linked libs `pthread X11 dl`. These come from `GLFW` and may vary 
depending on your system configuration.

## cmake 
If you use `cmake` in your project, then it might be useful to have 
a `circe.cmake` (listed later) that conveniently sets variables - CIRCE_LIBRARIES and
CIRCE_INCLUDES - pointing to respective `circe` locations. This method
requires you to do the following modifications to your `CMakeLists.txt`:
```cmake
include(ExternalProject)
# let cmake know where to find circe.cmake file
# for example, a folder called ext
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ext")
# add circe.cmake to your project
include(circe)
# set circe as a dependency to your target
add_dependencies(my_target circe)
# and finally, linking to your target this way
target_link_libraries(my_target PUBLIC ${CIRCE_LIBRARIES})
# and including the headers like this
target_include_directories(my_target PUBLIC ${CIRCE_INCLUDES})
```

I usually use the following `circe.cmake` file:
```cmake
# allow cmake to handle external projects (circe in this case)
include(ExternalProject)
# config circe package
ExternalProject_Add(
        circe PREFIX circe
        URL "https://github.com/filipecn/circe/archive/refs/heads/main.zip"
        CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DINSTALL_PATH=install"
)
# retrieve circe local install
ExternalProject_Get_Property(circe INSTALL_DIR)
set(INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/circe)
# set variable containing circe include directory
set(CIRCE_INCLUDES ${INSTALL_DIR}/install/include)
# set variable containing circe library
set(CIRCE_LIBRARIES 
    ${INSTALL_DIR}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}circe${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${INSTALL_DIR}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glad${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${INSTALL_DIR}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${INSTALL_DIR}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}tinyobjloader${CMAKE_STATIC_LIBRARY_SUFFIX}
    dl
    pthread
    X11
)
# output both variables
set(CIRCE_INCLUDES ${CIRCE_INCLUDES} CACHE STRING "")
set(CIRCE_LIBRARIES ${CIRCE_LIBRARIES} CACHE STRING "")
```

Notice that inside `CMAKE_ARGS` section, in `ExternalProject_Add` function, you can pass the
options you want from `circe`. We will need to do this for `CUDA` for example.

## CUDA
For any option other than BUILD_WITH_CUDA the process is exactly the same as above. Just
pass the `cmake` arguments the way you want and be happy. In the case of `CUDA`, there will be two main 
changes in the `circe.cmake`:
```cmake
# Add the CUDA option in CMAKE_ARGS section
ExternalProject_Add(
        ...
        CMAKE_ARGS
        ...
        "-DBUILD_WITH_CUDA=ON" 
        ...
)
# And add CUDA toolkit headers location to HERMES_INCLUDES variables
set(HERMES_INCLUDES
        ...
        ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
        )
```
> Note: CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES should be automatically defined by cmake when
> you set your project language to CUDA as well

There are several ways to compile `CUDA` code with `cmake`, here a possible configuration:

```cmake
target_compile_definitions(my_target PUBLIC
        -DENABLE_CUDA=1
        -D__CUDA_INCLUDE_COMPILER_INTERNAL_HEADERS__=1)
target_include_directories(helios PUBLIC ${HERMES_INCLUDES})
target_compile_options(my_target PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:
        --generate-line-info
        --use_fast_math
        -Xcompiler -pg
        --relocatable-device-code=true
        >)
set_target_properties(helios PROPERTIES
        LINKER_LANGUAGE CUDA
        CMAKE_CUDA_SEPARABLE_COMPILATION ON
        CUDA_RESOLVE_DEVICE_SYMBOLS OFF
        POSITION_INDEPENDENT_CODE ON
        CUDA_STANDARD 17
        CUDA_STANDARD_REQUIRED ON
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        )
```
---
Alright! If you are still there and could compile your code along with `circe`, then it is time to use! 
Please check the [introduction](0_getting_started.md) page to get yourself started!

## Vulkan

_TODO_

## Troubleshooting

_TODO_
