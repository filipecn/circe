# Circe

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

---

Circe is a minimalist framework that can be used in `OpenGL` (and soon `Vulkan`) based applications. Circe serves as a
boiler-plate solution for your graphics applications, so you don't spend time with the basics. The goal is to achieve
results with fewer lines of code, but also allowing great flexibility. In principle, circe should allow experienced
users to control advanced features, while allowing less experienced users to make use of it as well.

Here is what Circe can provide to you with ease:

|  | some features |
|--------------|--------|
| **rendering**     | shadow map, texture maps, hdr, pbr, deferred rendering, ssbos, uniform buffers |
| **scene**     |  instancing, built-in shapes, GPU picking |
| **UI**     | imgui included, profiler view, console output, gizmo, file dialog  |

You can find Circe in [github](https://github.com/filipecn/circe).

very old ancient blog: http://aergia.weebly.com/

## Usage

Please check the [docs]() for a good introduction, details of the API and examples. 

The example bellow creates a blank window of size 800x800:

```c++
#include <circe/circe.h>

// Circe provides a BaseApp class to help you quickly setup an application
// Just inherit from it and override some methods
class HelloCirce :
public circe::gl::BaseApp {
public:
  HelloCirce() : BaseApp(800, 800, "Hello Circe Window Title") {
  }
  // the render method is the only method required to override,
  // all other methods are optional
  void render(circe::CameraInterface *camera) override {
    // render your stuff
  }
};

int main() {
  return HelloCirce().run();
}
```

You can create and render your models like this:

```c++
// The scene model holds are OpenGL buffers and bind them properly
circe::gl::SceneModel model;
// You can create scene models from built-in shapes for example:
// create a wireframe box
model = circe::Shapes::box(hermes::bbox3::unitBox(), circe::shape_options::wireframe);
// create a sphere with uv coordinates and normals
model = circe::Shapes::icosphere({0,0,0}, 1, circe::shape_options::normals |
                                             circe::shape_options::uv);
// when rendering, just call
model.draw();
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

Suppose you would like to use `VULKAN` and build the examples, your `cmake` command then will look like this:
````shell
cmake .. -DUSE_VULKAN=ON -DBUILD_EXAMPLES=ON
````

### Dependencies
All dependencies are handled by `cmake` or have been included into the source, so there is no need to get them yourself.
- [hermes](https://github.com/filipecn/hermes) - for all the basics
- [GLFW](http://www.glfw.org) - for window management and user input
- [imgui](https://github.com/ocornut/imgui) - for user interface
- [stb](https://github.com/nothings/stb) - for image files and text fonts
- [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) - for reading/writing obj files
- [glad](https://github.com/Dav1dde/glad) - for handling OpenGL extension functions


## TODO

- [ ] create `cmake` variables for dependencies

## Notes
- This library is my personal lib that I use in my projects, at my own risk :) Please keep it in mind.
- I've been developing Circe under Ubuntu 20.04 and Nvidia, I have no idea how it behaves on other systems (or distributions).

## Contact

Please feel free to contact me :)

[e-mail](mailto:filipedecn@gmail.com)
