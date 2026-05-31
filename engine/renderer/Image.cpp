#include "Image.h"

#include <cstdint>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vector>

namespace drago::renderer
{

Image::Image(const std::string& path)
{
    stbi_uc *pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    data = std::vector<uint8_t>(pixels, pixels + width * height * 4);
    
    stbi_image_free(pixels);
}

}