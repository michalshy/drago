#include "Image.h"

#include <cstdint>
#include <stb_image.h>
#include <vector>

namespace drago::renderer
{

Image::Image(const std::string& path)
{
    stbi_uc *pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    data = std::vector<uint8_t>(pixels, pixels + width * height * channels);
    
    stbi_image_free(pixels);
}

}