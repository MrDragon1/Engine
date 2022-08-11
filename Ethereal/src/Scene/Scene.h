#pragma once

#include "Core/UUID.h"
#include "Core/Timestep.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Environment.h"
#include "Utils/Macro.h"
#include "entt/entt.hpp"

class b2World;
namespace Ethereal
{
    class RenderSystem;
    class Entity;
    struct RenderSceneData;
    class Scene : public Asset {
      public:
        Scene();
        ~Scene();

        static Ref<Scene> Copy(Ref<Scene> other);

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        Entity Create3DObject(ETHEREAL_BASIC_3DOBJECT type);

        void OnUpdateRuntime(Timestep ts);
        void OnUpdateEditor(Timestep ts, EditorCamera& editorCamera);
        void SubmitRenderScene(RenderSceneData& renderSceneData);

        void DestroyEntity(Entity entity);
        void DuplicateEntity(Entity entity);

        void OnRuntimeStart();
        void OnRuntimeStop();

        void OnViewportResize(uint32_t width, uint32_t height);
        Entity GetPrimaryCameraEntity();

        entt::registry& GetRegistry() { return m_Registry; }
        const std::string& GetName() const { return m_SceneName; }
        Ref<Environment> GetEnvironment() const { return m_Environment; }
        void SetName(const std::string& name) { m_SceneName = name; }
        void SetEnvironment(Ref<Environment> env) { m_Environment = env; }
      private:
        template <typename T>
        void OnComponentAdded(Entity entity, T& component);

      private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        std::string m_SceneName = "Untitled";

        Ref<Environment> m_Environment;

        b2World* m_PhysicsWorld = nullptr;

        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
        friend class RenderSystem;
    };
}  // namespace Ethereal