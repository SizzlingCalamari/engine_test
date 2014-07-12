#version 330 core

struct BaseLight
{
    vec3 colour;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight
{
    BaseLight base;
    vec3 direction;
};

struct Attenuation
{
    float constant;
    float linear;
    float exponential;
};

struct PointLight
{
    BaseLight base;
    vec3 position;
    Attenuation attenuation;
};

struct MaterialLightProps
{
    float specularIntensity;
    float specularPower;
};

vec4 CalcLightInternal(BaseLight light, vec3 lightDirection,
                       vec3 vertexNormal, vec3 vertexToEye,
                       MaterialLightProps materialProps)
{
    vertexToEye = normalize(vertexToEye);

    vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;

    float diffuseFactor = dot(vertexNormal, -lightDirection);
    vec4 diffuseColour = vec4(0.0f);
    vec4 specularColour = vec4(0.0f);
    if (diffuseFactor > 0.0f)
    {
        diffuseColour = vec4(light.colour, 1.0f)
                        * light.diffuseIntensity
                        * diffuseFactor;
        
        vec3 lightReflect = normalize(reflect(lightDirection, vertexNormal));
        float specularFactor = pow(dot(vertexToEye, lightReflect), materialProps.specularPower);
        if (specularFactor > 0.0f)
        {
            specularColour = vec4(light.colour, 1.0f)
                             * materialProps.specularIntensity
                             * specularFactor;
        }
    }

    return (ambientColour + diffuseColour + specularColour);
}

vec4 CalcDirectionalLight(DirectionalLight light,
                          vec3 vertexNormal, vec3 vertexToEye,
                          MaterialLightProps materialProps)
{
    return CalcLightInternal(light.base, light.direction,
                             vertexNormal, vertexToEye, materialProps);
}

vec4 CalcPointLight(PointLight light, vec3 lightToVertex,
                    vec3 vertexNormal, vec3 vertexToEye,
                    MaterialLightProps materialProps)
{
    vec4 colour = CalcLightInternal(light.base, normalize(lightToVertex),
                                    vertexNormal, vertexToEye, materialProps);
    float distToVertex = length(lightToVertex);
    float attenuation = light.attenuation.constant
                        + light.attenuation.linear * distToVertex
                        + light.attenuation.exponential * distToVertex * distToVertex;
    return colour / attenuation;
}

const int MAX_POINT_LIGHTS = 4;

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal;
in vec3 worldPosition;

// Ouput data
out vec4 fragColour;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform DirectionalLight g_directionalLight;
uniform int g_numPointLights;
uniform PointLight g_pointLights[MAX_POINT_LIGHTS];
uniform vec3 eyePosition_worldspace;
uniform MaterialLightProps g_materialProps;

void main()
{


    vec3 vertexNormal = normalize(normal);
    vec3 vertexToEye = normalize(eyePosition_worldspace - worldPosition);

    vec4 totalLight = CalcDirectionalLight(g_directionalLight, vertexNormal, vertexToEye, g_materialProps);
    for (int i = 0; i < g_numPointLights; ++i)
    {
        vec3 lightToVertex = (worldPosition - g_pointLights[i].position);
        totalLight += CalcPointLight(g_pointLights[i], lightToVertex, vertexNormal, vertexToEye, g_materialProps);
    }

    fragColour = texture(myTextureSampler, UV) * totalLight;
}
