# Basic Info
Name: Xu Wei A53302898

edX: xwei25cf xwei@ucsd.edu

URL to image-grader: https://lifan.s3.amazonaws.com/homework1/3e15c0ae2494fb5f5c77d3af7538caea/20200405013345/index.html

# Compilation Instructions
- Use CMake to build
    - C++ 17 may be required
    - I only tested on Windows with MinGW-w64
    - Main `raytracer` is a lib, exe is in `/tests/cse168`
- `/externs` contains 3rd-party libraries
    - `glm` for math functions
    - `stb-cmake` for image output

# Acceleration structure
In `/include/BVH.hpp` and `src/BVH.cpp` I implement Bounding Volume Hierarchy and use it to hold all triangles and spheres in the scene.
- `BVH::Construct` recursively constructs the binary tree. For each non-leaf node, it sorts centers of bounding boxes along the longest axis and splits in the middle
- `BVH::Traverse` recursively traverses the tree. For each non-leaf node, test if the ray hits the bounding box

# Other things
- I also finished homework 2 here
- The project structure looks like `pbrt` to some extent