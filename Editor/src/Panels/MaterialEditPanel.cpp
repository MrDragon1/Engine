#include "MaterialEditPanel.h"
#include "Asset/AssetManager.h"
#include "Core/GlobalContext.h"
#include "imgui.h"
namespace Ethereal
{
    MaterialEditPanel::MaterialEditPanel() {
        m_CheckerBoardTexture = AssetManager::GetAsset<Texture>("buildin/textures/Checkerboard.tga").As<Texture2D>();
    }

    MaterialEditPanel::~MaterialEditPanel() {}

    void MaterialEditPanel::SetSceneContext(const Ref<Scene>& context) { m_Context = context; }

    void MaterialEditPanel::SetSelectEntity(Entity entity) { m_SelectedEntity = entity; }

    void MaterialEditPanel::OnImGuiRender(bool isOpen) {
        const bool hasValidEntity = m_SelectedEntity && m_SelectedEntity.HasComponent<StaticMeshComponent>();
        bool Open = isOpen;
        ImGui::SetNextWindowSize(ImVec2(200.0f, 300.0f), ImGuiCond_Appearing);
        if (ImGui::Begin("Materials", &Open) && hasValidEntity) {
            const bool hasStaticMesh = m_SelectedEntity.HasComponent<StaticMeshComponent>() &&
                                       AssetManager::IsAssetHandleValid(m_SelectedEntity.GetComponent<StaticMeshComponent>().StaticMesh);

            if (hasStaticMesh) {
                Ref<MaterialTable> meshMaterialTable, componentMaterialTable;

                if (m_SelectedEntity.HasComponent<StaticMeshComponent>()) {
                    const auto& staticMeshComponent = m_SelectedEntity.GetComponent<StaticMeshComponent>();
                    componentMaterialTable = staticMeshComponent.MaterialTable;
                    auto mesh = AssetManager::GetAsset<StaticMesh>(staticMeshComponent.StaticMesh);
                    if (mesh) meshMaterialTable = mesh->GetMaterials();
                }

                if (componentMaterialTable) {
                    if (meshMaterialTable) {
                        if (componentMaterialTable->GetMaterialCount() < meshMaterialTable->GetMaterialCount())
                            componentMaterialTable->SetMaterialCount(meshMaterialTable->GetMaterialCount());
                    }

                    for (size_t i = 0; i < componentMaterialTable->GetMaterialCount(); i++) {
                        bool hasComponentMaterial = componentMaterialTable->HasMaterial(i);
                        bool hasMeshMaterial = meshMaterialTable && meshMaterialTable->HasMaterial(i);

                        if (hasMeshMaterial && !hasComponentMaterial)  // Not Override
                            RenderMaterial(i, meshMaterialTable->GetMaterial(i));
                        else if (hasComponentMaterial)  // Override
                            RenderMaterial(i, componentMaterialTable->GetMaterial(i));
                    }
                }
            }
        }
        ImGui::End();
    }

    void MaterialEditPanel::RenderMaterial(size_t materialIndex, Ref<MaterialAsset> materialAsset) {
        auto& material = materialAsset;
        bool transparent = material->IsTransparent();

        std::string name = material->GetMaterial()->GetName();
        if (name.empty()) name = "Unnamed Material";

        name = fmt::format("{0}", name);

        ImGuiTreeNodeFlags treeNodeFlags =
            ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

        if (materialIndex == 0) treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

        bool open = false;
        const float framePaddingX = 6.0f;
        const float framePaddingY = 6.0f;  // affects height of the header

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{framePaddingX, framePaddingY});

        ImGui::PushID(name.c_str());
        open = ImGui::TreeNodeEx("##dummy_id", treeNodeFlags, name.c_str());
        ImGui::PopID();

        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        if (!open) return;

        // Textures ------------------------------------------------------------------------------
        {
            // Albedo
            if (ImGui::CollapsingHeader("Albedo", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

                auto& albedoColor = material->GetAlbedoColor();
                Ref<Texture2D> albedoMap = material->GetAlbedoMap();
                bool hasAlbedoMap = albedoMap && !albedoMap.EqualsObject(GlobalContext::GetRenderSystem().GetWhiteTexture());
                Ref<Texture2D> albedoUITexture = hasAlbedoMap ? albedoMap : m_CheckerBoardTexture;

                ImVec2 textureCursorPos = ImGui::GetCursorPos();
                ImGui::Image((ImTextureID)albedoUITexture->GetRendererID(), ImVec2(64, 64));

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                        AssetHandle assetHandle = *((AssetHandle*)payload->Data);
                        Ref<Asset> asset = AssetManager::GetAsset<Asset>(assetHandle);
                        if (asset && asset->GetAssetType() == AssetType::Texture) {
                            albedoMap = asset.As<Texture2D>();
                            material->SetAlbedoMap(albedoMap);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::PopStyleVar();

                if (ImGui::IsItemHovered()) {
                    if (hasAlbedoMap) {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                        ImGui::PopTextWrapPos();
                        ImGui::Image((ImTextureID)albedoUITexture->GetRendererID(), ImVec2(384, 384));
                        ImGui::EndTooltip();
                    }

                    if (ImGui::IsItemClicked()) {
                        std::string filepath = FileSystem::OpenFileDialog("").string();
                        if (!filepath.empty()) {
                            albedoMap = Texture2D::Create(filepath);
                            material->SetAlbedoMap(albedoMap);
                        }
                    }
                }

                ImGui::SameLine();
                ImVec2 properCursorPos = ImGui::GetCursorPos();
                ImGui::SetCursorPos(textureCursorPos);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                if (hasAlbedoMap && ImGui::Button("X", ImVec2(18, 18))) {
                    materialAsset->ClearAlbedoMap();
                }
                ImGui::PopStyleVar();
                ImGui::SetCursorPos(properCursorPos);

                ImGui::BeginGroup();
                bool useAlbedoMap = material->IsUseAlbedoMap();
                if (ImGui::Checkbox("Use##Albedo", &useAlbedoMap)) material->SetUseAlbedoMap(useAlbedoMap);

                if (ImGui::ColorEdit3("Color##Albedo", glm::value_ptr(albedoColor), ImGuiColorEditFlags_NoInputs))
                    material->SetAlbedoColor(albedoColor);
                float& emissive = material->GetEmission();

                ImGui::SetNextItemWidth(100.0f);
                if (ImGui::DragFloat("Emission", &emissive, 0.1f, 0.0f, 20.0f)) material->SetEmission(emissive);  // Maybe SliderFloat better?

                ImGui::EndGroup();
            }
        }
        if (!transparent) {
            {
                // Normals
                if (ImGui::CollapsingHeader("Normals", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

                    bool useNormalMap = material->IsUseNormalMap();
                    Ref<Texture2D> normalMap = material->GetNormalMap();

                    bool hasNormalMap = normalMap && !normalMap.EqualsObject(GlobalContext::GetRenderSystem().GetWhiteTexture());
                    Ref<Texture2D> normalUITexture = hasNormalMap ? normalMap : m_CheckerBoardTexture;

                    ImVec2 textureCursorPos = ImGui::GetCursorPos();

                    ImGui::Image((ImTextureID)normalUITexture->GetRendererID(), ImVec2(64, 64));

                    if (ImGui::BeginDragDropTarget()) {
                        auto payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
                        if (payload) {
                            AssetHandle assetHandle = *((AssetHandle*)payload->Data);
                            Ref<Asset> asset = AssetManager::GetAsset<Asset>(assetHandle);
                            if (asset && asset->GetAssetType() == AssetType::Texture) {
                                normalMap = asset.As<Texture2D>();
                                material->SetNormalMap(normalMap);
                                material->SetUseNormalMap(true);
                            }
                        }

                        ImGui::EndDragDropTarget();
                    }

                    ImGui::PopStyleVar();

                    if (ImGui::IsItemHovered()) {
                        if (hasNormalMap) {
                            ImGui::BeginTooltip();
                            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                            ImGui::PopTextWrapPos();
                            ImGui::Image((ImTextureID)normalUITexture->GetRendererID(), ImVec2(384, 384));
                            ImGui::EndTooltip();
                        }

                        if (ImGui::IsItemClicked()) {
                            std::string filepath = FileSystem::OpenFileDialog("").string();

                            if (!filepath.empty()) {
                                normalMap = Texture2D::Create(filepath);
                                material->SetNormalMap(normalMap);
                                material->SetUseNormalMap(true);
                            }
                        }
                    }

                    ImVec2 nextRowCursorPos = ImGui::GetCursorPos();
                    ImGui::SameLine();
                    ImVec2 properCursorPos = ImGui::GetCursorPos();
                    ImGui::SetCursorPos(textureCursorPos);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (hasNormalMap && ImGui::Button("X", ImVec2(18, 18))) {
                        materialAsset->ClearNormalMap();
                    }
                    ImGui::PopStyleVar();
                    ImGui::SetCursorPos(properCursorPos);

                    if (ImGui::Checkbox("Use##NormalMap", &useNormalMap)) material->SetUseNormalMap(useNormalMap);
                    ImGui::SetCursorPos(nextRowCursorPos);
                }
            }
            {
                // Metalness
                if (ImGui::CollapsingHeader("Metallic", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

                    float& metalnessValue = material->GetMetalness();
                    Ref<Texture2D> metalnessMap = material->GetMetalnessMap();

                    bool hasMetalnessMap = metalnessMap && !metalnessMap.EqualsObject(GlobalContext::GetRenderSystem().GetWhiteTexture());
                    Ref<Texture2D> metalnessUITexture = hasMetalnessMap ? metalnessMap : m_CheckerBoardTexture;

                    ImVec2 textureCursorPos = ImGui::GetCursorPos();

                    ImGui::Image((ImTextureID)metalnessUITexture->GetRendererID(), ImVec2(64, 64));

                    if (ImGui::BeginDragDropTarget()) {
                        auto payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
                        if (payload) {
                            AssetHandle assetHandle = *((AssetHandle*)payload->Data);
                            Ref<Asset> asset = AssetManager::GetAsset<Asset>(assetHandle);
                            if (asset && asset->GetAssetType() == AssetType::Texture) {
                                metalnessMap = asset.As<Texture2D>();
                                material->SetMetalnessMap(metalnessMap);
                            }
                        }

                        ImGui::EndDragDropTarget();
                    }

                    ImGui::PopStyleVar();

                    if (ImGui::IsItemHovered()) {
                        if (hasMetalnessMap) {
                            ImGui::BeginTooltip();
                            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                            ImGui::PopTextWrapPos();
                            ImGui::Image((ImTextureID)metalnessUITexture->GetRendererID(), ImVec2(384, 384));
                            ImGui::EndTooltip();
                        }

                        if (ImGui::IsItemClicked()) {
                            std::string filepath = FileSystem::OpenFileDialog("").string();

                            if (!filepath.empty()) {
                                metalnessMap = Texture2D::Create(filepath);
                                material->SetMetalnessMap(metalnessMap);
                            }
                        }
                    }

                    ImGui::SameLine();
                    ImVec2 properCursorPos = ImGui::GetCursorPos();
                    ImGui::SetCursorPos(textureCursorPos);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (hasMetalnessMap && ImGui::Button("X", ImVec2(18, 18))) {
                        materialAsset->ClearMetalnessMap();
                    }
                    ImGui::PopStyleVar();
                    ImGui::SetCursorPos(properCursorPos);

                    ImGui::BeginGroup();
                    bool useMetallicMap = material->IsUseMetallicMap();
                    if (ImGui::Checkbox("Use##Metallic", &useMetallicMap)) material->SetUseMetalnessMap(useMetallicMap);
                    ImGui::SetNextItemWidth(200.0f);
                    if (ImGui::SliderFloat("Metalness Value##MetalnessInput", &metalnessValue, 0.0f, 1.0f)) material->SetMetalness(metalnessValue);
                    ImGui::EndGroup();
                }
            }
            {
                // Roughness
                if (ImGui::CollapsingHeader("Roughness", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

                    float& metalnessValue = material->GetRoughness();
                    Ref<Texture2D> metalnessMap = material->GetRoughnessMap();

                    bool hasRoughnessMap = metalnessMap && !metalnessMap.EqualsObject(GlobalContext::GetRenderSystem().GetWhiteTexture());
                    Ref<Texture2D> metalnessUITexture = hasRoughnessMap ? metalnessMap : m_CheckerBoardTexture;

                    ImVec2 textureCursorPos = ImGui::GetCursorPos();

                    ImGui::Image((ImTextureID)metalnessUITexture->GetRendererID(), ImVec2(64, 64));

                    if (ImGui::BeginDragDropTarget()) {
                        auto payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
                        if (payload) {
                            AssetHandle assetHandle = *((AssetHandle*)payload->Data);
                            Ref<Asset> asset = AssetManager::GetAsset<Asset>(assetHandle);
                            if (asset && asset->GetAssetType() == AssetType::Texture) {
                                metalnessMap = asset.As<Texture2D>();
                                material->SetRoughnessMap(metalnessMap);
                            }
                        }

                        ImGui::EndDragDropTarget();
                    }

                    ImGui::PopStyleVar();

                    if (ImGui::IsItemHovered()) {
                        if (hasRoughnessMap) {
                            ImGui::BeginTooltip();
                            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                            ImGui::PopTextWrapPos();
                            ImGui::Image((ImTextureID)metalnessUITexture->GetRendererID(), ImVec2(384, 384));
                            ImGui::EndTooltip();
                        }

                        if (ImGui::IsItemClicked()) {
                            std::string filepath = FileSystem::OpenFileDialog("").string();

                            if (!filepath.empty()) {
                                metalnessMap = Texture2D::Create(filepath);
                                material->SetRoughnessMap(metalnessMap);
                            }
                        }
                    }

                    ImGui::SameLine();
                    ImVec2 properCursorPos = ImGui::GetCursorPos();
                    ImGui::SetCursorPos(textureCursorPos);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (hasRoughnessMap && ImGui::Button("X", ImVec2(18, 18))) {
                        materialAsset->ClearRoughnessMap();
                    }
                    ImGui::PopStyleVar();
                    ImGui::SetCursorPos(properCursorPos);
                    ImGui::BeginGroup();
                    bool useRoughnessMap = material->IsUseRoughnessMap();
                    if (ImGui::Checkbox("Use##Roughness", &useRoughnessMap)) material->SetUseRoughnessMap(useRoughnessMap);
                    ImGui::SetNextItemWidth(200.0f);
                    if (ImGui::SliderFloat("Roughness Value##RoughnessInput", &metalnessValue, 0.0f, 1.0f)) material->SetRoughness(metalnessValue);
                    ImGui::EndGroup();
                }
            }
            {
                // Occlusion
                if (ImGui::CollapsingHeader("Occlusion", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

                    Ref<Texture2D> occlusionMap = material->GetOcclusionMap();

                    bool hasRoughnessMap = occlusionMap && !occlusionMap.EqualsObject(GlobalContext::GetRenderSystem().GetWhiteTexture());
                    Ref<Texture2D> occlusionUITexture = hasRoughnessMap ? occlusionMap : m_CheckerBoardTexture;

                    ImVec2 textureCursorPos = ImGui::GetCursorPos();

                    ImGui::Image((ImTextureID)occlusionUITexture->GetRendererID(), ImVec2(64, 64));

                    if (ImGui::BeginDragDropTarget()) {
                        auto payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
                        if (payload) {
                            AssetHandle assetHandle = *((AssetHandle*)payload->Data);
                            Ref<Asset> asset = AssetManager::GetAsset<Asset>(assetHandle);
                            if (asset && asset->GetAssetType() == AssetType::Texture) {
                                occlusionMap = asset.As<Texture2D>();
                                material->SetOcclusionMap(occlusionMap);
                            }
                        }

                        ImGui::EndDragDropTarget();
                    }

                    ImGui::PopStyleVar();

                    if (ImGui::IsItemHovered()) {
                        if (hasRoughnessMap) {
                            ImGui::BeginTooltip();
                            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                            ImGui::PopTextWrapPos();
                            ImGui::Image((ImTextureID)occlusionUITexture->GetRendererID(), ImVec2(384, 384));
                            ImGui::EndTooltip();
                        }

                        if (ImGui::IsItemClicked()) {
                            std::string filepath = FileSystem::OpenFileDialog("").string();

                            if (!filepath.empty()) {
                                occlusionMap = Texture2D::Create(filepath);
                                material->SetOcclusionMap(occlusionMap);
                            }
                        }
                    }

                    ImGui::SameLine();
                    ImVec2 properCursorPos = ImGui::GetCursorPos();
                    ImGui::SetCursorPos(textureCursorPos);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (hasRoughnessMap && ImGui::Button("X", ImVec2(18, 18))) {
                        materialAsset->ClearRoughnessMap();
                    }
                    ImGui::PopStyleVar();
                    ImGui::SetCursorPos(properCursorPos);
                    ImGui::BeginGroup();
                    ImGui::SetNextItemWidth(200.0f);
                    bool useOcclusionMap = material->IsUseOcclusionMap();
                    if (ImGui::Checkbox("Use##Occlusion", &useOcclusionMap)) material->SetUseOcclusionMap(useOcclusionMap);
                    ImGui::EndGroup();
                }
            }
        }
        ImGui::TreePop();
    }
}  // namespace Ethereal