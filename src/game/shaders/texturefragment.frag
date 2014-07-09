#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal;
in vec3 worldPosition;

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
uniform vec3 eyePosition_worldspace;
uniform float specularIntensity;
uniform float specularPower;

void main()
{
    vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;

    float diffuseFactor = dot(normalize(normal), -directionalLight.direction);
    vec4 diffuseColour = vec4(0.0f);
    vec4 specularColour = vec4(0.0f);
    if (diffuseFactor > 0.0f)
    {
        diffuseColour = vec4(directionalLight.colour, 1.0f)
                        * directionalLight.diffuseIntensity
                        * diffuseFactor;

        vec3 vertexToEye = normalize(eyePosition_worldspace - worldPosition);
        vec3 lightReflect = normalize(reflect(directionalLight.direction, normalize(normal)));
        float specularFactor = pow(dot(vertexToEye, lightReflect), specularPower);
        if (specularFactor > 0.0f)
        {
            specularColour = vec4(directionalLight.colour, 1.0f)
                             * specularIntensity
                             * specularFactor;
        }
    }

    fragColour = texture(myTextureSampler, UV) * (ambientColour + diffuseColour + specularColour);
}
