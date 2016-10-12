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

vec3 CalcAmbient(BaseLight light)
{
    return light.colour * light.ambientIntensity;
}

vec3 CalcAmbient(DirectionalLight light)
{
    return CalcAmbient(light.base);
}

vec3 CalcAmbient(PointLight light)
{
    return CalcAmbient(light.base);
}

vec3 CalcAmbient(SpotLight light)
{
    return CalcAmbient(light.base);
}

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 vertexLight;

// Ouput data
out vec4 fragColour;

const int MAX_POINT_LIGHTS = 1;
const int MAX_SPOT_LIGHTS = 10;

uniform DirectionalLight g_directionalLight;

uniform int g_numPointLights;
uniform PointLight g_pointLights[MAX_POINT_LIGHTS];

uniform int g_numSpotLights;
uniform SpotLight g_spotLights[MAX_SPOT_LIGHTS];

vec3 CalcTotalLight()
{
    vec3 ambientLight = CalcAmbient(g_directionalLight);
    //for (int i = 0; i < g_numPointLights; ++i)
    //{
    //    ambientLight += CalcAmbient(g_pointLights[i]);
    //}
    //for (int i = 0; i < g_numSpotLights; ++i)
    //{
    //    ambientLight += CalcAmbient(g_spotLights[i]);
    //}

    return vertexLight + ambientLight;
}

uniform sampler2D g_diffuseMapSampler;
uniform bool g_solidDiffuseColour;
uniform vec3 g_diffuseColour;

vec4 SampleDiffuseMap()
{
    vec4 diffuse = vec4(1.0);
    if (g_solidDiffuseColour)
    {
        diffuse = vec4(g_diffuseColour, 1.0f);
    }
    else
    {
        diffuse = texture(g_diffuseMapSampler, UV);
    }
    return diffuse;
}

void main()
{
    vec3 totalLight = CalcTotalLight();
    vec4 diffuse = SampleDiffuseMap();
    fragColour = vec4(diffuse.xyz * totalLight, diffuse.a);
}
