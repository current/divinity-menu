#include "window.hpp"
#include "animation_controller.hpp"
#include "content_controller.hpp"

void child_elements_t::multiselect(const std::string& label, std::vector<bool>* variable, std::vector<std::string> items) {
	auto position = context.pos + ImVec2(0.f, context.offset - context.scroll_offset);
	auto size = ImVec2(context.size.x - 30.f, 24.f);
	auto text_size = render_wrapper.text_size(label);

	auto this_hash = std::hash<std::string>()(label + animation_controller.current_child);
	auto hovered = utils.in_region(position + ImVec2(0.f, text_size.y), size) && content_controller.blocking == 0;
	auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	auto alpha = animation_controller.get(label + animation_controller.current_child); alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (content_controller.blocking == this_hash ? 1.f : -1.f));
	auto hover_alpha = animation_controller.get(label + "#" + animation_controller.current_child); hover_alpha.adjust(hover_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (hovered || content_controller.blocking == this_hash ? 1.f : -1.f));
	auto open_animation_static = animation_controller.get(label + "#dropdown_animation_static" + animation_controller.current_child);
	auto open_animation_interpolated = animation_controller.get(label + "#dropdown_animation_interpolated" + animation_controller.current_child);

	bool skip_next_frame = false;

	if (clicked) {
		content_controller.blocking = this_hash;
		skip_next_frame = true;
	}

	open_animation_static.adjust(content_controller.blocking == this_hash ? 20.f * items.size() : 0.f, false);
	if (open_animation_interpolated.value != open_animation_static.value) {
		open_animation_interpolated.value += (open_animation_static.value - open_animation_interpolated.value) * 0.2f;
		open_animation_interpolated.adjust(open_animation_interpolated.value, false);
	}
	if (content_controller.blocking == this_hash || alpha.value > 0.00f) {
		if (!utils.in_region(position + ImVec2(0.f, text_size.y + size.y + 5.f), ImVec2(size.x, open_animation_static.value)) && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !skip_next_frame) {
			content_controller.blocking = 0;
		}

		render_wrapper.set_draw_list(ImGui::GetForegroundDrawList());

		render_wrapper.fill_rect(position + ImVec2(0.f, text_size.y + size.y + 5.f), ImVec2(size.x, open_animation_interpolated.value), context.pallete.disabled_element.set_alpha(alpha.value * content_controller.get_alpha()), 4.f);
		render_wrapper.rect(position + ImVec2(0.f, text_size.y + size.y + 5.f), ImVec2(size.x, open_animation_interpolated.value), ImColor(50, 50, 50).set_alpha(alpha.value * content_controller.get_alpha()), 1.f, 4.f);

		auto invoke_item = [&](const std::string& name, vec2 pos, vec2 size, int active_index, int current_index) -> bool {
			auto hovered = utils.in_region(pos, size);
			auto clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left) && hovered;

			auto it_alpha = animation_controller.get(name + "#dropdown_iteration"); it_alpha.adjust(it_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (current_index == active_index ? 1.f : -1.f));
			auto it_hover_alpha = animation_controller.get(name + "#dropdown_iteration_hover"); it_hover_alpha.adjust(it_hover_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (hovered ? 1.f : -1.f));

			auto label_pos = size.y / 2 - render_wrapper.text_size(name).y / 2;
			render_wrapper.text(pos + ImVec2(4.f, label_pos), name, ImColor(50, 50, 50).set_alpha(alpha.value * content_controller.get_alpha()));
			render_wrapper.text(pos + ImVec2(4.f, label_pos), name, ImColor(65, 65, 65).set_alpha(it_hover_alpha.value * alpha.value * content_controller.get_alpha()));
			render_wrapper.text(pos + ImVec2(4.f, label_pos), name, context.pallete.accent.set_alpha(it_alpha.value * alpha.value * content_controller.get_alpha()));

			return clicked;
		};

		for (size_t i = 0; i < items.size(); i++) {
			auto it_pos = position + ImVec2(0.f, size.y + text_size.y + 5 + ((20.f * i) * (open_animation_interpolated.value / open_animation_static.value)));
			auto it_size = ImVec2(size.x, 20.f);

			if (invoke_item(items.at(i), it_pos, it_size, variable->at(i), 1)) {
				variable->at(i) = !variable->at(i);

				//content_controller.blocking = 0;
			}
		}
		render_wrapper.set_draw_list(ImGui::GetBackgroundDrawList());
	}

	render_wrapper.text(position, label, ImColor(50, 50, 50).set_alpha(content_controller.get_alpha()));
	render_wrapper.text(position, label, ImColor(65, 65, 65).set_alpha(hover_alpha.value * content_controller.get_alpha()));

	position.y += text_size.y;

	int v_size = 0;
	for (size_t i = 0; i < variable->size(); i++) {
		if (variable->at(i))
			v_size += 1;
	}

	std::string preview = v_size == 0 ? "None" : "";

	for (auto i = 0, j = 0; i < variable->size(); i++) {
		if (variable->at(i)) {
			if (j)
				preview += (", ") + (std::string)items.at(i);
			else
				preview = items.at(i);

			j++;
		}
	}

	//render.rect_gradient(position + ImVec2(1.f, 1.f), size - ImVec2(2.f, 2.f), context.pallete.disabled_element.set_alpha(content_controller.get_alpha()), ImColor(12, 12, 12).set_alpha(content_controller.get_alpha()), horizontal, 4.f);
	render_wrapper.fill_rect(position + ImVec2(1.f, 1.f), size - ImVec2(2.f, 2.f), context.pallete.disabled_element.set_alpha(content_controller.get_alpha()), 4.f);
	render_wrapper.rect(position, size, context.pallete.disabled_element_outline.set_alpha(content_controller.get_alpha()), 0.f, 4.f);
	render_wrapper.rect(position, size, ImColor(50, 50, 50).set_alpha(hover_alpha.value * content_controller.get_alpha()), 0.f, 3.f);

	render_wrapper.begin_clip_rect(position, size - ImVec2(4, 0));
	auto label_pos = size.y / 2 - render_wrapper.text_size(preview).y / 2;
	render_wrapper.text(position + ImVec2(label_pos + 2.f, label_pos), preview, ImColor(50, 50, 50).set_alpha(content_controller.get_alpha()));
	render_wrapper.text(position + ImVec2(label_pos + 2.f, label_pos), preview, ImColor(65, 65, 65).set_alpha(hover_alpha.value * content_controller.get_alpha()));
	render_wrapper.grad_h(position + ImVec2(size.x - 30, 1), ImVec2(26, size.y - 2), context.pallete.disabled_element.set_alpha(0.f), context.pallete.disabled_element.set_alpha(content_controller.get_alpha()));
	render_wrapper.end_clip_rect();
	context.offset += size.y + text_size.y + 10.f;
}