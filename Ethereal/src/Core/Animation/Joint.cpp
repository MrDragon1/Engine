#include "Joint.h"

namespace Ethereal
{
    void Joint::SetLocalTransform(AnimState state) {
        auto position = Math::Translate(Matrix4::IDENTITY, state.PositionState.Position);
        auto rotation = Math::Rotate(Matrix4::IDENTITY, state.RotationState.Rotation);
        auto scale = Math::Scale(Matrix4::IDENTITY, state.ScaleState.Scale);
        m_LocalTransform = position * rotation * scale;
    }
}  // namespace Ethereal