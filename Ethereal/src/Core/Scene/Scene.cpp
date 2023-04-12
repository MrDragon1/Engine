#include "Scene.h"

#include "Base/GlobalContext.h"
#include "Base/Meta/Raw/Scene.h"
#include "Components.h"
#include "Core/Asset/AssetManager.h"
#include "Core/Scene/Entity.h"
#include "Utils/IniParser.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"
#include "pch.h"
namespace Ethereal {
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
        const auto& name = srcSceneRegistry.get<BasicPropertyComponent>(e).Tag;
        Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
        enttMap[uuid] = (entt::entity)newEntity;
    }

    // Copy components (except IDComponent and TagComponent)
    CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    CopyComponent<StaticMeshComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    return newScene;
}

Entity Scene::CreateEntity(const std::string& name) { return CreateEntityWithUUID(UUID(), name); }

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {
    Entity entity = {m_Registry.create(), this};
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<TransformComponent>();
    auto& tag = entity.AddComponent<BasicPropertyComponent>();
    tag.Tag = name.empty() ? "DefaultEntity" : name;
    entity.SetName(tag.Tag);
    return entity;
}

Entity Scene::Create3DObject(ETHEREAL_BASIC_3DOBJECT type) {
    std::string filePath;
    std::string name;
    switch (type) {
        case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CUBE:
            filePath = "default/cube.ESMesh";
            name = "Cube";
            break;
        case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_SPHERE:
            filePath = "default/sphere.ESMesh";
            name = "Sphere";
            break;
        case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CYLINDER:
            filePath = "default/cylinder.ESMesh";
            name = "Cylinder";
            break;
        case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_CONE:
            filePath = "default/cone.ESMesh";
            name = "Cone";
            break;
        case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_TORUS:
            filePath = "default/torus.ESMesh";
            name = "Torus";
            break;
        case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_PLANE:
            filePath = "default/plane.ESMesh";
            name = "Plane";
            break;
        case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_MONKEY:
            filePath = "default/monkey.ESMesh";
            name = "Monkey";
            break;
        case ETHEREAL_BASIC_3DOBJECT::ETHEREAL_BASIC_3DOBJECT_QUAD:
            filePath = "default/quad.ESMesh";
            name = "Quad";
            break;
        default:
            ET_CORE_ASSERT(false, "Unknown 3D object type");
            break;
    }

    Entity Object = CreateEntity(name);
    std::filesystem::path path = Project::GetActive()->GetMeshPath() / filePath;
    Ref<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>(path.string());
    auto& staticMeshComponent = Object.AddComponent<StaticMeshComponent>();
    staticMeshComponent.StaticMeshHandle = mesh->Handle;
    staticMeshComponent.materialTable = Ref<MaterialTable>::Create(mesh->GetMaterials());
    staticMeshComponent.MaterialTableRaw.Materials.clear();
    for (auto& asset : staticMeshComponent.materialTable->GetMaterials()) {
        staticMeshComponent.MaterialTableRaw.Materials.push_back(asset->Handle);
    }

    return Object;
}

void Scene::OnUpdateRuntime(TimeStamp ts) {
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
            transform.Position.x = position.x;
            transform.Position.y = position.y;
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
            if (camera.Camera.Primary) {
                mainCamera = &camera.SceneCamera;
                cameraTransform = transform.getMatrix();
                cameraPosition = transform.Position;
                break;
            }
        }
    }

    if (mainCamera) {
        //            RenderSceneData renderSceneData;
        //            renderSceneData.ViewProjectionMatrix = mainCamera->GetProjection() * Math::Inverse(cameraTransform);
        //            renderSceneData.ViewMatrix = Math::Inverse(cameraTransform);
        //            renderSceneData.ProjectionMatrix = mainCamera->GetProjection();
        //            renderSceneData.CameraPosition = cameraPosition;
        //            if (!m_Environment) m_Environment = GlobalContext::GetRenderSystem().GetDefaultEnvironment();
        //            renderSceneData.Environment = m_Environment;
        //            SubmitRenderScene(renderSceneData);
    }
}

void Scene::OnUpdateEditor(TimeStamp ts) {
    if (!m_Environment) m_Environment = GlobalContext::GetRenderSystem().GetDefaultEnvironment();
    SceneParam& sp = Project::GetConfigManager().sUniformManagerConfig.SceneParam;
    sp.Environment = m_Environment;

    // TODO: Should update animation OnUpdateRuntime
    auto view = m_Registry.view<MeshComponent>();
    for (auto& entity : view) {
        const auto meshComponent = view.get<MeshComponent>(entity);
        if (!AssetManager::IsAssetHandleValid(meshComponent.MeshHandle)) continue;
        auto mesh = AssetManager::GetAsset<Mesh>(meshComponent.MeshHandle);
        mesh->GetAnimator()->UpdateAnimation(ts);
    }

    SubmitRenderScene();
}

void Scene::SubmitRenderScene() {
    GlobalContext::GetRenderSystem().SubmitRenderSceneData();

    auto staticMeshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
    for (auto entity : staticMeshView) {
        const auto [transformComponent, staticMeshComponent] = staticMeshView.get<TransformComponent, StaticMeshComponent>(entity);
        auto staticMesh = AssetManager::GetAsset<StaticMesh>(staticMeshComponent.StaticMeshHandle);
        if (staticMesh && !staticMesh->IsFlagSet(AssetFlag::Missing)) {
            Entity e = Entity(entity, this);
            Matrix4 transform = transformComponent.getMatrix();  // GetWorldSpaceTransformMatrix(e);
            GlobalContext::GetRenderSystem().SubmitStaticMesh(staticMesh, staticMeshComponent.materialTable, (uint32_t)e, transform);
        }
    }

    auto meshView = m_Registry.view<TransformComponent, MeshComponent>();
    for (auto entity : meshView) {
        const auto [transformComponent, meshComponent] = meshView.get<TransformComponent, MeshComponent>(entity);
        auto mesh = AssetManager::GetAsset<StaticMesh>(meshComponent.MeshHandle);
        if (mesh && !mesh->IsFlagSet(AssetFlag::Missing)) {
            Entity e = Entity(entity, this);
            Matrix4 transform = transformComponent.getMatrix();  // GetWorldSpaceTransformMatrix(e);
            GlobalContext::GetRenderSystem().SubmitMesh(mesh, meshComponent.materialTable, (uint32_t)e, transform);
        }
    }

    GlobalContext::GetUniformManager()->UpdateScene();
    GlobalContext::GetUniformManager()->UpdateCamera();
    GlobalContext::GetUniformManager()->UpdateFog();
    GlobalContext::GetUniformManager()->UpdateLight();
}

void Scene::DestroyEntity(Entity entity) { m_Registry.destroy(entity); }

void Scene::DuplicateEntity(Entity entity) {
    std::string name = entity.GetName();
    Entity newEntity = CreateEntity(name);

    CopyComponentIfExists<TransformComponent>(newEntity, entity);
    CopyComponentIfExists<CameraComponent>(newEntity, entity);
    CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
    CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
}

void Scene::OnRuntimeStart() {
    m_PhysicsWorld = new b2World({0.0f, -9.8f});
    m_Registry.view<Rigidbody2DComponent, TransformComponent>().each([&](const auto e, auto& rb2d, auto& tranform) {
        Entity entity{e, this};
        b2BodyDef bodyDef;
        bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
        bodyDef.position.Set(tranform.Position.x, tranform.Position.y);
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
        if (!cameraComponent.Camera.FixedAspectRatio) cameraComponent.SceneCamera.SetViewportSize(width, height);
    }
}

Entity Scene::GetPrimaryCameraEntity() {
    auto view = m_Registry.view<CameraComponent>();
    for (auto entity : view) {
        const auto& camera = view.get<CameraComponent>(entity);
        if (camera.Camera.Primary) return Entity{entity, this};
    }
    return {};
}

void Scene::Load(const std::filesystem::path& url) {
    ET_CORE_INFO("Loading scene from {0}", url.string());
    m_ScenePath = url;
    SceneRaw data;
    const bool IsLoadSuccess = AssetManager::LoadAsset_Ref(url, data);
    if (!IsLoadSuccess) {
        ET_CORE_ERROR("Failed to load scene: {0}", url.string());
        return;
    }
    m_SceneName = data.Name;
    m_Environment = AssetManager::GetAsset<Environment>(data.Environment);
    for (const EntityRaw entityraw : data.Entities) {
        Entity entity = {m_Registry.create(), this};
        bool is_loaded = entity.Load(entityraw);
        if (!is_loaded) {
            ET_CORE_ERROR("loading object " + entityraw.Name + " failed");
        }
    }
}

void Scene::Save() {
    SceneRaw data;
    data.Name = m_SceneName;
    data.Entities.reserve(m_Registry.size());
    data.Environment = m_Environment->Handle;
    auto view = m_Registry.view<IDComponent>();
    for (auto entity : view) {
        Entity e = {entity, this};
        EntityRaw entityRaw;
        e.Save(entityRaw);
        data.Entities.push_back(entityRaw);
    }
    AssetManager::SaveAsset_Ref(m_ScenePath, data);
}

Entity Scene::CreateEntityWithStaticMesh(AssetHandle assetHandle) {
    if (!AssetManager::IsAssetHandleValid(assetHandle)) {
        ET_CORE_WARN("Incorrect asset input!");
        return {};
    }
    const AssetMetaData& assetData = AssetManager::GetMetadata(assetHandle);

    Entity entity = CreateEntity(assetData.FilePath.stem().string());
    auto& component = entity.AddComponent<StaticMeshComponent>();
    component.StaticMeshHandle = assetHandle;

    auto mesh = AssetManager::GetAsset<StaticMesh>(assetHandle);
    StaticMeshDesc meshdesc;
    mesh->Save(meshdesc);
    component.MaterialTableRaw.Materials = meshdesc.Materials;
    component.PostLoad();

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
    component.MeshHandle = assetHandle;

    auto mesh = AssetManager::GetAsset<Mesh>(assetHandle);
    MeshDesc meshdesc;
    mesh->Save(meshdesc);
    component.MaterialTableRaw.Materials = meshdesc.Materials;
    component.PostLoad();

    return entity;
}

Entity Scene::GetEntityWithUUID(UUID uuid) {
    auto view = m_Registry.view<IDComponent>();
    for (auto entity : view) {
        const auto& id = view.get<IDComponent>(entity);
        if (id.ID == uuid) return Entity{entity, this};
    }
    return {};
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
    component.SceneCamera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
}
template <>
void Scene::OnComponentAdded<BasicPropertyComponent>(Entity entity, BasicPropertyComponent& component) {}
template <>
void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component) {}
template <>
void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component) {}

}  // namespace Ethereal