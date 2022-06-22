#include "SceneHierarchyPanel.hpp"

#include <imgui.h>

namespace Engine
{
    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene) {
        SetContext(scene);
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene) {
        m_Context = scene;
    }

    void SceneHierarchyPanel::OnImguiRender() {
        ImGui::Begin("Scene Hierarchy");

        m_Context->m_Registry.each([&](auto entityID) {
            Entity entity{entityID, m_Context.get()};
            DrawEntityNode(entity);
        });
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
        auto& tag = entity.GetComponent<TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str());
        if (ImGui::IsItemClicked()) {
            m_SelectionContext = entity;
        }

        if (opened) {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
            bool opened = ImGui::TreeNodeEx((void*)9817239, flags, "%s", tag.c_str());
            if (opened)
                ImGui::TreePop();
            ImGui::TreePop();
        }
    }
}  // namespace Engine