#pragma once
#include "ImGui/imgui.h"
#include "imgui/imgui_internal.h"

bool CustomSliderFloat(const char* label, float* value, float min, float max) {
    bool changed = false;
    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float slider_width = 200.0f;
    float slider_height = 8.0f;
    ImVec2 slider_min = cursor_pos;
    ImVec2 slider_max = ImVec2(cursor_pos.x + slider_width, cursor_pos.y + slider_height);
 
    ImGui::InvisibleButton(label, ImVec2(slider_width, slider_height));
    float mouse_pos = ImGui::GetIO().MousePos.x - slider_min.x;
   
    if (ImGui::IsItemActive()) {
        *value = min + ((mouse_pos / slider_width) * (max - min));
        *value = ImClamp(*value, min, max);
        changed = true;
    }

    ImU32 fill_color = IM_COL32(127, 131, 255, 255);
    ImU32 background_color = IM_COL32(60, 60, 60, 255);
    draw_list->AddRectFilled(slider_min, slider_max, background_color, 5.0f);
    float fill_width = ((*value - min) / (max - min)) * slider_width;
    ImVec2 fill_max = ImVec2(slider_min.x + fill_width, slider_max.y);
    draw_list->AddRectFilled(slider_min, fill_max, fill_color, 5.0f);
    draw_list->AddRect(slider_min, slider_max, fill_color, 5.0f, 0, 1.5f);
    char value_text[32];
    sprintf_s(value_text, "%.2f", *value);
    float text_width = ImGui::CalcTextSize(value_text).x;
    float text_height = ImGui::CalcTextSize(value_text).y;
    ImVec2 text_pos = ImVec2(slider_min.x + fill_width - (text_width / 2.0f), slider_min.y + (slider_height / 2) - (text_height / 2));
    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), value_text);
    ImVec2 label_pos = ImVec2(slider_min.x + slider_width + 10, slider_min.y - 2);
    draw_list->AddText(label_pos, IM_COL32_WHITE, label);
    return changed;
}
