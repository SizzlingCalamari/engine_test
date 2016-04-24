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

uniform bool g_solidDiffuseColour;
uniform bool g_noiseDiffuseMap;
uniform bool g_noiseBumpMap;
uniform bool g_celShaded;

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
    if (g_celShaded)
    {
        diffuseFactor = 0.5f * step(0.3f, diffuseFactor)
                        + 1.0f * step(0.8f, diffuseFactor);
    }
    diffuseFactor = max(diffuseFactor, 0.0f);

    diffuseColour = light.colour
                    * light.diffuseIntensity
                    * diffuseFactor;
        
    vec3 lightReflect = normalize(reflect(lightDirection, vertexNormal));
    float specularFactor = pow(dot(vertexToEye, lightReflect), materialProps.specularPower);
    specularFactor = max(specularFactor, 0.0f);
    if (!g_celShaded)
    {
        specularColour = light.colour
                            * materialProps.specularIntensity
                            * specularFactor;
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

const int MAX_POINT_LIGHTS = 1;
const int MAX_SPOT_LIGHTS = 10;

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal;
in vec3 worldPosition;
in vec4 directionalLightSpacePosition;
in vec3 position_noiseCoords;

// Ouput data
out vec4 fragColour;

// Values that stay constant for the whole mesh.
uniform vec3 eyePosition_worldspace;

uniform sampler2D g_shadowMapSampler;
uniform DirectionalLight g_directionalLight;

uniform int g_numPointLights;
uniform PointLight g_pointLights[MAX_POINT_LIGHTS];

uniform int g_numSpotLights;
uniform SpotLight g_spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D g_diffuseMapSampler;
uniform vec3 g_diffuseColour;

uniform float g_specularIntensity;
uniform float g_specularPower;

float snoise(vec3 v);

float turbulence(
    vec3    position,
    int     octaves,
    float   lacunarity,
    float   gain)
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

vec4 noiseDiffuseMap()
{
    float noise = turbulence(position_noiseCoords, 8, 2, 0.5);
    vec3 colour1 = vec3(0.0f);
    vec3 colour2 = vec3(1.0f);
    return vec4(mix(colour1, colour2, noise), 1.0f);
}

void main()
{
    vec3 vertexNormal = normal;
    
    // mess with the normal if bump mapping is enabled
    if (g_noiseBumpMap)
    {
        vertexNormal = vec3(normal.x*(snoise(1.5f*worldPosition) * 0.5f + 1.0f),
                            normal.y*(snoise(2.2f*worldPosition) * 0.5f + 1.0f),
                            normal.z*(snoise(5.3f*worldPosition) * 0.5f + 1.0f));
    }
    vertexNormal = normalize(vertexNormal);
    vec3 vertexToEye = normalize(eyePosition_worldspace - worldPosition);

    float depth = texture(g_shadowMapSampler, directionalLightSpacePosition.xy).x;
    float shadowFactor = float(depth >= directionalLightSpacePosition.z);

    MaterialLightProps materialProps;
    materialProps.specularIntensity = g_specularIntensity;
    materialProps.specularPower = g_specularPower;

    vec3 totalLight = CalcDirectionalLight(g_directionalLight, vertexNormal, vertexToEye, materialProps, shadowFactor);
    for (int i = 0; i < g_numPointLights; ++i)
    {
        vec3 lightToVertex = (worldPosition - g_pointLights[i].position);
        totalLight += CalcPointLight(g_pointLights[i], lightToVertex, vertexNormal, vertexToEye, materialProps, 1.0f);
    }
    for (int i = 0; i < g_numSpotLights; ++i)
    {
        vec3 lightToVertex = (worldPosition - g_spotLights[i].base.position);
        totalLight += CalcSpotLight(g_spotLights[i], lightToVertex, vertexNormal, vertexToEye, materialProps, 1.0f);
    }

    vec4 tex = vec4(1.0);
    if (g_noiseDiffuseMap)
    {
        tex = noiseDiffuseMap();
    }
    else if (g_solidDiffuseColour)
    {
        tex = vec4(g_diffuseColour, 1.0f);
    }
    else
    {
        tex = texture(g_diffuseMapSampler, UV);
    }
    fragColour = vec4(tex.xyz * totalLight, tex.a);
}
