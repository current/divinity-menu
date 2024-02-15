#pragma once

#include "imgui.h"
#include "color_pallete.hpp"
#include <functional>
#include <windows.h>
#include <WinUser.h>
struct context_t {
	ImVec2 position;
	ImVec2 size;
	color_pallete pallete;
};

struct subtab_t {
	int index;
	std::string name;
	std::function<void()> content;
	int* current_index;
};


struct inner_content_t {
	int index;
	int* subtab_index;
	float alpha;
	std::string name;
	std::vector<subtab_t> subtab;
};

struct inner_subtab_content_t {
	int index;
	float alpha;
	std::string name;
	std::function<void()> contents;
	std::string parent_name;
};

enum e_child_placement {
	e_child_top_left,
	e_child_bottom_left,
	e_child_top_right,
	e_child_bottom_right,
};

enum e_child_sizing {
	e_child_size_half,
	e_child_size_full
};


struct child_content_t  {
	std::string name;
	e_child_placement child_placement;
	e_child_sizing child_size;
	float offset;
	std::function<void()> contents;
};

struct util_t {
	bool in_region(ImVec2 pos, ImVec2 size) {
		return (ImGui::GetIO().MousePos.x > pos.x && ImGui::GetIO().MousePos.y > pos.y && ImGui::GetIO().MousePos.x < pos.x + size.x && ImGui::GetIO().MousePos.y < pos.y + size.y);
	}

	template <typename T>
	inline T map_number(const T& input, const T& input_min, const T& input_max, const T& output_min, const T& output_max) {
		return (input - input_min) / (input_max - input_min) * (output_max - output_min) + output_min;
	}

	int scroll_amount;
	void handle_scroll_amount(UINT msg, WPARAM param) {
		if (msg == WM_MOUSEWHEEL)
			scroll_amount = -(GET_WHEEL_DELTA_WPARAM(param) / (WHEEL_DELTA / 4));
	}
}; inline util_t utils;

inline const char* const key_names[] = {
	"none",
	"m1",
	"m2",
	"something",
	"m3",
	"m4",
	"m5",
	"unk",
	"back",
	"tab",
	"unk",
	"unk",
	"clear",
	"enter",
	"unk",
	"unk",
	"shift",
	"control",
	"menu",
	"pause",
	"capital",
	"kana",
	"unk",
	"junja",
	"final",
	"kanja",
	"unk",
	"escape",
	"convert",
	"nonconvert",
	"accept",
	"modechange",
	"space",
	"prior",
	"next",
	"end",
	"home",
	"left",
	"up",
	"right",
	"down",
	"select",
	"print",
	"exec",
	"snap",
	"insert",
	"delete",
	"help",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"q",
	"r",
	"s",
	"t",
	"u",
	"v",
	"w",
	"x",
	"y",
	"z",
	"lwin",
	"rwin",
	"apps",
	"unk",
	"sleep",
	"num0",
	"num1",
	"num2",
	"num3",
	"num4",
	"num5",
	"num6",
	"num7",
	"num8",
	"num9",
	"multiply",
	"add",
	"separator",
	"subtract",
	"dec",
	"divide",
	"f1",
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",
	"f9",
	"f10",
	"f11",
	"f12",
	"f13",
	"f14",
	"f15",
	"f16",
	"f17",
	"f18",
	"f19",
	"f20",
	"f21",
	"f22",
	"f23",
	"f24",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"numlock",
	"scroll",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"unk",
	"lshift",
	"rshit",
	"lcontrol",
	"rcontrol",
	"lwin",
	"rwin"
};