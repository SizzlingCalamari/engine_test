
#include "ImageLoader.h"
#include "stb_image.h"
#include <string.h>
#include <stdlib.h>

Image::Image():
    pixelData(nullptr)
{
}

Image::Image(Image&& other):
    pixelData(other.pixelData)
{
    other.pixelData = nullptr;
}

Image& Image::operator=(Image&& other)
{
    if(pixelData)
    {
        stbi_image_free(pixelData);
    }
    pixelData = other.pixelData;
    other.pixelData = nullptr;
    return *this;
}

Image::~Image()
{
    if(pixelData)
    {
        stbi_image_free(pixelData);
        pixelData = nullptr;
    }
}

Image LoadImageFromFilename(const char *filename)
{
    return Image();
}
