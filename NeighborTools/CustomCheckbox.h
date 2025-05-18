#pragma once 
#include "ImGui/imgui.h"

bool CustomCheckbox(const char* label, bool* v) {
    bool changed = false;
    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float box_size = 10.0f;
    float vertical_offset = 3.0f;

    ImVec2 box_min = ImVec2(cursor_pos.x, cursor_pos.y + vertical_offset);
    ImVec2 box_max = ImVec2(cursor_pos.x + box_size, cursor_pos.y + box_size + vertical_offset);

    ImGui::InvisibleButton(label, ImVec2(box_size, box_size + vertical_offset));
    if (ImGui::IsItemClicked()) {
        *v = !*v;
        changed = true;
    }

    ImU32 fill_color = IM_COL32(127, 131, 255, 255);
    ImU32 border_color = fill_color;
    draw_list->AddRect(box_min, box_max, border_color, 0, 0, 2.0f);

    if (*v) {
        draw_list->AddRectFilled(box_min, box_max, fill_color);
    }

    ImVec2 text_size = ImGui::CalcTextSize(label);
    ImVec2 text_pos = ImVec2(box_max.x + 5, box_min.y + (box_size - text_size.y) * 0.5f);
    draw_list->AddText(text_pos, IM_COL32_WHITE, label);

    return changed;
}
