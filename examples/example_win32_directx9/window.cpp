#include "window.hpp"
#include "content_controller.hpp"

void window_t::handle_toggle( bool& v ) {
    if ( GetAsyncKeyState( menu_key ) & 1 ) // change lol!
        v = !v;
}

void window_t::handle_movement( ImVec2& position, ImVec2 size, float header_height ) {
    if ( utils.in_region( position, { 167, 60 } ) && ImGui::IsMouseClicked( ImGuiMouseButton_Left ) ) {
        util.drag_controller.dragging = true;
        util.drag_controller.menu_click_pos = position;
        util.drag_controller.click_pos = ImGui::GetIO( ).MousePos;
    }

    if ( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) ) {
        util.drag_controller.dragging = false;
    }

    if ( util.drag_controller.dragging ) {
        auto display_size = ImGui::GetIO( ).DisplaySize;
        auto new_pos = util.drag_controller.menu_click_pos + ImGui::GetIO( ).MousePos - util.drag_controller.click_pos;

        position = ImClamp( new_pos, ImVec2( 10.f, 10.f ), display_size - size - ImVec2( 10.f, 10.f ) );
    }
}

void window_t::handle_size( ImVec2 position, ImVec2& size ) {
    auto clamp_size = 8.f;

    if ( !util.resize_controller.allow_resizing ) {
        return;
    }

    if ( utils.in_region( position + size - ImVec2( clamp_size, clamp_size ), { clamp_size , clamp_size } ) && ImGui::IsMouseClicked( ImGuiMouseButton_Left ) ) {
        util.resize_controller.resizing = true;
        util.resize_controller.menu_click_pos = size;
        util.resize_controller.click_pos = ImGui::GetIO( ).MousePos;
    }

    if ( util.resize_controller.resizing || utils.in_region( position + size - ImVec2( clamp_size, clamp_size ), { clamp_size, clamp_size } ) ) {
        ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeNWSE );
    }

    if ( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) ) {
        util.resize_controller.resizing = false;
    }

    if ( util.resize_controller.resizing ) {
        auto display_size = ImGui::GetIO( ).DisplaySize;
        auto new_size = util.resize_controller.menu_click_pos + ImGui::GetIO( ).MousePos - util.resize_controller.click_pos;

        size = ImClamp( new_size, util.min_size, display_size - ImVec2( 20.f, 20.f ) );
    }
}

void window_t::update_context( ImVec2 position, ImVec2 size ) {
    render_wrapper.set_draw_list( ImGui::GetBackgroundDrawList( ) );
    util.position = position;
    util.size = size;
    if ( !util.did_fill_min_size ) {
        util.min_size = size;
        util.did_fill_min_size = true;
    }
}

void window_t::begin_new_frame( const std::string& title, ImVec2& position, ImVec2& size, std::function<void( )> fn ) {
    /* handle menu input */
    handle_toggle( open );

    if ( !open ) {
        content_controller.blocking = 0;
    }

    alpha = ImClamp( alpha + 3.f * render_wrapper.get_min_deltatime( 0.4f ) * ( open ? 1.f : -1.f ), 0.f, 1.f );
    content_controller.window_alpha = alpha;

    if ( !alpha )
        return;

    update_context( position, size );

    auto header_height = 28.f;
    handle_movement( position, size, header_height );
    handle_size( position, size );

    // dummy rect so that our background looks like it fades out slower, imgui issues :neutral:
    render_wrapper.fill_rect( position, size, util.pallete.background.set_alpha( alpha ), 4.f );

    render_wrapper.fill_rect( position, ImVec2( 167, size.y ), ImColor( 9, 9, 9 ).set_alpha( alpha ), 4.f, ImDrawFlags_RoundCornersLeft );
    render_wrapper.grad_h( position + ImVec2( 167, 0 ), ImVec2( 25, size.y ), ImColor( 9, 9, 9 ).set_alpha( alpha ), util.pallete.background.set_alpha( 0 ) );
   
    render_wrapper.fill_rect( position + ImVec2( 165, 0 ), ImVec2( 2, size.y ), util.pallete.accent.set_alpha( alpha ) );
   
    render_wrapper.fill_rect( position + ImVec2( 167 + 15, 15 ), ImVec2( size.x - 30 - 167, size.y - 30 ), ImColor( 9, 9, 9 ).set_alpha( alpha ), 4.f );
    render_wrapper.rect( position + ImVec2( 167 + 15, 15 ), ImVec2( size.x - 30 - 167, size.y - 30 ), util.pallete.main_outline.set_alpha( alpha ), 1.f, 4.f );
   
    render_wrapper.set_font( content_controller.fonts.title );
    render_wrapper.text( position + ImVec2( 167 / 2 - render_wrapper.text_size( title ).x / 2, 15.f ), title, util.pallete.active_str.set_alpha( alpha ) );
    render_wrapper.pop_font( );
   
    render_wrapper.rect( position, size, util.pallete.main_outline.set_alpha( alpha ), 1.f, 4.f );

    	fn();
}

void window_t::enter_new_frame( ) {
    tab_controller.base.has_tabs = false;
}
