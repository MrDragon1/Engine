#include <Core/Asset/AssetManager.h>
#include "Animator.h"

namespace Ethereal
{
    Animator::Animator(Ref<Animation> anim, Ref<Skeleton> skel) {
        m_Animation = anim;
        m_Skeleton = skel;
        m_CurrentTime = 0.0f;
    }

    Animator::Animator(const Ref<Animator>& anim) {
        m_Animation = anim->m_Animation;
        m_Skeleton = anim->m_Skeleton;
        m_CurrentTime = 0.0f;
    }

    void Animator::Load(const AnimatorDesc& desc) {
        Handle = desc.Handle;
        m_Animation = AssetManager::GetAsset<Animation>(desc.Animation);
        m_Skeleton = AssetManager::GetAsset<Skeleton>(desc.Skeleton);
        m_CurrentTime = 0.0f;
    }
    void Animator::Save(AnimatorDesc& desc) {
        desc.Handle = Handle;
        desc.Animation = m_Animation->Handle;
        desc.Skeleton = m_Skeleton->Handle;
    }

    void Animator::PlayAnimation() { m_CurrentTime = 0.0f; }

    void Animator::UpdateAnimation(TimeStamp ts) {
        m_DeltaTime = ts;
        if (m_Animation) {
            m_CurrentTime += m_Animation->GetFramesPerSecond() * ts;
            m_CurrentTime = fmod(m_CurrentTime, m_Animation->GetDuration());
            // Get animation pose for current frame
            AnimInterClip clip = m_Animation->GetInterpolateClip(m_CurrentTime);
            // Update Matrices
            m_Skeleton->UpdatePose(clip);
        }
    }

}  // namespace Ethereal