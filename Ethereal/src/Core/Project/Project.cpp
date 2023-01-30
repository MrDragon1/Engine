#include "pch.h"
#include "Project.h"

#include "Core/Asset/AssetManager.h"
#include "Core/Renderer/RenderResource.h"
namespace Ethereal
{

    Project::Project() {}

    Project::~Project() {}

    void Project::SetActive(Ref<Project> project) {
        if (s_ActiveProject) {
            RenderResource::Shutdown();
            AssetManager::Shutdown();
        }

        s_ActiveProject = project;
        if (s_ActiveProject) {
            AssetManager::Init();
            RenderResource::Init();
        }
    }

    void Project::OnDeserialized() {}

}  // namespace Ethereal
