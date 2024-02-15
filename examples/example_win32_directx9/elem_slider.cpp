#include "window.hpp"
#include "animation_controller.hpp"
#include "content_controller.hpp"
#include <filesystem>
#include "tinyformat.h"


void child_elements_t::slider(const std::string& label, float* variable, float min, float max, std::string format) {
	auto position = context.pos + ImVec2(0.f, context.offset - context.scroll_offset);
	auto size = ImVec2(context.size.x - 30.f, 12.f);
	auto text_size = render_wrapper.text_size(label);

	auto hash = std::hash<std::string>()(label + animation_controller.current_child);

	auto active_str = ImColor(context.pallete.active_str.Value.x, context.pallete.active_str.Value.y, context.pallete.active_str.Value.z, content_controller.get_alpha());
	render_wrapper.text(position, label, active_str);
	auto n = tfm::format(format.data(), *variable);
	render_wrapper.text(position + ImVec2(size.x - render_wrapper.text_size(n).x, 0.f), n, active_str);

	render_wrapper.text(position + ImVec2(size.x - render_wrapper.text_size(n).x, 0.f), n, active_str);

	position.y += text_size.y;

	auto hovered = utils.in_region(position, size) && utils.in_region(context.pos, context.size) && content_controller.blocking == 0;
	auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	if (content_controller.blocking == 0) {
		if (clicked)
			content_controller.blocking = hash;
	}
	else if (content_controller.blocking == hash) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {

			float offset = ImClamp(ImGui::GetIO().MousePos.x - position.x, 0.f, size.x);
			*variable = ImClamp(utils.map_number<float>(offset, 0, size.x, min, max), min, max);
		}
		else {
			content_controller.blocking = 0;
		}
	}

	auto alpha = animation_controller.get(label); alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * -1.f);

	auto hover_alpha = animation_controller.get(label + "#" + animation_controller.current_child); hover_alpha.adjust(hover_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (hovered || content_controller.blocking == hash ? 1.f : -1.f));

	render_wrapper.fill_rect(position + ImVec2(1.f, 1.f), size - ImVec2(2.f, 2.f), context.pallete.disabled_element.set_alpha(content_controller.get_alpha()), 3.f);
	auto width = utils.map_number<float>(*variable, min, max, 0.f, size.x);
	if (*variable != min)
		render_wrapper.fill_rect(position + ImVec2(1.f, 1.f), ImVec2(width, size.y) - ImVec2(2.f, 2.f), context.pallete.accent.set_alpha(content_controller.get_alpha()), 3.f);

	auto disabled_element_outline = ImColor(context.pallete.disabled_element_outline.Value.x, context.pallete.disabled_element_outline.Value.y, context.pallete.disabled_element_outline.Value.z, content_controller.get_alpha());
	render_wrapper.rect(position, size, disabled_element_outline, 0.f, 3.f);

	auto outline_col = ImColor(50 / 255.f, 50 / 255.f, 50 / 255.f, hover_alpha.value * content_controller.get_alpha());
	render_wrapper.rect(position, size, outline_col, 0.f, 3.f);


	context.offset += size.y + text_size.y + 12.f;
}

void child_elements_t::slider(const std::string& label, int* variable, int min, int max, std::string format) {
	auto position = context.pos + ImVec2(0.f, context.offset - context.scroll_offset );
	auto size = ImVec2(context.size.x - 30.f, 12.f);
	auto text_size = render_wrapper.text_size(label);

	auto hash = std::hash<std::string>()(label);

	auto active_str = ImColor(context.pallete.active_str.Value.x, context.pallete.active_str.Value.y, context.pallete.active_str.Value.z, content_controller.get_alpha());
	render_wrapper.text(position, label, active_str);
	//render.text(position + ImVec2(size.x - render.text_size(std::vformat(format, std::make_format_args(*variable))).x, 0.f), std::vformat(format, std::make_format_args(*variable)), active_str);
	render_wrapper.text(position + ImVec2(size.x - render_wrapper.text_size(std::to_string(*variable)).x, 0.f), std::to_string(*variable), active_str);

	position.y += text_size.y;

	auto hovered = utils.in_region(position, size) && utils.in_region( context.pos, context.size ) && content_controller.blocking == 0;
	auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	if (content_controller.blocking == 0) {
		if (clicked)
			content_controller.blocking = hash;
	}
	else if (content_controller.blocking == hash) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {

			//float offset = ImClamp(ImGui::GetIO().MousePos.x - position.x, 0.f, size.x);
			//*variable = ImClamp(utils.map_number<int>(offset, 0, size.x, min, max), min, max);

            float offset = ImClamp( ImGui::GetIO( ).MousePos.x - position.x, 0.f, size.x );
            *variable = ImClamp( utils.map_number<float>( offset, 0, size.x, (float)min, max ), (float)min, ( float )max );
		}
		else {
			content_controller.blocking = 0;
		}
	}

	auto alpha = animation_controller.get(label); alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * -1.f);

	auto hover_alpha = animation_controller.get(label + "#"); hover_alpha.adjust(hover_alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (hovered || content_controller.blocking == hash ? 1.f : -1.f));

	render_wrapper.fill_rect(position + ImVec2(1.f, 1.f), size - ImVec2(2.f, 2.f), context.pallete.disabled_element.set_alpha(content_controller.get_alpha()), 3.f);

    auto width = utils.map_number<float>( *variable, min, max, 0.f, size.x );
    if ( *variable != min )
        render_wrapper.fill_rect( position + ImVec2( 1.f, 1.f ), ImVec2( width, size.y ) - ImVec2( 2.f, 2.f ), context.pallete.accent.set_alpha( content_controller.get_alpha( ) ), 3.f );

	auto disabled_element_outline = ImColor(context.pallete.disabled_element_outline.Value.x, context.pallete.disabled_element_outline.Value.y, context.pallete.disabled_element_outline.Value.z, content_controller.get_alpha());
	render_wrapper.rect(position, size, disabled_element_outline, 0.f, 3.f);

	auto outline_col = ImColor(50 / 255.f, 50 / 255.f, 50 / 255.f, hover_alpha.value * content_controller.get_alpha());
	render_wrapper.rect(position, size, outline_col, 0.f, 3.f);


	context.offset += size.y + text_size.y + 12.f;
}
