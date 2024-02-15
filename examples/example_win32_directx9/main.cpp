// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#define  IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>

#include "window.hpp"
#include "kanit regular.hpp"
#include "kanit semibold.hpp"
#include "content_controller.hpp"
#include "kanit extralight.hpp"
#include "render_wrapper.hpp"

#include "bytes.hpp"
#include "custom.hpp"

#define ALPHA    ( ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )
#define NO_ALPHA ( ImGuiColorEditFlags_NoTooltip    | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )

using namespace ImGui;

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
   // ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    ImGui::GetIO( ).FontDefault = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( kanit_regular_data, kanit_regular_size, 20.f );
    content_controller.fonts.title = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( kanit_semibold_data, kanit_semibold_size, 42.f );
    content_controller.fonts.sub_title = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( kanit_semibold_data, kanit_semibold_size, 22.f );
    content_controller.fonts.extra_light_title = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( kanit_extralight_data, kanit_extralight_size, 22.f );
    content_controller.fonts.weapons = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( weapon_icons_data, weapon_icons_size, 24.f );


    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        static bool bools[50]{};
        static int ints[50]{};
        vector < const char* > items = { "Head", "Chest", "Body", "Legs", "Feet" };

        static float color[4] = { 1.f, 1.f, 1.f, 1.f };
        static int tabindx = 0;
        std::vector<std::string> tabs = { "Aimbot", "Visuals", "Misc" };
        

        render_wrapper.set_draw_list( ImGui::GetBackgroundDrawList( ) );
        render_wrapper.set_font( content_controller.fonts.sub_title );
#ifndef nigga
        auto title = "DIVINITY DEV";
#else
        auto title = "DIVINITY BETA";
#endif
        auto title_size = render_wrapper.text_size( title );
        render_wrapper.pop_font( );

        static auto fps = 0;
        static auto last_update = 0;
        if ( ImGui::GetFrameCount( ) > last_update + 60 ) {
            fps = ImGui::GetIO( ).Framerate;
            last_update = ImGui::GetFrameCount( );
        }

        auto ms = 0;
        auto str = std::string( "framework" ).append( " / " ).append( std::to_string( fps ) ).append( " fps" );// / ").append(std::to_string(ms)).append("ms");
        auto text_size = render_wrapper.text_size( str );
        auto size = ImVec2( title_size.x + 10 + text_size.x + 12, 22 );
        auto screen_size = ImGui::GetIO( ).DisplaySize;
        auto pos = ImVec2( screen_size.x - size.x - 10, 10 );

        render_wrapper.fill_rect( pos, size, ImColor( 9, 9, 9 ), 4.f );
        render_wrapper.fill_rect( pos + ImVec2( 10 + title_size.x, 0 ), size - ImVec2( 10 + title_size.x, 0 ), ImColor( 12, 12, 12 ), 4.f, ImDrawFlags_RoundCornersRight );
        render_wrapper.grad_h( pos + ImVec2( 10 + title_size.x, 0 ), ImVec2( 20, size.y ), ImColor( 9, 9, 9 ), ImColor( 9, 9, 9, 0 ) );

        ImColor accent = ImColor( 255, 0, 0 );
        render_wrapper.fill_rect( pos + ImVec2( 10 + title_size.x - 1, 0 ), ImVec2( 2, size.y ), accent );
        render_wrapper.rect( pos, size, ImColor( 32, 32, 32 ), 1.f, 4.f );

        render_wrapper.set_font( content_controller.fonts.sub_title );
        render_wrapper.text( pos + ImVec2( 5, -1 ), title, ImColor( 215, 215, 215 ) );
        render_wrapper.pop_font( );

        render_wrapper.text( pos + ImVec2( 10 + title_size.x + 6, 0 ), str, ImColor( 120, 120, 120 ) );
        render_wrapper.text( pos + ImVec2( 10 + title_size.x + 6, 0 ), "framework", ImColor( 215, 215, 215 ) );
        render_wrapper.text( pos + ImVec2( 10 + title_size.x + 6 + render_wrapper.text_size( std::string( "framework" ).append( " / " ) ).x, 0 ), std::to_string( ( int )fps ), ImColor( 215, 215, 215 ) );

        static ImVec2 posi = ImVec2( 100, 100 );
        static ImVec2 sizem = ImVec2( 650, 450 );

        _tab_controler.setup( {
            posi, sizem, color_pallete( )
                              } );

        _child.setup( {
            posi, sizem, color_pallete( )
                              } );

        static int tab_nr;
        static int stab_nr;

        static bool check = false;

        static int sl = 0;
        static int sl3 = 0;
        static int sl4 = 0;
        static int sl5 = 0;
        static float sl2 = 0;

        static float color_bos[ 4 ] = { 1.0f, 1.0f, 1.0f, 1.0f };

        static bool a[ 5 ];
        static bool a2[ 50 ];
        static std::vector<bool> elem = {
                     check, a2[ 1 ], a2[ 2 ], a2[ 3 ], a2[ 4 ]
        };

        std::vector< subtab_t > tab_1subtab;

        tab_1subtab.push_back( subtab_t{ 0, "rage", [ & ]( ) {
            _child.begin( [ ]( ) {
                _child.add_child( "child 1", e_child_top_left, e_child_size_full, [ & ]( ) {
                    _child.child_elem.color_picker( "Element: colorpicker", color_bos, true );
                    _child.child_elem.checkbox( "Element: checkbox", &check );
                    _child.child_elem.keybind( "Element: keybind", &sl5 );
                    _child.child_elem.checkbox( "Element: checkbox", &check );
                    _child.child_elem.label( "Element: label" );
                    _child.child_elem.button( "Element: button", [ ]( ) { } );

                    _child.child_elem.slider( "Element: slider int", &sl, 0, 100 );
                    _child.child_elem.slider( "Element: slider float", &sl2, 0, 100 );
                    _child.child_elem.slide_select( "Element: slide select", &sl3, { "item 1", "item 2", "item 3" } );
                    _child.child_elem.dropdown( "Element: dropdown", &sl4, { "item 1", "item 2", "item 3" } );

               
                  
                  _child.child_elem.multiselect( "Element: multidropdown", &elem, { "it1", "it2", "it3", "it4" } );

                } );

                _child.add_child( "child 2", e_child_top_right, e_child_size_full, [ & ]( ) {
                    for ( int i = 0; i < 50; i++ ) {
                        _child.child_elem.checkbox( "checkbox " + std::to_string( i ), &a2[ i ] );
                    }
                 } );
            } );
        }, &stab_nr } );
        tab_1subtab.push_back( subtab_t{ 1, "visuals", [ ]( ) { }, &stab_nr } );
        tab_1subtab.push_back( subtab_t{ 2, "anti-aim", [ ]( ) { }, &stab_nr } );

        _window.begin_new_frame( "Framework", posi, sizem, [ & ]( ) {
            _tab_controler.begin( [ & ]( ) {
                _tab_controler.add_tab( "tab 1", &tab_nr, tab_1subtab );
                _tab_controler.add_tab( "tab 2", &tab_nr, {} );
                _tab_controler.add_tab( "tab 3", &tab_nr, {} );
                _tab_controler.add_tab( "tab 4", &tab_nr, {} );

            } );
        } );


        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f), (int)(clear_color.y*clear_color.w*255.0f), (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
