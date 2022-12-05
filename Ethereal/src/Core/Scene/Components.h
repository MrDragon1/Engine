#pragma once
#include "Base/UUID.h"
#include "Core/Renderer/SceneCamera.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/MaterialAsset.h"

#include "Base/Meta/Reflection.h"
#include "Base/Meta/Raw/Camera.h"
#include "Resource/Descriptor/Material.h"

#include <functional>
#include <string>

namespace Ethereal
{
    REFLECTION_TYPE(Component)
    CLASS(Component,WhiteListFields)
    {
        REFLECTION_BODY(Component);
        public:
          virtual void PostLoad(){};
    };

    REFLECTION_TYPE(IDComponent)
    CLASS(IDComponent: public Component, WhiteListFields)
    {
        REFLECTION_BODY(IDComponent);
        public:
          META(Enable)
          UUID ID;
          IDComponent() = default;
          IDComponent(const UUID& id)
              : ID(id) {}
          IDComponent(const IDComponent& other)
              : ID(other.ID) {}
    };

    REFLECTION_TYPE(TagComponent)
    CLASS(TagComponent: public Component, WhiteListFields)
    {
        REFLECTION_BODY(TagComponent);
        public:
          META(Enable)
          std::string Tag;
          TagComponent() = default;
          TagComponent(const std::string& tag)
              : Tag(tag) {}
          TagComponent(const TagComponent& other)
              : Tag(other.Tag) {}
    };

    REFLECTION_TYPE(TransformComponent)
    CLASS(TransformComponent: public Component, WhiteListFields)
    {
        REFLECTION_BODY(TransformComponent);
        public:
          META(Enable)
          Vector3 Position{Vector3::ZERO};
          META(Enable)
          Vector3 Scale{Vector3::ONE};
          META(Enable)
          Quaternion Rotation{Quaternion::IDENTITY};

          TransformComponent() = default;
          TransformComponent(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
              : Position{position}, Scale{scale}, Rotation{rotation} {}
          TransformComponent(const TransformComponent& other)
              : Position(other.Position), Scale(other.Scale), Rotation(other.Rotation) {}

          Matrix4 getMatrix() const {
              Matrix4 temp = Math::Translate(Matrix4::IDENTITY, Position) * Matrix4(Rotation) * Math::Scale(Matrix4::IDENTITY, Scale);
              return temp;
          }
    };

    REFLECTION_TYPE(StaticMeshComponent)
    CLASS(StaticMeshComponent: public Component, WhiteListFields)
    {
        REFLECTION_BODY(StaticMeshComponent);
        public:
          META(Enable)
          AssetHandle StaticMeshHandle;
          META(Enable)
          MaterialTableRaw MaterialTableRaw;

          Ref<MaterialTable> materialTable {nullptr};
          StaticMeshComponent() = default;
          StaticMeshComponent(const StaticMeshComponent& other)
              : StaticMeshHandle(other.StaticMeshHandle), MaterialTableRaw(other.MaterialTableRaw) {}
          StaticMeshComponent(AssetHandle handle) : StaticMeshHandle(handle) {}

          void PostLoad() override;
    };

    REFLECTION_TYPE(MeshComponent)
    CLASS(MeshComponent: public Component, WhiteListFields)
    {
        REFLECTION_BODY(MeshComponent);
        public:
          META(Enable)
          AssetHandle MeshHandle;
          META(Enable)
          MaterialTableRaw MaterialTableRaw;

          Ref<MaterialTable> materialTable {nullptr};

          MeshComponent() = default;
          MeshComponent(const MeshComponent& other)
          : MeshHandle(other.MeshHandle) {}
          MeshComponent(AssetHandle handle) : MeshHandle(handle) {}

          void PostLoad() override;
    };

    REFLECTION_TYPE(CameraComponent)
    CLASS(CameraComponent: public Component, WhiteListFields)
    {
        REFLECTION_BODY(CameraComponent);
        public:
          META(Enable)
          CameraRaw Camera;
          CameraComponent() = default;
          CameraComponent(const CameraComponent& other)
              : Camera(other.Camera) {}

          void PostLoad() override {
              SceneCamera.SetProjectionType(Camera.Perspective ? SceneCamera::ProjectionType::Perspective : SceneCamera::ProjectionType::Orthographic);
              SceneCamera.SetPerspective(Camera.Fov, Camera.OrthographicNear, Camera.OrthographicFar);
              SceneCamera.SetOrthographic(Camera.OrthographicSize, Camera.Near, Camera.Far);
              SceneCamera.SetAspectRatio(Camera.AspectRatio);
          }

          SceneCamera SceneCamera;
    };


    struct Rigidbody2DComponent {
        enum class BodyType
        {
            Static = 0,
            Dynamic,
            Kinematic
        };
        BodyType Type = BodyType::Static;
        bool FixedRotation = false;

        // Storage for runtime
        void* Body = nullptr;

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
    };

    struct BoxCollider2DComponent {
        Vector2 Offset = {0.0f, 0.0f};
        Vector2 Size = {0.5f, 0.5f};

        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;

        // Storage for runtime
        void* Fixture = nullptr;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
    };

}  // namespace Ethereal