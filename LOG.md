## 12/01
- Goals:
    - [x] print the cube with perspective
- rewatched 5.05 array of points

- this won't compile given warning:

```c
const int N_POINTS = 9 * 9 * 9;
vec3_t cube_points[N_POINTS];
```

because `N_POINTS` should be a constant expression:

```c
#define N_POINTS 729

vec3_t cube_points[N_POINTS];
```

- 6.01 orthographic projection
- 6.02 perspective projection
- 6.03 implementing the perspective divide
- 6.04 coordinate system handedness

- 7.02 vector transformations
- 7.03 review sine cosine tangent
- 7.04 vector rotation function

- 7.05 vector rotation function

## 14/01
- 7.05 vector rotation function

## 23/01
- 7.05 vector rotation function

## 24/01
- 7.06 proof of angle sine addition
- 7.08 proof of angle cosine addition
- 8.01 fixing game loop time step
- 8.02 using delay function

## 16/02
- 9.01 triangles and meshes

## 22/02
- 9.02 vertices and triangle faces
- 9.03 drawing triangles
- 10.01 line equation
- 10.02 dda line drawing algorithm

line drawing algorithms:
- dda -> easy
- bresenham -> fast

- 10.03 coding a function to draw lines
- 11.01 dynamic arrays

TODO: make functions pure -> globals bad
TODO: functions should take pointers to instances and modify, weird otherwise

## 23-24/02
- 12.xx obj files

## 25/02
- 13.01 backface culling motivation
- 13.03 vector magnitude
- 13.04 vector addition and substraction
- 13.05 vector scalar multiplication and division
- 13.06 vector cross product
- 13.07 finding the normal vector
- 13.08 dot product

## 11/03
- 13.10 backface culling algorithm
- 13.11 backface culling code
while i'm having a great time with this 3d renderer, and really respect graphic programmers, i'm finding out that this is not for me.

## 12/03
- 13.13 vector normalization
- 14.01 triangle fill
- 14.02 flat-bottom & flat-top technique
- 14.04 finding the triangle midpoint vertex

## 14/03
- 14.05 coding the triangle midpoint vertex
- 14.06 flat-bottom triangle algorithm
- 14.07 flat-bottom triangle code

## 16/03
- 14.08 flat-top triangle algorithm
- 14.09 flat-top triangle code
- 14.10 avoiding division by zero
TODO: add a logger to log events and stuff

## 17/03
- 14.12 different rendering options

## 18/03
- 14.15 colored triangle faces
TODO: save controls and config in config file (find a format or just invent it?)
- 15.01 painter's algorithm
- 15.03 coding a sorting function

## 19/03
- 16.02 matrix operations

## 30/03
- 16.06 2d rotation matrix
- 17.01 3d matrix transformations
 
