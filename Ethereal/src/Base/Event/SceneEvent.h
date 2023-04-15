#pragma once
#include "Event.h"
#include "Core/Scene/Entity.h"
#include "Core/Scene/Scene.h"
#include "Core/Editor/SelectionManager.h"

namespace Ethereal {
class SceneEvent : public Event {
   public:
    const Ref<Scene>& GetScene() const { return mScene; }
    Ref<Scene> GetScene() { return mScene; }

    EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryScene)
   protected:
    SceneEvent(const Ref<Scene>& scene) : mScene(scene) {}

    Ref<Scene> mScene;
};

class ScenePreStartEvent : public SceneEvent {
   public:
    ScenePreStartEvent(const Ref<Scene>& scene) : SceneEvent(scene) {}

    std::string ToString() const override {
        std::stringstream ss;
        ss << "ScenePreStartEvent: " << mScene->GetName();
        return ss.str();
    }

    EVENT_CLASS_TYPE(ScenePreStart)
};

class ScenePostStartEvent : public SceneEvent {
   public:
    ScenePostStartEvent(const Ref<Scene>& scene) : SceneEvent(scene) {}

    std::string ToString() const override {
        std::stringstream ss;
        ss << "ScenePostStartEvent: " << mScene->GetName();
        return ss.str();
    }

    EVENT_CLASS_TYPE(ScenePostStart)
};

class ScenePreStopEvent : public SceneEvent {
   public:
    ScenePreStopEvent(const Ref<Scene>& scene) : SceneEvent(scene) {}

    std::string ToString() const override {
        std::stringstream ss;
        ss << "ScenePreStopEvent: " << mScene->GetName();
        return ss.str();
    }

    EVENT_CLASS_TYPE(ScenePreStop)
};

class ScenePostStopEvent : public SceneEvent {
   public:
    ScenePostStopEvent(const Ref<Scene>& scene) : SceneEvent(scene) {}

    std::string ToString() const override {
        std::stringstream ss;
        ss << "ScenePostStopEvent: " << mScene->GetName();
        return ss.str();
    }

    EVENT_CLASS_TYPE(ScenePostStop)
};

// TODO(Peter): Probably move this somewhere else...
class SelectionChangedEvent : public Event {
   public:
    SelectionChangedEvent(SelectionContext contextID, UUID selectionID, bool selected)
        : mContext(contextID), mSelectionID(selectionID), mSelected(selected) {}

    SelectionContext GetContextID() const { return mContext; }
    UUID GetSelectionID() const { return mSelectionID; }
    bool IsSelected() const { return mSelected; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "EntitySelectionChangedEvent: Context(" << (int32_t)mContext << "), Selection(" << mSelectionID << "), " << mSelected;
        return ss.str();
    }

    EVENT_CLASS_CATEGORY(EventCategoryScene)
    EVENT_CLASS_TYPE(SelectionChanged)
   private:
    SelectionContext mContext;
    UUID mSelectionID;
    bool mSelected;
};
}  // namespace Ethereal