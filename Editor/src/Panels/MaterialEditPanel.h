#pragma once
#include "Core/Editor/EditorPanel.h"
#include "Core/Renderer/MaterialAsset.h"
#include "Core/Scene/Entity.h"
#include "Core/Scene/Scene.h"

namespace Ethereal {
class MaterialEditPanel : public EditorPanel {
   public:
    MaterialEditPanel();
    ~MaterialEditPanel();

    void SetSceneContext(const Ref<Scene>& context);
    void OnImGuiRender(bool& isOpen) override;
    void OnEvent(Event& event) override;

   private:
    void RenderMaterial(size_t materialIndex, Ref<MaterialAsset> materialAsset);

   private:
    Ref<Scene> mContext;
    Entity mSelectedEntity;
    Ref<Texture> mCheckerBoardTexture;
};

}  // namespace Ethereal
