#include "RenderSystem.hpp"
#include "Ethereal/Scene/Components.hpp"
#include "Ethereal/Scene/Entity.hpp"
#include "Ethereal/Renderer/Renderer2D.hpp"
#include "Ethereal/Scene/Scene.hpp"

#include "Ethereal/Scene/Components.hpp"
#include "Ethereal/Scene/ScriptableEntity.hpp"
//Temporary
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_fixture.h"
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
    
    void RenderSystem::OnUpdateRuntime(Timestep ts, const Ref<Scene>& scene)
    {
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
            auto view =  scene->m_Registry.view<Rigidbody2DComponent>();
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

        Camera* mainCamera = nullptr;
        glm::mat4 cameraTransform = glm::mat4(1.0f);
        {
            auto view =  scene->m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view) {
                const auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
                if (camera.Primary) {
                    // ET_CORE_INFO("Found primary camera " + m_Registry.get<TagComponent>(entity).Tag);
                    mainCamera = &camera.Camera;
                    cameraTransform = transform.GetTransform();
                    break;
                }
            }
        }

        if (mainCamera) {
            Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

            auto group =  scene->m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group) {
                const auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
            }

            Renderer2D::EndScene();
        }
    }

    void RenderSystem::OnUpdateEditor(Timestep ts, const Ref<Scene>& scene, const EditorCamera& camera) {
        m_RenderScene->Clear();
        auto group = scene->GetRegistry().group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            const auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Ref<GameObject> gameObject = CreateRef<GameObject>();
            RenderEntity renderEntity;

            Entity Entity{entity, scene.get()};
            renderEntity.m_AssetID = Entity.GetUUID();

            GameObjectTransformDesc transformDesc;
            transformDesc.Translation = transform.Translation;
            transformDesc.Rotation = transform.Rotation;
            transformDesc.Scale = transform.Scale;
            renderEntity.m_Transform_Desc = transformDesc;

            RenderMeshData renderMeshData;
            {
                BufferLayout layout = {
                    {ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float4, "a_Color"},       {ShaderDataType::Float2, "a_TexCoord"},
                    {ShaderDataType::Float, "a_TexIndex"},  {ShaderDataType::Float, "a_TilingFactor"}, {ShaderDataType::Int, "a_EntityID"},
                };
                renderMeshData.m_static_mesh_data.m_layout = layout;

                constexpr size_t quadVertexCount = 4;
                constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
                const float textureIndex = 0.0f;  // White Texture
                const float tilingFactor = 1.0f;

                renderMeshData.m_static_mesh_data.m_vertex_buffer = CreateRef<BufferData>(quadVertexCount * sizeof(QuadVertex));

                QuadVertex* QuadVertexBufferPtr = (QuadVertex*)renderMeshData.m_static_mesh_data.m_vertex_buffer->m_data;
                for (size_t i = 0; i < quadVertexCount; i++) {
                    QuadVertexBufferPtr->Position = transformDesc.GetTransform() * QuadVertexPositions[i];
                    QuadVertexBufferPtr->Color = sprite.Color;
                    QuadVertexBufferPtr->TexCoord = textureCoords[i];
                    QuadVertexBufferPtr->TexIndex = textureIndex;
                    QuadVertexBufferPtr->TilingFactor = tilingFactor;
                    QuadVertexBufferPtr->EntityID = (int)entity;
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
            gameObject->AddRenderEntity(renderEntity);
            m_RenderResource->UploadRenderResource(renderEntity, renderMeshData);
            m_RenderScene->AddGameObject(gameObject);
        }

        m_RenderScene->UpdateVisiableMeshNode(m_RenderResource);
        m_RenderScene->BeginRender(camera);
    }
}  // namespace Ethereal