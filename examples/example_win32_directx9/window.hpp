#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <string>
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <stack>
#include <vector>
#include <map>
#include <wtypes.h>
#include <functional>

// The main things
#include "util.hpp"
#include "render_wrapper.hpp"

struct tab_controller_t {

    struct {
        bool has_tabs = false;

        // Pushed by tab_controller_t::setup()
        ImVec2 window_position = ImVec2( 0, 0 );
        ImVec2 window_size = ImVec2( 0, 0 );
        float window_header_size = 32.f; // retarded but can be manually changed if we really need to change it loL!

        color_pallete pallete;
    } base;

    struct {
        int current_tab = 0;
        int current_subtab = 0;
        int last_tab = -1;
        int tab_index = 0;
        int subtab_index = 0;
        std::vector<inner_content_t> tabs;
        std::vector<inner_subtab_content_t> subtabs;
    } elements;

    void setup( context_t context );

    void begin( std::function<void( )> content );

    void add_tab( const std::string& tab_name, int* tab_index, std::vector<subtab_t> subtabs );

    void add_subtabtab( const std::string& parent, const std::string& tab_name, std::function<void( )> contents );

    void invoke( );
}; inline tab_controller_t _tab_controler;


struct element_context_t {
	float offset;
	ImVec2 pos;
	ImVec2 size;
	color_pallete pallete;
	float scroll_offset;
};

struct child_elements_t {
	void update(element_context_t d_context);

	element_context_t context;

	void label(const std::string& label);
	void button(const std::string& label, std::function<void()> on_click = []{});
	bool checkbox(const std::string& label, bool* variable);
	void slider(const std::string& label, float* variable, float min, float max, std::string format = "%.2f");
	void slider(const std::string& label, int* variable, int min, int max, std::string format = "%");
	void slide_select(const std::string& label, int* variable, std::vector<std::string> options);
	void dropdown(const std::string& label, int* variable, std::vector<std::string> items);
	void color_picker(const std::string& label, float* variable, bool show_alpha_bar = false);
	//void color_picker(const std::string& label, Color* variable, bool show_alpha_bar = false);
	void keybind(const std::string& label, int* variable);
	void multiselect(const std::string& label, std::vector<bool>* variable, std::vector<std::string> items);
	void text_input(const std::string& label, std::string* variable);
};

struct child_controller_t {

	struct {
		ImVec2 window_position;
		ImVec2 window_size;
		float window_header_size = 28.f;
		float child_header_size = 32.f;

		color_pallete pallete;
	} base;

	struct {
		std::vector<child_content_t> childs;
	} elements;
	
	child_elements_t child_elem;

	void setup(context_t context);

	void begin(std::function<void()> content);

	void add_child(const std::string& child_name, e_child_placement child_placement, e_child_sizing child_size, std::function<void()> child_contents);

	void invoke();
}; inline child_controller_t _child;

struct window_t {

    bool open = false;
    int menu_key = VK_INSERT;
    float alpha = 0.f;

    struct {
        struct {
            bool dragging;
            ImVec2 menu_click_pos;
            ImVec2 click_pos;
        } drag_controller;

        struct {
            bool resizing;
            ImVec2 menu_click_pos;
            ImVec2 click_pos;
            bool allow_resizing = true;
        } resize_controller;

        // Only really used in the tab controller and child controller because we dont use classes and cant call position from a deprived class :D
        ImVec2 position;
        ImVec2 size;
        bool did_fill_min_size = false;
        ImVec2 min_size;

        color_pallete pallete;
    } util;

    void handle_toggle( bool& v );
    void handle_movement( ImVec2& position, ImVec2 size, float header_height );
    void handle_size( ImVec2 position, ImVec2& size );
    void update_context( ImVec2 position, ImVec2 size );

    // Begins window
    void begin_new_frame( const std::string& title, ImVec2& position, ImVec2& size, std::function<void( )> fn );
    // Recalibrates everything ready for the next render frame
    void enter_new_frame( );

    tab_controller_t tab_controller;
    child_controller_t child_controller;

}; inline window_t _window;
