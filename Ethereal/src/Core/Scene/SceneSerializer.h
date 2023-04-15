#pragma once

#include "Core/Scene/Scene.h"
#include "Utils/EngineMarco.h"

namespace Ethereal {
class SceneSerializer {
   public:
    SceneSerializer(const Ref<Scene>& scene);

    void Serialize(const std::string& filepath) {
        mScene->mScenePath = filepath;
        mScene->Save();
    }
    void SerializeRuntime(const std::string& filepath);

    bool Deserialize(const std::string& filepath) {
        mScene->Load(filepath);
        return true;
    }
    bool DeserializeRuntime(const std::string& filepath);

   private:
    Ref<Scene> mScene;
};

}  // namespace Ethereal