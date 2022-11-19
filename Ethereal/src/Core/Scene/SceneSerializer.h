#pragma once

#include "Core/Scene/Scene.h"
#include "Base/Utils.h"

namespace Ethereal
{
    class SceneSerializer {
      public:
        SceneSerializer(const Ref<Scene>& scene);

        void Serialize(const std::string& filepath){
            m_Scene->m_ScenePath = filepath;
            m_Scene->Save();
        }
        void SerializeRuntime(const std::string& filepath);

        bool Deserialize(const std::string& filepath){
            m_Scene->Load(filepath);
            return true;
        }
        bool DeserializeRuntime(const std::string& filepath);

      private:
        Ref<Scene> m_Scene;
    };

}  // namespace Ethereal