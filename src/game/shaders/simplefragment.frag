#version 330 core

// Interpolated values from the vertex shaders
in vec3 position_noiseCoords;

// Ouput data
out vec3 color;

float snoise(vec3 v);

float turbulence(vec3 position,
                 int octaves,
                 float lacunarity, float gain)
{
    float sum = 0.0f;
    float scale = 1.0f;
    float totalGain = 1.0f;
    for (int i = 0; i < octaves; ++i)
    {
        sum += totalGain * snoise(position*scale);
        scale *= lacunarity;
        totalGain *= gain;
    }
    return abs(sum);
}

void main()
{
    float noise = turbulence(position_noiseCoords, 8, 2, 0.5);
    vec3 colour1 = vec3(0.0f);
    vec3 colour2 = vec3(1.0f);
    color = mix(colour1, colour2, noise);
}
