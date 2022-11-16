#pragma once
#include "Base/UUID.h"
#include "Core/Renderer/SceneCamera.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/MaterialAsset.h"

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace Ethereal
{
    struct IDComponent {
        UUID ID;

        IDComponent() = default;
        IDComponent(UUID uuid) : ID(uuid){};
        IDComponent(const IDComponent&) = default;
    };

    struct TagComponent {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag) : Tag(tag) {}
    };
    struct TransformComponent {
        Vector3 Translation = Vector3(0.0f);
        Vector3 Rotation = Vector3(0.0f);
        Vector3 Scale = Vector3(1.0f);

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;

        Matrix4 GetTransform() {
            Matrix4 rotation = Matrix4(Quaternion(Rotation));
            return Math::Translate(Matrix4::IDENTITY, Translation) * Math::Rotate(Matrix4::IDENTITY, rotation) * Math::Scale(Matrix4::IDENTITY, Scale);
        }
    };

    struct StaticMeshComponent {
        AssetHandle StaticMesh;
        Ref<Ethereal::MaterialTable> MaterialTable = Ref<Ethereal::MaterialTable>::Create();

        StaticMeshComponent() = default;
        StaticMeshComponent(const StaticMeshComponent& other)
            : StaticMesh(other.StaticMesh), MaterialTable(Ref<Ethereal::MaterialTable>::Create(other.MaterialTable)) {}
        StaticMeshComponent(AssetHandle staticMesh) : StaticMesh(staticMesh) {}
    };

    struct MeshComponent {
        AssetHandle Mesh;
        Ref<Ethereal::MaterialTable> MaterialTable = Ref<Ethereal::MaterialTable>::Create();

        MeshComponent() = default;
        MeshComponent(const MeshComponent& other) : Mesh(other.Mesh), MaterialTable(Ref<Ethereal::MaterialTable>::Create(other.MaterialTable)) {}
        MeshComponent(AssetHandle mesh) : Mesh(mesh) {}
    };

    struct CameraComponent {
        SceneCamera Camera;
        bool Primary = true;
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    class ScriptableEntity;
    struct NativeScriptComponent {
        ScriptableEntity* Instance = nullptr;

        std::function<void()> InstantiateFunction;
        std::function<void()> DestroyInstanceFunction;

        template <typename T>
        void Bind() {
            InstantiateFunction = [&]() { Instance = new T(); };
            DestroyInstanceFunction = [&]() {
                delete (T*)Instance;
                Instance = nullptr;
            };
        }
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