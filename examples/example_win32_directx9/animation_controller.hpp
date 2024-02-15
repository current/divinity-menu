#pragma once
#include <string>

struct anim_context_t {
	float value;
	unsigned int id;

	void adjust(float adjust_to, bool clamp = true);
};

enum e_ease_type {
	in_circ = 0 << 0,
	out_circ = 1 << 0,
	in_out_circ = 2 << 0,
	in_quad = 3 << 0,
	out_quad = 4 << 0,
	in_out_quad = 5 << 0,
};

struct animation_controller_t {

	std::string current_child;

	anim_context_t get(std::string str);

	float ease(e_ease_type type, float in);

	void clear_stack();

}; inline animation_controller_t animation_controller;