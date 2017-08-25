#version 330 core

uniform vec3 TriCoords[3];
out vec3 TexCoord0;

// Fullscreen tri
// https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/

void main()
{
    TexCoord0 = TriCoords[gl_VertexID];

    vec2 xy = -1.0 + vec2((gl_VertexID & 1) << 2, (gl_VertexID & 2) << 1);
    gl_Position = vec4(xy, 0.0, 1.0);
}
