#pragma once

namespace drago::renderer
{
    
class IRenderer
{
public:
    virtual ~IRenderer() = default;
    virtual void pre_frame() = 0;
    virtual void draw() = 0;
    virtual void post_frame() = 0;
    virtual void cleanup() = 0;
};

}