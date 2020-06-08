# Basic Info
Name: Xu Wei A53302898

edX: xwei25cf xwei@ucsd.edu

URL to final images:
- HW1: https://lifan.s3.amazonaws.com/homework1/3e15c0ae2494fb5f5c77d3af7538caea/20200405013345/index.html
- HW2: https://lifan.s3.amazonaws.com/homework2/3e15c0ae2494fb5f5c77d3af7538caea/20200421210614/index.html
- HW3:
    - https://lifan.s3.amazonaws.com/homework3a/3e15c0ae2494fb5f5c77d3af7538caea/20200503221630/index.html
    - https://lifan.s3.amazonaws.com/homework3b/3e15c0ae2494fb5f5c77d3af7538caea/20200430003049/index.html
- HW4
    - https://lifan.s3.amazonaws.com/homework4a/3e15c0ae2494fb5f5c77d3af7538caea/20200514220909/index.html
    - https://lifan.s3.amazonaws.com/homework4b/3e15c0ae2494fb5f5c77d3af7538caea/20200514220448/index.html
- Final: https://ucsdcse168wxzzj.wordpress.com/2020/05/26/ucsd-cse-168-2020-spring-final-project

# Compilation Instructions
- Use CMake to build
    - C++ 17 may be required
    - I tested with GCC 9.3 or MSVC 2019
- `/externs` contains 3rd-party libraries
    - `stb-cmake` for image output

# Acceleration structure
In `kernel.cpp`, I use Embree to do ray tracing. Old code of geometry tracing and BVH has been removed.

# Other things
- The project structure looks like `pbrt` to some extent
- Enable multi-threading to accelerate rendering
- `directional`, `point`, `integrator simple`, `nexteventestimation off` have been removed
- I use std::variant to handle different materials so that they can return BRDF/BSSRDF of different types without memory allocation
- In `path_tracer.cpp`, `Bounce<T>` structure hides the difference between BRDF/BSSRDF so that modification is minimized
