#version 330 core

in vec2 UV;
out vec4 fragColour;

uniform sampler2D g_fboSampler;
uniform float g_time;

void main()
{
    vec2 coord = UV;
    coord.x += sin(coord.y * 4*2*3.14159 + g_time) / 100.0f;

    vec4 colorMod = vec4(mod(g_time, 1.0f), mod(g_time + 0.2f, 1.0f), mod(g_time + 0.4f, 1.0f), 1.0f);
    fragColour = colorMod * texture(g_fboSampler, coord);
}
