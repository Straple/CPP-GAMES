#define GAME_ENGINE_TAG "lunar oblivion engine v1.0"

#define DEBUG_MODE

#include <windows.h>

#undef max
#undef min

#include "utils.h"

#include "Objects/objects.h"

Engine_state eng_state(RUN_SETUP | CONSOLE_VIS | FULLSCREEN | CAMERA);

#include "Sprites/sprite.cpp"

Camera camera;

Render_state render_state;

dot arena_half_size;

#include "Render/render.cpp"

#include "Game/game.cpp"

void update_controls(HWND& window, Input& input) {

	// обнуляем значение нажатия кнопки
	for (int i = 0; i < BUTTON_COUNT; i++) {
		input.set_button(button_t(i), input.button_is_down(button_t(i)), false);
	}

	auto button_up = [&input](button_t b) -> void {
		input.set_button(b, false, true);
	};

	auto button_down = [&input](button_t b) -> void {
		input.set_button(b, true, true);
	};

	bool isPeekMessage = false;

	MSG message;
	while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
		isPeekMessage = true;

		switch (message.message) {

		case WM_LBUTTONUP: {

			button_up(BUTTON_MOUSE_L);

		}break;

		case WM_LBUTTONDOWN: {

			button_down(BUTTON_MOUSE_L);

		}break;

		case WM_RBUTTONUP: {
			button_up(BUTTON_MOUSE_R);

		}break;

		case WM_RBUTTONDOWN: {

			button_down(BUTTON_MOUSE_R);

		}break;

		case WM_KEYUP:
		case WM_KEYDOWN: {

			u64 vk_code = message.wParam;
			bool is_down = (message.message == WM_KEYDOWN);


#define update_button(b, vk)\
case vk: {\
	input.set_button(b, is_down, (is_down != input.button_is_down(b)));\
} break;

			switch (vk_code) {
				update_button(BUTTON_UP, VK_UP);
				update_button(BUTTON_DOWN, VK_DOWN);
				update_button(BUTTON_W, 'W');
				update_button(BUTTON_S, 'S');
				update_button(BUTTON_D, 'D');
				update_button(BUTTON_A, 'A');
				update_button(BUTTON_B, 'B');
				update_button(BUTTON_C, 'C');
				update_button(BUTTON_E, 'E');
				update_button(BUTTON_F, 'F');
				update_button(BUTTON_J, 'J');
				update_button(BUTTON_K, 'K');
				update_button(BUTTON_DEL, VK_DELETE);
				update_button(BUTTON_LEFT, VK_LEFT);
				update_button(BUTTON_RIGHT, VK_RIGHT);
				update_button(BUTTON_ENTER, VK_RETURN);
				update_button(BUTTON_ESC, VK_ESCAPE);
				update_button(BUTTON_TAB, VK_TAB);
				update_button(BUTTON_SPACE, VK_SPACE);
				update_button(BUTTON_0, VK_NUMPAD0);
				update_button(BUTTON_1, VK_NUMPAD1);
				update_button(BUTTON_2, VK_NUMPAD2);
				update_button(BUTTON_3, VK_NUMPAD3);
				update_button(BUTTON_4, VK_NUMPAD4);
				update_button(BUTTON_5, VK_NUMPAD5);
				update_button(BUTTON_6, VK_NUMPAD6);
				update_button(BUTTON_7, VK_NUMPAD7);
				update_button(BUTTON_8, VK_NUMPAD8);
				update_button(BUTTON_9, VK_NUMPAD9);
			}

#undef update_button

		}break;

		default: {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		}
	}

	if (isPeekMessage) {
		// mouse update

		RECT rect;
		GetWindowRect(window, &rect);

		mouse.pos = dot(
			static_cast<s64>(message.pt.x) - std::max<s32>(0, rect.left) + 0.2,
			static_cast<s64>(rect.bottom) - message.pt.y
		)
			/ scale_factor - arena_half_size;
	}
}

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	switch (uMsg) {
	case WM_CLOSE:
	case WM_DESTROY: {
		eng_state.flip(RUN_SETUP);
	} break;

	case WM_SIZE: {

		// get rect window
		{
			RECT rect;
			GetClientRect(hwnd, &rect);

			render_state.resize(rect.right, rect.bottom);
		}

		// relax scaling
		scale_factor = render_state.height * render_scale;

		// relax arena
		arena_half_size = dot(static_cast<point_t>(render_state.width) / scale_factor, static_cast<point_t>(1) / render_scale) * 0.5;

	} break;

	default: {
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
	return result;
}

void create_window_objects() {

}

int main() {

	//=========================================//
	//==============CREATE WINDOW==============//
	//=========================================//


	ShowWindow(GetConsoleWindow(), eng_state.show_console() ? SW_SHOW : SW_HIDE);

	ShowCursor(eng_state.show_cursor());

	read_sprites();

	// Create Window class
	WNDCLASS window_class = {};
	{
		window_class.style = CS_HREDRAW | CS_VREDRAW;
		window_class.lpszClassName = L"Game Window Class";
		window_class.lpfnWndProc = window_callback;
		window_class.hIcon = static_cast<HICON>(LoadImage(NULL, L"apple.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	}

	// Register class
	RegisterClass(&window_class);

	HINSTANCE hInstance = GetModuleHandle(NULL); // дескриптор указанного модуля

	// Create window
	HWND window = CreateWindow(window_class.lpszClassName, L"C++ Game Engine", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1600, 900, 0, 0, hInstance, 0);
	
	// SetWindowLong WS макросы
	// 
	// WS_CAPTION | WS_SYSMENU | WS_TABSTOP | WS_MINIMIZEBOX | WS_SIZEBOX
	// WS_CAPTION создает верхнюю панель окна и убирает окно из fullscreen
	// WS_SYSMENU создает "крестик" на панели окна
	// WS_TABSTOP создает "квадратик" на панели окна
	// WS_MINIMIZEBOX создает кнопку сворачивания окна
	// WS_SIZEBOX создает бох для изменения размеров окна
	// вместе они создают обычное окно

	auto set_fullscreen_mod = [&]() {
		SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		EnableWindow(window, true);
	};
	
	auto set_window_mod = [&]() {
		SetWindowLong(window, GWL_STYLE, WS_CAPTION | WS_SYSMENU | WS_TABSTOP | WS_MINIMIZEBOX | WS_SIZEBOX);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_SHOWWINDOW);
		EnableWindow(window, true);
	};

	if (eng_state.fullscreen_mode()) {
		set_fullscreen_mod();
	}
	else {
		set_window_mod();
	}

	// дескриптор устройства (DC) для клиентской области указанного окна или для всего экрана
	// Используется в последующих функциях GDI для рисования в DС
	HDC hdc = GetDC(window); 

	Input input;

	// получение статуса мира
	if(is_client) {
		clear_screen(0xff4d4d4d);
		draw_text_align("waiting for players...", dot(), 1, WHITE);

		// render screen
		{
			StretchDIBits(hdc, 0, 0, render_state.width, render_state.height,
				0, 0, render_state.width, render_state.height,
				reinterpret_cast<void*>(render_state.render_memory), &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		}

		int n = recv(socket_read, mem_pool, sizeof(mem_pool), 0);

		set_world_state(std::string(mem_pool + 1, n - 1));

		mem_pool[0] = '\0';
	}

	if (!is_client) {
		build_world();
		Players.resize(1);
		Players[0].name[0] = '\0';
	}

	point_t delta_time = 0;
	auto update_delta_time = [&delta_time]() -> void {
		static Timer timer;

		delta_time = timer.time();
		timer.reset();
	};

	update_delta_time();

	Timer global_time;

	std::string win_player;

	while (eng_state.run()) {

		update_controls(window, input);

		if (is_client) {
			if (!win_player.empty()) {
				clear_screen(0xff4d4d4d);
				draw_text_align("WINNER", dot(0, 20), 3, GREEN);
				draw_text_align(win_player.c_str(), dot(0, -10), 1.75, GREEN);

				draw_text("pressed button ESC to exit", dot(-arena_half_size.x + 10, -arena_half_size.y + 10), 0.75, WHITE);

				if (pressed(BUTTON_ESC)) {
					eng_state.flip(RUN_SETUP);
				}
			}
			else {
				send(socket_write, reinterpret_cast<const char*>(&input), sizeof(input), 0);

				simulate_input(input, [&]() -> void {
					eng_state.flip(FULLSCREEN);

					if (eng_state.fullscreen_mode()) {
						set_fullscreen_mod();
					}
					else {
						set_window_mod();
					}
					});

				camera.simulate(Players[find_player_ind(player_id)].pos, delta_time);

				int n = recv(socket_read, mem_pool, sizeof(mem_pool), 0);

				if (mem_pool[0] == 't') { // have winner
					mem_pool[n] = '\0';
					win_player = mem_pool + 1;

					continue;
				}
				set_world_state(std::string(mem_pool + 1, n - 1));

				mem_pool[0] = '\0';

				render_game(player_id);
			}
		}
		else {
			simulate_game(input, delta_time, [&]() -> void {
				eng_state.flip(FULLSCREEN);

				if (eng_state.fullscreen_mode()) {
					set_fullscreen_mod();
				}
				else {
					set_window_mod();
				}
			});
		}

		// update fps
		{
			static s32 fps = 0;
			static s32 frame_count = 0;
			static point_t frame_time_accum = 0;

			frame_count++;
			frame_time_accum += delta_time;
			if (frame_time_accum > 0.5) {
				fps = frame_count * 2;
				frame_time_accum = 0;
				frame_count = 0;
			}

			if (eng_state.show_fps()) {
				draw_object(fps, dot(5, 5) - arena_half_size, 0.5, WHITE);

				draw_object(int(delta_time * 1000), dot(20, 5) - arena_half_size, 0.5, WHITE);

				draw_object(global_time, dot(35, 5) - arena_half_size, 0.5, WHITE);
			}
		}

		// render screen
		{
			draw_text(GAME_ENGINE_TAG, dot(arena_half_size.x - 50, -arena_half_size.y + 5), 0.33, WHITE);

			StretchDIBits(hdc, 0, 0, render_state.width, render_state.height,
				0, 0, render_state.width, render_state.height,
				reinterpret_cast<void*>(render_state.render_memory), &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		}

		update_delta_time();
	}

	return 0;
}
