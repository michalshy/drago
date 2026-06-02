#pragma once

#include <glm/glm.hpp>

namespace drago::renderer
{

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 tex_coord;      
};

}