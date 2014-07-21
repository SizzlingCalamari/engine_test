#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Output data ; will be interpolated for each fragment.
out vec3 position_noiseCoords;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
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
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

    position_noiseCoords = transformToNoiseCoords(vertexPosition_modelspace);
}
