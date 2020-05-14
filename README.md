# Basic Info
Name: Xu Wei A53302898

edX: xwei25cf xwei@ucsd.edu

URL to image-grader:
- HW1: https://lifan.s3.amazonaws.com/homework1/3e15c0ae2494fb5f5c77d3af7538caea/20200405013345/index.html
- HW2: https://lifan.s3.amazonaws.com/homework2/3e15c0ae2494fb5f5c77d3af7538caea/20200421210614/index.html
- HW3:
    - https://lifan.s3.amazonaws.com/homework3a/3e15c0ae2494fb5f5c77d3af7538caea/20200503221630/index.html
    - https://lifan.s3.amazonaws.com/homework3b/3e15c0ae2494fb5f5c77d3af7538caea/20200430003049/index.html
- HW4
    - https://lifan.s3.amazonaws.com/homework4a/3e15c0ae2494fb5f5c77d3af7538caea/20200514220909/index.html
    - https://lifan.s3.amazonaws.com/homework4b/3e15c0ae2494fb5f5c77d3af7538caea/20200514220448/index.html

# Compilation Instructions
- Use CMake to build
    - C++ 17 may be required
    - I tested with GCC 9.3 or MSVC 2019
- `/externs` contains 3rd-party libraries
    - `stb-cmake` for image output

# Acceleration structure
In `bvh.hpp` and `bvh.cpp` I implement Bounding Volume Hierarchy and use it to hold all triangles and spheres in the scene.
- `BVH::BVH` recursively constructs the binary tree. For each non-leaf node, it sorts centers of bounding boxes along the longest axis and splits in the middle
- `BVH::Hit` recursively traverses the tree. For each non-leaf node, test if the ray hits the bounding box

# Other things
- The project structure looks like `pbrt` to some extent
- Russian roulette can be enabled by template parameter. I use template to reduce code duplication
- Enable multi-threading to accelerate rendering
- `BVH` stores shapes following data-oriented pattern
- `directional`, `point`, `integrator simple`, `nexteventestimation off` have been removed
