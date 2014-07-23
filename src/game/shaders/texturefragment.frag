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

struct SpotLight
{
    PointLight base;
    vec3 coneDirection;
    float cosineConeAngle;
};

struct MaterialLightProps
{
    float specularIntensity;
    float specularPower;
};

vec3 CalcLightInternal(BaseLight light, vec3 lightDirection,
                       vec3 vertexNormal, vec3 vertexToEye,
                       MaterialLightProps materialProps,
                       float shadowFactor)
{
    lightDirection = normalize(lightDirection);
    vertexToEye = normalize(vertexToEye);

    vec3 ambientColour = light.colour * light.ambientIntensity;

    float diffuseFactor = dot(vertexNormal, -lightDirection);
    vec3 diffuseColour = vec3(0.0f);
    vec3 specularColour = vec3(0.0f);
    diffuseFactor = 0.5f * step(0.3f, diffuseFactor)
                    + 1.0f * step(0.6f, diffuseFactor);
    if (diffuseFactor > 0.0f)
    {
        diffuseColour = light.colour
                        * light.diffuseIntensity
                        * diffuseFactor;
        
        vec3 lightReflect = normalize(reflect(lightDirection, vertexNormal));
        float specularFactor = pow(dot(vertexToEye, lightReflect), materialProps.specularPower);
        if (specularFactor > 0.0f)
        {
            specularColour = light.colour
                             * materialProps.specularIntensity
                             * specularFactor;
        }
    }

    return (ambientColour + shadowFactor*(diffuseColour + specularColour));
}

vec3 CalcDirectionalLight(DirectionalLight light,
                          vec3 vertexNormal, vec3 vertexToEye,
                          MaterialLightProps materialProps,
                          float shadowFactor)
{
    return CalcLightInternal(light.base, light.direction,
                             vertexNormal, vertexToEye, materialProps, shadowFactor);
}

vec3 CalcPointLight(PointLight light, vec3 lightToVertex,
                    vec3 vertexNormal, vec3 vertexToEye,
                    MaterialLightProps materialProps,
                    float shadowFactor)
{
    vec3 colour = CalcLightInternal(light.base, lightToVertex,
                                    vertexNormal, vertexToEye, materialProps, shadowFactor);
    float distToVertex = length(lightToVertex);
    float attenuation = light.attenuation.constant
                        + light.attenuation.linear * distToVertex
                        + light.attenuation.exponential * distToVertex * distToVertex;
    return colour / attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 lightToVertex,
                   vec3 vertexNormal, vec3 vertexToEye,
                   MaterialLightProps materialProps,
                   float shadowFactor)
{
    float spotFactor = dot(normalize(lightToVertex), normalize(light.coneDirection));
    vec3 spotColour = vec3(0.0f);
    if (spotFactor > light.cosineConeAngle)
    {
        vec3 colour = CalcPointLight(light.base, lightToVertex,
                                     vertexNormal, vertexToEye, materialProps, shadowFactor);
        spotColour = colour * (1.0f - (1.0f - spotFactor) * 1.0f / (1.0f - light.cosineConeAngle));
    }
    return spotColour;
}

const int MAX_POINT_LIGHTS = 4;
const int MAX_SPOT_LIGHTS = 4;

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal;
in vec3 worldPosition;
in vec4 directionalLightSpacePosition;

// Ouput data
out vec4 fragColour;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform sampler2D g_shadowMapSampler;
uniform DirectionalLight g_directionalLight;
uniform int g_numPointLights;
uniform PointLight g_pointLights[MAX_POINT_LIGHTS];
uniform int g_numSpotLights;
uniform SpotLight g_spotLights[MAX_SPOT_LIGHTS];
uniform vec3 eyePosition_worldspace;
uniform MaterialLightProps g_materialProps;

float snoise(vec3 v);

void main()
{
    vec3 vertexNormal = vec3(normal.x*(snoise(1.5f*worldPosition) * 0.5f + 1.0f),
                             normal.y*(snoise(3.2f*worldPosition) * 0.5f + 1.0f),
                             normal.z*(snoise(2.3f*worldPosition) * 0.5f + 1.0f));
    vertexNormal = normalize(vertexNormal);
    vec3 vertexToEye = normalize(eyePosition_worldspace - worldPosition);

    float shadowFactor = 1.0f;
    float depth = texture(g_shadowMapSampler, directionalLightSpacePosition.xy).x;
    if (depth < directionalLightSpacePosition.z)
    {
        shadowFactor = 0.0f;
    }

    vec3 totalLight = CalcDirectionalLight(g_directionalLight, vertexNormal, vertexToEye, g_materialProps, shadowFactor);
    for (int i = 0; i < g_numPointLights; ++i)
    {
        vec3 lightToVertex = (worldPosition - g_pointLights[i].position);
        totalLight += CalcPointLight(g_pointLights[i], lightToVertex, vertexNormal, vertexToEye, g_materialProps, 1.0f);
    }
    for (int i = 0; i < g_numSpotLights; ++i)
    {
        vec3 lightToVertex = (worldPosition - g_spotLights[i].base.position);
        totalLight += CalcSpotLight(g_spotLights[i], lightToVertex, vertexNormal, vertexToEye, g_materialProps, 1.0f);
    }

    vec3 colour = texture(myTextureSampler, UV).xyz * totalLight;
    fragColour = vec4(colour, 1.0f);
}
