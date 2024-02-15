#include "window.hpp"
#include "animation_controller.hpp"
#include "content_controller.hpp"
#include <algorithm>

std::string to_upper(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return toupper(c); });
	return s;
}

void child_elements_t::text_input(const std::string& label, std::string* variable) {
	auto position = context.pos + ImVec2(0.f, context.offset - context.scroll_offset);
	auto size = ImVec2(context.size.x - 30.f, 24.f);
	auto text_size = render_wrapper.text_size(label);

	auto this_hash = std::hash<std::string>()(label + animation_controller.current_child);
	auto hovered = utils.in_region(position + ImVec2(0.f, text_size.y), size) && utils.in_region(context.pos, context.size) && content_controller.blocking == 0;
	auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	auto alpha = animation_controller.get(label + animation_controller.current_child); alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (content_controller.blocking == this_hash ? 1.f : -1.f));
	auto hover_alpha = animation_controller.get(label + "#" + animation_controller.current_child); hover_alpha.adjust(hover_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (hovered || content_controller.blocking == this_hash ? 1.f : -1.f));
	
	bool skip_next_frame = false;

	if (clicked) {
		content_controller.blocking = this_hash;
		skip_next_frame = true;
	}


	if (content_controller.blocking == this_hash || alpha.value > 0.00f) {
		if (!utils.in_region(position + ImVec2(0.f, text_size.y + size.y + 5.f), size) && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !skip_next_frame) {
			content_controller.blocking = 0;
		}

		auto str = *variable;

		for (size_t i = 0; i <= 255; i++) {
			if (ImGui::IsKeyPressed(i)) {
				if (i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT || i == VK_DELETE) {
					content_controller.blocking = 0;
					continue;
				}

				if (i == VK_SPACE) {
					str.append(" ");
					continue;
				}

				if (i == VK_BACK) {
					str = str.substr(0, strlen(str.data()) - 1);
					continue;
				}

				if (i == VK_SHIFT) {
					continue;
				}

				if (key_names[i] != nullptr) {
					if (ImGui::IsKeyDown(( ImGuiKey )VK_SHIFT)) {
						str.append(to_upper(key_names[i]));
					}
					else {
						str.append(key_names[i]);
					}
				}
			}
		}

		*variable = str;
	}

	render_wrapper.text(position, label, ImColor(50, 50, 50).set_alpha(content_controller.get_alpha()));
	render_wrapper.text(position, label, ImColor(65, 65, 65).set_alpha(hover_alpha.value * content_controller.get_alpha()));

	position.y += text_size.y;

	render_wrapper.fill_rect(position + ImVec2(1.f, 1.f), size - ImVec2(2.f, 2.f), context.pallete.disabled_element.set_alpha(content_controller.get_alpha()), 4.f);
	render_wrapper.rect(position, size, context.pallete.disabled_element_outline.set_alpha(content_controller.get_alpha()), 0.f, 4.f);
	render_wrapper.rect(position, size, ImColor(50, 50, 50).set_alpha(hover_alpha.value * content_controller.get_alpha()), 0.f, 3.f);

	render_wrapper.begin_clip_rect(position, size - ImVec2(4, 0));
	auto label_pos = size.y / 2 - render_wrapper.text_size(*variable).y / 2;
	render_wrapper.text(position + ImVec2(label_pos + 2.f, label_pos), *variable, ImColor(50, 50, 50).set_alpha(content_controller.get_alpha()));
	render_wrapper.text(position + ImVec2(label_pos + 2.f, label_pos), *variable, ImColor(65, 65, 65).set_alpha(hover_alpha.value * content_controller.get_alpha()));
	render_wrapper.grad_h(position + ImVec2(size.x - 30, 1), ImVec2(26, size.y - 2), context.pallete.disabled_element.set_alpha(0.f), context.pallete.disabled_element.set_alpha(content_controller.get_alpha()));
	render_wrapper.end_clip_rect();

	context.offset += size.y + text_size.y + 5.f;
}
