
#include "TextureLoader_JPG.h"
#include "stb_image.h"

Texture LoadTextureFromJPG(const char *filename)
{
    Texture tex;
    int width, height, comp;
    auto *pixels = stbi_load(filename, &width, &height, &comp, 0);
    tex.LoadTexture2D(pixels, comp*sizeof(uint8), width, height);
    stbi_image_free(pixels);
    return tex;
}
