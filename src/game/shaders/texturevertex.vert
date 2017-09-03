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
out vec3 position_noiseCoords;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 g_depthMVP;
uniform mat4 modelToWorld;
uniform mat3x2 cUVTransform;
uniform vec3 minAABB;

float minOfVec3(vec3 vec)
{
    return min(vec.x, min(vec.y, vec.z));
}

// convert from [minAABB, maxAABB] to [-1, 1]
vec3 transformToNoiseCoords(vec3 position)
{
    float normalizeValue = minOfVec3(minAABB);
	return position/normalizeValue;
}

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace, 1.0f);

    // output coordinates in directional light shadow depth coordinates
    directionalLightSpacePosition = g_depthMVP * vec4(vertexPosition_modelspace, 1.0f);

    // Apply uv scale, rot, offset
    UV = (cUVTransform * vec3(vertexUV, 1.0)).xy;

    // convert the normal to world space
    normal = (modelToWorld * vec4(vertexNormal_modelspace, 0.0f)).xyz;

    worldPosition = (modelToWorld * vec4(vertexPosition_modelspace, 1.0f)).xyz;

    // output the position normalized to [-1,1]
    position_noiseCoords = transformToNoiseCoords(vertexPosition_modelspace);
}
