#pragma once

#include "Ethereal/Core/UUID.hpp"
#include "Ethereal/Core/Timestep.hpp"
#include "Ethereal/Renderer/EditorCamera.hpp"
#include "Ethereal/Utils/Macro.hpp"
#include "entt/entt.hpp"

class b2World;
namespace Ethereal
{
    struct SkyboxData {
        std::string BackgroundMapPath = "assets/skyboxs/Newport_Loft/Newport_Loft_8k.jpg";
        std::string EnvironmentMapPath = "assets/skyboxs/Newport_Loft/Newport_Loft_Env.hdr";
        std::string ReflectionMapPath = "assets/skyboxs/Newport_Loft/Newport_Loft_Ref.hdr";
    };

    class RenderSystem;
    class Entity;
    struct RenderSceneData;
    class Scene {
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
        const std::string& GetSkybox() const { return m_SkyboxPath; }
        void SetName(const std::string& name) { m_SceneName = name; }
        void SetSkybox(const std::string& path);

      private:
        template <typename T>
        void OnComponentAdded(Entity entity, T& component);

      private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        std::string m_SceneName = "Untitled";

        std::string m_SkyboxPath;
        SkyboxData m_SkyboxData;

        b2World* m_PhysicsWorld = nullptr;

        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
        friend class RenderSystem;
    };
}  // namespace Ethereal