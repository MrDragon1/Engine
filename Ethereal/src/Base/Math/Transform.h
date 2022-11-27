#pragma once
#include "Base/Math/Matrix.h"
#include "Base/Math/Quaternion.h"
#include "Base/Math/Vector.h"
#include "Base/Meta/Reflection.h"

namespace Ethereal
{
    REFLECTION_TYPE(Transform)
    CLASS(Transform, Fields) {
        REFLECTION_BODY(Transform);

      public:
        Vector3 m_position{Vector3::ZERO};
        Vector3 m_scale{Vector3::ONE};
        Quaternion m_rotation{Quaternion::IDENTITY};

        Transform() = default;
        Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
            : m_position{position}, m_scale{scale}, m_rotation{rotation} {}

        Matrix4 getMatrix() const {
            Matrix4 temp = Math::Translate(Matrix4::IDENTITY, m_position) * Matrix4(m_rotation) * Math::Scale(Matrix4::IDENTITY, m_scale);
            return temp;
        }
    };
}  // namespace Ethereal
