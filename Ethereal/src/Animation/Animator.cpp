#include "Animator.h"

namespace Ethereal
{
    Animator::Animator(Ref<Animation> anim) {
        m_Animation = anim;
        m_CurrentTime = 0.0f;
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