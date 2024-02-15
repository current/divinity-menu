#include "render_wrapper.hpp"

void render_t::set_draw_list(ImDrawList* m_draw_list) {
	draw_list = m_draw_list;
}

void render_t::text(vec2 pos, std::string label, ImColor color) {
	draw_list->AddText(pos, color, label.data());
}

vec2 render_t::text_size(std::string label) {
	return ImGui::CalcTextSize(label.data());
}

void render_t::set_font(ImFont* font) {
	ImGui::PushFont(font);
}

void render_t::pop_font() {
	ImGui::PopFont();
}

void render_t::rect(vec2 pos, vec2 size, ImColor color, float thickness, float rounding, ImDrawFlags flags) {
	draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), color, rounding, flags, thickness);
}

void render_t::fill_rect(vec2 pos, vec2 size, ImColor color, float rounding, ImDrawFlags flags ) {
	draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), color, rounding, flags);
}

void render_t::line(vec2 from, vec2 to, ImColor color, float thickness) {
	draw_list->AddLine(from, ImVec2(from.x + to.x, from.y + to.y), color, thickness);
}

float render_t::get_deltatime(float scale) {
	return ImGui::GetIO().DeltaTime * scale;
}

float render_t::get_min_deltatime(float scale) {
	return (1.f / 0.2f) * ImGui::GetIO().DeltaTime * scale;
}

void render_t::begin_clip_rect(vec2 pos, vec2 size) {
	draw_list->PushClipRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), true);
}

void render_t::end_clip_rect() {
	draw_list->PopClipRect();
}

void render_t::grad_h(vec2 pos, vec2 size, ImColor left, ImColor right) {
	draw_list->AddRectFilledMultiColor(pos, ImVec2(pos.x + size.x, pos.y + size.y), left, right, right, left);
}

void render_t::grad_v(vec2 pos, vec2 size, ImColor top, ImColor bottom) {
	draw_list->AddRectFilledMultiColor(pos, ImVec2(pos.x + size.x, pos.y + size.y), top, top, bottom, bottom);
}

void render_t::circle(vec2 pos, float radius, ImColor color, float thickness) {
	draw_list->AddCircle(pos, radius, color, 0, thickness);
}

void render_t::circle_fill(vec2 pos, float radius, ImColor color) {
	draw_list->AddCircleFilled(pos, radius, color, 0);
}

void render_t::rect_gradient(vec2 pos, vec2 size, ImColor color1, ImColor color2, e_gradient_type type, float rounding, float color_stop, ImDrawFlags flags) {

	// todo: rounded gradients for now are imgui only, add for other renderers too

	auto shade_verts_linear_color_gradient = [&](int vert_start_idx, int vert_end_idx, vec2 start_pos, vec2 to_size, ImColor col0, ImColor col1) {
		constexpr int r_shift = 0;
		constexpr int g_shift = 8;
		constexpr int b_shift = 16;
		constexpr int a_shift = 24;

		vec2 gradient_extent = to_size - start_pos;
		float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
		auto vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
		auto vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
		const int col0_r = (int)(col0 >> r_shift) & 0xFF;
		const int col0_g = (int)(col0 >> g_shift) & 0xFF;
		const int col0_b = (int)(col0 >> b_shift) & 0xFF;
		const int col0_a = (int)(col0 >> a_shift) & 0xFF;
		const int col_delta_r = ((int)(col1 >> r_shift) & 0xFF) - col0_r;
		const int col_delta_g = ((int)(col1 >> g_shift) & 0xFF) - col0_g;
		const int col_delta_b = ((int)(col1 >> b_shift) & 0xFF) - col0_b;
		const int col_delta_a = ((int)(col1 >> a_shift) & 0xFF) - col0_a;

		for (auto vert = vert_start; vert < vert_end; vert++) {
			float d = ImDot(vert->pos - start_pos, gradient_extent);
			float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
			int r = (int)(col0_r + col_delta_r * t);
			int g = (int)(col0_g + col_delta_g * t);
			int b = (int)(col0_b + col_delta_b * t);
			int a = (int)(col0_a + col_delta_a * t);
			vert->col = (r << r_shift) | (g << g_shift) | (b << b_shift) | (a << a_shift);
		}
	};

	// optimize and don't do shading if not needed
	if (rounding == 0.f) {
		draw_list->AddRectFilledMultiColor(pos, pos + size, color1, color2, color1, color2);
		return; // stop execution
	}

	const int vtx_idx_0 = draw_list->VtxBuffer.Size;
	draw_list->AddRectFilled(pos, pos + size, ImColor(1.f, 1.f, 1.f, color1.Value.w), rounding, flags); // draw a rect to then shade
	const int vtx_idx_1 = draw_list->VtxBuffer.Size;

	// todo: fix color stop pls
	vec2 stop = { pos + (size * color_stop) };

	switch (type) {
	case e_gradient_type::horizontal:
		shade_verts_linear_color_gradient(vtx_idx_0, vtx_idx_1, { pos.x, pos.y + stop.y }, { pos.x + size.x, pos.y + stop.y }, color1, color2);
		break;
	case e_gradient_type::vertical:
		shade_verts_linear_color_gradient(vtx_idx_0, vtx_idx_1, { pos.x + stop.x, pos.y }, { pos.x + stop.x, pos.y + size.y }, color1, color2);
		break;
		// todo: add all sides rounded gradients
	}
}
static constexpr float M_PI_F2 = ((float)(3.1415926535897932384f));
__forceinline float RAD2DEG2(float x) { return((float)(x) * (float)(180.f / M_PI_F2)); }
__forceinline float DEG2RAD2(float x) { return((float)(x) * (float)(M_PI_F2 / 180.f)); }
void render_t::arc(vec2 pos, float radius, float min_angle, float max_angle, ImColor color, float thickness) {
	draw_list->PathArcTo(pos, radius, DEG2RAD2(min_angle), DEG2RAD2(max_angle), 32);
	draw_list->PathStroke(color, false, thickness);
}