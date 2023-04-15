#include <Core/Asset/AssetManager.h>
#include "Animator.h"

namespace Ethereal
{
    Animator::Animator(Ref<Animation> anim, Ref<Skeleton> skel) {
        mAnimation = anim;
        mSkeleton = skel;
        mCurrentTime = 0.0f;
    }

    Animator::Animator(const Ref<Animator>& anim) {
        mAnimation = anim->mAnimation;
        mSkeleton = anim->mSkeleton;
        mCurrentTime = 0.0f;
    }

    void Animator::Load(const AnimatorDesc& desc) {
        Handle = desc.Handle;
        mAnimation = AssetManager::GetAsset<Animation>(desc.Animation);
        mSkeleton = AssetManager::GetAsset<Skeleton>(desc.Skeleton);
        mCurrentTime = 0.0f;
    }
    void Animator::Save(AnimatorDesc& desc) {
        desc.Handle = Handle;
        desc.Animation = mAnimation->Handle;
        desc.Skeleton = mSkeleton->Handle;
    }

    void Animator::PlayAnimation() { mCurrentTime = 0.0f; }

    void Animator::UpdateAnimation(TimeStamp ts) {
        mDeltaTime = ts;
        if (mAnimation) {
            mCurrentTime += mAnimation->GetFramesPerSecond() * ts;
            mCurrentTime = fmod(mCurrentTime, mAnimation->GetDuration());
            // Get animation pose for current frame
            AnimInterClip clip = mAnimation->GetInterpolateClip(mCurrentTime);
            // Update Matrices
            mSkeleton->UpdatePose(clip);
        }
    }

}  // namespace Ethereal