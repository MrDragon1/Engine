#pragma once
#include "Base/Meta/Reflection.h"
#include "Base/Meta/_generated/serializer/all_serializer.h"
#include "Base/Meta/_generated/reflection/Vector.reflection.gen.h"

namespace Ethereal
{
    namespace Reflection
    {
        void TypeMetaRegister::Register() { TypeWrappersRegister::Vector2(); }
    }  // namespace Reflection
}  // namespace Ethereal
