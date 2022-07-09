#pragma once

namespace Ethereal
{
    class RenderPass {
      public:
        RenderPass() = default;
        virtual ~RenderPass() = default;
        virtual void Draw() = 0;
        
    };
}  // namespace Ethereal