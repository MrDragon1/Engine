#pragma once
#include "Core/Renderer/Texture.h"
#include "Resource/ResourceLoader.h"
#include "Utils/FileSystem.h"
#include "Core/Asset/AssetManager.h"
#include "Core/Renderer/Environment.h"

#include <filesystem>

namespace Ethereal
{
class RenderResource {
public:

    inline static Ref<Texture> WhiteTexture;
    inline static Ref<Texture> BlackTexture;
    inline static Ref<Texture> BRDFLutTexture;
    inline static Ref<Texture> CheckerBoardTexture;

    inline static Ref<StaticMesh> Cube;
    inline static Ref<StaticMesh> Quad;

    inline static MaterialDesc DefaultMaterialDesc;


//    inline static Ref<Environment> Environment;

    static void Init() {
        WhiteTexture = AssetManager::GetAsset<Texture>("buildin/textures/white.png");
        BlackTexture = AssetManager::GetAsset<Texture>("buildin/textures/black.png");
        BRDFLutTexture = AssetManager::GetAsset<Texture>("buildin/textures/BRDF_LUT.tga");
        CheckerBoardTexture = AssetManager::GetAsset<Texture>("buildin/textures/Checkerboard.tga");

        StaticMeshDesc desc;
        AssetManager::LoadAsset_Ref("assets/meshes/default/basicCube.ESMesh", desc);
        Cube = Ref<StaticMesh>::Create();
        Cube->Load(desc);

        AssetManager::LoadAsset_Ref("assets/meshes/default/quad.ESMesh", desc);
        Quad = Ref<StaticMesh>::Create();
        Quad->Load(desc);


        {
            DefaultMaterialDesc.Name = "DefaultMaterial";
            DefaultMaterialDesc.AlbedoMap = WhiteTexture->Handle;
            DefaultMaterialDesc.NormalMap = WhiteTexture->Handle;
            DefaultMaterialDesc.MetallicMap = WhiteTexture->Handle;
            DefaultMaterialDesc.RoughnessMap = WhiteTexture->Handle;
            DefaultMaterialDesc.OcclusionMap = WhiteTexture->Handle;
        }


    }

    static void Shutdown() {
        // Must call this function before shutdown
        WhiteTexture.Reset();
        BlackTexture.Reset();
        BRDFLutTexture.Reset();
        CheckerBoardTexture.Reset();

        Cube.Reset();
        Quad.Reset();
    }

private:
    static Ref<Texture2D> LoadTexture(const std::filesystem::path& relativePath) {
        std::filesystem::path path = std::filesystem::path("assets") /"buildin" / "Editor" / relativePath;

        if (!FileSystem::Exists(path)) {
            ET_CORE_ASSERT(false,"Failed to load icon {0}! The file doesn't exist.", path.string());
            return nullptr;
        }

        return Texture2D::Create(path.string());
    }
};
}  // namespace Ethereal
