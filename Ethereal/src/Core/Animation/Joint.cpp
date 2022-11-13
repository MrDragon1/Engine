#include "Joint.h"

namespace Ethereal
{
    void Joint::SetLocalTransform(AnimState state) {
        auto position = Math::getTranslateMatrix(state.PositionState.Position);
        auto rotation = Math::getRotateMatrix(state.RotationState.Rotation);
        auto scale = Math::getScaleMatrix(state.ScaleState.Scale);
        m_LocalTransform = position * rotation * scale;
    }
}  // namespace Ethereal