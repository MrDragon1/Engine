#pragma once

#include "Ethereal/Core/UUID.hpp"
#include "Ethereal/Core/Timestep.hpp"
#include "Ethereal/Renderer/EditorCamera.hpp"
#include "Ethereal/Utils/Macro.hpp"
#include "entt/entt.hpp"

class b2World;
namespace Ethereal
{
    class RenderSystem;
    class Entity;
    class Scene {
      public:
        Scene();
        ~Scene();

        static Ref<Scene> Copy(Ref<Scene> other);

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        Entity Create3DObject(ETHEREAL_BASIC_3DOBJECT type);


        void OnUpdateRuntime(Timestep ts, RenderSystem& renderSystem);
        void OnUpdateEditor(Timestep ts, EditorCamera& editorCamera, RenderSystem& renderSystem);
        void SubmitRenderScene(RenderSystem& renderSystem, const glm::mat4& viewProjectionMatrix);

        void DestroyEntity(Entity entity);
        void DuplicateEntity(Entity entity);

        void OnRuntimeStart();
        void OnRuntimeStop();

        void OnViewportResize(uint32_t width, uint32_t height);
        Entity GetPrimaryCameraEntity();

        entt::registry& GetRegistry() { return m_Registry; }
        const std::string& GetName() const { return m_SceneName; }
        void SetName(const std::string& name) { m_SceneName = name; }
      private:
        template <typename T>
        void OnComponentAdded(Entity entity, T& component);

      private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        std::string m_SceneName = "Untitled";
        b2World* m_PhysicsWorld = nullptr;

        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
        friend class RenderSystem;
    };
}  // namespace Ethereal