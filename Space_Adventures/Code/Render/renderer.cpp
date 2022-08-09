namespace Render_realization {

	void draw_pixels(int_dot pos0, int_dot pos1, u32 color, u64 draw_id) {
		register unsigned short y, x;
		register u32* pixel, * draw;
		for (y = pos0.y; y < pos1.y; y++) {
			pixel = (u32*)render_state.memory + (u32)pos0.x + y * render_state.width;
			draw = (u32*)render_state.drawing_id + (u32)pos0.x + y * render_state.width;

			for (x = pos0.x; x < pos1.x; x++) {
				if (*draw < draw_id) {
					*pixel = color;
					*draw = draw_id;
				}
				pixel++;
				draw++;
			}
		}
	}

	void clear_screen(u32 color, u64 draw_id) {
		draw_pixels(int_dot(0, 0), int_dot(render_state.width, render_state.height), color, draw_id);
	}

	void draw_rect_in_pixels(int_dot pos0, int_dot pos1, u32 color, u64 draw_id) {

		pos0.x = my_clamp(0, pos0.x, render_state.width);
		pos1.x = my_clamp(0, pos1.x, render_state.width);
		pos0.y = my_clamp(0, pos0.y, render_state.height);
		pos1.y = my_clamp(0, pos1.y, render_state.height);

		draw_pixels(pos0, pos1, color, draw_id);
	}

	global_variable const float render_scale = 0.01f;

	void draw_arena_borders(float arena_x, float arena_y, u32 color, u64 draw_id) {
		arena_x *= render_state.height * render_scale;
		arena_y *= render_state.height * render_scale;

		int x0 = (int)((float)render_state.width * .5f - arena_x);
		int x1 = (int)((float)render_state.width * .5f + arena_x);

		int y0 = (int)((float)render_state.height * .5f - arena_y);
		int y1 = (int)((float)render_state.height * .5f + arena_y);

		draw_rect_in_pixels(int_dot(0, 0), int_dot(render_state.width, y0), color, draw_id);
		draw_rect_in_pixels(int_dot(0, y1), int_dot(x1, render_state.height), color, draw_id);
		draw_rect_in_pixels(int_dot(0, y0), int_dot(x0, y1), color, draw_id);
		draw_rect_in_pixels(int_dot(x1, y0), int_dot(render_state.width, render_state.height), color, draw_id);
	}

	void draw_rect(dot pos, float half_size_x, float half_size_y, u32 color, u64 draw_id) {
		pos *= render_state.height * render_scale;

		half_size_x *= render_state.height * render_scale;
		half_size_y *= render_state.height * render_scale;

		// Change to pixels
		int_dot pos0 = int_dot(pos.x - half_size_x, pos.y - half_size_y);
		int_dot pos1 = int_dot(pos.x + half_size_x, pos.y + half_size_y);

		draw_rect_in_pixels(pos0, pos1, color, draw_id);
	}

	/*
		a - top left dot
		b - down right dot
		a----+
		|	 |
		+----b
	*/
	void draw_rectangle(dot a, dot b, u32 color, u64 draw_id) {
		a *= render_state.height * render_scale;
		b *= render_state.height * render_scale;

		// Change to pixels
		int_dot pos0 = int_dot(a.x, b.y);
		int_dot pos1 = int_dot(b.x, a.y);

		draw_rect_in_pixels(pos0, pos1, color, draw_id);
	}

	void draw_sprite(dot pos, float size, sprite_type sprite, u64 draw_id) {
		size *= .5f;
		float half_size = size * .5f, original_x = pos.x;

		for (int i = Sprites[sprite].height - 1; i >= 0; i--) {
			for (int j = 0; j < Sprites[sprite].width; j++) {
				if (Sprites[sprite].used[i][j]) {
					draw_rect(pos, half_size, half_size, Sprites[sprite].picture[i][j], draw_id);
				}
				pos.x += size;
			}
			pos.y += size;
			pos.x = original_x;
		}
	}

	// 0 - серый, 1 - белый, 2 - красный, 3 - синий, 4 - желтый
	const char* spaceship[27] = { // корабль
	"       1       ",
	"      111      ",
	"     11011     ",
	"     10001     ",
	"     11111     ",
	"     10001     ",
	"     11111     ",
	"     11111     ",
	"     11111     ",
	"    0111110    ",
	"    0011100    ",
	"   110111011   ",
	"  11101110111  ",
	" 1110011100111 ",
	" 1110111110111 ",
	"113101111101311",
	"111101111101111",
	"000001111100000",
	"111011111110111",
	" 1101111111011 ",
	"    1101011    ",
	"     00100     ",
	"     24242     ",
	"     24442     ",
	"     22422     ",
	"      222      ",
	"       2       ",
	};
	void draw_spaceship(dot pos, float size, u64 draw_id) {
		u32 colors[] = { 0x7a7a7a, 0xffffff, 0xff0000, 0x00acff, 0xfff600 };
		size *= .5f;
		float half_size = size * .5f;
		float original_y = pos.y;
		float original_x = pos.x;
		for (int i = 0; i < 27; i++) {
			const char* row = spaceship[i];
			while (*row) {
				if (*row != ' ') {
					draw_rect(pos, half_size, half_size, colors[*row - '0'], draw_id);
				}
				pos.y += size;
				row++;
			}
			pos.x -= size;
			pos.y = original_y;
		}
	}

	// 0 - €ркий желтый, 1 - тусклый желтый
	const char* star[9] = {
	"    0",
	"   000",
	"   000",
	"000 0 000",
	" 00 0 00",
	"  00000",
	"  00000",
	" 00   00",
	" 0     0",
	};
	void draw_star(dot pos, float size, u64 draw_id) {
		u32 star_colors[] = { 0xFFFF00, 0x808000 };
		size *= 0.09f;
		float half_size = size * .5f;
		float original_y = pos.y;
		float original_x = pos.x;
		for (int i = 0; i < 9; i++) {
			const char* row = star[i];
			while (*row) {
				if (*row != ' ') {
					draw_rect(pos, half_size, half_size, star_colors[*row - '0'], draw_id);
				}
				pos.x += size;
				row++;
			}
			pos.y -= size;
			pos.x = original_x;
		}
	}

	const char* cursor[21] = {
	"1",
	"11",
	"111",
	"1111",
	"11111",
	"111111",
	"1111111",
	"11111111",
	"111111111",
	"1111111111",
	"11111111111",
	"111111111111",
	"11111111",
	"11111111",
	"111  1111",
	"11   1111",
	"1     1111",
	"      1111",
	"       1111",
	"       1111",
	"        11",
	};
	void draw_cursor(dot Mouse_position, float size, u32 color, u64 draw_id) {
		size *= .2f;
		dot pos = dot(Mouse_position.x - 1, Mouse_position.y);
		float half_size = size * .5f;
		for (int i = 0; i < 21; i++) {
			const char* row = cursor[i];
			while (*row) {
				if (*row == '1') {
					draw_rect(pos, half_size, half_size, color, draw_id);
				}
				pos.x += size;
				row++;
			}
			pos.y -= size;
			pos.x = Mouse_position.x - 1;
		}
	}

#include "letter.cpp" // include letters
	// draw letter
	static void draw_arr_char(char letter, dot pos, float size, u32 color, u64 draw_id) {
		float half_size = size * .5f, original_x = pos.x;

		const char** a_letter = get_arr_char(letter);

		if (letter == 'q' || letter == 'p' || letter == 'y' || letter == 'g') {
			pos.y -= 2 * size;
		}

		for (int i = 0; i < 7; i++) {
			const char* row = a_letter[i];
			while (*row) {
				if (*row == '0') {
					draw_rect(pos, half_size, half_size, color, draw_id);
				}
				pos.x += size;
				row++;
			}
			pos.y -= size;
			pos.x = original_x;
		}
	}

	// draw letter, number symbol, ' ', '-', '.'
	static void draw_symbol(char symbol, dot pos, float size, u32 color, u64 draw_id) {
		float half_size = size * .5f;

		if (symbol != ' ') {
			draw_arr_char(symbol, pos, size, color, draw_id);
		}
	}

	static void draw_text(const char* text, dot pos, float size, u32 color, bool is_align, u64 draw_id) {
		if (is_align) {
			pos.x -= text_len(text) * size / 2;
		}
		while (*text) {
			draw_symbol(*text, pos, size, color, draw_id);
			pos.x += size * symbol_len(*text);
			text++;
		}
	}

	// number length when drawing
	static float nlwd(int number, float size) {
		std::string str = num_to_str(number);
		return text_len(str.c_str()) * size;
	}

	// draw number. Example: -1409
	static void draw_number(int number, dot pos, float size, u32 color, u64 draw_id) {
		std::string str = num_to_str(number);
		for (int i = 0; i < str.size(); i++) {
			draw_symbol(str[i], pos, size, color, draw_id);
			pos.x += size * symbol_len(str[i]);
		}
	}
	
	static void draw(const char* text, dot pos, float default_size, u64 draw_id, std::list<int> Arguments) {
		float size = default_size;
		u32 color = 0xffffff;

		auto it = Arguments.begin();
		float original_x = pos.x;
		while (*text) {
			if (*text == '%') {
				text++;
				switch (*text) {
				case 'c': {
					color = *it;
					it++;
				}break;
				case 's': {
					sprite_type sp = sprite_type(*it);
					float half_size = size * 7.f / (Sprites[sp].len.y * 1.f);
					draw_sprite(dot(pos.x, pos.y - size * 7.f), half_size, sp, draw_id);
					pos.x += half_size * (Sprites[sp].len.x * 1.f) + size * 2;
					it++;
				}break;
				case 'd': {
					draw_number(*it, pos, size, color, draw_id);
					pos.x += nlwd(*it, size);
					it++;
				}break;
				case 'n': {
					pos.y -= size * 12.f;
					pos.x = original_x;
				}break;
				case 'h': {
					size = *it * .01f;
					it++;
				}break;
				case 'x': {
					original_x = pos.x;
				}break;
				}
			}
			else {
				draw_symbol(*text, pos, size, color, draw_id);
				pos.x += size * symbol_len(*text);
			}
			text++;
		}

	}
}