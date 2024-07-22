#pragma once

#include "imgui-docking/imgui.h"
#include "../serialization_utils.hpp"

static void Draw_TODO_element(const serialization_utils::TODO_Element& TODO_element, const ImVec2& size, uint64_t index)
{
	if (ImGui::BeginChild(index, size, ImGuiChildFlags_Border))
	{
		ImU32 priority_color = IM_COL32(0, 0, 255, 255);

		switch (TODO_element.priority)
		{
		case serialization_utils::Priority::Low:
			priority_color = IM_COL32(0, 255, 0, 255);
			break;
		case serialization_utils::Priority::Medium:
			priority_color = IM_COL32(255, 235, 59, 255);
			break;
		case serialization_utils::Priority::High:
			priority_color = IM_COL32(255, 0, 0, 255);
			break;
		default:
			break;
		}

		ImVec2 pos = ImGui::GetWindowPos();
		pos.x += 25.f;
		pos.y += 46;
		ImGui::GetWindowDrawList()->AddCircleFilled(pos, 10.f, priority_color, 256);

		pos.x += 25.f;
		pos.y -= 13.f;

		ImGui::SetCursorScreenPos(pos);

		//ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]); // Assuming Fonts[0] is Arial
		ImGui::Text(TODO_element.content.c_str());
		//ImGui::PopFont();
		
		if (ImGui::IsItemHovered(/*ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_DelayNone*/))
		{
			ImGui::Text("Double click to delete this TODO");

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				serialization_utils::TODO_elements.erase(serialization_utils::TODO_elements.begin() + index);
				serialization_utils::SaveToSerializationFile();
			}
		}
		

		ImGui::EndChild();
	}
}

namespace menu
{
	inline bool Running = true;

	char* buffer = new char[1024];

	// Run once before main loop
	void Setup()
	{
		serialization_utils::CheckAndReadSerializationFile();
		ZeroMemory(buffer, 1024);

		ImGuiStyle& style = ImGui::GetStyle();

		constexpr const ImVec4 BlackCol = ImVec4(0.05f, 0.05f, 0.05f, 0.83f);

		style.WindowRounding = 10.0f;
		style.PopupRounding = 10.0f;
		style.FrameRounding = 2.3f;

		style.Colors[ImGuiCol_WindowBg] = BlackCol;
		style.Colors[ImGuiCol_TitleBg] = BlackCol;
		style.Colors[ImGuiCol_TitleBgCollapsed] = BlackCol;
		style.Colors[ImGuiCol_TitleBgActive] = BlackCol;
	}

	//Run in main loop
	void Draw()
	{
		ImGui::Begin(reinterpret_cast<const char*>(u8"TODO APP"), &Running);
		{
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Assuming Fonts[0] is Arial
			ImGui::Text("Your tasks");
			ImGui::PopFont();

			ImGui::SameLine(ImGui::GetWindowSize().x - 200);

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]); // Assuming Fonts[0] is Arial
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.f);
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.26f, 0.65f, 0.79f, 1.f });
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Красный цвет

			if (ImGui::Button("New Task", { 180, 40 }))
			{
				ImGui::OpenPopup("New Task");
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			ImGui::PopFont();

			ImGui::SetNextWindowPos(ImGui::GetIO().MousePos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("New Task", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::InputText("Task", buffer, 1024);

				const char* priorities[] = {"Low", "Medium", "High"};
				static int item_current_idx = 0;
				const char* combo_preview_value = priorities[item_current_idx];

				if (ImGui::BeginCombo("Priority", combo_preview_value))
				{
					for (int n = 0; n < IM_ARRAYSIZE(priorities); n++)
					{
						const bool is_selected = (item_current_idx == n);
						if (ImGui::Selectable(priorities[n], is_selected))
							item_current_idx = n;
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("OK", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();

					serialization_utils::TODO_elements.push_back(serialization_utils::TODO_Element(std::string(buffer), (serialization_utils::Priority)item_current_idx));

					serialization_utils::SaveToSerializationFile();
				}

				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }


				ImGui::EndPopup();
			}

			ImGui::SeparatorText("");
			
			if (ImGui::BeginChild("main_child", { ImGui::GetWindowSize().x - 25 , ImGui::GetWindowSize().y - 145 }/*, ImGuiChildFlags_Border */))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.f);

				if (serialization_utils::TODO_elements.size() > 0)
				{
					for (size_t i = 0; i < serialization_utils::TODO_elements.size(); ++i)
					{
						Draw_TODO_element(serialization_utils::TODO_elements[i], {ImGui::GetWindowSize().x - 0, 100}, i);
					}

					/*
					for (const serialization_utils::TODO_Element& element : serialization_utils::TODO_elements)
					{
						Draw_TODO_element(element, { ImGui::GetWindowSize().x - 0, 100 });
					}
					*/
				}
				else
				{
					if (ImGui::BeginChild(1, { ImGui::GetWindowSize().x - 0, 100 }, ImGuiChildFlags_Border))
					{
						ImGui::Text("There are no tasks, you can rest");

						ImGui::EndChild();
					}
				}

				ImGui::PopStyleVar();

				ImGui::EndChild();
			}

			ImGui::End();
		}
	}
}