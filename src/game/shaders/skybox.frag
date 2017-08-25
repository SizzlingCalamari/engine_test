#version 330 core

in vec3 TexCoord0;

out vec4 fragColour;

uniform samplerCube CubeMapTexture;

void main()
{
    fragColour = texture(CubeMapTexture, TexCoord0);
}
