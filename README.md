# Ivorium
Ivorium is a slim modular game development framework written in C++ that aims to provide tools and guidelines for smart, scalable and robust game development process.
It allows you to ommit, modify or completely rewrite most of its parts in case their implementations do not suit your particular game architecture.
All new code can be easily integrated with the rest of the framework in order to leverage its robust debugging capabilities and to cleanly interface with existing code.
In contrast with traditional game engines that focus mostly on providing graphics, physics and other multimedia and simulation capabilities, Ivorium is mainly concerned with clean, robust and scalable gameplay development.

  * [Documentation](https://ivorne.github.io/ivorium)
  * [Example project](https://github.com/ivorne/iv_example)
  
## Features
  * **CMake** - Build system.
  * **Git submodules** - For dependency management, giving powerful multiplatform support to the build system.
  * **Windowing and core game loop** - GLFW (Linux, Windows, MacOS) and GLFM (Android, iOS, Web/Emscripten).
  * **2D / 3D scene graph** - Uses simple OpenGL 3 / OpenGL ES 3 / WebGL 2.0 renderer, pending rewrite to use [bkaradzic/BGFX](https://github.com/bkaradzic/bgfx). Current development focuses on 2D, but all existing code is ready for 3D.
  * **Animation control system** - Each animation update advances scene graph attributes to more reflect current state of input data.
  * **UI library** - Seamlessly extendable and skinnable, with color themes.
  * **Resource management system** - Seamlessly extandable, with compile-time resource preparation.
  * **Input processing system** - Uses semantic input tree to determine input priorities and provides flexible input binding management.
  * **Diagnostics** - Both for framework code and gameplay code, with user-definable logging channels and filters and with complete state dumps.
  * **Common gameplay modules** - Collection of standard Model, View and Behavior classes for common game use cases. Things like Entity2D for general entity abstraction, Physical2D for PlayRho physics, Ability for activatable abilities, etc.

## Fetch and build
Recommended way of starting with ivorium is by cloning the example project [ivorne/iv_example](https://github.com/ivorne/iv_example). Follow instructions in README.md of that project.

To use Ivorium, clone it into subdirectory in your project, then build and link it as CMake library.
Use [ivorne/iv_rctools](https://github.com/ivorne/iv_rctools) to prepare (textures, shaders, fonts, ...) for use with the framework.
Other libraries depending on ivorium, such as [ivorne/iv_components](https://github.com/ivorne/iv_components), can be added as additional subdirectories and linked with CMake.

Ivorium uses:
  - [CMake >=3.10](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) for build project generation
  - [GLFW](https://www.glfw.org/docs/latest/compile_guide.html#compile_cmake) for windowing on desktop platforms
  - [GLFM](https://github.com/brackeen/glfm#build-the-example-glfm-projects) for windowing on mobile platforms and Emscripten (web)
  - [SoLoud](https://github.com/ivorne/soloud/blob/master/contrib/Configure.cmake) for sound support

All guides that apply to building with those projects should also apply to building with ivorium.

## Community
Ivorium is in early stages of its transition to open-source. Documentation is currently being worked on. Some important parts of Ivorium may not be easy to find or understand, so take it easy and do not get too frustrated trying it out. Current example project is very bare-bones, it does not even use [ivorne/iv_components](https://github.com/ivorne/iv_components) module. More examples will be created / open-sourced.

This project is open to contributions with some limitations. Documentation of each module will contain informations about future plans and possible improvements and types of contributions that would be accepted. I do not want to discourage any potential contributors, so feel free to discuss potential changes beforehand in [Issues](https://github.com/ivorne/ivorium/issues) (label `bug` or `enhancement`). I will tell you if I am interested in such a modification, will give advices for its possible implementation, and will provide conditions under which such contribution will be accepted. Straight bugs are very likely to be accepted so do not be afraid to make a Pull request.

I would like to hear about your experience trying out ivorium. Did you have any problems understanding something? Does something need more explaining or better documentation? What do you like and dislike? Tell me about your time with ivorium in [Issues](https://github.com/ivorne/ivorium/issues) with label `my-story` so I have a better idea who I am collaborating with.

## Projects that use ivorium
#### Elor
Small 48-hour gamejam project, not open-sourced. Play in web browser: https://ivorne.github.io/elor/game.html.

![screenshot](https://ivorne.github.io/elor/screenshot.png)
