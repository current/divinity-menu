#include "window.hpp"
#include "animation_controller.hpp"
#include "content_controller.hpp"

bool child_elements_t::checkbox(const std::string& label, bool* variable) {
	auto position = context.pos + ImVec2(0.f, context.offset - context.scroll_offset);
	auto size = ImVec2(16.f, 16.f);
	auto text_size = render_wrapper.text_size(label);

	auto hovered = utils.in_region(position, size + ImVec2(text_size.x + 10.f, 0.f)) && content_controller.blocking == 0;
	auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	if (clicked) {
		*variable = !*variable;
	}

	auto alpha = animation_controller.get(label + animation_controller.current_child); alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.3f) * (*variable ? 1.f : -1.f));
	auto hover_alpha = animation_controller.get(label + "#" + animation_controller.current_child); hover_alpha.adjust(hover_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (hovered ? 1.f : -1.f));
	auto click_animation_static = animation_controller.get(label + "#checkbox_click_animation_static" + animation_controller.current_child);
	auto click_animation_interpolated = animation_controller.get(label + "#checkbox_click_animation_interpolated" + animation_controller.current_child);
	click_animation_static.adjust(*variable, false);
	if (click_animation_interpolated.value != click_animation_static.value) {
		click_animation_interpolated.value += (click_animation_static.value - click_animation_interpolated.value) * 0.12f;
		click_animation_interpolated.adjust(click_animation_interpolated.value, false);
	}
	render_wrapper.fill_rect(position + ImVec2(1.f, 1.f), size - ImVec2(2.f, 2.f), context.pallete.disabled_element.set_alpha(content_controller.get_alpha()), 3.f);
	render_wrapper.rect(position, size, context.pallete.disabled_element_outline.set_alpha(content_controller.get_alpha()), 0.f, 3.f);

	auto outline_col = ImColor(50 / 255.f, 50 / 255.f, 50 / 255.f, hover_alpha.value * content_controller.get_alpha());
	render_wrapper.rect(position, size, outline_col, 0.f, 3.f);
	auto eased = *variable ? sqrt(1 - pow(click_animation_interpolated.value - 1, 2)) : 1.f - pow(1.f - click_animation_interpolated.value, 5);
	render_wrapper.fill_rect(position + ImVec2(1.f * eased, 1.f * eased) + ImVec2(size.x / 2 * (1.f - eased), size.y / 2 * (1.f - eased)), (size - ImVec2(2.f, 2.f)) * eased, context.pallete.accent.set_alpha(eased * content_controller.get_alpha()), 3.f + (20.f * (1.f - eased)));
	
	render_wrapper.text(position + ImVec2(size.x + 6.f, -2.f), label, ImColor(50 / 255.f, 50 / 255.f, 50 / 255.f, content_controller.get_alpha()));

	auto label_hover_col = ImColor(65 / 255.f, 65 / 255.f, 65 / 255.f, hover_alpha.value * content_controller.get_alpha());
	render_wrapper.text(position + ImVec2(size.x + 6.f, -2.f), label, label_hover_col);

	render_wrapper.text(position + ImVec2(size.x + 6.f, -2.f), label, context.pallete.active_str.set_alpha(alpha.value * content_controller.get_alpha()));

	context.offset += size.y + 10.f;
	return *variable;
}