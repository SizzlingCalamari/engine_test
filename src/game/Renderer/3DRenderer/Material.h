
#pragma once

#include <glm/vec3.hpp>

struct Material
{
    uint diffuseMap = 0;
    glm::vec3 diffuseSolidColour = glm::vec3();
    float specularIntensity = 1.0f;
    float specularPower = 32.0f;
    
    bool noiseDiffuseMap = false;
    bool noiseBumpMap = false;
    bool celShaded = false;
};
