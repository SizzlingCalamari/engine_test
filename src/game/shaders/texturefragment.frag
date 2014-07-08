#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal;

// Ouput data
out vec4 fragColour;

struct DirectionalLight
{
    vec3 colour;
    float ambientIntensity;
    vec3 direction;
    float diffuseIntensity;
};

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform DirectionalLight directionalLight;

void main()
{
    vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;

    float diffuseFactor = dot(normalize(normal), -directionalLight.direction);
    vec4 diffuseColour = vec4(0.0f);
    if (diffuseFactor > 0)
    {
        diffuseColour = vec4(directionalLight.colour, 1.0f)
                        * directionalLight.diffuseIntensity
                        * diffuseFactor;
    }

    fragColour = texture(myTextureSampler, UV) * (ambientColour + diffuseColour);
}
