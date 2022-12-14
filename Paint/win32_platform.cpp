#include <windows.h>
#include "utils.cpp"

global_variable bool running = true, first_screen = true;

struct Render_State {
	int height, width;
	void* memory;

	BITMAPINFO bitmap_info;
};

global_variable Render_State render_state;

#include "platform_common.cpp"
#include "renderer.cpp"
#include "game.cpp"
#include "edeque.cpp"

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	switch (uMsg) {  		
		case WM_CLOSE:
		case WM_DESTROY: {
			running = false; /// ????????? ????, ???? ???? ??????? ??? ?????????
		} break;

		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;

			int size = render_state.width * render_state.height * sizeof(unsigned int);

			if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
			render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
			render_state.bitmap_info.bmiHeader.biPlanes = 1;
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
		} break;

		default: {
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	return result;
}


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Create Window class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = "Game Window Class";
	window_class.lpfnWndProc = window_callback;

	// Register class
	RegisterClass(&window_class);
	ShowCursor(FALSE);

	// Create window  // ??????? ????, ?? ???? ?? ?? ???????? ??????????? ????, ?? ??? ??? ?? ?????????
	HWND window = CreateWindow(window_class.lpszClassName, "Paint!", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1920,1280, 0, 0, hInstance, 0);
	{
		//Fullscreen!!!!!!

		SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi );
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
	
	HDC hdc = GetDC(window);

	Input input = {};

	float delta_time = 0.016666f; // ????????
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time);


	float performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (float)perf.QuadPart;
	}
	// ???????? ????? ??? ???? ????? ? ??? ???? ?????????? ??? ??? ?????? ????????

	while (running) { /// ??????????? ????
		// Input
		MSG message;

		for (int i = 0; i < BUTTON_COUNT; i++) {
			input.buttons[i].changed = false;
		}

		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			
			{ // mouse update
				float multi = .095f;
				input.Mouse = Point{ message.pt.x * multi, -message.pt.y * multi};
			}

// ???????
#define process_button(b, vk)\
case vk: {\
	input.buttons[b].changed = is_down != input.buttons[b].is_down;\
	input.buttons[b].is_down = is_down; \
} break;
//

//
#define clear_button(b)\
	input.buttons[b].is_down = false;\
//
			switch (message.message) {
				case WM_LBUTTONUP: {
					// ?????: ????? ??????? ?????? ? ?????????? ??? "?????????" ? WM_KEYUP, ? ? ???? ???
					clear_button(BUTTON_MOUSE_L);
				}
				case WM_LBUTTONDOWN: {
					u32 vk_code = (u32)message.wParam;//button
					bool is_down = ((message.lParam & (1 << 31)) == 0);// ?????? ?? ???

					switch (vk_code) {
						process_button(BUTTON_MOUSE_L, MK_LBUTTON);
						process_button(BUTTON_MOUSE_R, MK_RBUTTON);
					}
				}break;
				case WM_RBUTTONUP: {
					clear_button(BUTTON_MOUSE_R);
				}
				case WM_RBUTTONDOWN: {
					u32 vk_code = (u32)message.wParam;//button
					bool is_down = ((message.lParam & (1 << 31)) == 0);// ?????? ?? ???

					switch (vk_code) {
						process_button(BUTTON_MOUSE_R, MK_RBUTTON);
					}
				}break;
				case WM_KEYUP:
				case WM_KEYDOWN: {
					u32 vk_code = (u32)message.wParam;//button
					bool is_down = ((message.lParam & (1 << 31)) == 0);// ?????? ?? ???

					switch (vk_code) {
						process_button(BUTTON_UP, VK_UP);
						process_button(BUTTON_DOWN, VK_DOWN);
						process_button(BUTTON_W, 'W');
						process_button(BUTTON_S, 'S');
						process_button(BUTTON_D, 'D');
						process_button(BUTTON_A, 'A');
						process_button(BUTTON_B, 'B');
						process_button(BUTTON_C, 'C');
						process_button(BUTTON_E, 'E');
						process_button(BUTTON_F, 'F');
						process_button(BUTTON_DEL, VK_DELETE);
						process_button(BUTTON_LEFT, VK_LEFT);
						process_button(BUTTON_RIGHT, VK_RIGHT);
						process_button(BUTTON_ENTER, VK_RETURN);
						process_button(BUTTON_ESC, VK_ESCAPE);
						process_button(BUTTON_TAB, VK_TAB);
						process_button(BUTTON_SPACE, VK_SPACE);
						process_button(BUTTON_0, VK_NUMPAD0);
						process_button(BUTTON_1, VK_NUMPAD1);
						process_button(BUTTON_2, VK_NUMPAD2);
						process_button(BUTTON_3, VK_NUMPAD3);
						process_button(BUTTON_4, VK_NUMPAD4);
						process_button(BUTTON_5, VK_NUMPAD5);
						process_button(BUTTON_6, VK_NUMPAD6); 
						process_button(BUTTON_7, VK_NUMPAD7);
						process_button(BUTTON_8, VK_NUMPAD8);
						process_button(BUTTON_9, VK_NUMPAD9);
					}
				}break;

				default: {
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}

		}

		// Simulate  // 0x00ff55 - ??????? ????
		simulate_game(&input, delta_time);
		

		// Render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);


		LARGE_INTEGER frame_end_time; // ?????????? ????????
		QueryPerformanceCounter(&frame_end_time);
		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
		frame_begin_time = frame_end_time;
		//
	}
}