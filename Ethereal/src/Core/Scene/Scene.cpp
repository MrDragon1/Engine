#include "Scene.h"

#include "Components.h"
#include "Ethereal/src/Base/GlobalContext.h"
#include "ScriptableEntity.h"
#include "Utils/IniParser.h"
#include "Core/Asset/AssetManager.h"

#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"
#include "pch.h"
namespace Ethereal
{
    static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType) {
        switch (bodyType) {
            case Rigidbody2DComponent::BodyType::Static:
                return b2_staticBody;
            case Rigidbody2DComponent::BodyType::Dynamic:
                return b2_dynamicBody;
            case Rigidbody2DComponent::BodyType::Kinematic:
                return b2_kinematicBody;
        }

        ET_CORE_ASSERT(false, "Unknown body type");
        return b2_staticBody;
    }

    Scene::Scene() {}

    Scene::~Scene() {}

    template <typename Component>
    static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap) {
        auto view = src.view<Component>();
        for (auto e : view) {
            UUID uuid = src.get<IDComponent>(e).ID;
            ET_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
            entt::entity dstEnttID = enttMap.at(uuid);

            auto& component = src.get<Component>(e);
            dst.emplace_or_replace<Component>(dstEnttID, component);
        }
    }

    template <typename Component>
    static void CopyComponentIfExists(Entity dst, Entity src) {
        if (src.HasComponent<Component>()) dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
    }

    Ref<Scene> Scene::Copy(Ref<Scene> other) {
        Ref<Scene> newScene = Ref<Scene>::Create();

        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        auto& srcSceneRegistry = other->m_Registry;
        auto& dstSceneRegistry = newScene->m_Registry;
        std::unordered_map<UUID, entt::entity> enttMap;

        // Create entities in new scene
        auto idView = srcSceneRegistry.view<IDComponent>();
        for (auto e : idView) {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
            const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
            Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
            enttMap[uuid] = (entt::entity)newEntity;
        }

        // Copy components (except IDComponent and TagComponent)
        CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<StaticMeshComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        return newScene;
    }

    Entity Scene::CreateEntity(const std::string& name) { return CreateEntityWithUUID(UUID(), name); }

    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "DefaultEntity" : name;
        return entity;
    }

    Entity Scene::Create3DObject(ETHEREAL_BASIC_3DOBJECT type) {
        std::string filePath;
        std::string name;
        switch (type) {
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CUBE:
                filePath = "default/cube.hsmesh";
                name = "Cube";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_SPHERE:
                filePath = "default/sphere.hsmesh";
                name = "Sphere";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CYLINDER:
                filePath = "default/cylinder.hsmesh";
                name = "Cylinder";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CONE:
                filePath = "default/cone.hsmesh";
                name = "Cone";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_TORUS:
                filePath = "default/torus.hsmesh";
                name = "Torus";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_PLANE:
                filePath = "default/plane.hsmesh";
                name = "Plane";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_MONKEY:
                filePath = "default/monkey.hsmesh";
                name = "Monkey";
                break;
            default:
                ET_CORE_ASSERT(false, "Unknown 3D object type");
                break;
        }

        Entity Object = CreateEntity(name);
        std::filesystem::path path = Project::GetActive()->GetMeshPath() / filePath;
        Ref<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>(path.string());
        auto& staticMeshComponent = Object.AddComponent<StaticMeshComponent>();
        staticMeshComponent.StaticMesh = mesh->Handle;
        staticMeshComponent.MaterialTable = Ref<MaterialTable>::Create(mesh->GetMaterials());

        return Object;
    }

    void Scene::OnUpdateRuntime(TimeStamp ts) {
        // Update Scripts
        {
            m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& scriptable) {
                if (!scriptable.Instance) {
                    scriptable.InstantiateFunction();
                    scriptable.Instance->m_Entity = Entity{entity, this};
                    scriptable.Instance->OnCreate();
                }
                scriptable.Instance->OnUpdate(ts);
            });
        }
        // Physics
        {
            const int32_t velocityIterations = 6;
            const int32_t positionIterations = 2;
            m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

            // Retrieve transform from Box2D
            auto view = m_Registry.view<Rigidbody2DComponent>();
            for (auto e : view) {
                Entity entity = {e, this};
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

                b2Body* body = (b2Body*)rb2d.Body;
                const auto& position = body->GetPosition();
                transform.Translation.x = position.x;
                transform.Translation.y = position.y;
                transform.Rotation.z = body->GetAngle();
            }
        }

        SceneCamera* mainCamera = nullptr;
        Matrix4 cameraTransform = Matrix4::IDENTITY;
        Vector3 cameraPosition = Vector3(1.0f);
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view) {
                const auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
                if (camera.Primary) {
                    mainCamera = &camera.Camera;
                    cameraTransform = transform.GetTransform();
                    cameraPosition = transform.Translation;
                    break;
                }
            }
        }

        if (mainCamera) {
            RenderSceneData renderSceneData;
            renderSceneData.ViewProjectionMatrix = mainCamera->GetProjection() * Math::Inverse(cameraTransform);
            renderSceneData.ViewMatrix = Math::Inverse(cameraTransform);
            renderSceneData.ProjectionMatrix = mainCamera->GetProjection();
            renderSceneData.CameraPosition = cameraPosition;
            if (!m_Environment) m_Environment = GlobalContext::GetRenderSystem().GetDefaultEnvironment();
            renderSceneData.Environment = m_Environment;
            SubmitRenderScene(renderSceneData);
        }
    }

    void Scene::OnUpdateEditor(TimeStamp ts, RenderSceneData& renderSceneData) {
        if (!m_Environment) m_Environment = GlobalContext::GetRenderSystem().GetDefaultEnvironment();
        renderSceneData.Environment = m_Environment;

        // TODO: Should update animation OnUpdateRuntime
        auto view = m_Registry.view<MeshComponent>();
        for (auto& entity : view) {
            const auto meshComponent = view.get<MeshComponent>(entity);
            auto mesh = AssetManager::GetAsset<Mesh>(meshComponent.Mesh);
            mesh->GetAnimator()->UpdateAnimation(ts);
        }

        SubmitRenderScene(renderSceneData);
    }

    void Scene::SubmitRenderScene(RenderSceneData& renderSceneData) {
        GlobalContext::GetRenderSystem().SubmitRenderSceneData(renderSceneData);

        auto staticMeshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
        for (auto entity : staticMeshView) {
            const auto [transformComponent, staticMeshComponent] = staticMeshView.get<TransformComponent, StaticMeshComponent>(entity);
            auto staticMesh = AssetManager::GetAsset<StaticMesh>(staticMeshComponent.StaticMesh);
            if (staticMesh && !staticMesh->IsFlagSet(AssetFlag::Missing)) {
                Entity e = Entity(entity, this);
                Matrix4 transform = transformComponent.GetTransform();  // GetWorldSpaceTransformMatrix(e);
                GlobalContext::GetRenderSystem().SubmitStaticMesh(staticMesh, staticMeshComponent.MaterialTable, (uint32_t)e, transform);
            }
        }

        auto meshView = m_Registry.view<TransformComponent, MeshComponent>();
        for (auto entity : meshView) {
            const auto [transformComponent, meshComponent] = meshView.get<TransformComponent, MeshComponent>(entity);
            auto mesh = AssetManager::GetAsset<StaticMesh>(meshComponent.Mesh);
            if (mesh && !mesh->IsFlagSet(AssetFlag::Missing)) {
                Entity e = Entity(entity, this);
                Matrix4 transform = transformComponent.GetTransform();  // GetWorldSpaceTransformMatrix(e);
                GlobalContext::GetRenderSystem().SubmitMesh(mesh, meshComponent.MaterialTable, (uint32_t)e, transform);
            }
        }
    }

    void Scene::DestroyEntity(Entity entity) { m_Registry.destroy(entity); }

    void Scene::DuplicateEntity(Entity entity) {
        std::string name = entity.GetName();
        Entity newEntity = CreateEntity(name);

        CopyComponentIfExists<TransformComponent>(newEntity, entity);
        CopyComponentIfExists<CameraComponent>(newEntity, entity);
        CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
        CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
        CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
    }

    void Scene::OnRuntimeStart() {
        m_PhysicsWorld = new b2World({0.0f, -9.8f});
        m_Registry.view<Rigidbody2DComponent, TransformComponent>().each([&](const auto e, auto& rb2d, auto& tranform) {
            Entity entity{e, this};
            b2BodyDef bodyDef;
            bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
            bodyDef.position.Set(tranform.Translation.x, tranform.Translation.y);
            bodyDef.angle = tranform.Rotation.z;

            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
            body->SetFixedRotation(rb2d.FixedRotation);
            rb2d.Body = body;

            if (entity.HasComponent<BoxCollider2DComponent>()) {
                auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

                b2PolygonShape polygonShape;
                polygonShape.SetAsBox(bc2d.Size.x * tranform.Scale.x, bc2d.Size.y * tranform.Scale.y);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &polygonShape;
                fixtureDef.friction = bc2d.Friction;
                fixtureDef.density = bc2d.Density;
                fixtureDef.restitution = bc2d.Restitution;
                fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }
        });
    }

    void Scene::OnRuntimeStop() {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        // Resize our non-FixedAspectRatio cameras
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio) cameraComponent.Camera.SetViewportSize(width, height);
        }
    }

    Entity Scene::GetPrimaryCameraEntity() {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            const auto& camera = view.get<CameraComponent>(entity);
            if (camera.Primary) return Entity{entity, this};
        }
        return {};
    }

    Entity Scene::CreateEntityWithStaticMesh(AssetHandle assetHandle) {
        if (!AssetManager::IsAssetHandleValid(assetHandle)) {
            ET_CORE_WARN("Incorrect asset input!");
            return {};
        }
        const AssetMetaData& assetData = AssetManager::GetMetadata(assetHandle);

        Entity entity = CreateEntity(assetData.FilePath.stem().string());
        auto& component = entity.AddComponent<StaticMeshComponent>();
        component.StaticMesh = assetHandle;

        auto mesh = AssetManager::GetAsset<StaticMesh>(assetHandle);
        if (mesh->GetMaterials()->GetMaterialCount() > component.MaterialTable->GetMaterialCount()) {
            component.MaterialTable->SetMaterialCount(mesh->GetMaterials()->GetMaterialCount());
        }

        // Get a material from meshComponent materialTable if it has (not the copy of the material)
        for (int index = 0; index < component.MaterialTable->GetMaterialCount(); index++) {
            if (mesh->GetMaterials()->HasMaterial(index)) {
                component.MaterialTable->SetMaterial(index, mesh->GetMaterials()->GetMaterial(index));
            }
        }
        return entity;
    }

    Entity Scene::CreateEntityWithMesh(AssetHandle assetHandle) {
        if (!AssetManager::IsAssetHandleValid(assetHandle)) {
            ET_CORE_WARN("Incorrect asset input!");
            return {};
        }
        const AssetMetaData& assetData = AssetManager::GetMetadata(assetHandle);

        Entity entity = CreateEntity(assetData.FilePath.stem().string());
        auto& component = entity.AddComponent<MeshComponent>();
        component.Mesh = assetHandle;

        auto mesh = AssetManager::GetAsset<Mesh>(assetHandle);
        if (mesh->GetMaterials()->GetMaterialCount() > component.MaterialTable->GetMaterialCount()) {
            component.MaterialTable->SetMaterialCount(mesh->GetMaterials()->GetMaterialCount());
        }

        // Get a material from meshComponent materialTable if it has (not the copy of the material)
        for (int index = 0; index < component.MaterialTable->GetMaterialCount(); index++) {
            if (mesh->GetMaterials()->HasMaterial(index)) {
                component.MaterialTable->SetMaterial(index, mesh->GetMaterials()->GetMaterial(index));
            }
        }
        return entity;
    }

    template <typename T>
    void Scene::OnComponentAdded(Entity entity, T& component) {
        static_assert(sizeof(T) == 0);
    }
    template <>
    void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component) {}
    template <>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {}
    template <>
    void Scene::OnComponentAdded<StaticMeshComponent>(Entity entity, StaticMeshComponent& component) {}
    template <>
    void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component) {}
    template <>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
        component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }
    template <>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component) {}
    template <>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {}
    template <>
    void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component) {}
    template <>
    void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component) {}

}  // namespace Ethereal