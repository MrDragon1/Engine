#pragma once
#include "pch.h"
#include "imgui.h"
#include <imgui_node_editor/imgui_node_editor.h>

#include "Utils/EngineEnum.h"

namespace Ethereal {

typedef size_t PinID;
typedef size_t NodeID;
typedef size_t LinkID;

class BlueprintNodeBuilder {
    BlueprintNodeBuilder(ImTextureID texture = nullptr, int textureWidth = 0, int textureHeight = 0);

    void Begin(NodeID id);
    void End();

    void Header(const ImVec4& color = ImVec4(1, 1, 1, 1));
    void EndHeader();

    void Input(PinID id);
    void EndInput();

    void Middle();

    void Output(PinID id);
    void EndOutput();

   private:
    enum class Stage { Invalid, Begin, Header, Content, Input, Output, Middle, End };

    bool SetStage(Stage stage);

    void Pin(PinID id, PinKind kind);
    void EndPin();

    ImTextureID HeaderTextureId;
    int HeaderTextureWidth;
    int HeaderTextureHeight;
    NodeID CurrentNodeId;
    Stage CurrentStage;
    ImU32 HeaderColor;
    ImVec2 NodeMin;
    ImVec2 NodeMax;
    ImVec2 HeaderMin;
    ImVec2 HeaderMax;
    ImVec2 ContentMin;
    ImVec2 ContentMax;
    bool HasHeader;
};

}  // namespace Ethereal