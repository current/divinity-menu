#pragma once
#include <vector>

struct content_controller_t {
	float window_alpha = 0.f;

	size_t blocking = 0;

	int current_tab = 0;
	int current_subtab = 0;
	std::vector<float> alpha;
	std::vector<float> subtab_alpha;
	float get_alpha() {
		return window_alpha;
	}
	float get_subtab_alpha() {
		return subtab_alpha.at(current_subtab) * window_alpha;
	}

	struct {
		ImFont* extra_light;
		ImFont* title;
		ImFont* sub_title;
		ImFont* extra_light_title;
		ImFont* fa_icons;
		ImFont* small_pixel;
		ImFont* weapons;
	} fonts;
}; inline content_controller_t content_controller;