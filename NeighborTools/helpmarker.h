#pragma once
#include "ImGui/imgui.h"

void HelpMarker(const char* desc, bool useRed = false)
{
	ImVec4 originalColor = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);

	if (useRed)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "(?)");
	}
	else
	{
		ImGui::TextDisabled("(?)");
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}