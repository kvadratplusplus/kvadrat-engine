#include <math.h>
#include <stdint.h>

#include "../include/kmatrix.h"

kvec2 kvec2_normalize(const kvec2 * a)
{
    const float l = kvec2_length(a);
    return (kvec2) {
        .x = a->x / l,
        .y = a->y / l
    };
}

float kvec2_length(const kvec2 * a)
{
    return sqrtf(a->x * a->x + a->y * a->y);
}

kvec2 kvec2_sub(const kvec2 * a, const kvec2 * b)
{
    return (kvec2) {
        .x = a->x - b->x,
        .y = a->y - b->y
    };
}

float kvec2_distance(const kvec2 * a, const kvec2 * b)
{
    kvec2 c = kvec2_sub(a, b);
    return kvec2_length(&c);
}

float kvec2_dot(const kvec2 * a, const kvec2 * b)
{
    return (a->x * b->x + a->y * b->y);
}

/*
float determinant(float dx, float dy, float vx, float vy)
{
    return dx * vy - dy * vx;
}

uint8_t kvec2_check_intersect
(
    const kvec2* wall_point_1, const kvec2* wall_point_2,
    const kvec2* ray_point, const kvec2* ray_dir
)
{
    kvec2 ab = {wall_point_2->x - wall_point_1->x, wall_point_2->y - wall_point_1->y};
    kvec2 pa = {ray_point->x - wall_point_1->x, ray_point->y - wall_point_1->y};

    float det = determinant(ray_dir->x, ray_dir->y, ab.x, ab.y);

    if (det == 0.f)
        return 0;

    float inv_det = 1.f / det;
    float u = determinant(pa.x, pa.y, ab.x, ab.y) * inv_det;
    float t = determinant(ray_dir->x, ray_dir->y, pa.x, pa.y) * inv_det;

    if (t >= 0 && u >= 0 && u <= 1)
        return 1;
    else
        return 0;
}
*/

//kvec3

kvec3 kvec_add(const kvec3 * a, const kvec3 * b)
{
    return (kvec3) {
        .x = a->x + b->x,
        .y = a->y + b->y,
        .z = a->z + b->z
    };
}

kvec3 kvec_sub(const kvec3 * a, const kvec3 * b)
{
    return (kvec3) {
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z
    };
}

void kvec_copy(kvec3 * dest, const kvec3 * src)
{
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

kvec3 kvec_cross(const kvec3 * a, const kvec3 * b)
{
    return (kvec3) {
        .x = a->y * b->z - a->z * b->y,
        .y = a->z * b->x - a->x * b->z,
        .z = a->x * b->y - a->y * b->x
    };
}

float kvec_dot(const kvec3 * a, const kvec3 * b)
{
    return (a->x * b->x + a->y * b->y + a->z * b->z);
}

float kvec_length(const kvec3 * a)
{
    return sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
}

kvec3 kvec_normalize(const kvec3 * a)
{
    const float l = kvec_length(a);
    return (kvec3) {
        .x = a->x / l,
        .y = a->y / l,
        .z = a->z / l
    };
}

kvec3 kvec_scale(const kvec3 * a, const float b)
{
    return (kvec3) {
        .x = a->x * b,
        .y = a->y * b,
        .z = a->z * b
    };
}

float kvec_distance(const kvec3 * a, const kvec3 * b)
{
    kvec3 c = kvec_sub(a, b);
    return kvec_length(&c);
}

//kmat4

//x1 x2 x3 x4
//y1 y2 y3 y4
//z1 z2 z3 z4
//w1 w2 w3 w4

kmat4 kmat_perspective(int width, int height,
    float fov_degrees, float far_plane, float near_plane)
{
    float aspect_ratio = ((float)width / (float)height);
    float fov_radians = deg_to_rad(fov_degrees);
    float f = 1 / tan(fov_radians / 2);
    float d = far_plane - near_plane;

    return (kmat4) {
        .x1 = f / aspect_ratio,
        .y2 = f,
        .z3 = -(near_plane + far_plane) / d,
        .z4 = (-2 * near_plane * far_plane) / d,
        .w3 = -1
    };
}

kmat4 kmat_identity(void)
{
    return (kmat4) {
        .x1 = 1, .x2 = 0, .x3 = 0, .x4 = 0,
        .y1 = 0, .y2 = 1, .y3 = 0, .y4 = 0,
        .z1 = 0, .z2 = 0, .z3 = 1, .z4 = 0,
        .w1 = 0, .w2 = 0, .w3 = 0, .w4 = 1
    };
}

kmat4 kmat_lookat(kvec3 * eye, kvec3 * target, kvec3 * up)
{
    kvec3 temp = kvec_sub(target, eye);
    kvec3 forward = kvec_normalize(&temp);
    
    temp = kvec_cross(&forward, up);
    kvec3 right = kvec_normalize(&temp);
    
    kvec3 new_up = kvec_cross(&right, &forward); // Новый вектор вверх
    kvec3 normalized_new_up = kvec_normalize(&new_up);

    return (kmat4) {
        .x1 = right.x, .x2 = right.y, .x3 = right.z, .x4 = -kvec_dot(&right, eye),
        .y1 = normalized_new_up.x, .y2 = normalized_new_up.y, .y3 = normalized_new_up.z, .y4 = -kvec_dot(&normalized_new_up, eye),
        .z1 = -forward.x, .z2 = -forward.y, .z3 = -forward.z, .z4 = kvec_dot(&forward, eye),
        .w4 = 1
    };
}

kmat4 kmat_orthogonal(float near, float far, float left, float right, float top, float bottom)
{
    return (kmat4) {
        .x1 = 2 / (right - left), .x4 = -((right + left) / (right - left)),
        .y2 = 2 / (top - bottom), .y4 = -((top + bottom) / (top - bottom)),
        .z3 = -(2 / (far - near)), .z4 = -((far + near) / (far - near)),
        .w4 = 1
    };
}

kmat4 kmat_translate(kvec3 * delta)
{
    return (kmat4) {
        .x1 = 1, .y2 = 1, .z3 = 1, .w4 = 1,
        .x4 = delta->x,
        .y4 = delta->y,
        .z4 = delta->z
    };
}
