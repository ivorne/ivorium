# Ivorium
Ivorium is a slim modular game development framework written in C++ that aims to provide tools and guidelines for smart, scalable and robust game development process.
It allows you to ommit, modify or completely rewrite most of its parts in case their implementation does not suit your particular game architecture.
All new code can be easily integrated with the rest of the framework in order to leverage its robust debugging capabilities and to cleanly interface with existing code.
In contrast with traditional game engines that focus mostly on providing graphics, physics and other multimedia and simulation capabilities, Ivorium is mainly concerned with clean, robust and scalable gameplay development.

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
  * **Common gameplay modules** - Basic and advanced components that make up games - Entity2D, PhysicalBody2D, GameState, Ability, etc.

## Fetch and build
Ivorium is meant to be used as git submodule in your project and built as CMake library linked with your code.
Most dependencies are pulled in as git submodules to `ivorium/libs` subdirectory.
Python project [ivorne/iv_rctools](https://github.com/ivorne/iv_rctools) can be used to prepare and index resources (textures, shaders, fonts, ...) for use within the framework. Other plugins / modules, such as [ivorne/iv_components](https://github.com/ivorne/iv_components), can also be added as git submodules and linked as CMake targets.

Suggested way of starting with Ivorium is by cloning and then extending example project [ivorne/iv_example](https://github.com/ivorne/iv_example). Follow instructions in README.md of that project.

## Community
Ivorium is in early stages of its transition to open-source.

Documentation is currently being worked on. Some important parts of Ivorium may not be easy to find or understand, so take it easy and do not get too frustrated trying it out. Current example project is very bare-bones, it does not even use [ivorne/iv_components](https://github.com/ivorne/iv_components) module. More examples will be created / open-sourced.

This project is open to contributions with some limitations. Documentation of each module will contain information about future plans and improvements and types of contributions that would gladly be accepted. I don't want to discourage any potential contirbutors, so feel free to discuss potential changes beforehand in [Issues](https://github.com/ivorne/ivorium/issues) (label `bug` or `enhancement`). I will tell you if I am interested in such a modification, will give advices for possible implementation, and will provide conditions under which it will be accepted. Straight bugs are very likely to be accepted so do not be afraid to make a Pull request.

And I would like to hear about your experience trying Ivorium out. Did you have any problems understanding something? Does something need more explaining or better documentation? What do you use Ivorium for? What do you like and dislike? Tell me about your time with Ivorium in [Issues](https://github.com/ivorne/ivorium/issues) with label `my-story` so I have better idea who I am developing for.
