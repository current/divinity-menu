#include "window.hpp"
#include "animation_controller.hpp"
#include "content_controller.hpp"
#include <iostream>
#include "fa icons.hpp"

void child_elements_t::color_picker(const std::string& label, float* variable, bool show_alpha_bar) {
	auto size = ImVec2(24.f, 16.f);
	auto position = context.pos + ImVec2(context.size.x - 30 - size.x, context.offset - context.scroll_offset);
	auto text_size = render_wrapper.text_size(label);

	auto this_hash = std::hash<std::string>()(label);
	auto hovered = utils.in_region(position, size) && content_controller.blocking == 0;
	auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
	auto open_animation_static = animation_controller.get(label + "#colorpicker_animation_static");
	auto open_animation_interpolated = animation_controller.get(label + "#colorpicker_animation_interpolated");

	static bool edit_blocking = 0;

	auto alpha = animation_controller.get(label + "#color_picker"); alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (content_controller.blocking == this_hash ? 1.f : -1.f));
	auto edit = animation_controller.get(label + "#color_picker_editor"); edit.adjust(edit.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (edit_blocking ? 1.f : -1.f));
	auto edit_interpolated = animation_controller.get(label + "#color_picker_editor_interpolated");
	auto hover_alpha = animation_controller.get(label + "#color_picker_hover"); hover_alpha.adjust(hover_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (hovered ? 1.f : -1.f));
	
	if (edit_interpolated.value != edit_blocking) {
		edit_interpolated.value += (edit_blocking - edit_interpolated.value) * 0.2f;
		edit_interpolated.adjust(edit_interpolated.value);
	}

	bool skip_next_frame = false;
	if (clicked) {
		content_controller.blocking = this_hash;
		skip_next_frame = true;
	}

	open_animation_static.adjust(content_controller.blocking == this_hash ? 1.f : 0.f, false);
	if (open_animation_interpolated.value != open_animation_static.value) {
		open_animation_interpolated.value += (open_animation_static.value - open_animation_interpolated.value) * 0.18f;
		open_animation_interpolated.adjust(open_animation_interpolated.value, false);
	}

	static int blocking = 0;
	if (content_controller.blocking == this_hash || alpha.value > 0.1f) {
		auto picker_pos = position + ImVec2(size.x + 5, size.y + 5);
		auto picker_size = ImVec2(190 * open_animation_interpolated.value, (show_alpha_bar ? 190 : 170) * open_animation_interpolated.value);

		auto hsv_box_pos = picker_pos + ImVec2(10.f * open_animation_interpolated.value, 10.f * open_animation_interpolated.value);
		auto hsv_box_size = ImVec2(150 * open_animation_interpolated.value, 150 * open_animation_interpolated.value);

		auto alpha_bar_pos = picker_pos + ImVec2(10.f * open_animation_interpolated.value, 170.f * open_animation_interpolated.value);
		auto alpha_bar_size = ImVec2(150.f * open_animation_interpolated.value, 10.f * open_animation_interpolated.value);

		auto hue_pos = picker_pos + ImVec2(170.f * open_animation_interpolated.value, 10.f * open_animation_interpolated.value);
		auto hue_size = ImVec2(10.f * open_animation_interpolated.value, (150 / 6) * open_animation_interpolated.value);

		auto edit_pos = picker_pos + ImVec2(170.f * open_animation_interpolated.value, 170.f * open_animation_interpolated.value);
		auto edit_size = ImVec2(10.f, 10.f) * open_animation_interpolated.value;

		if (!utils.in_region(picker_pos, picker_size) && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !skip_next_frame) {
			content_controller.blocking = 0;
		}

		float h, s, v;
		float r, g, b;
		float display_r, display_g, display_b;
		ImGui::ColorConvertRGBtoHSV(variable[0], variable[1], variable[2], h, s, v);
		ImGui::ColorConvertHSVtoRGB(h, 1.f, 1.f, display_r, display_g, display_b);

		if (utils.in_region(hsv_box_pos, hsv_box_size) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			blocking = 1;
		}

		if (utils.in_region(hue_pos, ImVec2(hue_size.x, hue_size.y * 6)) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			blocking = 2;
		}

		if (utils.in_region(alpha_bar_pos, alpha_bar_size) && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && show_alpha_bar) {
			blocking = 3;
		}

		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			blocking = 0;
		}

		if (blocking == 1) {
			s = ImClamp((ImGui::GetIO().MousePos.x - hsv_box_pos.x) / hsv_box_size.x, 0.01f, 1.f);
			v = ImClamp(1.f - (ImGui::GetIO().MousePos.y - hsv_box_pos.y) / hsv_box_size.y, 0.01f, 1.f);
		}

		if (blocking == 2) {
			h = ImClamp((ImGui::GetIO().MousePos.y - hue_pos.y) / (hue_size.y * 6), 0.01f, 1.f);
		}

		if (blocking == 3) {
			variable[3] = ImClamp((ImGui::GetIO().MousePos.x - alpha_bar_pos.x) / alpha_bar_size.x, 0.01f, 1.f);
		}

		render_wrapper.set_draw_list(ImGui::GetForegroundDrawList());
		render_wrapper.begin_clip_rect(picker_pos, picker_size);

		render_wrapper.fill_rect(picker_pos, picker_size, context.pallete.disabled_element.set_alpha(alpha.value * content_controller.get_alpha()), 4.f);
		//render.rect_gradient(hsv_box_pos, hsv_box_size, ImColor(255, 255, 255).set_alpha(alpha.value * content_controller.get_alpha()), ImColor(display_r, display_g, display_b).set_alpha(alpha.value * content_controller.get_alpha()), horizontal, 4.f);
		//render.rect_gradient(hsv_box_pos, hsv_box_size, ImColor(0, 0, 0, 1), ImColor(0, 0, 0).set_alpha(alpha.value * content_controller.get_alpha()), vertical, 4.f);
		render_wrapper.grad_h(hsv_box_pos, hsv_box_size, ImColor(255, 255, 255).set_alpha(alpha.value * content_controller.get_alpha()), ImColor(display_r, display_g, display_b).set_alpha(alpha.value * content_controller.get_alpha()));
		render_wrapper.grad_v(hsv_box_pos, hsv_box_size, ImColor(0, 0, 0, 0), ImColor(0, 0, 0).set_alpha(alpha.value * content_controller.get_alpha()));
		render_wrapper.rect(picker_pos, picker_size, context.pallete.disabled_element_outline.set_alpha(alpha.value * content_controller.get_alpha()), 1.f, 4.f);
		if (show_alpha_bar) {
			render_wrapper.grad_h(alpha_bar_pos, alpha_bar_size, ImColor(0, 0, 0).set_alpha(alpha.value * content_controller.get_alpha()), ImColor(variable[0], variable[1], variable[2]).set_alpha(alpha.value * content_controller.get_alpha()));
		}

		const ImU32 col_hues[6 + 1] = { ImColor(255,0,0).set_alpha(alpha.value * content_controller.get_alpha()), ImColor(255,255,0).set_alpha(alpha.value * content_controller.get_alpha()), ImColor(0,255,0).set_alpha(alpha.value * content_controller.get_alpha()), ImColor(0,255,255).set_alpha(alpha.value * content_controller.get_alpha()), ImColor(0,0,255).set_alpha(alpha.value * content_controller.get_alpha()), ImColor(255,0,255).set_alpha(alpha.value * content_controller.get_alpha()), ImColor(255,0,0).set_alpha(alpha.value * content_controller.get_alpha()) };
		for (size_t i = 0; i < 6; i++) {
			render_wrapper.grad_v(hue_pos + ImVec2(0, hue_size.y * i), hue_size, col_hues[i], col_hues[i + 1]);
		}

		render_wrapper.rect(hsv_box_pos - ImVec2(1, 1), hsv_box_size + ImVec2(2, 2), context.pallete.disabled_element_outline.set_alpha(alpha.value * content_controller.get_alpha()));
		render_wrapper.rect(hue_pos - ImVec2(1, 1), ImVec2(hue_size.x, hsv_box_size.y) + ImVec2(2, 2), context.pallete.disabled_element_outline.set_alpha(alpha.value * content_controller.get_alpha()));
		if (show_alpha_bar) {
			render_wrapper.rect(alpha_bar_pos - ImVec2(1, 1), alpha_bar_size + ImVec2(2, 2), context.pallete.disabled_element_outline.set_alpha(alpha.value * content_controller.get_alpha()));
		}

		auto hsv_picker_pos = hsv_box_pos + ImVec2((hsv_box_size.x * s) * open_animation_interpolated.value, (hsv_box_size.y * (1.f - v)) * open_animation_interpolated.value);

		render_wrapper.circle_fill(hsv_picker_pos, 6.f * alpha.value, ImColor(variable[0], variable[1], variable[2]).set_alpha(alpha.value * content_controller.get_alpha()));
		render_wrapper.circle(hsv_picker_pos, 6.f * alpha.value, ImColor(40, 40, 40).set_alpha(alpha.value * content_controller.get_alpha()), 2.f);

		render_wrapper.fill_rect(hue_pos + ImVec2(0, (hsv_box_size.y * h) * open_animation_interpolated.value - 3), ImVec2(10.f, 6.f), ImColor(80, 80, 80).set_alpha(alpha.value * content_controller.get_alpha()), 2.f);
		render_wrapper.rect(hue_pos + ImVec2(0, (hsv_box_size.y * h) * open_animation_interpolated.value - 3), ImVec2(10.f, 6.f), ImColor(40, 40, 40).set_alpha(alpha.value * content_controller.get_alpha()), 1.f, 2.f);

		if (show_alpha_bar) {
			render_wrapper.fill_rect(alpha_bar_pos + ImVec2((alpha_bar_size.x * variable[3]) * open_animation_interpolated.value - 3, 0), ImVec2(6.f, 10.f), ImColor(80, 80, 80).set_alpha(alpha.value * content_controller.get_alpha()), 2.f);
			render_wrapper.rect(alpha_bar_pos + ImVec2((alpha_bar_size.x * variable[3]) * open_animation_interpolated.value - 3, 0), ImVec2(6.f, 10.f), ImColor(40, 40, 40).set_alpha(alpha.value * content_controller.get_alpha()), 1.f, 2.f);
		}

	/*	render.set_font(content_controller.fonts.fa_icons);
		render.text(edit_pos + edit_size / 2 - render.text_size(ICON_FA_CIRCLE_HALF_STROKE) / 2 - ImVec2(0, 1), ICON_FA_CIRCLE_HALF_STROKE, ImColor(50, 50, 50).set_alpha(alpha.value * content_controller.get_alpha()));
		render.text(edit_pos + edit_size / 2 - render.text_size(ICON_FA_CIRCLE_HALF_STROKE) / 2 - ImVec2(0, 1), ICON_FA_CIRCLE_HALF_STROKE, ImColor(80, 80, 80).set_alpha(edit.value * alpha.value * content_controller.get_alpha()));
		render.pop_font();

		if (utils.in_region(edit_pos, edit_size) && blocking == 0 && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			edit_blocking = !edit_blocking;
		}*/



		render_wrapper.end_clip_rect();
		render_wrapper.set_draw_list(ImGui::GetBackgroundDrawList());

		ImGui::ColorConvertHSVtoRGB(h, s, v, variable[0], variable[1], variable[2]);
	}

	render_wrapper.fill_rect(position + ImVec2(1.f, 1.f), size - ImVec2(2.f, 2.f), context.pallete.disabled_element.set_alpha(content_controller.get_alpha()), 3.f);
	render_wrapper.rect(position, size, context.pallete.disabled_element_outline.set_alpha(content_controller.get_alpha()), 0.f, 3.f);

	render_wrapper.rect(position, size, ImColor(50, 50, 50).set_alpha(hover_alpha.value * content_controller.get_alpha()), 0.f, 3.f);
	render_wrapper.fill_rect(position + ImVec2(1.f, 1.f), size - ImVec2(2.f, 2.f), ImColor(variable[0], variable[1], variable[2]).set_alpha(variable[3] * content_controller.get_alpha()), 3.f);
}
