#include "Animation.h"

namespace Ethereal
{
    AnimInterClip Animation::GetInterpolateClip(TimeStamp ts) {
        AnimInterClip clips;
        clips.States.clear();
        for (auto& kc : m_KeyClips) {
            AnimKeyClip& interClip = clips.States.emplace_back();
            interClip.JointID = kc.JointID;

            int index = 0;
            while (index < kc.States.size() && kc.States[index].TimeStamp > ts) index++;
            ET_CORE_ASSERT(index != 0 && index < kc.States.size(), "Unexpected error when interpolate animation!")
            AnimState prev = kc.States[index - 1];
            AnimState next = kc.States[index];

            AnimState interState = InterpolateState(prev, next, ts);

            interClip.States.clear();
            interClip.States.emplace_back(interState);
        }

        return clips;
    }

    AnimState Animation::InterpolateState(AnimState prev, AnimState next, TimeStamp ts) {
        TimeStamp interFactor = (ts - prev.TimeStamp) / (next.TimeStamp - prev.TimeStamp);

        AnimState interState;
        interState.Position = glm::mix(prev.Position, next.Position, interFactor);
        interState.Rotation = glm::normalize(glm::slerp(prev.Rotation, next.Rotation, interFactor));
        interState.Scale = glm::mix(prev.Scale, next.Scale, interFactor);
        return interState;
    }
}  // namespace Ethereal