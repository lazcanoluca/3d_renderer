## 12/01
- Goals:
    - [ ] print the cube with perspective
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


