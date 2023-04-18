#include "GraphEditor.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace Ethereal {
namespace ed = ax::NodeEditor;

BlueprintNodeBuilder::BlueprintNodeBuilder(ImTextureID texture, int textureWidth, int textureHeight)
    : HeaderTextureId(texture),
      HeaderTextureWidth(textureWidth),
      HeaderTextureHeight(textureHeight),
      CurrentNodeId(0),
      CurrentStage(Stage::Invalid),
      HasHeader(false) {}

void BlueprintNodeBuilder::Begin(NodeID id) {
    HasHeader = false;
    HeaderMin = HeaderMax = ImVec2();

    ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

    ed::BeginNode(id);

    ImGui::PushID(id);
    CurrentNodeId = id;

    SetStage(Stage::Begin);
}

void BlueprintNodeBuilder::End() {
    SetStage(Stage::End);

    ed::EndNode();

    if (ImGui::IsItemVisible()) {
        auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);

        auto drawList = ed::GetNodeBackgroundDrawList(CurrentNodeId);

        const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth * 0.5f;

        auto headerColor = IM_COL32(0, 0, 0, alpha) | (HeaderColor & IM_COL32(255, 255, 255, 0));
        if ((HeaderMax.x > HeaderMin.x) && (HeaderMax.y > HeaderMin.y) && HeaderTextureId) {
            const auto uv = ImVec2((HeaderMax.x - HeaderMin.x) / (float)(4.0f * HeaderTextureWidth),
                                   (HeaderMax.y - HeaderMin.y) / (float)(4.0f * HeaderTextureHeight));

            drawList->AddImageRounded(HeaderTextureId, HeaderMin - ImVec2(8 - halfBorderWidth, 4 - halfBorderWidth),
                                      HeaderMax + ImVec2(8 - halfBorderWidth, 0), ImVec2(0.0f, 0.0f), uv, headerColor, ed::GetStyle().NodeRounding,
                                      1 | 2);

            auto headerSeparatorMin = ImVec2(HeaderMin.x, HeaderMax.y);
            auto headerSeparatorMax = ImVec2(HeaderMax.x, HeaderMin.y);

            if ((headerSeparatorMax.x > headerSeparatorMin.x) && (headerSeparatorMax.y > headerSeparatorMin.y)) {
                drawList->AddLine(headerSeparatorMin + ImVec2(-(8 - halfBorderWidth), -0.5f),
                                  headerSeparatorMax + ImVec2((8 - halfBorderWidth), -0.5f), ImColor(255, 255, 255, 96 * alpha / (3 * 255)), 1.0f);
            }
        }
    }

    CurrentNodeId = 0;

    ImGui::PopID();

    ed::PopStyleVar();

    SetStage(Stage::Invalid);
}

void BlueprintNodeBuilder::Header(const ImVec4& color) {
    HeaderColor = ImColor(color);
    SetStage(Stage::Header);
}

void BlueprintNodeBuilder::EndHeader() { SetStage(Stage::Content); }

void BlueprintNodeBuilder::Input(PinID id) {
    if (CurrentStage == Stage::Begin) SetStage(Stage::Content);

    const auto applyPadding = (CurrentStage == Stage::Input);

    SetStage(Stage::Input);

    if (applyPadding) ImGui::Spring(0);

    Pin(id, PinKind::Input);

    ImGui::BeginHorizontal(id);
}

void BlueprintNodeBuilder::EndInput() {
    ImGui::EndHorizontal();

    EndPin();
}

void BlueprintNodeBuilder::Middle() {
    if (CurrentStage == Stage::Begin) SetStage(Stage::Content);

    SetStage(Stage::Middle);
}

void BlueprintNodeBuilder::Output(PinID id) {
    if (CurrentStage == Stage::Begin) SetStage(Stage::Content);

    const auto applyPadding = (CurrentStage == Stage::Output);

    SetStage(Stage::Output);

    if (applyPadding) ImGui::Spring(0);

    Pin(id, PinKind::Output);

    ImGui::BeginHorizontal(id);
}

void BlueprintNodeBuilder::EndOutput() {
    ImGui::EndHorizontal();

    EndPin();
}

bool BlueprintNodeBuilder::SetStage(Stage stage) {
    if (stage == CurrentStage) return false;

    auto oldStage = CurrentStage;
    CurrentStage = stage;

    ImVec2 cursor;
    switch (oldStage) {
        case Stage::Begin:
            break;

        case Stage::Header:
            ImGui::EndHorizontal();
            HeaderMin = ImGui::GetItemRectMin();
            HeaderMax = ImGui::GetItemRectMax();

            // spacing between header and content
            ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);

            break;

        case Stage::Content:
            break;

        case Stage::Input:
            ed::PopStyleVar(2);

            ImGui::Spring(1, 0);
            ImGui::EndVertical();

            // #debug
            // ImGui::GetWindowDrawList()->AddRect(
            //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

            break;

        case Stage::Middle:
            ImGui::EndVertical();

            // #debug
            // ImGui::GetWindowDrawList()->AddRect(
            //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

            break;

        case Stage::Output:
            ed::PopStyleVar(2);

            ImGui::Spring(1, 0);
            ImGui::EndVertical();

            // #debug
            // ImGui::GetWindowDrawList()->AddRect(
            //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

            break;

        case Stage::End:
            break;

        case Stage::Invalid:
            break;
    }

    switch (stage) {
        case Stage::Begin:
            ImGui::BeginVertical("node");
            break;

        case Stage::Header:
            HasHeader = true;

            ImGui::BeginHorizontal("header");
            break;

        case Stage::Content:
            if (oldStage == Stage::Begin) ImGui::Spring(0);

            ImGui::BeginHorizontal("content");
            ImGui::Spring(0, 0);
            break;

        case Stage::Input:
            ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);

            ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
            ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));

            if (!HasHeader) ImGui::Spring(1, 0);
            break;

        case Stage::Middle:
            ImGui::Spring(1);
            ImGui::BeginVertical("middle", ImVec2(0, 0), 1.0f);
            break;

        case Stage::Output:
            if (oldStage == Stage::Middle || oldStage == Stage::Input)
                ImGui::Spring(1);
            else
                ImGui::Spring(1, 0);
            ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);

            ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
            ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));

            if (!HasHeader) ImGui::Spring(1, 0);
            break;

        case Stage::End:
            if (oldStage == Stage::Input) ImGui::Spring(1, 0);
            if (oldStage != Stage::Begin) ImGui::EndHorizontal();
            ContentMin = ImGui::GetItemRectMin();
            ContentMax = ImGui::GetItemRectMax();

            ImGui::Spring(0);
            ImGui::EndVertical();
            NodeMin = ImGui::GetItemRectMin();
            NodeMax = ImGui::GetItemRectMax();
            break;

        case Stage::Invalid:
            break;
    }

    return true;
}

void BlueprintNodeBuilder::Pin(PinID id, PinKind kind) {
    ed::BeginPin((ed::PinId)id, kind == PinKind::Input ? ed::PinKind::Input : ed::PinKind::Output);
}

void BlueprintNodeBuilder::EndPin() {
    ed::EndPin();

    // #debug
    // ImGui::GetWindowDrawList()->AddRectFilled(
    //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 64));
}

void BlueprintNodeBuilder::DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color,
                                    ImU32 innerColor) {
    auto rect = ImRect(a, b);
    auto rect_x = rect.Min.x;
    auto rect_y = rect.Min.y;
    auto rect_w = rect.Max.x - rect.Min.x;
    auto rect_h = rect.Max.y - rect.Min.y;
    auto rect_center_x = (rect.Min.x + rect.Max.x) * 0.5f;
    auto rect_center_y = (rect.Min.y + rect.Max.y) * 0.5f;
    auto rect_center = ImVec2(rect_center_x, rect_center_y);
    const auto outline_scale = rect_w / 24.0f;
    const auto extra_segments = static_cast<int>(2 * outline_scale);  // for full circle

    if (type == IconType::Flow) {
        const auto origin_scale = rect_w / 24.0f;

        const auto offset_x = 1.0f * origin_scale;
        const auto offset_y = 0.0f * origin_scale;
        const auto margin = (filled ? 2.0f : 2.0f) * origin_scale;
        const auto rounding = 0.1f * origin_scale;
        const auto tip_round = 0.7f;  // percentage of triangle edge (for tip)
        // const auto edge_round = 0.7f; // percentage of triangle edge (for corner)
        const auto canvas =
            ImRect(rect.Min.x + margin + offset_x, rect.Min.y + margin + offset_y, rect.Max.x - margin + offset_x, rect.Max.y - margin + offset_y);
        const auto canvas_x = canvas.Min.x;
        const auto canvas_y = canvas.Min.y;
        const auto canvas_w = canvas.Max.x - canvas.Min.x;
        const auto canvas_h = canvas.Max.y - canvas.Min.y;

        const auto left = canvas_x + canvas_w * 0.5f * 0.3f;
        const auto right = canvas_x + canvas_w - canvas_w * 0.5f * 0.3f;
        const auto top = canvas_y + canvas_h * 0.5f * 0.2f;
        const auto bottom = canvas_y + canvas_h - canvas_h * 0.5f * 0.2f;
        const auto center_y = (top + bottom) * 0.5f;
        // const auto angle = AX_PI * 0.5f * 0.5f * 0.5f;

        const auto tip_top = ImVec2(canvas_x + canvas_w * 0.5f, top);
        const auto tip_right = ImVec2(right, center_y);
        const auto tip_bottom = ImVec2(canvas_x + canvas_w * 0.5f, bottom);

        drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
        drawList->PathBezierCurveTo(ImVec2(left, top), ImVec2(left, top), ImVec2(left, top) + ImVec2(rounding, 0));
        drawList->PathLineTo(tip_top);
        drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
        drawList->PathBezierCurveTo(tip_right, tip_right, tip_bottom + (tip_right - tip_bottom) * tip_round);
        drawList->PathLineTo(tip_bottom);
        drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
        drawList->PathBezierCurveTo(ImVec2(left, bottom), ImVec2(left, bottom), ImVec2(left, bottom) - ImVec2(0, rounding));

        if (!filled) {
            if (innerColor & 0xFF000000) drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

            drawList->PathStroke(color, true, 2.0f * outline_scale);
        } else
            drawList->PathFillConvex(color);
    } else {
        auto triangleStart = rect_center_x + 0.32f * rect_w;

        auto rect_offset = -static_cast<int>(rect_w * 0.25f * 0.25f);

        rect.Min.x += rect_offset;
        rect.Max.x += rect_offset;
        rect_x += rect_offset;
        rect_center_x += rect_offset * 0.5f;
        rect_center.x += rect_offset * 0.5f;

        if (type == IconType::Circle) {
            const auto c = rect_center;

            if (!filled) {
                const auto r = 0.5f * rect_w / 2.0f - 0.5f;

                if (innerColor & 0xFF000000) drawList->AddCircleFilled(c, r, innerColor, 12 + extra_segments);
                drawList->AddCircle(c, r, color, 12 + extra_segments, 2.0f * outline_scale);
            } else
                drawList->AddCircleFilled(c, 0.5f * rect_w / 2.0f, color, 12 + extra_segments);
        }

        if (type == IconType::Square) {
            if (filled) {
                const auto r = 0.5f * rect_w / 2.0f;
                const auto p0 = rect_center - ImVec2(r, r);
                const auto p1 = rect_center + ImVec2(r, r);

                drawList->AddRectFilled(p0, p1, color, 0, 15 + extra_segments);
            } else {
                const auto r = 0.5f * rect_w / 2.0f - 0.5f;
                const auto p0 = rect_center - ImVec2(r, r);
                const auto p1 = rect_center + ImVec2(r, r);

                if (innerColor & 0xFF000000) drawList->AddRectFilled(p0, p1, innerColor, 0, 15 + extra_segments);

                drawList->AddRect(p0, p1, color, 0, 15 + extra_segments, 2.0f * outline_scale);
            }
        }

        if (type == IconType::Grid) {
            const auto r = 0.5f * rect_w / 2.0f;
            const auto w = ceilf(r / 3.0f);

            const auto baseTl = ImVec2(floorf(rect_center_x - w * 2.5f), floorf(rect_center_y - w * 2.5f));
            const auto baseBr = ImVec2(floorf(baseTl.x + w), floorf(baseTl.y + w));

            auto tl = baseTl;
            auto br = baseBr;
            for (int i = 0; i < 3; ++i) {
                tl.x = baseTl.x;
                br.x = baseBr.x;
                drawList->AddRectFilled(tl, br, color);
                tl.x += w * 2;
                br.x += w * 2;
                if (i != 1 || filled) drawList->AddRectFilled(tl, br, color);
                tl.x += w * 2;
                br.x += w * 2;
                drawList->AddRectFilled(tl, br, color);

                tl.y += w * 2;
                br.y += w * 2;
            }

            triangleStart = br.x + w + 1.0f / 24.0f * rect_w;
        }

        if (type == IconType::RoundSquare) {
            if (filled) {
                const auto r = 0.5f * rect_w / 2.0f;
                const auto cr = r * 0.5f;
                const auto p0 = rect_center - ImVec2(r, r);
                const auto p1 = rect_center + ImVec2(r, r);

                drawList->AddRectFilled(p0, p1, color, cr, 15);
            } else {
                const auto r = 0.5f * rect_w / 2.0f - 0.5f;
                const auto cr = r * 0.5f;
                const auto p0 = rect_center - ImVec2(r, r);
                const auto p1 = rect_center + ImVec2(r, r);

                if (innerColor & 0xFF000000) drawList->AddRectFilled(p0, p1, innerColor, cr, 15);

                drawList->AddRect(p0, p1, color, cr, 15, 2.0f * outline_scale);
            }
        } else if (type == IconType::Diamond) {
            if (filled) {
                const auto r = 0.607f * rect_w / 2.0f;
                const auto c = rect_center;

                drawList->PathLineTo(c + ImVec2(0, -r));
                drawList->PathLineTo(c + ImVec2(r, 0));
                drawList->PathLineTo(c + ImVec2(0, r));
                drawList->PathLineTo(c + ImVec2(-r, 0));
                drawList->PathFillConvex(color);
            } else {
                const auto r = 0.607f * rect_w / 2.0f - 0.5f;
                const auto c = rect_center;

                drawList->PathLineTo(c + ImVec2(0, -r));
                drawList->PathLineTo(c + ImVec2(r, 0));
                drawList->PathLineTo(c + ImVec2(0, r));
                drawList->PathLineTo(c + ImVec2(-r, 0));

                if (innerColor & 0xFF000000) drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

                drawList->PathStroke(color, true, 2.0f * outline_scale);
            }
        } else {
            const auto triangleTip = triangleStart + rect_w * (0.45f - 0.32f);

            drawList->AddTriangleFilled(ImVec2(ceilf(triangleTip), rect_y + rect_h * 0.5f), ImVec2(triangleStart, rect_center_y + 0.15f * rect_h),
                                        ImVec2(triangleStart, rect_center_y - 0.15f * rect_h), color);
        }
    }
}

void BlueprintNodeBuilder::Icon(const ImVec2& size, IconType type, bool filled, const ImVec4& color /*= ImVec4(1, 1, 1, 1)*/,
                                const ImVec4& innerColor /*= ImVec4(0, 0, 0, 0)*/) {
    if (ImGui::IsRectVisible(size)) {
        auto cursorPos = ImGui::GetCursorScreenPos();
        auto drawList = ImGui::GetWindowDrawList();
        DrawIcon(drawList, cursorPos, cursorPos + size, type, filled, ImColor(color), ImColor(innerColor));
    }

    ImGui::Dummy(size);
}

}  // namespace Ethereal