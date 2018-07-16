
#include "TextureLoader_JPG.h"
#include "stb_image.h"
#include <string.h>
#include <stdlib.h>

// Assumes in is the same size as out.
// The pixel data from stb_image has the topleft
// as (0,0). Opengl needs the bottomleft to be (0,0).
// So flip the image data.
void VerticalShufflePixels(uint8* in, uint8* out,
                           int width, int height, int comp)
{
    int up = 0;
    int down = height - 1;
    int bytesPerLine = width*comp;
    for (; down >= 0; --down, ++up)
    {
        memcpy(out + up*bytesPerLine, in + down*bytesPerLine, bytesPerLine);
    }
}

Texture LoadTextureFromJPG(const char *filename)
{
    Texture tex;
    int width, height, comp;
    // Load the image
    auto *pixels = stbi_load(filename, &width, &height, &comp, 0);

    // Vertical flip the pixels
    uint8* flippedPixels = (uint8*)malloc(width*height*comp);
    VerticalShufflePixels(pixels, flippedPixels, width, height, comp);
    stbi_image_free(pixels);

    // Load into ogl
    tex.LoadTexture2D(flippedPixels, comp*sizeof(uint8), width, height);
    free(flippedPixels);
    return tex;
}
