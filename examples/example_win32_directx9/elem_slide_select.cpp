#include "window.hpp"
#include "animation_controller.hpp"
#include "content_controller.hpp"

void child_elements_t::slide_select(const std::string& label, int* variable, std::vector<std::string> options) {
	auto position = context.pos + ImVec2(0.f, context.offset);
	auto size = ImVec2(context.size.x - 30.f, 22.f);
	auto text_size = render_wrapper.text_size(label);

	auto hovered = utils.in_region(position, size) && content_controller.blocking == 0;
	auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
	bool skip_next_frame = false;

	auto alpha = animation_controller.get(label); alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * -1.f);
	auto hover_alpha = animation_controller.get(label + "#"); hover_alpha.adjust(hover_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (hovered ? 1.f : -1.f));

	render_wrapper.fill_rect(position, size, context.pallete.disabled_element.set_alpha(content_controller.get_alpha()), 3.f);
	render_wrapper.rect(position, size, context.pallete.disabled_element_outline.set_alpha(content_controller.get_alpha()), 0.f, 3.f);

	auto outline_col = ImColor(50 / 255.f, 50 / 255.f, 50 / 255.f, hover_alpha.value * content_controller.get_alpha());
	render_wrapper.rect(position, size, outline_col, 0.f, 3.f);

	auto capsule_size = ImVec2(size.x / 3, size.y);
	auto invoke_capsule = [&](const std::string& name, ImVec2 pos, ImVec2 size, int self_index) -> bool {

		auto hovered = utils.in_region(pos, size);
		auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
		auto is_active_element = *variable == self_index;

		auto alpha = animation_controller.get(name);
		auto hover_alpha = animation_controller.get(name + "#hover");

		alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (is_active_element ? 1.f : -1.f));
		hover_alpha.adjust(hover_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (hovered ? 1.f : -1.f));

		render_wrapper.text(pos + size / 2 - render_wrapper.text_size(name) / 2, name, ImColor(50, 50, 50, int(255 * content_controller.window_alpha)));
		render_wrapper.text(pos + size / 2 - render_wrapper.text_size(name) / 2, name, ImColor(125, 125, 125, int(255 * content_controller.window_alpha * hover_alpha.value)));

		render_wrapper.text(pos + size/2 - render_wrapper.text_size(name) / 2, name, context.pallete.accent.set_alpha(alpha.value * content_controller.window_alpha));

		return clicked;
	};

	auto slide_animation = animation_controller.get(label + "#slide_animation");
	auto v = slide_animation.value;
	if (v != *variable) {
		v += (*variable - v)* 0.2f;
		slide_animation.adjust(v, false);
	}

	render_wrapper.begin_clip_rect(position, size);
	for (size_t i = 0; i < options.size(); i++) {
		auto pos = ImVec2(position.x + size.x / 2 - capsule_size.x / 2, position.y); pos.x += capsule_size.x * i; pos.x -= capsule_size.x * slide_animation.value;

		if (invoke_capsule(options.at(i), pos, capsule_size, i) && !skip_next_frame) {
			*variable = i;

			skip_next_frame = true;
		}
	}
	render_wrapper.end_clip_rect();

	context.offset += size.y + 5.f;
}
