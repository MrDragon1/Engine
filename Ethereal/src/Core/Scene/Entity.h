#pragma once
#include "Core/Scene/Scene.h"
#include "Core/Scene/Components.h"
#include "Base/Meta/Raw/Scene.h"
#include "entt/entt.hpp"

namespace Ethereal {
class Entity {
   public:
    Entity() = default;
    Entity(const Entity&) = default;
    Entity(entt::entity handle, Scene* Scene);

    bool Load(const EntityRaw& raw);
    bool Save(EntityRaw& raw);

    template <typename T, typename... Args>
    T& AddComponent(Args&&... args) {
        ET_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
        T& component = mScene->mRegistry.emplace<T>(mEntity, std::forward<Args>(args)...);
        mScene->OnComponentAdded<T>(*this, component);
        return component;
    }

    template <typename T>
    T& GetComponent() {
        ET_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
        return mScene->mRegistry.get<T>(mEntity);
    }

    template <typename T>
    bool HasComponent() {
        return mScene->mRegistry.any_of<T>(mEntity);
    }

    template <typename T>
    void RemoveComponent() {
        ET_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
        mScene->mRegistry.remove<T>(mEntity);
    }

    template <typename T, typename... Args>
    T& AddOrReplaceComponent(Args&&... args) {
        T& component = mScene->mRegistry.emplace_or_replace<T>(mEntity, std::forward<Args>(args)...);
        mScene->OnComponentAdded<T>(*this, component);
        return component;
    }

    UUID GetUUID() { return GetComponent<IDComponent>().ID; };
    const std::string& GetName() {
        ET_CORE_ASSERT(HasComponent<BasicPropertyComponent>(), "Entity does not have BasicPropertyComponent!");
        return GetComponent<BasicPropertyComponent>().Tag;
    }
    void SetName(const std::string& name) {
        ET_CORE_ASSERT(HasComponent<BasicPropertyComponent>(), "Entity does not have BasicPropertyComponent!");
        GetComponent<BasicPropertyComponent>().Tag = name;
    }

    bool IsVisible() {
        ET_CORE_ASSERT(HasComponent<BasicPropertyComponent>(), "Entity does not have BasicPropertyComponent!");
        return GetComponent<BasicPropertyComponent>().Visible;
    }
    void SetVisible(bool Visible) {
        ET_CORE_ASSERT(HasComponent<BasicPropertyComponent>(), "Entity does not have BasicPropertyComponent!");
        GetComponent<BasicPropertyComponent>().Visible = Visible;
    }
    void ChangeVisible() { SetVisible(!IsVisible()); }

    operator bool() const { return mEntity != entt::null && mScene != nullptr; }
    operator uint32_t() const { return (uint32_t)mEntity; }
    operator entt::entity() const { return mEntity; }

    bool operator==(const Entity& other) const { return mEntity == other.mEntity && mScene == other.mScene; }
    bool operator!=(const Entity& other) const { return !(*this == other); }

   private:
    entt::entity mEntity = entt::null;
    Scene* mScene = nullptr;
};
}  // namespace Ethereal