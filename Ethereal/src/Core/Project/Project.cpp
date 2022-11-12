#include "pch.h"
#include "Project.h"

#include "Core/Asset/AssetManager.h"
namespace Ethereal
{

    Project::Project() {}

    Project::~Project() {}

    void Project::SetActive(Ref<Project> project) {
        if (s_ActiveProject) {
            AssetManager::Shutdown();
        }

        s_ActiveProject = project;
        if (s_ActiveProject) {
            AssetManager::Init();
        }
    }

    void Project::OnDeserialized() {}

}  // namespace Ethereal
