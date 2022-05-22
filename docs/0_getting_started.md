# Getting Started

\tableofcontents

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub](https://badgen.net/badge/icon/github?icon=github&label)](https://github.com/filipecn/circe)

Circe is a minimalist framework that can be used in `OpenGL` (and soon `Vulkan`) based applications. Circe serves as a
boiler-plate solution for your graphics applications, so you don't spend time with the basics. The goal is to achieve
results with fewer lines of code, but also with great flexibility. In principle, circe should allow experienced
users to control advanced features, while allowing less experienced users to make use of it as well.

> This library is my personal lib I use in my projects, at my own risk :) Please keep it in mind. But 
> I really hope it can be useful to you too.

## Features
Here is some of what Circe can provide to you:

|  | some features |
|--------------|--------|
| **rendering**     | shadow map, texture maps, hdr, pbr, deferred rendering, ssbos, uniform buffers |
| **scene**     |  instancing, built-in shapes, GPU picking |
| **UI**     | imgui included, profiler view, console output, gizmo, file dialog  |

Check the [gallery]() for images and examples.

## Structure


## Download

You can find Circe at [github](https://github.com/filipecn/circe). 

> Please check the [build](1_build_and_install.md) and [link](2_linking.md) instructions to learn
 how to build and use `circe` into your project.

For the impatient, here is what you can do:
```shell
git clone https://github.com/filipecn/circe.git
cd circe
mkdir build
cd build
cmake .. -DINSTALL_PATH=/my/install/location
make -j8 install
```
and to compile along with your code here is what to do:
```shell
g++ -I/my/install/location/include                \
    -l/my/install/location/lib/libcirce.a         \
    -l/my/install/location/lib/libglad.a          \
    -l/my/install/location/lib/libglfw3.a         \
    -l/my/install/location/lib/libtinyobjloader.a \
    -lpthread                             \
    -lX11                                 \
    -ldl                                  \
    main.cpp  
```

> I've been developing Circe under Ubuntu 20.04 and Nvidia, I have no idea how it behaves on other configurations.

## Contact

Please feel free to contact me by [e-mail](mailto:filipedecn@gmail.com) :)
