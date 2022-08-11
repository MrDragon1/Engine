#include "pch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"
#include "Asset/AssetManager.h"
#include "Utils/YAMLSerializationHelpers.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Ethereal
{
    static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType) {
        switch (bodyType) {
            case Rigidbody2DComponent::BodyType::Static:
                return "Static";
            case Rigidbody2DComponent::BodyType::Dynamic:
                return "Dynamic";
            case Rigidbody2DComponent::BodyType::Kinematic:
                return "Kinematic";
        }

        ET_CORE_ASSERT(false, "Unknown body type");
        return {};
    }

    static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString) {
        if (bodyTypeString == "Static") return Rigidbody2DComponent::BodyType::Static;
        if (bodyTypeString == "Dynamic") return Rigidbody2DComponent::BodyType::Dynamic;
        if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

        ET_CORE_ASSERT(false, "Unknown body type");
        return Rigidbody2DComponent::BodyType::Static;
    }
    SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene) {}

    static void SerializeEntity(YAML::Emitter& out, Entity entity) {
        ET_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Entity must have an ID component");

        out << YAML::BeginMap;  // Entity
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

        if (entity.HasComponent<TagComponent>()) {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap;  // TagComponent

            auto& tag = entity.GetComponent<TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;

            out << YAML::EndMap;  // TagComponent
        }

        if (entity.HasComponent<StaticMeshComponent>()) {
            out << YAML::Key << "StaticMeshComponent";
            out << YAML::BeginMap;  // StaticMeshComponent

            StaticMeshComponent& smc = entity.GetComponent<StaticMeshComponent>();
            out << YAML::Key << "AssetID" << YAML::Value << smc.StaticMesh;
            auto materialTable = smc.MaterialTable;
            if (materialTable->GetMaterialCount() > 0) {
                out << YAML::Key << "MaterialTable" << YAML::Value << YAML::BeginMap;  // MaterialTable

                for (uint32_t i = 0; i < materialTable->GetMaterialCount(); i++) {
                    AssetHandle handle = (materialTable->HasMaterial(i) ? materialTable->GetMaterial(i)->Handle : (AssetHandle)0);
                    out << YAML::Key << i << YAML::Value << handle;
                }
                out << YAML::EndMap;  // MaterialTable
            }
            out << YAML::EndMap;  // StaticMeshComponent
        }

        if (entity.HasComponent<TransformComponent>()) {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;  // TransformComponent

            auto& tc = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

            out << YAML::EndMap;  // TransformComponent
        }

        if (entity.HasComponent<CameraComponent>()) {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap;  // CameraComponent

            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            auto& camera = cameraComponent.Camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap;  // Camera
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
            out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
            out << YAML::EndMap;  // Camera

            out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

            out << YAML::EndMap;  // CameraComponent
        }

        if (entity.HasComponent<Rigidbody2DComponent>()) {
            out << YAML::Key << "Rigidbody2DComponent";
            out << YAML::BeginMap;  // Rigidbody2DComponent

            auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
            out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
            out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.FixedRotation;

            out << YAML::EndMap;  // Rigidbody2DComponent
        }

        if (entity.HasComponent<BoxCollider2DComponent>()) {
            out << YAML::Key << "BoxCollider2DComponent";
            out << YAML::BeginMap;  // BoxCollider2DComponent

            auto& bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
            out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.Offset;
            out << YAML::Key << "Size" << YAML::Value << bc2dComponent.Size;
            out << YAML::Key << "Density" << YAML::Value << bc2dComponent.Density;
            out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.RestitutionThreshold;

            out << YAML::EndMap;  // BoxCollider2DComponent
        }
        out << YAML::EndMap;  // Entity
    }

    void SceneSerializer::Serialize(const std::string& filepath) {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "Scene";
        out << YAML::BeginMap;  // TransformComponent
        out << YAML::Key << "Name" << YAML::Value << m_Scene->GetName();
        out << YAML::Key << "Environment" << YAML::Value << m_Scene->GetEnvironment()->Handle;
        out << YAML::EndMap;  // TransformComponent

        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_Scene->m_Registry.each([&](auto entityID) {
            Entity entity = {entityID, m_Scene.Raw()};
            if (!entity) return;

            SerializeEntity(out, entity);
        });
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    void SceneSerializer::SerializeRuntime(const std::string& filepath) {
        // Not implemented
        ET_CORE_ASSERT(false);
    }

    bool SceneSerializer::Deserialize(const std::string& filepath) {
        YAML::Node data;
        try {
            data = YAML::LoadFile(filepath);
        } catch (YAML::ParserException e) {
            ET_CORE_ERROR("Failed to load .Scene file '{0}'\n     {1}", filepath, e.what());
            return false;
        }
        if (!data["Scene"]) return false;

        std::string sceneName = data["Scene"]["Name"].as<std::string>();
        m_Scene->SetName(sceneName);
        AssetHandle assetHandle = data["Scene"]["Environment"].as<uint64_t>();
        if (AssetManager::IsAssetHandleValid(assetHandle)) {
            const AssetMetaData& metadata = AssetManager::GetMetadata(assetHandle);
            m_Scene->SetEnvironment(AssetManager::GetAsset<Environment>(assetHandle));
        } else {
            ET_CORE_WARN("No Environment Loaded");
        }

        ET_CORE_TRACE("Deserializing scene '{0}' {1}", sceneName, assetHandle);

        auto entities = data["Entities"];
        if (entities) {
            for (auto entity : entities) {
                uint64_t uuid = entity["Entity"].as<uint64_t>();
                std::string name;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent) name = tagComponent["Tag"].as<std::string>();

                ET_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

                Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent) {
                    // Entities always have transforms
                    auto& tc = deserializedEntity.GetComponent<TransformComponent>();
                    tc.Translation = transformComponent["Translation"].as<glm::vec3>();
                    tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                    tc.Scale = transformComponent["Scale"].as<glm::vec3>();
                }

                auto staticMeshComponent = entity["StaticMeshComponent"];
                if (staticMeshComponent) {
                    auto& component = deserializedEntity.AddComponent<StaticMeshComponent>();

                    AssetHandle assetHandle = staticMeshComponent["AssetID"].as<uint64_t>();
                    if (AssetManager::IsAssetHandleValid(assetHandle)) {
                        const AssetMetaData& metadata = AssetManager::GetMetadata(assetHandle);
                        component.StaticMesh = assetHandle;
                    }

                    if (staticMeshComponent["MaterialTable"]) {
                        YAML::Node materialTableNode = staticMeshComponent["MaterialTable"];
                        auto mesh = AssetManager::GetAsset<StaticMesh>(component.StaticMesh);
                        if (mesh->GetMaterials()->GetMaterialCount() > component.MaterialTable->GetMaterialCount()) {
                            component.MaterialTable->SetMaterialCount(mesh->GetMaterials()->GetMaterialCount());
                        }

                        // Get a material from meshComponent materialTable if it has (not the copy of the material)
                        for (int index = 0; index < component.MaterialTable->GetMaterialCount(); index++) {
                            if (mesh->GetMaterials()->HasMaterial(index)) {
                                component.MaterialTable->SetMaterial(index, mesh->GetMaterials()->GetMaterial(index));
                            }
                        }

                        // Override the materialTable
                        for (auto materialEntry : materialTableNode) {
                            auto index = materialEntry.first.as<uint32_t>();
                            auto materialAsset = materialEntry.second.as<AssetHandle>();

                            // User has specified an override material, just point to that material
                            if (materialAsset && AssetManager::IsAssetHandleValid(materialAsset))
                                component.MaterialTable->SetMaterial(index, AssetManager::GetAsset<MaterialAsset>(materialAsset));
                        }
                    }
                }

                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent) {
                    auto& cc = deserializedEntity.AddComponent<CameraComponent>();

                    auto cameraProps = cameraComponent["Camera"];
                    cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

                    cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
                    cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
                    cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

                    cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
                    cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
                    cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

                    cc.Primary = cameraComponent["Primary"].as<bool>();
                    cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
                }

                auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
                if (rigidbody2DComponent) {
                    auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
                    rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
                    rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
                }

                auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
                if (boxCollider2DComponent) {
                    auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
                    bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
                    bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
                    bc2d.Density = boxCollider2DComponent["Density"].as<float>();
                    bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
                    bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
                    bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
                }
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filepath) {
        // Not implemented
        ET_CORE_ASSERT(false);
        return false;
    }

}  // namespace Ethereal