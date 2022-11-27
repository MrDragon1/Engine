#pragma once
#include "src/Core/Scene/Scene.h"
#include "src/Core/Scene/Entity.h"
#include "src/Core/Renderer/MaterialAsset.h"
namespace Ethereal
{
    class MaterialEditPanel {
      public:
        MaterialEditPanel();
        ~MaterialEditPanel();

        void SetSceneContext(const Ref<Scene>& context);
        void OnImGuiRender(bool isOpen = true);
        void SetSelectEntity(Entity entity);

      private:
        void RenderMaterial(size_t materialIndex, Ref<MaterialAsset> materialAsset);

      private:
        Ref<Scene> m_Context;
        Entity m_SelectedEntity;
        Ref<Texture2D> m_CheckerBoardTexture;
    };

}  // namespace Ethereal
