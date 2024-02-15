#include "window.hpp"
#include "content_controller.hpp"
#include "animation_controller.hpp"
#include <iostream>

void child_controller_t::setup(context_t context) {
	base.window_position = context.position;
	base.window_size = context.size;
	base.pallete = context.pallete;
}

void child_controller_t::begin(std::function<void()> content) {

	elements.childs.clear();

	content();
	invoke();
}

void child_controller_t::add_child(const std::string& child_name, e_child_placement child_placement, e_child_sizing child_size, std::function<void()> child_contents) {
	elements.childs.push_back({ child_name, child_placement, child_size, 0, child_contents });
}

void child_controller_t::invoke() {
	// render children
	for (auto& child : elements.childs) {
		// init on top left
		auto position = base.window_position + ImVec2(197, 30.f);
		auto size = base.window_size - ImVec2(197, 30);

		if (child.child_size == e_child_size_full) {
			size -= ImVec2(45.f, 30.f);
			size.x /= 2.f;
		}
		else {
			size -= ImVec2(45.f, 45.f);
			size /= 2.f;
		}

		switch (child.child_placement) {
		case e_child_top_left:
			position = base.window_position + ImVec2(197, 30.f);
			break;
		case e_child_bottom_left:
			position = base.window_position + ImVec2(197, 30.f) + ImVec2(0.f, size.y + 15.f);
			break;
		case e_child_top_right:
			position = base.window_position + ImVec2(197, 30.f) + ImVec2(size.x + 15.f, 0.f);
			break;
		case e_child_bottom_right:
			position = base.window_position + ImVec2(197, 30.f) + ImVec2(size.x + 15.f, size.y + 15.f);
			break;
		}

		render_wrapper.fill_rect(position, size, base.pallete.child_background.set_alpha(content_controller.get_alpha()), 3.f);
		render_wrapper.fill_rect(position, ImVec2(size.x, base.child_header_size), base.pallete.child_header.set_alpha(content_controller.get_alpha()), 3.f, ImDrawFlags_RoundCornersTop);
		render_wrapper.fill_rect(position + ImVec2(0, base.child_header_size), ImVec2(size.x, 2), base.pallete.accent.set_alpha(content_controller.get_alpha()));
		render_wrapper.rect(position, size, base.pallete.main_outline.set_alpha(content_controller.get_alpha()), 1.f, 3.f);
		auto active_str = ImColor(base.pallete.active_str.Value.x, base.pallete.active_str.Value.y, base.pallete.active_str.Value.z,content_controller.get_alpha());

		render_wrapper.set_font(content_controller.fonts.sub_title);
		auto title_placement = base.child_header_size / 2.f - render_wrapper.text_size(child.name).y / 2;
		render_wrapper.text(position + ImVec2(title_placement + 2, title_placement), child.name, active_str);
		render_wrapper.pop_font();
		
		auto scroll_amount = animation_controller.get(child.name + "#scroll_amount");
		auto scroll_offset_static = animation_controller.get(child.name + "#scroll_offset_static");
		auto scroll_offset_interpolated = animation_controller.get(child.name + "#scroll_offset_interpolated");
		auto visible_size = size.y - base.child_header_size;
		auto scroll_max = scroll_amount.value - visible_size;

		child_elem.update({ 15.f, position + ImVec2(15.f, base.child_header_size), ImVec2(size.x, size.y - base.child_header_size), base.pallete, scroll_offset_interpolated.value });

		if (scroll_offset_interpolated.value != scroll_offset_static.value) {
			scroll_offset_interpolated.value += (scroll_offset_static.value - scroll_offset_interpolated.value) * 0.2f;
			scroll_offset_interpolated.adjust(scroll_offset_interpolated.value, false);
		}

		if (scroll_amount.value > visible_size && content_controller.blocking == 0 && utils.in_region(position + ImVec2(0, base.child_header_size), ImVec2(size.x, size.y - base.child_header_size))) {
			//std::cout << "my nigga '" << child.name << "' has scroll g" << std::endl;

			if (ImGui::GetIO().MouseWheel > 0) {
				scroll_offset_static.adjust(ImClamp(scroll_offset_static.value - 15, 0.f, scroll_max), false);
				std::cout << "scrolled up" << std::endl;
			}

			if (ImGui::GetIO().MouseWheel < 0) {
				scroll_offset_static.adjust(ImClamp(scroll_offset_static.value + 15, 0.f, scroll_max), false);
				std::cout << "scrolled down" << std::endl;
			}
		}
		animation_controller.current_child = child.name;
		render_wrapper.begin_clip_rect(position + ImVec2(0, base.child_header_size + 2), ImVec2(size.x, size.y - base.child_header_size - 4));
		child.contents();
		render_wrapper.end_clip_rect();

		//std::cout << "child name: " << child.name << ", visible_size: " << visible_size << ", context offset: " << child_elem.context.offset << std::endl;
		scroll_amount.adjust(child_elem.context.offset, false);
	}
}

void child_elements_t::update(element_context_t d_context) {
	context = d_context;
}