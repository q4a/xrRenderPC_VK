## xrRenderPC_VK

This is an attempt to create a renderer which is compatible with [X-Ray](https://github.com/OpenXRay/xray-16) engine but uses Vulkan in back end. The project is trying to follow vanilla renderers design, however adding modern features like multithreading and render passes require major architecture changes. The task is quite challenging since even latest R4 renderer heavy relies on immediate DX11 mode.

We are on the very early stage hence current code mostly consists of TODOs. Despite the incompleteness there are several things running already:

* Resource management (pipelines, textures, UBO)
* HLSL shaders cross-compilation
* Font render
* Animated textures
* Basic menu
* Depth buffer and render targets
* Post-processing

(Check for the latest [screenshot](docs/screenshots/screenshot-01-Apr-2019.png))

And upcoming functionality is:

* THM loader
* Particles manager
* Model pool

No detailed roadmap is developed yet and the only scratch pad for ideas can be found in [this](docs/todo.md) TODO document.

### Build instructions

Please note: VS project file is outdated. It will take a while to brush the things up.

< TBD >

### Third party

List of wonderful libraries used in the project:

* `LunarG Vulkan SDK` for Vulkan loader, headers, static libraries and validation layers
* AMD's ``VMA`` for low level memory allocation
* ``shaderc`` for online shader cross-compiling
* ``SPIRV-Cross`` for SPIRV reflection
* ``GLI`` for DDS textures loading
* ``GLM`` for all the math inside
