#pragma once
#include "pch.h"
#include "Animation.h"
#include "Skeleton.h"
#include "Asset/Asset.h"
#include "Core/TimeStamp.h"
namespace Ethereal
{
    // Control how to play animation
    class Animator : public Asset {
      public:
        Animator(Ref<Animation> anim, Ref<Skeleton> skel);
        Animator(const Ref<Animator>& anim);
        Animator(){};
        void PlayAnimation();
        void UpdateAnimation(TimeStamp ts);
        std::unordered_map<size_t, glm::mat4> GetFinalBoneMatrices() { return m_Skeleton->GetFinalBoneMatrices(); }

        static AssetType GetStaticType() { return AssetType::Animator; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }

      public:
        Ref<Animation> m_Animation;
        Ref<Skeleton> m_Skeleton;
        TimeStamp m_CurrentTime;
        TimeStamp m_DeltaTime;
    };

}  // namespace Ethereal
