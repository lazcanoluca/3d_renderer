#include "clipping.h"
#include <math.h>

#define NUM_PLANES 6
plane_t frustum_planes[NUM_PLANES];

void init_frustum_planes(float fov_x, float fov_y, float z_near, float z_far) {
    float cos_half_fov_x = cos(fov_x / 2);
    float sin_half_fov_x = sin(fov_x / 2);
    float cos_half_fov_y = cos(fov_y / 2);
    float sin_half_fov_y = sin(fov_y / 2);

    frustum_planes[LEFT_FRUSTRUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.x = cos_half_fov_x;
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.z = sin_half_fov_x;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov_x;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

    frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov_y;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

    frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov_y;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

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
polygon_t polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2, tex2_t t0,
                                tex2_t t1, tex2_t t2) {
    polygon_t polygon = {
        .vertices = {v0, v1, v2}, .texcoords = {t0, t1, t2}, .num_vertices = 3};

    return polygon;
}

float float_lerp(float a, float b, float t) { return a + t * (b - a); }

void polygon_clip_against_plane(polygon_t *polygon, int plane) {
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    // The array of inside vertices that will be part of the final polygon
    // returned via parameter
    vec3_t inside_vertices[MAX_NUM_POLY_VERTICES];
    tex2_t inside_texcoords[MAX_NUM_POLY_VERTICES];
    int num_inside_vertices = 0;

    // Start current and previous vertex with the first and last polygon
    // vertices
    vec3_t *curr_vertex = &polygon->vertices[0];
    tex2_t *curr_texcoord = &polygon->texcoords[0];
    vec3_t *prev_vertex = &polygon->vertices[polygon->num_vertices - 1];
    tex2_t *prev_texcoord = &polygon->texcoords[polygon->num_vertices - 1];

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
            // vec3_t intersection_point = vec3_clone(curr_vertex); // I = Qc
            vec3_t intersection_point = {
                .x = float_lerp(prev_vertex->x, curr_vertex->x, t),
                .y = float_lerp(prev_vertex->y, curr_vertex->y, t),
                .z = float_lerp(prev_vertex->z, curr_vertex->z, t),
            };

            // intersection_point =
            //     vec3_sub(intersection_point, *prev_vertex); // I = (Qc-Qp)
            // intersection_point =
            //     vec3_mul(intersection_point, t); // I = t(Qc-Qp)
            // intersection_point =
            //     vec3_add(intersection_point, *prev_vertex); // I = Qp +
            //     t(Qc-Qp)

            tex2_t interpolated_texcoord = {
                .u = float_lerp(prev_texcoord->u, curr_texcoord->u, t),
                .v = float_lerp(prev_texcoord->v, curr_texcoord->v, t),
            };
            // tex2_t interpolated_texcoord = {
            //     .u = prev_texcoord->u +
            //          t * (curr_texcoord->u - prev_texcoord->u),
            //     .v = prev_texcoord->v +
            //          t * (curr_texcoord->v - prev_texcoord->v),
            // };

            // Insert new intersection in the list of "inside vertices"
            inside_vertices[num_inside_vertices] =
                vec3_clone(&intersection_point);
            inside_texcoords[num_inside_vertices] =
                tex2_clone(&interpolated_texcoord);
            num_inside_vertices++;
        }

        if (curr_dot > 0) {
            // Insert current vertex in the list of "inside vertices"
            inside_vertices[num_inside_vertices] = vec3_clone(curr_vertex);
            inside_texcoords[num_inside_vertices] = tex2_clone(curr_texcoord);
            num_inside_vertices++;
        }

        prev_dot = curr_dot;
        prev_vertex = curr_vertex;
        prev_texcoord = curr_texcoord;
        curr_vertex++;
        curr_texcoord++;
    }

    for (int i = 0; i < num_inside_vertices; i++) {
        polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
        polygon->texcoords[i] = tex2_clone(&inside_texcoords[i]);
    }

    polygon->num_vertices = num_inside_vertices;

    // TODO: copy vertices from inside into the destination
}

void triangles_from_polygon(polygon_t *polygon, triangle_t triangles[],
                            int *num_triangles) {
    for (int i = 0; i < polygon->num_vertices - 2; i++) {
        int idx0 = 0;
        int idx1 = i + 1;
        int idx2 = i + 2;

        triangles[i].points[0] = vec4_from_vec3(polygon->vertices[idx0]);
        triangles[i].points[1] = vec4_from_vec3(polygon->vertices[idx1]);
        triangles[i].points[2] = vec4_from_vec3(polygon->vertices[idx2]);

        triangles[i].texcoords[0] = tex2_clone(&polygon->texcoords[idx0]);
        triangles[i].texcoords[1] = tex2_clone(&polygon->texcoords[idx1]);
        triangles[i].texcoords[2] = tex2_clone(&polygon->texcoords[idx2]);
    }

    *num_triangles = polygon->num_vertices - 2;
}

void polygon_clip(polygon_t *polygon) {
    polygon_clip_against_plane(polygon, LEFT_FRUSTRUM_PLANE);
    polygon_clip_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
    polygon_clip_against_plane(polygon, TOP_FRUSTUM_PLANE);
    polygon_clip_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
    polygon_clip_against_plane(polygon, NEAR_FRUSTUM_PLANE);
    polygon_clip_against_plane(polygon, FAR_FRUSTUM_PLANE);
}