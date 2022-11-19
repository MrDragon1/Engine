#pragma once
#include "Base/Meta/Reflection.h"
#include "Base/Meta/_generated/serializer/all_serializer.h"
#include "Base/Meta/_generated/reflection/Components.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Quaternion.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Mesh.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Vector.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Matrix.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Transform.reflection.gen.h"
#include "Base/Meta/_generated/reflection/UUID.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Camera.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Scene.reflection.gen.h"

namespace Ethereal
{
    namespace Reflection
    {
        void TypeMetaRegister::Register() {
            TypeWrappersRegister::CameraComponent();
            TypeWrappersRegister::Component();
            TypeWrappersRegister::IDComponent();
            TypeWrappersRegister::MeshComponent();
            TypeWrappersRegister::StaticMeshComponent();
            TypeWrappersRegister::TagComponent();
            TypeWrappersRegister::TransformComponent();
            TypeWrappersRegister::Quaternion();
            TypeWrappersRegister::MaterialTableRaw();
            TypeWrappersRegister::Vector2();
            TypeWrappersRegister::Vector3();
            TypeWrappersRegister::Vector4();
            TypeWrappersRegister::Matrix3();
            TypeWrappersRegister::Matrix4();
            TypeWrappersRegister::Transform();
            TypeWrappersRegister::UUID();
            TypeWrappersRegister::CameraRaw();
            TypeWrappersRegister::EntityRaw();
            TypeWrappersRegister::SceneRaw();
        }
    }  // namespace Reflection
}  // namespace Ethereal

