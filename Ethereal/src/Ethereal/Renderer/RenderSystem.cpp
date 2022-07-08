#include "RenderSystem.hpp"
#include "Ethereal/Scene/Components.hpp"
#include "Ethereal/Scene/Entity.hpp"
#include "Ethereal/Renderer/Renderer2D.hpp"
#include "Ethereal/Scene/Scene.hpp"

#include "Ethereal/Scene/Components.hpp"
#include "Ethereal/Scene/ScriptableEntity.hpp"
// Temporary
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_fixture.h"

#include "Ethereal/Utils/AssetLoader.hpp"

namespace Ethereal
{

    static glm::vec4 QuadVertexPositions[4];
    RenderSystem::RenderSystem() {
        QuadVertexPositions[0] = {0.5f, 0.5f, 0.0f, 1.0f};
        QuadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
        QuadVertexPositions[2] = {-0.5f, -0.5f, 0.0f, 1.0f};
        QuadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

        m_RenderScene = CreateRef<RenderScene>("assets/shaders/Test.glsl");
        m_RenderResource = CreateRef<RenderResource>();
    }

    void RenderSystem::OnUpdateRuntime(Timestep ts, const Ref<Scene>& scene) {
        // Update Scripts
        {
            scene->m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& scriptable) {
                if (!scriptable.Instance) {
                    scriptable.InstantiateFunction();
                    scriptable.Instance->m_Entity = Entity{entity, scene.get()};
                    scriptable.Instance->OnCreate();
                }
                scriptable.Instance->OnUpdate(ts);
            });
        }
        // Physics
        {
            const int32_t velocityIterations = 6;
            const int32_t positionIterations = 2;
            scene->m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

            // Retrieve transform from Box2D
            auto view = scene->m_Registry.view<Rigidbody2DComponent>();
            for (auto e : view) {
                Entity entity = {e, scene.get()};
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
        {
            auto view = scene->m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view) {
                const auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
                if (camera.Primary) {
                    mainCamera = &camera.Camera;
                    cameraTransform = transform.GetTransform();
                    break;
                }
            }
        }

        if (mainCamera) {
            m_RenderScene->Clear();
            auto group = scene->GetRegistry().group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group) {
                const auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                Ref<GameObject> gameObject = CreateRef<GameObject>();

                RenderEntity renderEntity;
                Entity Entity{entity, scene.get()};

                CastEntityToRenderEntity(Entity, renderEntity);
                gameObject->AddRenderEntity(renderEntity);
                m_RenderScene->AddGameObject(gameObject);
            }

            m_RenderScene->UpdateVisiableMeshNode(m_RenderResource);
            m_RenderScene->BeginRender(mainCamera->GetProjection(), cameraTransform);
        }
    }

    void RenderSystem::CastEntityToRenderEntity(Entity& entity, RenderEntity& renderEntity) {
        ET_CORE_ASSERT(entity.HasComponent<TransformComponent>() && entity.HasComponent<SpriteRendererComponent>(),
                       "Entity must have TransformComponent and SpriteRendererComponent");
        TransformComponent transform = entity.GetComponent<TransformComponent>();
        renderEntity.m_InstanceID = entity.GetUUID();

        GameObjectTransformDesc transformDesc;
        transformDesc.Translation = transform.Translation;
        transformDesc.Rotation = transform.Rotation;
        transformDesc.Scale = transform.Scale;
        renderEntity.m_Transform_Desc = transformDesc;

        GameObjectMeshDesc meshDesc = entity.GetComponent<MeshComponent>().Desc;
        GameObjectMaterialDesc materialDesc = entity.GetComponent<MaterialComponent>().Desc;

        renderEntity.m_Mesh_Desc = meshDesc;
        renderEntity.m_Material_Desc = materialDesc;

        //* Load Mesh Data
        RenderMeshData renderMeshData;
        bool is_MeshLoaded = m_RenderScene->getMeshAssetIdAllocator().hasElement(meshDesc);
        if (!is_MeshLoaded) {
            renderMeshData = m_RenderResource->LoadMeshData(meshDesc, (int)(uint32_t)entity);
            ET_CORE_INFO("File path {}",meshDesc.m_filePath);
        }
        renderEntity.m_MeshAssetID = m_RenderScene->getMeshAssetIdAllocator().allocUUID(meshDesc);

        //* Load Material Data
        RenderMaterialData renderMaterialData;
        bool is_MaterialLoaded = m_RenderScene->getMaterialAssetIdAllocator().hasElement(materialDesc);
        if (!is_MaterialLoaded) {
            renderMaterialData = m_RenderResource->LoadMaterialData(materialDesc);
        }
        renderEntity.m_MaterialAssetID = m_RenderScene->getMaterialAssetIdAllocator().allocUUID(materialDesc);

        if(!is_MeshLoaded) {
            m_RenderResource->UploadRenderResource(renderEntity,renderMeshData);
        }

        if(!is_MaterialLoaded) {
            m_RenderResource->UploadRenderResource(renderEntity,renderMaterialData);
        }
    }

    void RenderSystem::OnUpdateEditor(Timestep ts, const Ref<Scene>& scene, const EditorCamera& camera) {
        m_RenderScene->Clear();

        // Ref<GameObject> gameObject = CreateRef<GameObject>();
        // RenderEntity renderEntity;
        // renderEntity.m_InstanceID = 123124123;
        // // Mesh
        // renderEntity.m_MeshAssetID = 1071314643;
        // GameObjectMeshDesc desc;
        // desc.m_filePath = "assets/models/block/block.obj";
        // renderEntity.m_Mesh_Desc = desc;

        // RenderMeshData meshData = m_RenderResource->LoadMeshData(desc);

        // // Transform
        // GameObjectTransformDesc transformDesc;
        // renderEntity.m_Transform_Desc = transformDesc;

        // // Material
        // renderEntity.m_MaterialAssetID = 1071314643333;
        // GameObjectMaterialDesc materialDesc;
        // materialDesc.m_PureColor = glm::vec4(0.0, 1.0, 0.0, 1.0);
        // renderEntity.m_Material_Desc = materialDesc;

        // RenderMaterialData renderMaterialData = m_RenderResource->LoadMaterialData(materialDesc);

        // m_RenderResource->UploadRenderResource(renderEntity, meshData, renderMaterialData);
        // gameObject->AddRenderEntity(renderEntity);
        // m_RenderScene->AddGameObject(gameObject);

        auto group = scene->GetRegistry().group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            const auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Ref<GameObject> gameObject = CreateRef<GameObject>();

            RenderEntity renderEntity;
            Entity Entity{entity, scene.get()};

            CastEntityToRenderEntity(Entity, renderEntity);
            gameObject->AddRenderEntity(renderEntity);
            m_RenderScene->AddGameObject(gameObject);
        }

        m_RenderScene->UpdateVisiableMeshNode(m_RenderResource);
        m_RenderScene->BeginRender(camera);
    }
}  // namespace Ethereal