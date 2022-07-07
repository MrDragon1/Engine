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

#include "Ethereal/Utils/ObjLoader.hpp"

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
        SpriteRendererComponent sprite = entity.GetComponent<SpriteRendererComponent>();

        renderEntity.m_AssetID = entity.GetUUID();

        GameObjectTransformDesc transformDesc;
        transformDesc.Translation = transform.Translation;
        transformDesc.Rotation = transform.Rotation;
        transformDesc.Scale = transform.Scale;
        renderEntity.m_Transform_Desc = transformDesc;

        RenderMeshData renderMeshData;
        {
            BufferLayout layout = {
                {ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"}, {ShaderDataType::Float4, "a_Color"},
                {ShaderDataType::Float2, "a_TexCoord"}, {ShaderDataType::Int, "a_EntityID"},
            };
            renderMeshData.m_static_mesh_data.m_layout = layout;

            constexpr size_t quadVertexCount = 4;
            constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
            const float textureIndex = 0.0f;  // White Texture
            const float tilingFactor = 1.0f;

            renderMeshData.m_static_mesh_data.m_vertex_buffer = CreateRef<BufferData>(quadVertexCount * sizeof(MeshVertex));

            MeshVertex* QuadVertexBufferPtr = (MeshVertex*)renderMeshData.m_static_mesh_data.m_vertex_buffer->m_data;
            for (size_t i = 0; i < quadVertexCount; i++) {
                QuadVertexBufferPtr->Position = transformDesc.GetTransform() * QuadVertexPositions[i];
                QuadVertexBufferPtr->Normal = glm::vec3(0.0f, 0.0f, 1.0f);
                QuadVertexBufferPtr->Color = sprite.Color;
                QuadVertexBufferPtr->TexCoord = textureCoords[i];
                QuadVertexBufferPtr->EntityID = (int)(uint32_t)entity;
                QuadVertexBufferPtr++;
            }
            renderMeshData.m_static_mesh_data.m_index_buffer = CreateRef<BufferData>(6 * sizeof(uint32_t));
            uint32_t* quadIndices = (uint32_t*)renderMeshData.m_static_mesh_data.m_index_buffer->m_data;
            quadIndices[0] = 0;
            quadIndices[1] = 1;
            quadIndices[2] = 3;
            quadIndices[3] = 1;
            quadIndices[4] = 2;
            quadIndices[5] = 3;
        }
        renderEntity.m_MeshData = renderMeshData;
        m_RenderResource->UploadRenderResource(renderEntity, renderMeshData);
    }

    void RenderSystem::OnUpdateEditor(Timestep ts, const Ref<Scene>& scene, const EditorCamera& camera) {
        m_RenderScene->Clear();

        //ObjLoader objLoader;
        GameObjectMeshDesc desc;
        desc.m_filePath = "assets/models/cornell_box/cornell_box.obj";

        RenderMeshData meshData;
        meshData.m_static_mesh_data.m_layout = {
            {ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"}, {ShaderDataType::Float4, "a_Color"},
            {ShaderDataType::Float2, "a_TexCoord"}, {ShaderDataType::Int, "a_EntityID"},
        };
        ObjLoader::Load(desc,  meshData.m_static_mesh_data.m_vertex_buffer,  meshData.m_static_mesh_data.m_index_buffer);

        GameObjectTransformDesc transformDesc;
        Ref<GameObject> gameObject = CreateRef<GameObject>();
        RenderEntity renderEntity;
        renderEntity.m_AssetID = 1071314643;
        renderEntity.m_Transform_Desc = transformDesc;
        renderEntity.m_MeshData = meshData;
        m_RenderResource->UploadRenderResource(renderEntity, meshData);
        gameObject->AddRenderEntity(renderEntity);
        m_RenderScene->AddGameObject(gameObject);


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