/* 
*  ======================Developed by Green Game Developer======================
*  Space Adventures C++ game using libraries windows.h, audiere.h 
*  internal namespace Simple_Object, Utils, Sprite, Audio, Render, Object, Game_Object, Level, Render_Realization
*/

// C++
#include <fstream> 
#include <vector>
#include <list>
#include <queue>

#include <thread> // threads
#include <mutex>

#include<windows.h> // window
//#pragma comment(lib, "audiere.lib")
#include<audiere.h> // audio
using namespace audiere;
using namespace std;

#include "utils.h" // утилиты
#include "object\\simple_object.cpp" // простые обьекты

global_variable bool running = true, debug = true;
global_variable Render_State render_state;
global_variable thread *Threads;
global_variable bool* Thread_busy;
global_variable size_t current_thread = 1;

global_variable float time_fps_update_cooldown = .5f, time_between_fps = 0;
global_variable int fps = 0, global_fps = 0;

global_variable float arena_half_size_y, arena_half_size_x, arena_center_x;

#include "Object\\platform_common.cpp"
#include "sprite.cpp"
#include "Audio\\audio.cpp"

#include "Render\\renderer.cpp"

#include "thread.cpp" // thread machine

#include "Object\\object.cpp"
#include "Object\\game_object.cpp"

global_variable Mouse mouse;

#include "Game\\game.cpp"

void update_controls(HWND &window, Input &input) {
	for (int i = 0; i < BUTTON_COUNT; i++) {
		input.buttons[i].changed = false;
	}

	// Input
	MSG message;
	while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {

		// mouse update
		{ 
			float multi = .095f;
			mouse.pos = dot(my_clamp(0.f, message.pt.x * multi, arena_half_size_x),
				my_clamp(0.f, -message.pt.y * multi + arena_half_size_y, arena_half_size_y));
			//mouse.pos.y += arena_half_size_y;
		}

// Макросс
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
			// Вывод: после нажатия кнопки у клавиатура она "чистилась" в WM_KEYUP, а в мыши нет
			clear_button(BUTTON_MOUSE_L);
		}
		case WM_LBUTTONDOWN: {
			u32 vk_code = (u32)message.wParam;//button
			bool is_down = ((message.lParam & (1 << 31)) == 0);// нажата ли она

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
			bool is_down = ((message.lParam & (1 << 31)) == 0);// нажата ли она

			switch (vk_code) {
				process_button(BUTTON_MOUSE_R, MK_RBUTTON);
			}
		}break;

		case WM_KEYUP:
		case WM_KEYDOWN: {
			u32 vk_code = (u32)message.wParam;//button
			bool is_down = ((message.lParam & (1 << 31)) == 0);// нажата ли она

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
				process_button(BUTTON_J, 'J');
				process_button(BUTTON_K, 'K');
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
}

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	switch (uMsg) {
	case WM_CLOSE:
	case WM_DESTROY: {
		running = false; // закрывает цикл, если окно закрыто или разрушено
	} break;

	case WM_SIZE: {
		RECT rect;
		GetClientRect(hwnd, &rect);
		render_state.width = rect.right - rect.left;
		render_state.height = rect.bottom - rect.top;

		int size = render_state.width * render_state.height * sizeof(unsigned int);

		if (render_state.memory) {
			VirtualFree(render_state.memory, 0, MEM_RELEASE);
			VirtualFree(render_state.drawing_id, 0, MEM_RELEASE);
		}
		render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		render_state.drawing_id = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
		render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
		render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
		render_state.bitmap_info.bmiHeader.biPlanes = 1;
		render_state.bitmap_info.bmiHeader.biBitCount = 32;
		render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

		// update arena_half_size
		{
			arena_half_size_x = render_state.width / (render_state.height * Render_realization::render_scale);
			arena_half_size_y = 1.f / Render_realization::render_scale;
			arena_center_x = arena_half_size_x * .5f;
		}
	} break;

	default: {
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
	return result;
}

int main() {
	ShowWindow(GetConsoleWindow(), HIDE_WINDOW); //Спрятать окно

	// Create Window class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = L"Game Window Class";
	window_class.lpfnWndProc = window_callback;
	window_class.hIcon = (HICON)LoadImage(NULL, TEXT("icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	HINSTANCE hInstance = GetModuleHandle(NULL);

	// Register class
	RegisterClass(&window_class);
	ShowCursor(FALSE);

	// Create window
	HWND window = CreateWindow(window_class.lpszClassName, L"Space_Adventures!", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1280, 0, 0, hInstance, 0);
	//Fullscreen!!!!!!
	{
		SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}

	HDC hdc = GetDC(window);

	Input input = {};

	float delta_time = 0.016666f; // скорость
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time);


	float performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (float)perf.QuadPart;
	}
	// Скорость нужна для того чтобы у нас была одинаковая она для любого масштаба

	// reading options
	{
		std::string str;
		ifstream input("options.txt");
		int number_of_threads;
		input >> str >> str >> str >> number_of_threads;
		number_of_threads = my_clamp(1, number_of_threads, 16);
		current_thread = number_of_threads - 1;
		input >> str >> str >> str;
		debug = str == "ON";

		input >> str >> str >> global_volume;
		input >> str >> str >> sound_volume;
		input >> str >> str >> music_volume;
		input >> str >> str >> UI_volume;

		input.close();
	}

	// threads
	{
		Threads = new thread[current_thread];
		Thread_busy = new bool[current_thread];
		for (int i = 0; i < current_thread; i++) {
			Thread_busy[i] = false;
			Threads[i] = thread(thr::simulate_thread, i);
		}
	}

	// show loading screen
	{
		draw_loading_screen(); // показывает экран загрузки
		thr::simulate_render_thread();
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}

	AudioDevicePtr device;
	// audio initialization
	{
		device = OpenDevice();
		init_audio(device);
	}

	initialization(mouse);

	play_music(MT_menu); // play music 

	while (running) { /// бесконечный цикл
		
		// update controls
		update_controls(window, input);

		// Simulate
		simulate_game(&input, delta_time);

		thr::simulate_render_thread();

		// Render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		
		// time update
		{
			LARGE_INTEGER frame_end_time;
			QueryPerformanceCounter(&frame_end_time);
			delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
			frame_begin_time = frame_end_time;
		}
	}

	// options write
	{
		ofstream output("options.txt");
		output << "number of threads " << current_thread + 1 << "\n";
		output << "debug mode " << (debug ? "ON" : "OFF") << "\n";
		output << "global volume " << global_volume << "\n";
		output << "sound volume " << sound_volume << "\n";
		output << "music volume " << music_volume << "\n";
		output << "UI volume " << UI_volume;
		output.close();
	}
	return 0;
}