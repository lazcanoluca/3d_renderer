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
- 7.05 proof of angle sine addition
