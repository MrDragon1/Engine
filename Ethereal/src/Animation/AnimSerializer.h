#pragma once
#include <yaml-cpp/emitter.h>
#include "Animation.h"
#include "Skeleton.h"
#include "Animator.h"
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

    class SkelSerializer {
      public:
        SkelSerializer(const Ref<Skeleton>& skel) : m_Skeleton(skel){};

        void Serialize(const std::string& filepath);
        bool Deserialize(const std::string& filepath);

      private:
        Ref<Skeleton> m_Skeleton;

      private:
        void SerializeJoint(Ref<Joint> node, YAML::Emitter& out);
        Ref<Joint> DeserializeJoint(YAML::Node& node);
    };

    class AnimatorSerializer {
      public:
        AnimatorSerializer(const Ref<Animator>& anim) : m_Animator(anim){};

        void Serialize(const std::string& filepath);
        bool Deserialize(const std::string& filepath);

      private:
        Ref<Animator> m_Animator;
    };

}  // namespace Ethereal
