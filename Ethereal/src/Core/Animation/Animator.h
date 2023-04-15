#pragma once
#include "pch.h"
#include "Core/Animation/Animation.h"
#include "Core/Animation/Skeleton.h"
#include "Core/Asset/Asset.h"
#include "Base/TimeStamp.h"
#include "Resource/Descriptor/Animation.h"

namespace Ethereal
{
    // Control how to play animation
    class Animator : public Asset {
      public:
        Animator(Ref<Animation> anim, Ref<Skeleton> skel);
        Animator(const Ref<Animator>& anim);
        Animator(){};
        Animator(const AnimatorDesc& desc) { Load(desc); };

        void Load(const AnimatorDesc& desc);
        void Save(AnimatorDesc& desc);

        void PlayAnimation();
        void UpdateAnimation(TimeStamp ts);
        std::unordered_map<size_t, Matrix4> GetFinalBoneMatrices() { return mSkeleton->GetFinalBoneMatrices(); }

        static AssetType GetStaticType() { return AssetType::Animator; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }

      public:
        Ref<Animation> mAnimation;
        Ref<Skeleton> mSkeleton;
        TimeStamp mCurrentTime;
        TimeStamp mDeltaTime;
    };

}  // namespace Ethereal
