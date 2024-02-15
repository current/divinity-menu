#include "window.hpp"
#include "content_controller.hpp"

void child_elements_t::label(const std::string& label) {
	
	auto position = context.pos + ImVec2(0.f, -2 + context.offset - context.scroll_offset);
	auto text_size = render_wrapper.text_size(label);

	auto active_str = ImColor(context.pallete.active_str.Value.x, context.pallete.active_str.Value.y, context.pallete.active_str.Value.z,content_controller.get_alpha());
	render_wrapper.text(position, label, active_str);

	context.offset += text_size.y + 10.f;
}