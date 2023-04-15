#pragma once

#include <filesystem>
#include "Base/UUID.h"
#include "Base/TimeStamp.h"
#include "Core/Renderer/EditorCamera.h"
#include "Core/Renderer/Environment.h"
#include "Utils/EngineEnum.h"
#include "entt/entt.hpp"

class b2World;
namespace Ethereal {
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
    void OnUpdateEditor(TimeStamp ts);
    void SubmitRenderScene();

    void DestroyEntity(Entity entity);
    void DuplicateEntity(Entity entity);

    void OnRuntimeStart();
    void OnRuntimeStop();

    void OnViewportResize(uint32_t width, uint32_t height);
    Entity GetPrimaryCameraEntity();

    void Load(const std::filesystem::path& url);
    void Save();

    entt::registry& GetRegistry() { return mRegistry; }
    const std::string& GetName() const { return mSceneName; }
    Ref<Environment> GetEnvironment() const { return mEnvironment; }
    void SetName(const std::string& name) { mSceneName = name; }
    void SetEnvironment(Ref<Environment> env) { mEnvironment = env; }

   private:
    template <typename T>
    void OnComponentAdded(Entity entity, T& component);

   private:
    entt::registry mRegistry;
    uint32_t mViewportWidth = 0, mViewportHeight = 0;
    std::string mSceneName = "Untitled";
    std::filesystem::path mScenePath;

    Ref<Environment> mEnvironment;

    b2World* mPhysicsWorld = nullptr;

    friend class Entity;
    friend class SceneSerializer;
    friend class SceneHierarchyPanel;
    friend class RenderSystem;
};
}  // namespace Ethereal