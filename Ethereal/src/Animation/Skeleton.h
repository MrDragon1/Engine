#pragma once
#include "Joint.h"
#include "Animation.h"

namespace Ethereal
{
    // The skeleton that describes how the joints connect
    class Skeleton : public RefCounted {
      public:
        Skeleton();
        void UpdatePose(AnimInterClip clip);

        std::unordered_map<size_t, glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }

      public:
        Ref<Joint> m_Root;
        std::unordered_map<size_t, Ref<Joint>> m_JointsMap;
        std::unordered_map<size_t, glm::mat4> m_FinalBoneMatrices;
        std::string m_Name;

      private:
        void CalculateMatrices(Ref<Joint> node, glm::mat4 parentTransform);
    };

}  // namespace Ethereal
