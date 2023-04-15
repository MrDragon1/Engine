#include "pch.h"
#include "Project.h"

#include "Core/Asset/AssetManager.h"
#include "Core/Renderer/RenderResource.h"
namespace Ethereal
{

    Project::Project() {}

    Project::~Project() {}

    void Project::SetActive(Ref<Project> project) {
        if (sActiveProject) {
            RenderResource::Shutdown();
            AssetManager::Shutdown();
        }

        sActiveProject = project;
        if (sActiveProject) {
            AssetManager::Init();
            RenderResource::Init();
        }
    }

    void Project::OnDeserialized() {}

}  // namespace Ethereal
