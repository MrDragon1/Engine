#pragma once
#include "Base/Math/Matrix.h"
#include "Base/Math/Quaternion.h"
#include "Base/Math/Vector.h"
#include "Base/Meta/Reflection.h"

namespace Ethereal {
REFLECTION_TYPE(Transform)
CLASS(Transform, Fields) {
    REFLECTION_BODY(Transform);

   public:
    Vector3 mPosition{Vector3::ZERO};
    Vector3 mScale{Vector3::ONE};
    Quaternion mRotation{Quaternion::IDENTITY};

    Transform() = default;
    Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale) : mPosition{position}, mScale{scale}, mRotation{rotation} {}

    Matrix4 getMatrix() const {
        Matrix4 temp = Math::Translate(Matrix4::IDENTITY, mPosition) * Matrix4(mRotation) * Math::Scale(Matrix4::IDENTITY, mScale);
        return temp;
    }
};
}  // namespace Ethereal
