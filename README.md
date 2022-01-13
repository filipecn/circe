# Circe
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

---

Circe is a minimalist framework used in the development of `OpenGL` (and soon `Vulkan`) based applications.
Circe serves as a boiler-plate solution for your graphics applications, so you don't spend time with 
the basics. The goal is to achieve results with fewer lines of code, but also allowing great flexibility. 
In principle, circe should allow experienced users to control advanced features, 
while allowing less experienced users to make use of it as well.

Here is what Circe can provide to you with ease:

|  | some features |
|--------------|--------|
| **rendering**     | shadow map, texture maps, hdr, pbr, deferred rendering, ssbos, uniform buffers |
| **scene**     |  instancing, built-in shapes, GPU picking |
| **UI**     | imgui included, profiler view, console output, gizmo, file dialog  |

You can find Circe in [github](https://github.com/filipecn/circe).

very old ancient blog: http://aergia.weebly.com/

## Usage
```c++
#include <circe/circe.h>

// Circe provides a BaseApp class to help you quickly setup an application
// Just inherit from it and override the render method
class HelloCirce : public circe::gl::BaseApp {
public:
  HelloCirce() : BaseApp(800, 800, "") {}
  void render(circe::CameraInterface *camera) override {
    // render your stuff
  }
};

int main() {
  return HelloCirce().run();
}
```

## Build
In order to build and use Circe (with no options), you can do as usual with `cmake`:
```shell
git clone https://github.com/filipecn/hermes.git
cd hermes
mkdir build
cd build
cmake ..
make -j8 install
```

Depending on what you want to compile, you may need to set some `cmake` options:

| variable | description | default  |
|--------------|--------|-----|
| BUILD_ALL  | set all variables below to ON | OFF |
| USE_VULKAN  | compiles with support to Vulkan | OFF |
| BUILD_WITH_CUDA  | compiles with support to CUDA | OFF |
| BUILD_TESTS  | build unit-tests | OFF |
| BUILD_EXAMPLES  | build examples | OFF |
| BUILD_DOCS  | generates documentation | OFF |


### Dependencies

Circe dependencies are:
 - [GLFW](http://www.glfw.org)
 - [imgui](https://github.com/ocornut/imgui)
 - [stb](https://github.com/nothings/stb)
 - [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)
 - [glad](https://github.com/Dav1dde/glad)

They are all handled by `cmake` or have been included into the source, so there is no need to get them yourself.

## TODO
 - [ ] create `cmake` variables for dependencies

## Changelog

## Contact
Please feel free to contact me by [e-mail]((mailto:filipedecn@gmail.com)

## Acknowledgements