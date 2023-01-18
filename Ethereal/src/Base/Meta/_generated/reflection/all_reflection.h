#pragma once
#include "Base/Meta/Reflection.h"
#include "Base/Meta/_generated/serializer/all_serializer.h"
#include "Base/Meta/_generated/reflection/Quaternion.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Vector.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Material.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Matrix.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Ref.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Transform.reflection.gen.h"
#include "Base/Meta/_generated/reflection/UUID.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Asset.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Animation.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Mesh.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Camera.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Components.reflection.gen.h"
#include "Base/Meta/_generated/reflection/Scene.reflection.gen.h"

namespace Ethereal
{
    namespace Reflection
    {
        void TypeMetaRegister::Register() {
            TypeWrappersRegister::Quaternion();
            TypeWrappersRegister::Vector2();
            TypeWrappersRegister::Vector3();
            TypeWrappersRegister::Vector4();
            TypeWrappersRegister::MaterialDesc();
            TypeWrappersRegister::MaterialTableRaw();
            TypeWrappersRegister::Matrix3();
            TypeWrappersRegister::Matrix4();
            TypeWrappersRegister::RefCounted();
            TypeWrappersRegister::Transform();
            TypeWrappersRegister::UUID();
            TypeWrappersRegister::Asset();
            TypeWrappersRegister::AnimKeyClip();
            TypeWrappersRegister::AnimPositionState();
            TypeWrappersRegister::AnimRotationState();
            TypeWrappersRegister::AnimScaleState();
            TypeWrappersRegister::AnimState();
            TypeWrappersRegister::AnimationDesc();
            TypeWrappersRegister::AnimatorDesc();
            TypeWrappersRegister::JointDesc();
            TypeWrappersRegister::SkeletonDesc();
            TypeWrappersRegister::MeshDesc();
            TypeWrappersRegister::StaticMeshDesc();
            TypeWrappersRegister::CameraRaw();
            TypeWrappersRegister::BasicPropertyComponent();
            TypeWrappersRegister::CameraComponent();
            TypeWrappersRegister::Component();
            TypeWrappersRegister::IDComponent();
            TypeWrappersRegister::MeshComponent();
            TypeWrappersRegister::StaticMeshComponent();
            TypeWrappersRegister::TransformComponent();
            TypeWrappersRegister::EntityRaw();
            TypeWrappersRegister::SceneRaw();
        }
    }  // namespace Reflection
}  // namespace Ethereal

