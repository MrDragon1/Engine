#include "ReflectionRegister.h"

#include "Utils/Yaml.h"
#include "Base/Meta/Reflection.h"
#include "Base/Meta/Serializer.h"
#include "Base/Meta/_generated/reflection/all_reflection.h"
#include "Base/Meta/_generated/serializer/all_serializer.ipp"

#include <assert.h>

namespace Ethereal {
namespace Reflection {
void TypeMetaRegister::Unregister() { TypeMetaRegisterinterface::unregisterAll(); }
}  // namespace Reflection
}  // namespace Ethereal
