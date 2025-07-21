#version 330 core

in vec3 col;
in vec2 tex_coord;

out vec4 frag_col;

uniform sampler2D tex;

void main()
{
    frag_col = vec4(col * texture(tex, tex_coord).rgb, 1.0) ;
}

