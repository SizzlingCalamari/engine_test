
#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x2.hpp>

struct Material
{
    Material():
        diffuseMap(0),
        diffuseSolidColour(1.0f),
        specularIntensity(1.0f),
        specularPower(32.0f),
        alphaTestValue(1.0f),
        uvTransform(1.0f),
        noiseDiffuseMap(false),
        noiseBumpMap(false),
        celShaded(false)
    {
    }

    uint diffuseMap;
    glm::vec3 diffuseSolidColour;
    float specularIntensity;
    float specularPower;
    float alphaTestValue;

    glm::mat3x2 uvTransform;

    bool noiseDiffuseMap;
    bool noiseBumpMap;
    bool celShaded;
};
