#pragma once

#include <filesystem>
#include "Base/UUID.h"
#include "Base/TimeStamp.h"
#include "Core/Renderer/EditorCamera.h"
#include "Core/Renderer/Environment.h"
#include "Utils/Macro.h"
#include "entt/entt.hpp"

class b2World;
namespace Ethereal
{
    class RenderSystem;
    class Entity;
    struct ShaderCommonData;
    class Scene : public Asset {
      public:
        Scene();
        ~Scene();

        static Ref<Scene> Copy(Ref<Scene> other);

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        Entity Create3DObject(ETHEREAL_BASIC_3DOBJECT type);
        Entity CreateEntityWithStaticMesh(AssetHandle assetHandle);
        Entity CreateEntityWithMesh(AssetHandle assetHandle);
        Entity GetEntityWithUUID(UUID uuid);

        void OnUpdateRuntime(TimeStamp ts);
        void OnUpdateEditor(TimeStamp ts, ShaderCommonData& shaderCommonData);
        void SubmitRenderScene(ShaderCommonData& shaderCommonData);

        void DestroyEntity(Entity entity);
        void DuplicateEntity(Entity entity);

        void OnRuntimeStart();
        void OnRuntimeStop();

        void OnViewportResize(uint32_t width, uint32_t height);
        Entity GetPrimaryCameraEntity();

        void Load(const std::filesystem::path& url);
        void Save();


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
        std::filesystem::path m_ScenePath;

        Ref<Environment> m_Environment;

        b2World* m_PhysicsWorld = nullptr;


        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
        friend class RenderSystem;
    };
}  // namespace Ethereal