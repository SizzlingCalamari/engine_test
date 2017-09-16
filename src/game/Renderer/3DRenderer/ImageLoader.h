
#pragma once

#include <memory>

struct Image
{
    Image();
    Image(const Image& other) = delete;
    Image(Image&& other);
    ~Image();

    Image& operator=(const Image& other) = delete;
    Image& operator=(Image&& other);

    uint8* pixelData;
};

Image LoadImageFromFilename(const char *filename);
