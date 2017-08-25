
#include "ImageLoader.h"
#include "stb_image.h"
#include <cstring>
#include <cstdlib>

Image::Image():
    pixelData(nullptr)
{
}

Image::Image(Image&& other) noexcept:
    pixelData(other.pixelData)
{
    other.pixelData = nullptr;
}

Image& Image::operator=(Image&& other) noexcept
{
    if(pixelData)
    {
        stbi_image_free(pixelData);
    }
    pixelData = other.pixelData;
    other.pixelData = nullptr;
}

Image::~Image() noexcept
{
    if(pixelData)
    {
        stbi_image_free(pixelData);
        pixelData = nullptr;
    }
}

Image LoadImageFromFilename(const char *filename)
{

}
