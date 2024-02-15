#include "window.hpp"
#include "util.hpp"

#include "animation_controller.hpp"
#include "content_controller.hpp"

void tab_controller_t::setup(context_t context) {
	base.window_position = context.position;
	base.window_size = context.size;
	base.pallete = context.pallete;
}

void tab_controller_t::begin(std::function<void()> content) {
	// Well we're calling the main tab controller no? We obviously have tabs
	base.has_tabs = true;


	elements.tab_index = 0;
	elements.tabs.clear();
	elements.subtabs.clear();

	content();
	invoke();
}

void tab_controller_t::add_tab(const std::string& tab_name, int* tab_index, std::vector<subtab_t> subtabs) {
	elements.tabs.push_back({ elements.tab_index++, tab_index, 0.f, tab_name, subtabs });
}

void tab_controller_t::add_subtabtab(const std::string& parent, const std::string& tab_name, std::function<void()> contents) {
	elements.subtabs.push_back({ elements.subtab_index++, 0.f, tab_name, contents });
}

// Main tab rendering
void tab_controller_t::invoke() {

	auto invoke_tab = [&](const std::string& name, ImVec2 pos, ImVec2 size, int self_index) -> bool {

		auto hovered = utils.in_region(pos, size);
		auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
		auto is_active_element = elements.current_tab == self_index;

		auto alpha = animation_controller.get(name + "#invoked_tab");

		alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (is_active_element || hovered ? 1.f : -1.f));

		render_wrapper.fill_rect(pos, size, ImColor(12, 12, 12).set_alpha(content_controller.window_alpha), 4.f, self_index == 0 ? ImDrawFlags_RoundCornersTop : self_index == elements.tabs.size() - 1 ? is_active_element ? ImDrawFlags_RoundCornersNone : ImDrawFlags_RoundCornersBottom : ImDrawFlags_RoundCornersNone);
		render_wrapper.rect(pos, size, base.pallete.main_outline.set_alpha(content_controller.window_alpha),1.f, 4.f, self_index == 0 ? ImDrawFlags_RoundCornersTop : self_index == elements.tabs.size() - 1 ? is_active_element ? ImDrawFlags_RoundCornersNone : ImDrawFlags_RoundCornersBottom : ImDrawFlags_RoundCornersNone);
		//render.fill_rect(pos + ImVec2(1, 0), ImVec2(2, size.y), base.pallete.accent.set_alpha(content_controller.window_alpha * alpha.value), 4.f, self_index == 0 ? ImDrawFlags_RoundCornersTop : self_index == elements.tabs.size() - 1 ? is_active_element ? ImDrawFlags_RoundCornersNone : ImDrawFlags_RoundCornersBottom : ImDrawFlags_RoundCornersNone);
		render_wrapper.set_font(content_controller.fonts.extra_light_title);
		auto label_calculation = size.y / 2.f - render_wrapper.text_size(name).y / 2.f;
		render_wrapper.text(pos + ImVec2(10, label_calculation), name, ImColor(50, 50, 50, int(255 * content_controller.window_alpha)));
		render_wrapper.pop_font();
		
		render_wrapper.set_font(content_controller.fonts.sub_title);
		label_calculation = size.y / 2.f - render_wrapper.text_size(name).y / 2.f;
		auto active_color = ImColor(base.pallete.active_str.Value.x, base.pallete.active_str.Value.y, base.pallete.active_str.Value.z, alpha.value * content_controller.window_alpha);
		render_wrapper.text(pos + ImVec2(10, label_calculation), name, active_color);
		render_wrapper.pop_font();

		return clicked;
	};

	auto invoke_subtab = [&](const std::string& name, const std::string& parent_name, ImVec2 pos, ImVec2 size, float state_alpha, int active_index, int self_index) -> bool {
		auto hovered = utils.in_region(pos, size);
		auto clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
		auto is_active_element = active_index == self_index;

		auto alpha = animation_controller.get(name + "#invoked_subtab" + parent_name);

		alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.4f) * (is_active_element || hovered ? 1.f : -1.f));

		render_wrapper.set_font(content_controller.fonts.extra_light);
		auto label_calculation = size.y / 2.f - render_wrapper.text_size(name).y / 2.f;
		render_wrapper.text(pos + ImVec2(12, label_calculation), name, ImColor(50, 50, 50, int(255 * content_controller.window_alpha * state_alpha)));

		render_wrapper.text(pos + ImVec2(12, label_calculation), name, base.pallete.accent.set_alpha(alpha.value * content_controller.window_alpha * state_alpha));

		render_wrapper.pop_font();

		return clicked;
	};

	content_controller.alpha.resize(elements.tabs.size());

	float offset = 0;
	for (auto& tab : elements.tabs) {
		auto pos = ImVec2(base.window_position.x + 15, base.window_position.y + 64.f + offset);
		if (invoke_tab(tab.name, pos, ImVec2(167 - 30, 32), tab.index)) {
			elements.last_tab = elements.current_tab, elements.current_tab = tab.index;
		}
		
		auto open_size_static = animation_controller.get(tab.name + "#static_open"); open_size_static.adjust(0.f, false);
		auto open_size_interpolated = animation_controller.get(tab.name + "#interp_open");

		auto subtabs = tab.subtab;

		if (tab.index == elements.current_tab) {
			open_size_static.value = 10.f + (18.f * subtabs.size());
		}

		if (content_controller.window_alpha != 1.00f) {
			open_size_static.value = 0.f;
		}

		if (open_size_interpolated.value != open_size_static.value) {
			auto v = open_size_interpolated.value += (open_size_static.value - open_size_interpolated.value) * 0.2f;
			open_size_interpolated.adjust(v, false);
		}

		auto alpha = animation_controller.get(tab.name + "#transition"); alpha.adjust(alpha.value + 3.f * render_wrapper.get_min_deltatime(0.2f) * (tab.index == elements.current_tab ? 1.f : -1.f));
		render_wrapper.fill_rect(pos + ImVec2(0, 32.f), ImVec2(167 - 30, open_size_interpolated.value), ImColor(12, 12, 12).set_alpha(content_controller.window_alpha * alpha.value), 4.f, tab.index == elements.tabs.size() - 1 ? ImDrawFlags_RoundCornersBottom : ImDrawFlags_RoundCornersNone);
		render_wrapper.rect(pos + ImVec2(0, 31.f), ImVec2(167 - 30, open_size_interpolated.value + 1), base.pallete.main_outline.set_alpha(content_controller.window_alpha * alpha.value), 1.f, 4.f, tab.index == elements.tabs.size() - 1 ? ImDrawFlags_RoundCornersBottom : ImDrawFlags_RoundCornersNone);
		render_wrapper.begin_clip_rect(pos + ImVec2(0, 32.f), ImVec2(167 - 30, open_size_interpolated.value));
		if (alpha.value > 0.0f) {
			for (auto& current_subtab : subtabs) {
				auto base = pos + ImVec2(0, (32.f + 5.f + (18.f * current_subtab.index)) * (open_size_interpolated.value / open_size_static.value));
				if (invoke_subtab(current_subtab.name, tab.name, base, ImVec2(167 - 30, 18), alpha.value, *current_subtab.current_index, current_subtab.index)) {
					*current_subtab.current_index = current_subtab.index;
					*tab.subtab_index = current_subtab.index;
				}
			}
		}
		render_wrapper.end_clip_rect();
		render_wrapper.grad_v(pos + ImVec2(1, 32.f), ImVec2(167 - 32, 8), ImColor(15, 15, 15).set_alpha(content_controller.window_alpha * alpha.value), ImColor(15, 15, 15).set_alpha(0));

		content_controller.current_tab = elements.current_tab;
		//content_controller.alpha.at(tab.index) = alpha.value;
		offset += 31.f + open_size_interpolated.value /* if tab is open*/;
	}


	auto subtab_index = *elements.tabs.at(elements.current_tab).subtab_index;
	elements.tabs.at(elements.current_tab).subtab.at( subtab_index ).content();
}
