#pragma once

//3.14159265358979323846
#define PI 3.14159265358979323846
#define deg_to_rad(X) (X * PI / 180)

//normal matrix format
//x1 x2 x3 x4
//y1 y2 y3 y4
//z1 z2 z3 z4
//w1 w2 w3 w4

//opengl matrix format
//x1 y1 z1 w1
//x2 y2 z2 w2
//x3 y3 z3 w3
//x4 y4 z4 w4

typedef struct
{
    float x1, y1, z1, w1;
    float x2, y2, z2, w2;
    float x3, y3, z3, w3;
    float x4, y4, z4, w4;
} kmat4;

typedef struct
{
    float x, y, z;
} kvec3;

typedef struct
{
    float x, y;
} kvec2;

kvec2 kvec2_sub(const kvec2 * a, const kvec2 * b);
float kvec2_distance(const kvec2 * a, const kvec2 * b);
float kvec2_length(const kvec2 * a);
float kvec2_dot(const kvec2 * a, const kvec2 * b);
kvec2 kvec2_normalize(const kvec2 * a);
uint8_t kvec2_check_intersect
(
    const kvec2* wall_point_1, const kvec2* wall_point_2,
    const kvec2* ray_point, const kvec2* ray_dir
);

kvec3 kvec_add(const kvec3 *a, const kvec3 *b);
kvec3 kvec_sub(const kvec3 *a, const kvec3 *b);
kvec3 kvec_cross(const kvec3 *a, const kvec3 *b);
float kvec_dot(const kvec3 *a, const kvec3 *b);
float kvec_length(const kvec3 *a);
kvec3 kvec_normalize(const kvec3 *a);
kvec3 kvec_scale(const kvec3 *a, const float b);
float kvec_distance(const kvec3 * a, const kvec3 * b);

kmat4 kmat_perspective(int width, int height, float fov_degrees, float far_plane, float near_plane);
kmat4 kmat_orthogonal(float near, float far, float left, float right, float top, float bottom);
kmat4 kmat_identity(void);
kmat4 kmat_lookat(kvec3 *eye, kvec3 *target, kvec3 *up);
kmat4 kmat_translate(kvec3 *delta);
