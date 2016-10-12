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

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 vertexLight;

const int MAX_POINT_LIGHTS = 1;
const int MAX_SPOT_LIGHTS = 10;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 g_depthMVP;
uniform mat4 modelToWorld;

uniform sampler2D g_shadowMapSampler;
uniform DirectionalLight g_directionalLight;

uniform int g_numPointLights;
uniform PointLight g_pointLights[MAX_POINT_LIGHTS];

uniform int g_numSpotLights;
uniform SpotLight g_spotLights[MAX_SPOT_LIGHTS];

//float CalcSpec(vec3 lightDirection,
//              vec3 vertexNormal,
//              vec3 vertexToEye,
//              MaterialLightProps matProps)
//{
//    vec3 lightReflect = normalize(reflect(lightDirection, vertexNormal));
//    float specularFactor = max(0.0f, pow(dot(vertexToEye, lightReflect), materialProps.specularPower));
//    return materialProps.specularIntensity * specularFactor;
//}

float CalcSpec(
    vec3 lightDirection,
    vec3 vertexNormal,
    vec3 vertexToEye,
    MaterialLightProps matProps)
{
    return 0.0f;
}

float CalcDiffuse(
    vec3 lightDirection,
    vec3 vertexNormal,
    float diffuseIntensity)
{
    float diffuseFactor = max(0.0f, dot(vertexNormal, -lightDirection));
    return diffuseIntensity * diffuseFactor;
}

vec3 CalcLightInternal(
    BaseLight light,
    vec3 lightDirection,
    vec3 vertexNormal,
    vec3 vertexToEye,
    MaterialLightProps materialProps,
    float shadowFactor)
{
    lightDirection = normalize(lightDirection);
    vertexToEye = normalize(vertexToEye);

    float diffuse = CalcDiffuse(lightDirection, vertexNormal, light.diffuseIntensity);
    float spec = CalcSpec(lightDirection, vertexNormal, vertexToEye, materialProps);

    vec3 totalColour = light.colour * (diffuse + spec);
    return (shadowFactor * totalColour);
}

vec3 CalcDirectionalLight(DirectionalLight light,
                          vec3 vertexNormal, vec3 vertexToEye,
                          MaterialLightProps materialProps,
                          float shadowFactor)
{
    return CalcLightInternal(
        light.base, light.direction, vertexNormal,
        vertexToEye, materialProps, shadowFactor);
}

vec3 CalcPointLight(
    PointLight light,
    vec3 lightToVertex,
    vec3 vertexNormal,
    vec3 vertexToEye,
    MaterialLightProps materialProps,
    float shadowFactor)
{
    vec3 colour = CalcLightInternal(
        light.base, lightToVertex, vertexNormal,
        vertexToEye, materialProps, shadowFactor);

    float distToVertex = length(lightToVertex);
    float attenuation = light.attenuation.constant
                        + light.attenuation.linear * distToVertex
                        + light.attenuation.exponential * distToVertex * distToVertex;
    return colour / attenuation;
}

vec3 CalcSpotLight(
    SpotLight light,
    vec3 lightToVertex,
    vec3 vertexNormal,
    vec3 vertexToEye,
    MaterialLightProps materialProps,
    float shadowFactor)
{
    float spotFactor = dot(normalize(lightToVertex), normalize(light.coneDirection));
    vec3 spotColour = vec3(0.0f);
    if (spotFactor > light.cosineConeAngle)
    {
        vec3 colour = CalcPointLight(
            light.base, lightToVertex, vertexNormal,
            vertexToEye, materialProps, shadowFactor);

        spotColour = colour * (1.0f - (1.0f - spotFactor) * 1.0f / (1.0f - light.cosineConeAngle));
    }
    return spotColour;
}

uniform float g_specularIntensity;
uniform float g_specularPower;
uniform vec3 eyePosition_worldspace;

vec3 CalcVertexLight()
{
    MaterialLightProps materialProps;
    materialProps.specularIntensity = g_specularIntensity;
    materialProps.specularPower = g_specularPower;

    vec3 vertexNormal = (modelToWorld * vec4(vertexNormal_modelspace, 0.0f)).xyz;

    vec3 worldPosition = (modelToWorld * vec4(vertexPosition_modelspace, 1.0f)).xyz;
    vec3 vertexToEye = normalize(eyePosition_worldspace - worldPosition);

    vec4 directionalLightSpacePosition = g_depthMVP * vec4(vertexPosition_modelspace, 1.0f);
    float depth = texture(g_shadowMapSampler, directionalLightSpacePosition.xy).x;
    float shadowFactor = float(depth >= directionalLightSpacePosition.z);

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
    return totalLight;
}

void main()
{
    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace, 1.0f);

    // UV of the vertex. No special space for this one.
    UV = vertexUV;

    vertexLight = CalcVertexLight();
}
