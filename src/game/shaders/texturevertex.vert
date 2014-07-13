#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 normal;
out vec3 worldPosition;
out vec4 directionalLightSpacePosition;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 g_depthMVP;
uniform mat4 modelToWorld;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace, 1.0f);

    // output coordinates in directional light shadow depth coordinates
    directionalLightSpacePosition = g_depthMVP * vec4(vertexPosition_modelspace, 1.0f);

	// UV of the vertex. No special space for this one.
	UV = vertexUV;

    // convert the normal to world space
    normal = (modelToWorld * vec4(vertexNormal_modelspace, 0.0f)).xyz;

    worldPosition = (modelToWorld * vec4(vertexPosition_modelspace, 1.0f)).xyz;
}
