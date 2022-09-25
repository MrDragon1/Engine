#pragma once
#include "Animation.h"
namespace Ethereal
{
    class AnimSerializer {
      public:
        AnimSerializer(const Ref<Animation>& anim) : m_Animation(anim){};

        void Serialize(const std::string& filepath);
        bool Deserialize(const std::string& filepath);

      private:
        Ref<Animation> m_Animation;
    };

}  // namespace Ethereal
