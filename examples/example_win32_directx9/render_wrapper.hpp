#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include <string>

typedef ImVec2 vec2;

enum e_gradient_type {
	horizontal,
	vertical
};

struct render_t {

	ImDrawList* draw_list;

	void set_draw_list(ImDrawList* m_draw_list);

	void text(vec2 pos, std::string label, ImColor color);
	vec2 text_size(std::string label);

	void set_font(ImFont* font);
	void pop_font();

	void rect(vec2 pos, vec2 size, ImColor color, float thickness = 1.f, float rounding = 0.f, ImDrawFlags flags = 0);
	void fill_rect(vec2 pos, vec2 size, ImColor color, float rounding = 0.f, ImDrawFlags flags = 0);

	void line(vec2 from, vec2 to, ImColor color, float thickness = 1.f);

	float get_deltatime(float scale = 1.f);
	float get_min_deltatime(float scale = 1.f);

	void begin_clip_rect(vec2 pos, vec2 size);
	void end_clip_rect();

	void grad_h(vec2 pos, vec2 size, ImColor left, ImColor right);
	void grad_v(vec2 pos, vec2 size, ImColor top, ImColor bottom);

	void circle(vec2 pos, float radius, ImColor color, float thickness = 1.f);
	void circle_fill(vec2 pos, float radius, ImColor color);

	void rect_gradient(vec2 pos, vec2 size, ImColor color1, ImColor color2, e_gradient_type type, float rounding = 0.f, float color_stop = 0.5f, ImDrawFlags flags = 0);

	void arc(vec2 pos, float radius, float min_angle, float max_angle, ImColor color, float thickness);

}; inline render_t render_wrapper;