#pragma once
#include "pch.h"
#include "Base/Math/Vector.h"
#include "Core/Asset/Asset.h"
#include "Resource/Descriptor/Animation.h"
namespace Ethereal {
struct AnimInterClip {
    std::vector<AnimState> States;
};

class Animation : public Asset {
   public:
    Animation() = default;
    Animation(const Ref<Animation>& anim);
    Animation(const AnimationDesc& desc) { Load(desc); };
    AnimInterClip GetInterpolateClip(TimeStamp animationTime);

    void Load(const AnimationDesc& desc);
    void Save(AnimationDesc& desc);

    double GetFramesPerSecond() const { return mFramesPersecond; }
    TimeStamp GetDuration() const { return mDuration; }

    static AssetType GetStaticType() { return AssetType::Animation; }
    AssetType GetAssetType() const override { return GetStaticType(); }

   public:
    std::vector<AnimKeyClip> mKeyClips;
    double mFramesPersecond;  // Frame per second
    TimeStamp mDuration;      // Duration (total frames)
    std::string mName{"Default Animation"};

   private:
    std::vector<int> GetPositionIndex(TimeStamp animationTime);
    std::vector<int> GetRotationIndex(TimeStamp animationTime);
    std::vector<int> GetScaleIndex(TimeStamp animationTime);
    float GetScaleFactor(TimeStamp lastTimeStamp, TimeStamp nextTimeStamp, TimeStamp animationTime);

    AnimPositionState InterpolatePosition(int jointIndex, int keyIndex, TimeStamp animationTime);
    AnimRotationState InterpolateRotation(int jointIndex, int keyIndex, TimeStamp animationTime);
    AnimScaleState InterpolateScale(int jointIndex, int keyIndex, TimeStamp animationTime);
};

}  // namespace Ethereal
