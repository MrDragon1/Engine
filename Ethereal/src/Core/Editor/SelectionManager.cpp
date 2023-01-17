#include "pch.h"
#include "SelectionManager.h"

#include "Base/Application.h"
#include "Base/Event/SceneEvent.h"
namespace Ethereal
{
    void SelectionManager::UniqueSelect(SelectionContext contextID, UUID selectionID) {
        SelectionManager::DeselectAll();
        auto& contextSelections = s_Contexts[contextID];
        if (std::find(contextSelections.begin(), contextSelections.end(), selectionID) != contextSelections.end()) return;

        contextSelections.push_back(selectionID);
        Application::Get().DispatchEvent<SelectionChangedEvent>(contextID, selectionID, true);
    }

    void SelectionManager::Select(SelectionContext contextID, UUID selectionID) {
        auto& contextSelections = s_Contexts[contextID];
        if (std::find(contextSelections.begin(), contextSelections.end(), selectionID) != contextSelections.end()) return;

        // TODO: Maybe verify if the selectionID is already selected in another context?

        contextSelections.push_back(selectionID);
        Application::Get().DispatchEvent<SelectionChangedEvent>(contextID, selectionID, true);
    }

    bool SelectionManager::IsSelected(UUID selectionID) {
        for (const auto& [contextID, contextSelections] : s_Contexts) {
            if (std::find(contextSelections.begin(), contextSelections.end(), selectionID) != contextSelections.end()) {
                return true;
            }
        }

        return false;
    }

    bool SelectionManager::IsSelected(SelectionContext contextID, UUID selectionID) {
        const auto& contextSelections = s_Contexts[contextID];
        return std::find(contextSelections.begin(), contextSelections.end(), selectionID) != contextSelections.end();
    }

    void SelectionManager::Deselect(UUID selectionID) {
        for (auto& [contextID, contextSelections] : s_Contexts) {
            auto it = std::find(contextSelections.begin(), contextSelections.end(), selectionID);
            if (it == contextSelections.end()) continue;

            Application::Get().DispatchEvent<SelectionChangedEvent>(contextID, selectionID, false);
            contextSelections.erase(it);
            break;
        }
    }

    void SelectionManager::Deselect(SelectionContext contextID, UUID selectionID) {
        auto& contextSelections = s_Contexts[contextID];
        auto it = std::find(contextSelections.begin(), contextSelections.end(), selectionID);
        if (it == contextSelections.end()) return;

        contextSelections.erase(it);
    }

    void SelectionManager::DeselectAll() {
        for (auto& [ctxID, contextSelections] : s_Contexts) {
            for (const auto& selectionID : contextSelections) Application::Get().DispatchEvent<SelectionChangedEvent>(ctxID, selectionID, false);
            contextSelections.clear();
        }
    }

    void SelectionManager::DeselectAll(SelectionContext contextID) {
        auto& contextSelections = s_Contexts[contextID];

        for (const auto& selectionID : contextSelections) Application::Get().DispatchEvent<SelectionChangedEvent>(contextID, selectionID, false);

        contextSelections.clear();
    }

    UUID SelectionManager::GetSelection(SelectionContext context, size_t index) {
        auto& contextSelections = s_Contexts[context];
        ET_CORE_ASSERT(index >= 0 && index < contextSelections.size());
        return contextSelections[index];
    }

    size_t SelectionManager::GetSelectionCount(SelectionContext contextID) { return s_Contexts[contextID].size(); }

}  // namespace Ethereal