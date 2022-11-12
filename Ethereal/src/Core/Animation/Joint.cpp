#include "Joint.h"

namespace Ethereal
{
    void Joint::SetLocalTransform(AnimState state) {
        auto position = glm::translate(glm::mat4(1.0f), state.PositionState.Position);
        auto rotation = glm::toMat4(state.RotationState.Rotation);
        auto scale = glm::scale(glm::mat4(1.0f), state.ScaleState.Scale);
        m_LocalTransform = position * rotation * scale;
    }
}  // namespace Ethereal