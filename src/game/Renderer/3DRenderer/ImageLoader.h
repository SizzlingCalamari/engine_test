
#pragma once

#include <memory>

struct Image
{
    Image();
    Image(const Image& other) = delete;
    Image(Image&& other) noexcept;
    ~Image() noexcept;

    Image& operator=(const Image& other) = delete;
    Image& operator=(Image&& other) noexcept;

    uint8* pixelData;
};

Image LoadImageFromFilename(const char *filename);
