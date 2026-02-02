#include "clipping.h"
#include <math.h>

#define NUM_PLANES 6
plane_t frustum_planes[NUM_PLANES];

void init_frustum_planes(float fov, float z_near, float z_far) {
    float cos_half_fov = cos(fov / 2);
    float sin_half_fov = sin(fov / 2);

    frustum_planes[LEFT_FRUSTRUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.x = cos_half_fov;
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.z = sin_half_fov;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov;

    frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov;

    frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov;

    frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_near);
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

    frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_far);
    frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

// TODO: polygon_t polygon_from_triangle(triangle_t triangle) {
polygon_t polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2) {
    polygon_t polygon = {.vertices = {v0, v1, v2}, .num_vertices = 3};

    return polygon;
}

void polygon_clip_against_plane(polygon_t *polygon, int plane) {
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    // The array of inside vertices that will be part of the final polygon
    // returned via parameter
    vec3_t inside_vertices[MAX_NUM_POLY_VERTICES];
    int num_inside_vertices = 0;

    // Start current and previous vertex with the first and last polygon
    // vertices
    vec3_t *curr_vertex = &polygon->vertices[0];
    vec3_t *prev_vertex = &polygon->vertices[polygon->num_vertices - 1];

    // Start the current and previous dot product
    float curr_dot = 0;
    // vec3_dot(vec3_sub(*curr_vertex, plane_point), plane_normal);
    float prev_dot =
        vec3_dot(vec3_sub(*prev_vertex, plane_point), plane_normal);

    // bool curr_inside = curr_dot > 0;
    // bool is_inside = curr_dot > 0;

    // Loop while the current vertex is different than the last vertex
    // TODO: why not for loop?
    // TODO: check the - 1, it wasnt in the tutorial
    // oh its not even checking against the last one, its just iterating
    // over all the vertices
    // there must a more elegant way -> iterator?
    while (curr_vertex != &polygon->vertices[polygon->num_vertices]) {
        curr_dot = vec3_dot(vec3_sub(*curr_vertex, plane_point), plane_normal);

        // Change in sign -> inside to outside or viceversa
        if (curr_dot * prev_dot < 0) {
            // TODO: get intersection point
            float t = prev_dot / (prev_dot - curr_dot);

            // Calculate the intersection point I = Q1 + t(Q2 - Q1)
            vec3_t intersection_point = vec3_clone(curr_vertex); // I = Qc
            intersection_point =
                vec3_sub(intersection_point, *prev_vertex); // I = (Qc-Qp)
            intersection_point =
                vec3_mul(intersection_point, t); // I = t(Qc-Qp)
            intersection_point =
                vec3_add(intersection_point, *prev_vertex); // I = Qp + t(Qc-Qp)

            // Insert new intersection in the list of "inside vertices"
            inside_vertices[num_inside_vertices] =
                vec3_clone(&intersection_point);
            num_inside_vertices++;
        }

        if (curr_dot > 0) {
            // Insert current vertex in the list of "inside vertices"
            inside_vertices[num_inside_vertices] = vec3_clone(curr_vertex);
            num_inside_vertices++;
        }

        prev_dot = curr_dot;
        prev_vertex = curr_vertex;
        curr_vertex++;
    }

    for (int i = 0; i < num_inside_vertices; i++) {
        polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
    }

    polygon->num_vertices = num_inside_vertices;

    // TODO: copy vertices from inside into the destination
}

void polygon_clip(polygon_t *polygon) {
    polygon_clip_against_plane(polygon, LEFT_FRUSTRUM_PLANE);
    polygon_clip_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
    polygon_clip_against_plane(polygon, TOP_FRUSTUM_PLANE);
    polygon_clip_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
    polygon_clip_against_plane(polygon, NEAR_FRUSTUM_PLANE);
    polygon_clip_against_plane(polygon, FAR_FRUSTUM_PLANE);
}