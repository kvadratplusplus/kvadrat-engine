#pragma once

#include <stdint.h>

#include <defines.h>
#include <kmatrix.h>

typedef struct
{
    kvec2 points[4];
    float top;
    float bottom;

    uint8_t portal[4];
    float connection_top[4];
    float connection_bottom[4];

    char texture_names[6][128];
    char vertex_shader_names[6][128];
    char fragment_shader_names[6][128];
    kvec2 tex_coords[6];
} SectorLayout;

void sector_create(SectorLayout * layout, Sector * sector);
void sector_draw(Sector * sector);
