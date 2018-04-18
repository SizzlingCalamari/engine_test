#version 330 core

layout(location = 0) in vec2 vertexPosition_normalized;
out vec2 UV;

void main()
{
    gl_Position =  vec4(vertexPosition_normalized, 0.0f, 1.0f);
    UV = (vertexPosition_normalized * 0.5f) + 0.5f;
}
