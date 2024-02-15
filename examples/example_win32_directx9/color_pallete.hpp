#pragma once
#include "imgui.h"
#include <string>

struct color_pallete {
	ImColor background = ImColor(15, 15, 15);
	ImColor main_outline = ImColor(32, 32, 32);
	ImColor header = ImColor(25, 25, 30);
	ImColor header_mark = ImColor(35, 35, 35);
	ImColor subtab_area = ImColor(15, 15, 15);
	ImColor subtab_area_mark = ImColor(25, 25, 25);
	ImColor disabled_element = ImColor(18, 18, 18);
	ImColor disabled_element_outline = ImColor(30, 30, 30);
	ImColor disabled_str = ImColor(80, 80, 80);
	ImColor active_str = ImColor(215, 215, 215);
	ImColor accent = ImColor(189, 203, 242);

	// Children (WTF!)
	ImColor child_background = ImColor(15, 15, 15);
	ImColor child_outline = ImColor(20, 20, 20);
	ImColor child_header = ImColor(9, 9, 9);
	ImColor child_header_mark = ImColor(35, 35, 35);
};