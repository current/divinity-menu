#include "window.hpp"
#include "animation_controller.hpp"
#include "content_controller.hpp"

void child_elements_t::keybind(const std::string& label, int* variable) {

	auto this_hash = std::hash<std::string>()(label + animation_controller.current_child);

	auto render_str = *variable <= 0 ? "..." : key_names[*variable] == nullptr ? "err" : key_names[*variable];
	auto render_str_size = render_wrapper.text_size(render_str);
	auto open_key_size = render_wrapper.text_size("Press a key");
	auto open_animation_static = animation_controller.get(label + "#keybind_open_animation_static" + animation_controller.current_child);
	open_animation_static.adjust((content_controller.blocking == this_hash) ? open_key_size.x : render_str_size.x, false);
	auto open_animation_interpolated = animation_controller.get(label + "#keybind_open_animation_interpolated" + animation_controller.current_child);

	if (open_animation_interpolated.value != open_animation_static.value) {
		open_animation_interpolated.value += (open_animation_static.value - open_animation_interpolated.value) * 0.2f;
		open_animation_interpolated.adjust(open_animation_interpolated.value, false);
	}

	auto size = ImVec2(8.f, 16.f); size.x += open_animation_interpolated.value;
	auto position = context.pos + ImVec2(context.size.x - 30.f - size.x, context.offset - context.scroll_offset);
	auto text_size = render_wrapper.text_size(label);

	auto hovered = utils.in_region(position, size) && utils.in_region(context.pos, context.size) && content_controller.blocking == 0;
	auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	bool skip_next_frame = false;
	if (clicked) {
		content_controller.blocking = this_hash;
		skip_next_frame = true;
	}

	if (content_controller.blocking == this_hash) {
		if (ImGui::IsKeyPressed(VK_ESCAPE, false) || ImGui::IsKeyPressed(VK_INSERT, false)) {
			*variable = 0;
			content_controller.blocking = 0;
		}
		else {
			for (auto i = 0; i < 5; i++) {
				if (ImGui::GetIO().MouseClicked[i] && !skip_next_frame) {
					switch (i) {
					case 0:
						*variable = VK_LBUTTON;
						break;
					case 1:
						*variable = VK_RBUTTON;
						break;
					case 2:
						*variable = VK_MBUTTON;
						break;
					case 3:
						*variable = VK_XBUTTON1;
						break;
					case 4:
						*variable = VK_XBUTTON2;
						break;
					}

					content_controller.blocking = 0;
				}
			}

			for (int i = 0; i < 255; i++) {
				if (ImGui::IsKeyPressed(i, false)) {
					*variable = i;
					content_controller.blocking = 0;
				}
			}
		}
	}


	auto alpha = animation_controller.get(label); alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (content_controller.blocking == this_hash ? 1.f : -1.f));
	auto hover_alpha = animation_controller.get(label + "#"); hover_alpha.adjust(hover_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (hovered ? 1.f : -1.f));

	render_wrapper.fill_rect(position + ImVec2(1.f, 1.f), size - ImVec2(2.f, 2.f), context.pallete.disabled_element.set_alpha(content_controller.get_alpha()), 3.f);
	render_wrapper.rect(position, size, context.pallete.disabled_element_outline.set_alpha(content_controller.get_alpha()), 1.f, 3.f);

	render_wrapper.rect(position, size, ImColor(50, 50, 50).set_alpha(hover_alpha.value * content_controller.get_alpha()), 1.f, 3.f);
	render_wrapper.begin_clip_rect(position, size);
	render_wrapper.text(position + size / 2 - render_wrapper.text_size(render_str) / 2, render_str, context.pallete.disabled_str.set_alpha(content_controller.get_alpha() - alpha.value));
	render_wrapper.text(position + size / 2 - render_wrapper.text_size("Press a key") / 2, "Press a key", context.pallete.accent.set_alpha(alpha.value * content_controller.get_alpha()));
	render_wrapper.end_clip_rect();
}