#pragma once
#include "Base/Meta/Reflection.h"
#include "Base/Meta/_generated/serializer/all_serializer.h"
#include "Base/Meta/_generated/reflection/Matrix.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Quaternion.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Vector.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Transform.reflection.gen.h"

namespace Ethereal
{
    namespace Reflection
    {
        void TypeMetaRegister::Register() {
            TypeWrappersRegister::Matrix4x4_();
            TypeWrappersRegister::Quaternion();
            TypeWrappersRegister::Vector2();
            TypeWrappersRegister::Vector3();
            TypeWrappersRegister::Vector4();
            TypeWrappersRegister::Transform();
        }
    }  // namespace Reflection
}  // namespace Ethereal

