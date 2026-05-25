#pragma once

#include <cstdint>
#include <string>
#include <vector>
namespace drago::renderer
{
    
struct Image
{
    int width;
    int height;
    int channels;

    std::vector<uint8_t> data;

    Image(const std::string& path);
};

}