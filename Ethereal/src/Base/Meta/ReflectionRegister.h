#pragma once
namespace Ethereal
{
    namespace Reflection
    {
        class TypeMetaRegister {
          public:
            static void Register();
            static void Unregister();
        };
    }  // namespace Reflection
}  // namespace Ethereal