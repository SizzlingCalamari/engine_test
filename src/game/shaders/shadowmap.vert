#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 g_depthMVP;

void main()
{
	gl_Position =  g_depthMVP * vec4(vertexPosition_modelspace, 1.0f);
}
