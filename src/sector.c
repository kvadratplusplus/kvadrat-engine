#include <string.h>
#include <stdio.h>

#include <defines.h>
#include <kmatrix.h>
#include <sector.h>
#include <glfuncs.h>

extern Config main_config;

void sector_draw(Sector * sector)
{
    size_t indeces[main_config.shader_lights_count];
    kmat4 matrix = kmat_identity();

    glf_find_nearest_lights(&sector->center, indeces);

    glf_load_light_pos(indeces, sector->shader_programs[0]);
    glf_draw(sector->shader_programs[0], sector->textures[0], sector->vao, &matrix, 0, 6);

    glf_load_light_pos(indeces, sector->shader_programs[1]);
    glf_draw(sector->shader_programs[1], sector->textures[1], sector->vao, &matrix, 6, 12);

    size_t start = 12;
    size_t end = 15;
    for (size_t i = 0; i < 4; i++) {
        const uint8_t portal = sector->portal[i];

        if (portal == 4)
            continue;

        if (portal == 1) {
            end += 3;
            glf_load_light_pos(indeces, sector->shader_programs[i + 2]);
            glf_draw(sector->shader_programs[i + 2], sector->textures[i + 2], sector->vao, &matrix, start, end);
            start += 6;
            end += 3;
            continue;
        }
        if (portal == 0 || portal == 2 || portal == 3) {
            end += 9;
            glf_load_light_pos(indeces, sector->shader_programs[i + 2]);
            glf_draw(sector->shader_programs[i + 2], sector->textures[i + 2], sector->vao, &matrix, start, end);
            start += 12;
            end += 3;
            continue;
        }
    }
}

void sector_create(SectorLayout * layout, Sector * sector)
{
    size_t triangles = 4;

    memcpy(sector->portal, layout->portal, sizeof(uint8_t) * 4);
    for (size_t i = 0; i < 4; i++) {
        const uint8_t portal = layout->portal[i];

        if (portal == 4)
            continue;

        if (portal == 1) {
            triangles += 4;
            continue;
        }
        if (portal == 2 || portal == 3 || portal == 0) {
            triangles += 2;
            continue;
        }
    }

    float center_x = 0;
    float center_z = 0;
    for (size_t i = 0; i < 4; i++) {
        center_x += layout->points[i].x;
        center_z += layout->points[i].y;
    }
    sector->center.x = center_x / 4;
    sector->center.y = (layout->top + layout->bottom) / 2;
    sector->center.z = center_z / 4;

    float vertices[triangles * 3 * VERTICES_ATTRIBS];

    //floor
    //(n_triang * points_in_triang + n_point) * attribs + n_attrib
    //1 triang
    vertices[(0 * 3 + 0) * 8 + 0] = layout->points[0].x;
    vertices[(0 * 3 + 0) * 8 + 1] = layout->bottom;
    vertices[(0 * 3 + 0) * 8 + 2] = layout->points[0].y;
    vertices[(0 * 3 + 0) * 8 + 3] = 0;
    vertices[(0 * 3 + 0) * 8 + 4] = 0;

    vertices[(0 * 3 + 1) * 8 + 0] = layout->points[1].x;
    vertices[(0 * 3 + 1) * 8 + 1] = layout->bottom;
    vertices[(0 * 3 + 1) * 8 + 2] = layout->points[1].y;
    vertices[(0 * 3 + 1) * 8 + 3] = (layout->points[1].x - layout->points[0].x) / layout->tex_coords[0].x;
    vertices[(0 * 3 + 1) * 8 + 4] = (layout->points[1].y - layout->points[0].y) / layout->tex_coords[0].y;

    vertices[(0 * 3 + 2) * 8 + 0] = layout->points[2].x;
    vertices[(0 * 3 + 2) * 8 + 1] = layout->bottom;
    vertices[(0 * 3 + 2) * 8 + 2] = layout->points[2].y;
    vertices[(0 * 3 + 2) * 8 + 3] = (layout->points[2].x - layout->points[0].x) / layout->tex_coords[0].x;
    vertices[(0 * 3 + 2) * 8 + 4] = (layout->points[2].y - layout->points[0].y) / layout->tex_coords[0].y;
    //2 triang
    vertices[(1 * 3 + 0) * 8 + 0] = layout->points[0].x;
    vertices[(1 * 3 + 0) * 8 + 1] = layout->bottom;
    vertices[(1 * 3 + 0) * 8 + 2] = layout->points[0].y;
    vertices[(1 * 3 + 0) * 8 + 3] = 0;
    vertices[(1 * 3 + 0) * 8 + 4] = 0;

    vertices[(1 * 3 + 1) * 8 + 0] = layout->points[2].x;
    vertices[(1 * 3 + 1) * 8 + 1] = layout->bottom;
    vertices[(1 * 3 + 1) * 8 + 2] = layout->points[2].y;
    vertices[(1 * 3 + 1) * 8 + 3] = (layout->points[2].x - layout->points[0].x) / layout->tex_coords[0].x;
    vertices[(1 * 3 + 1) * 8 + 4] = (layout->points[2].y - layout->points[0].y) / layout->tex_coords[0].y;

    vertices[(1 * 3 + 2) * 8 + 0] = layout->points[3].x;
    vertices[(1 * 3 + 2) * 8 + 1] = layout->bottom;
    vertices[(1 * 3 + 2) * 8 + 2] = layout->points[3].y;
    vertices[(1 * 3 + 2) * 8 + 3] = (layout->points[3].x - layout->points[0].x) / layout->tex_coords[0].x;
    vertices[(1 * 3 + 2) * 8 + 4] = (layout->points[3].y - layout->points[0].y) / layout->tex_coords[0].y;

    kvec3 floor_normal = {0, 1, 0};
    for (size_t i = 0; i < 2; i++)
        for (size_t j = 0; j < 3; j++) {
            vertices[(i * 3 + j) * VERTICES_ATTRIBS + 5] = floor_normal.x;
            vertices[(i * 3 + j) * VERTICES_ATTRIBS + 6] = floor_normal.y;
            vertices[(i * 3 + j) * VERTICES_ATTRIBS + 7] = floor_normal.z;
        }

    sector->shader_programs[0] = glf_load_shader_program(layout->vertex_shader_names[0], layout->fragment_shader_names[0]);
    sector->textures[0] = glf_load_texture(layout->texture_names[0]);

    vertices[(2 * 3 + 0) * 8 + 0] = layout->points[2].x;
    vertices[(2 * 3 + 0) * 8 + 1] = layout->top;
    vertices[(2 * 3 + 0) * 8 + 2] = layout->points[2].y;
    vertices[(2 * 3 + 0) * 8 + 3] = (layout->points[2].x - layout->points[0].x) / layout->tex_coords[1].x;
    vertices[(2 * 3 + 0) * 8 + 4] = (layout->points[2].y - layout->points[0].y) / layout->tex_coords[1].y;

    vertices[(2 * 3 + 1) * 8 + 0] = layout->points[1].x;
    vertices[(2 * 3 + 1) * 8 + 1] = layout->top;
    vertices[(2 * 3 + 1) * 8 + 2] = layout->points[1].y;
    vertices[(2 * 3 + 1) * 8 + 3] = (layout->points[1].x - layout->points[0].x) / layout->tex_coords[1].x;
    vertices[(2 * 3 + 1) * 8 + 4] = (layout->points[1].y - layout->points[0].y) / layout->tex_coords[1].y;

    vertices[(2 * 3 + 2) * 8 + 0] = layout->points[0].x;
    vertices[(2 * 3 + 2) * 8 + 1] = layout->top;
    vertices[(2 * 3 + 2) * 8 + 2] = layout->points[0].y;
    vertices[(2 * 3 + 2) * 8 + 3] = 0;
    vertices[(2 * 3 + 2) * 8 + 4] = 0;
    //2 triang
    vertices[(3 * 3 + 0) * 8 + 0] = layout->points[3].x;
    vertices[(3 * 3 + 0) * 8 + 1] = layout->top;
    vertices[(3 * 3 + 0) * 8 + 2] = layout->points[3].y;
    vertices[(3 * 3 + 0) * 8 + 3] = (layout->points[3].x - layout->points[0].x) / layout->tex_coords[1].x;
    vertices[(3 * 3 + 0) * 8 + 4] = (layout->points[3].y - layout->points[0].y) / layout->tex_coords[1].y;

    vertices[(3 * 3 + 1) * 8 + 0] = layout->points[2].x;
    vertices[(3 * 3 + 1) * 8 + 1] = layout->top;
    vertices[(3 * 3 + 1) * 8 + 2] = layout->points[2].y;
    vertices[(3 * 3 + 1) * 8 + 3] = (layout->points[2].x - layout->points[0].x) / layout->tex_coords[1].x;
    vertices[(3 * 3 + 1) * 8 + 4] = (layout->points[2].y - layout->points[0].y) / layout->tex_coords[1].y;

    vertices[(3 * 3 + 2) * 8 + 0] = layout->points[0].x;
    vertices[(3 * 3 + 2) * 8 + 1] = layout->top;
    vertices[(3 * 3 + 2) * 8 + 2] = layout->points[0].y;
    vertices[(3 * 3 + 2) * 8 + 3] = 0;
    vertices[(3 * 3 + 2) * 8 + 4] = 0;
    
    kvec3 ceil_normal = {0, -1, 0};
    for (size_t i = 2; i < 4; i++)
        for (size_t j = 0; j < 3; j++) {
            vertices[(i * 3 + j) * VERTICES_ATTRIBS + 5] = ceil_normal.x;
            vertices[(i * 3 + j) * VERTICES_ATTRIBS + 6] = ceil_normal.y;
            vertices[(i * 3 + j) * VERTICES_ATTRIBS + 7] = ceil_normal.z;
        }

    sector->shader_programs[1] = glf_load_shader_program(layout->vertex_shader_names[1], layout->fragment_shader_names[1]);
    sector->textures[1] = glf_load_texture(layout->texture_names[1]);

    //walls
    for (size_t i = 0, j = 4; i < 4; i++) {
        size_t next = (i == 3 ? (0) : (i + 1));
        const uint8_t portal = layout->portal[i];

        if (portal == 4)
            continue;

        if (portal == 0) {
            kvec2 wall_vector = kvec2_sub(&layout->points[i], &layout->points[next]);
            kvec3 normal = {0};

            normal.x = -wall_vector.y;
            normal.y = 0;
            normal.z = wall_vector.x;
            normal = kvec_normalize(&normal);

            //(n_triang * points_in_triang + n_point) * attribs + n_attrib
            vertices[(j * 3 + 0) * 8 + 0] = layout->points[i].x;
            vertices[(j * 3 + 0) * 8 + 1] = layout->top;
            vertices[(j * 3 + 0) * 8 + 2] = layout->points[i].y;
            vertices[(j * 3 + 0) * 8 + 3] = 0;
            vertices[(j * 3 + 0) * 8 + 4] = 0;

            vertices[(j * 3 + 1) * 8 + 0] = layout->points[next].x;
            vertices[(j * 3 + 1) * 8 + 1] = layout->top;
            vertices[(j * 3 + 1) * 8 + 2] = layout->points[next].y;
            vertices[(j * 3 + 1) * 8 + 3] = kvec2_distance(&layout->points[i], &layout->points[next]) / layout->tex_coords[i + 2].x;
            vertices[(j * 3 + 1) * 8 + 4] = 0;

            vertices[(j * 3 + 2) * 8 + 0] = layout->points[next].x;
            vertices[(j * 3 + 2) * 8 + 1] = layout->bottom;
            vertices[(j * 3 + 2) * 8 + 2] = layout->points[next].y;
            vertices[(j * 3 + 2) * 8 + 3] = kvec2_distance(&layout->points[i], &layout->points[next]) / layout->tex_coords[i + 2].x;
            vertices[(j * 3 + 2) * 8 + 4] = (layout->top - layout->bottom) / layout->tex_coords[i + 2].y;

            for (size_t k = 0; k < 3; k++) {
                vertices[(j * 3 + k) * 8 + 5] = normal.x;
                vertices[(j * 3 + k) * 8 + 6] = normal.y;
                vertices[(j * 3 + k) * 8 + 7] = normal.z;
            }

            j++;

            vertices[(j * 3 + 0) * 8 + 0] = layout->points[i].x;
            vertices[(j * 3 + 0) * 8 + 1] = layout->top;
            vertices[(j * 3 + 0) * 8 + 2] = layout->points[i].y;
            vertices[(j * 3 + 0) * 8 + 3] = 0;
            vertices[(j * 3 + 0) * 8 + 4] = 0;

            vertices[(j * 3 + 1) * 8 + 0] = layout->points[next].x;
            vertices[(j * 3 + 1) * 8 + 1] = layout->bottom;
            vertices[(j * 3 + 1) * 8 + 2] = layout->points[next].y;
            vertices[(j * 3 + 1) * 8 + 3] = kvec2_distance(&layout->points[i], &layout->points[next]) / layout->tex_coords[i + 2].x;;
            vertices[(j * 3 + 1) * 8 + 4] = (layout->top - layout->bottom) / layout->tex_coords[i + 2].y;

            vertices[(j * 3 + 2) * 8 + 0] = layout->points[i].x;
            vertices[(j * 3 + 2) * 8 + 1] = layout->bottom;
            vertices[(j * 3 + 2) * 8 + 2] = layout->points[i].y;
            vertices[(j * 3 + 2) * 8 + 3] = 0;
            vertices[(j * 3 + 2) * 8 + 4] = (layout->top - layout->bottom) / layout->tex_coords[i + 2].y;

            for (size_t k = 0; k < 3; k++) {
                vertices[(j * 3 + k) * 8 + 5] = normal.x;
                vertices[(j * 3 + k) * 8 + 6] = normal.y;
                vertices[(j * 3 + k) * 8 + 7] = normal.z;
            }

            j++;
        }
        if (portal == 1 || portal == 2 || portal == 3) {
            kvec2 wall_vector = kvec2_sub(&layout->points[i], &layout->points[next]);
            kvec3 normal = {0};

            normal.x = wall_vector.y;
            normal.y = 0;
            normal.z = -wall_vector.x;
            normal = kvec_normalize(&normal);

            if (portal == 2 || portal == 1) {
                //(n_triang * points_in_triang + n_point) * attribs + n_attrib
                vertices[(j * 3 + 0) * 8 + 0] = layout->points[next].x;
                vertices[(j * 3 + 0) * 8 + 1] = layout->connection_top[i];
                vertices[(j * 3 + 0) * 8 + 2] = layout->points[next].y;
                vertices[(j * 3 + 0) * 8 + 3] = kvec2_distance(&layout->points[i], &layout->points[next]) / layout->tex_coords[i + 2].x;
                vertices[(j * 3 + 0) * 8 + 4] = (layout->connection_top[i] - layout->top) / layout->tex_coords[i + 2].y;

                vertices[(j * 3 + 1) * 8 + 0] = layout->points[i].x;
                vertices[(j * 3 + 1) * 8 + 1] = layout->top;
                vertices[(j * 3 + 1) * 8 + 2] = layout->points[i].y;
                vertices[(j * 3 + 1) * 8 + 3] = 0;
                vertices[(j * 3 + 1) * 8 + 4] = 0;

                vertices[(j * 3 + 2) * 8 + 0] = layout->points[next].x;
                vertices[(j * 3 + 2) * 8 + 1] = layout->top;
                vertices[(j * 3 + 2) * 8 + 2] = layout->points[next].y;
                vertices[(j * 3 + 2) * 8 + 3] = kvec2_distance(&layout->points[i], &layout->points[next]) / layout->tex_coords[i + 2].x;
                vertices[(j * 3 + 2) * 8 + 4] = 0;

                for (size_t k = 0; k < 3; k++) {
                    vertices[(j * 3 + k) * 8 + 5] = normal.x;
                    vertices[(j * 3 + k) * 8 + 6] = normal.y;
                    vertices[(j * 3 + k) * 8 + 7] = normal.z;
                }

                j++;

                vertices[(j * 3 + 0) * 8 + 0] = layout->points[next].x;
                vertices[(j * 3 + 0) * 8 + 1] = layout->connection_top[i];
                vertices[(j * 3 + 0) * 8 + 2] = layout->points[next].y;
                vertices[(j * 3 + 0) * 8 + 3] = kvec2_distance(&layout->points[i], &layout->points[next]) / layout->tex_coords[i + 2].x;
                vertices[(j * 3 + 0) * 8 + 4] = (layout->connection_top[i] - layout->top) / layout->tex_coords[i + 2].y;

                vertices[(j * 3 + 1) * 8 + 0] = layout->points[i].x;
                vertices[(j * 3 + 1) * 8 + 1] = layout->connection_top[i];
                vertices[(j * 3 + 1) * 8 + 2] = layout->points[i].y;
                vertices[(j * 3 + 1) * 8 + 3] = 0;
                vertices[(j * 3 + 1) * 8 + 4] = (layout->connection_top[i] - layout->top) / layout->tex_coords[i + 2].y;

                vertices[(j * 3 + 2) * 8 + 0] = layout->points[i].x;
                vertices[(j * 3 + 2) * 8 + 1] = layout->top;
                vertices[(j * 3 + 2) * 8 + 2] = layout->points[i].y;
                vertices[(j * 3 + 2) * 8 + 3] = 0;
                vertices[(j * 3 + 2) * 8 + 4] = 0;

                for (size_t k = 0; k < 3; k++) {
                    vertices[(j * 3 + k) * 8 + 5] = normal.x;
                    vertices[(j * 3 + k) * 8 + 6] = normal.y;
                    vertices[(j * 3 + k) * 8 + 7] = normal.z;
                }

                j++;
            }
            if (portal == 3 || portal == 1) {
                //(n_triang * points_in_triang + n_point) * attribs + n_attrib
                vertices[(j * 3 + 0) * 8 + 0] = layout->points[next].x;
                vertices[(j * 3 + 0) * 8 + 1] = layout->bottom;
                vertices[(j * 3 + 0) * 8 + 2] = layout->points[next].y;
                vertices[(j * 3 + 0) * 8 + 3] = 0;
                vertices[(j * 3 + 0) * 8 + 4] = 0;

                vertices[(j * 3 + 1) * 8 + 0] = layout->points[i].x;
                vertices[(j * 3 + 1) * 8 + 1] = layout->connection_bottom[i];
                vertices[(j * 3 + 1) * 8 + 2] = layout->points[i].y;
                vertices[(j * 3 + 1) * 8 + 3] = kvec2_distance(&layout->points[i], &layout->points[next]) / layout->tex_coords[i + 2].x;
                vertices[(j * 3 + 1) * 8 + 4] = (layout->connection_bottom[i] - layout->bottom) / layout->tex_coords[i + 2].y;

                vertices[(j * 3 + 2) * 8 + 0] = layout->points[next].x;
                vertices[(j * 3 + 2) * 8 + 1] = layout->connection_bottom[i];
                vertices[(j * 3 + 2) * 8 + 2] = layout->points[next].y;
                vertices[(j * 3 + 2) * 8 + 3] = 0;
                vertices[(j * 3 + 2) * 8 + 4] = (layout->connection_bottom[i] - layout->bottom) / layout->tex_coords[i + 2].y;

                for (size_t k = 0; k < 3; k++) {
                    vertices[(j * 3 + k) * 8 + 5] = normal.x;
                    vertices[(j * 3 + k) * 8 + 6] = normal.y;
                    vertices[(j * 3 + k) * 8 + 7] = normal.z;
                }

                j++;

                vertices[(j * 3 + 0) * 8 + 0] = layout->points[next].x;
                vertices[(j * 3 + 0) * 8 + 1] = layout->bottom;
                vertices[(j * 3 + 0) * 8 + 2] = layout->points[next].y;
                vertices[(j * 3 + 0) * 8 + 3] = 0;
                vertices[(j * 3 + 0) * 8 + 4] = 0;

                vertices[(j * 3 + 1) * 8 + 0] = layout->points[i].x;
                vertices[(j * 3 + 1) * 8 + 1] = layout->bottom;
                vertices[(j * 3 + 1) * 8 + 2] = layout->points[i].y;
                vertices[(j * 3 + 1) * 8 + 3] = kvec2_distance(&layout->points[i], &layout->points[next]) / layout->tex_coords[i + 2].x;
                vertices[(j * 3 + 1) * 8 + 4] = 0;

                vertices[(j * 3 + 2) * 8 + 0] = layout->points[i].x;
                vertices[(j * 3 + 2) * 8 + 1] = layout->connection_bottom[i];
                vertices[(j * 3 + 2) * 8 + 2] = layout->points[i].y;
                vertices[(j * 3 + 2) * 8 + 3] = kvec2_distance(&layout->points[i], &layout->points[next]) / layout->tex_coords[i + 2].x;
                vertices[(j * 3 + 2) * 8 + 4] = (layout->connection_bottom[i] - layout->bottom) / layout->tex_coords[i + 2].y;

                for (size_t k = 0; k < 3; k++) {
                    vertices[(j * 3 + k) * 8 + 5] = normal.x;
                    vertices[(j * 3 + k) * 8 + 6] = normal.y;
                    vertices[(j * 3 + k) * 8 + 7] = normal.z;
                }

                j++;
            }
        }
        sector->shader_programs[i + 2] = glf_load_shader_program(&layout->vertex_shader_names[i + 2][0], &layout->fragment_shader_names[i + 2][0]);
        sector->textures[i + 2] = glf_load_texture(layout->texture_names[i + 2]);
    }
    glf_load_buffers(&sector->vao, &sector->vbo, triangles * 3, vertices);
}
