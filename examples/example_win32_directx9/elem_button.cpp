#include "window.hpp"
#include "animation_controller.hpp"
#include "content_controller.hpp"
void child_elements_t::button(const std::string& label, std::function<void()> on_click) {
	auto position = context.pos + ImVec2(0.f, context.offset - context.scroll_offset);
	auto size = ImVec2(context.size.x - 30.f, 24.f);
	auto text_size = render_wrapper.text_size(label);

	auto hovered = utils.in_region(position, size) && utils.in_region(context.pos, context.size) && content_controller.blocking == 0;
	auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	auto alpha = animation_controller.get(label + animation_controller.current_child); alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) *-1.f);

	if (clicked) {
		alpha.adjust(1.f);
	}

	auto hover_alpha = animation_controller.get(label + "#"); hover_alpha.adjust(hover_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (hovered ? 1.f : -1.f));

	auto disabled_element = ImColor(context.pallete.disabled_element.Value.x, context.pallete.disabled_element.Value.y, context.pallete.disabled_element.Value.z,content_controller.get_alpha());
	render_wrapper.fill_rect(position, size, disabled_element, 3.f);
	auto disabled_element_outline = ImColor(context.pallete.disabled_element_outline.Value.x, context.pallete.disabled_element_outline.Value.y, context.pallete.disabled_element_outline.Value.z,content_controller.get_alpha());
	render_wrapper.rect(position, size, disabled_element_outline, 0.f, 3.f);

	auto outline_col = ImColor(50 / 255.f, 50 / 255.f, 50 / 255.f, hover_alpha.value *content_controller.get_alpha());
	render_wrapper.rect(position, size, outline_col, 0.f, 3.f);

	auto disabled = ImColor(50 / 255.f, 50 / 255.f, 50 / 255.f,content_controller.get_alpha());
	render_wrapper.text(position + size / 2 - text_size / 2, label, disabled);
	render_wrapper.text(position + size / 2 - text_size / 2, label, context.pallete.accent.set_alpha(alpha.value * content_controller.get_alpha()));

	context.offset += size.y + 5.f;
	if (clicked) {
		on_click();
	}
}
