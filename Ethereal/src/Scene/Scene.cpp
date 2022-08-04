#include "Scene.h"

#include "Components.h"
#include "Core/GlobalContext.h"
#include "Scene/ScriptableEntity.h"
#include "Utils/IniParser.h"
#include "Asset/AssetManager.h"

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
        CopyComponent<MeshComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<MaterialComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
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
                filePath = "source/cube.obj";
                name = "Cube";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_SPHERE:
                filePath = "source/sphere.obj";
                name = "Sphere";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CYLINDER:
                filePath = "source/cylinder.obj";
                name = "Cylinder";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CONE:
                filePath = "source/cone.obj";
                name = "Cone";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_TORUS:
                filePath = "source/torus.obj";
                name = "Torus";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_PLANE:
                filePath = "source/plane.obj";
                name = "Plane";
                break;
            case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_MONKEY:
                filePath = "source/monkey.obj";
                name = "Monkey";
                break;
            default:
                ET_CORE_ASSERT(false, "Unknown 3D object type");
                break;
        }

        Entity Object = CreateEntity(name);
        std::filesystem::path path = Project::GetActive()->GetMeshPath() / filePath;
        auto meshSource = Ref<MeshSource>::Create(path.string());
        Ref<StaticMesh> mesh = AssetManager::CreateNewAsset<StaticMesh>(path.filename().string(), path.parent_path().string(), meshSource);

        auto& staticMeshComponent = Object.AddComponent<StaticMeshComponent>();
        staticMeshComponent.StaticMesh = mesh->Handle;
        staticMeshComponent.MaterialTable = Ref<MaterialTable>::Create(mesh->GetMaterials());

        return Object;
    }

    void Scene::OnUpdateRuntime(Timestep ts) {
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
        glm::mat4 cameraTransform = glm::mat4(1.0f);
        glm::vec3 cameraPosition = glm::vec3(1.0f);
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
            renderSceneData.ViewProjectionMatrix = mainCamera->GetProjection() * glm::inverse(cameraTransform);
            renderSceneData.ViewMatrix = glm::inverse(cameraTransform);
            renderSceneData.ProjectionMatrix = mainCamera->GetProjection();
            renderSceneData.CameraPosition = cameraPosition;
            renderSceneData.Skybox = m_SkyboxData;
            SubmitRenderScene(renderSceneData);
        }
    }

    void Scene::OnUpdateEditor(Timestep ts, EditorCamera& editorCamera) {
        RenderSceneData renderSceneData;
        renderSceneData.ViewProjectionMatrix = editorCamera.GetViewProjection();
        renderSceneData.ViewMatrix = editorCamera.GetViewMatrix();
        renderSceneData.ProjectionMatrix = editorCamera.GetProjection();
        renderSceneData.CameraPosition = editorCamera.GetPosition();
        renderSceneData.Skybox = m_SkyboxData;

        SubmitRenderScene(renderSceneData);
    }

    void Scene::SubmitRenderScene(RenderSceneData& renderSceneData) {
        GlobalContext::GetRenderSystem().SubmitRenderSceneData(renderSceneData);

        auto view = m_Registry.view<TransformComponent, StaticMeshComponent>();
        for (auto entity : view) {
            const auto [transformComponent, staticMeshComponent] = view.get<TransformComponent, StaticMeshComponent>(entity);
            auto staticMesh = AssetManager::GetAsset<StaticMesh>(staticMeshComponent.StaticMesh);
            if (staticMesh && !staticMesh->IsFlagSet(AssetFlag::Missing)) {
                Entity e = Entity(entity, this);
                glm::mat4 transform = transformComponent.GetTransform();  // GetWorldSpaceTransformMatrix(e);
                GlobalContext::GetRenderSystem().SubmitStaticMesh(staticMesh, staticMeshComponent.MaterialTable, (uint32_t)e, transform);
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

    void Scene::SetSkybox(const std::string& path) {
        m_SkyboxPath = path;
        GlobalContext::GetRenderSystem().m_EnvironmentMapRenderPass->Reset();
        mINI::INIFile file(path);
        mINI::INIStructure ini;
        file.read(ini);

        std::string tmppath = path;
        replace(tmppath.begin(), tmppath.end(), '\\', '/');  //替换'\'为'/'
        std::string path_prefix = tmppath.substr(0, tmppath.find_last_of('/') + 1);
        m_SkyboxData.BackgroundMapPath = path_prefix + ini["Background"]["BGfile"].substr(1, ini["Background"]["BGfile"].length() - 2);
        m_SkyboxData.EnvironmentMapPath = path_prefix + ini["Enviroment"]["EVfile"].substr(1, ini["Enviroment"]["EVfile"].length() - 2);
        m_SkyboxData.ReflectionMapPath = path_prefix + ini["Reflection"]["REFfile"].substr(1, ini["Reflection"]["REFfile"].length() - 2);
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
    void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& component) {}
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