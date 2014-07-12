
#pragma once

#include <glm/vec3.hpp>

struct BaseLight
{
    BaseLight():
        colour(1.0f),
        ambientIntensity(0.0f),
        diffuseIntensity(0.0f)
    {
    }

    glm::vec3 colour;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight: public BaseLight
{
    DirectionalLight(): BaseLight(),
        direction(0.0f)
    {
    }

    glm::vec3 direction;
};

struct PointLight: public BaseLight
{
    PointLight(): BaseLight(),
        position(0.0f),
        attenuation({ 1.0f, 0.0f, 0.0f })
    {
    }

    glm::vec3 position;
    struct
    {
        float constant;
        float linear;
        float exp;
    } attenuation;
};

struct SpotLight: public PointLight
{
    SpotLight(): PointLight(),
        coneDirection(0.0f),
        cosineConeAngle(glm::one_over_root_two<float>()) // 45.0f degrees
    {
    }

    glm::vec3 coneDirection;
    float cosineConeAngle;
};
