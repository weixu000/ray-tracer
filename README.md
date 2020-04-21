# Basic Info
Name: Xu Wei A53302898

edX: xwei25cf xwei@ucsd.edu

URL to image-grader:
- HW1: https://lifan.s3.amazonaws.com/homework1/3e15c0ae2494fb5f5c77d3af7538caea/20200405013345/index.html
- HW2: https://lifan.s3.amazonaws.com/homework2/3e15c0ae2494fb5f5c77d3af7538caea/20200421210614/index.html

# Compilation Instructions
- Use CMake to build
    - C++ 17 may be required
    - I only tested on Windows with MinGW-w64
- `/externs` contains 3rd-party libraries
    - `stb-cmake` for image output

# Acceleration structure
In `primitive_group.hpp` and `primitive_group.cpp` I implement Bounding Volume Hierarchy and use it to hold all triangles and spheres in the scene.
- `PrimitiveGroup::PrimitiveGroup` recursively constructs the binary tree. For each non-leaf node, it sorts centers of bounding boxes along the longest axis and splits in the middle
- `PrimitiveGroup::Hit` recursively traverses the tree. For each non-leaf node, test if the ray hits the bounding box
- `PrimitiveList` holds leaf node, which may contain multiple objects and brutal-force loop over them

# Other things
- The project structure looks like `pbrt` to some extent
- `lights` are splited into `Light` (`QuadLight`) and `DeltaLight` (`PointLight`, `DirectionalLight`)
- `BRRF` hold information on how to render this point
- `Material` computes `BRDF` for specific position on the surface