void clear_screen(u32 color) {
	unsigned int* pixel = (u32*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = color /* good green color 0x00ff55 /*0xff5500*/;
		}
	}
}

void draw_rect_in_pixels(int x0, int y0, int x1, int y1, u32 color) {
	
	x0 = clamp(0, x0, render_state.width);
	x1 = clamp(0, x1, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	y1 = clamp(0, y1, render_state.height);

	for (int y = y0; y < y1; y++) {
		u32* pixel = (u32*)render_state.memory + x0 + y * render_state.width;
		for (int x = x0; x < x1; x++) {
			*pixel++ = color /* good green color - 0x00ff55      /*0xff5500*/;
		}
	}
}

global_variable float render_scale = 0.01f;


void draw_arena_borders(float arena_x, float arena_y, u32 color) {
	arena_x *= render_state.height * render_scale;
	arena_y *= render_state.height * render_scale;

	int x0 = (int)((float)render_state.width * .5f - arena_x);
	int x1 = (int)((float)render_state.width * .5f + arena_x);

	int y0 = (int)((float)render_state.height * .5f - arena_y);
	int y1 = (int)((float)render_state.height * .5f + arena_y);

	draw_rect_in_pixels(0, 0, render_state.width, y0, color);
	draw_rect_in_pixels(0, y1, x1, render_state.height, color);
	draw_rect_in_pixels(0, y0, x0, y1, color);
	draw_rect_in_pixels(x1, y0, render_state.width, render_state.height, color);
}


void draw_rect(float x, float y, float half_size_x, float half_size_y, u32 color) {
	x -= 88;
	y -= 37;

	x *= render_state.height * render_scale;
	y *= render_state.height * render_scale;
	half_size_x *= render_state.height * render_scale;
	half_size_y *= render_state.height * render_scale;

	x += render_state.width / 2.f;
	y += render_state.height / 2.f;
	
	// Change to pixels
	int x0 = x - half_size_x;
	int x1 = x + half_size_x;
	int y0 = y - half_size_y;
	int y1 = y + half_size_y;

	draw_rect_in_pixels(x0, y0, x1, y1, color);
}

const char* letters[][7] = {
" 00",
"0  0",
"0  0",
"0000", // A
"0  0",
"0  0",
"0  0",

"000",
"0  0",
"0  0", // B
"000",
"0  0",
"0  0",
"000",

" 000",
"0",
"0", // C
"0",
"0",
"0",
" 000",


"000",
"0  0",
"0  0", // D
"0  0",
"0  0",
"0  0",
"000",

"0000",
"0",
"0", // E
"0000",
"0",
"0",
"0000",

"0000",
"0",
"0", // F
"0000",
"0",
"0",
"0",

" 000",
"0",
"0", // G
"0 00",
"0  0",
"0  0",
" 000",

"0  0",
"0  0",
"0  0", // H
"0000",
"0  0",
"0  0",
"0  0",

"000",
" 0",
" 0", // I
" 0",
" 0",
" 0",
"000",

"0000",
"   0",
"   0", // J
"   0",
"   0",
"   0",
"000",


"0   0",
"0  0",
"0 0", // K
"0",
"0 0",
"0  0",
"0   0",

"0",
"0",
"0", // L
"0",
"0",
"0",
"0000",


"00 00",
"0 0 0",
"0   0", // M
"0   0",
"0   0",
"0   0",
"0   0",

"0   0",
"00  0",
"0 0 0", // N
"0 0 0",
"0 0 0",
"0  00",
"0   0",


" 000 ",
"0   0",
"0   0", // O
"0   0",
"0   0",
"0   0",
" 000",


"0000",
"0  0",
"0  0", // P
"0000",
"0",
"0",
"0",

" 000",
"0   0",
"0   0", // Q
"0   0",
"0   0",
" 000 ",
"  000",


"000",
"0  0",
"0  0", // R
"000",
"0  0",
"0  0",
"0  0",


" 0000",
"0",
"0", // S
" 000",
"    0",
"    0",
"0000",


"000",
" 0",
" 0", // T
" 0",
" 0",
" 0",
" 0",

"0  0",
"0  0",
"0  0", // U
"0  0",
"0  0",
"0  0",
" 00",


"0   0",
"0   0",
"0   0", // V
"0   0",
"0   0",
" 0 0",
"  0",

"0   0",
"0   0",
"0   0", // W
"0 0 0",
"0 0 0",
"00 00",
"0   0",


"0   0",
"0   0",
" 0 0", // X
"  0",
" 0 0",
"0   0",
"0   0",




"0   0",
"0   0",
" 0 0", // Y
"  0",
"  0",
"  0",
"  0",

"0000",
"   0",
"  0 ", // Z
" 0  ",
"0   ",
"0   ",
"0000",

};

// 0 - ����� ������, 1 - ������� ������
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

// 0 - �����, 1 - �����, 2 - �������, 3 - �����, 4 - ������
const char* spaceship[27] = {
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

struct sprite {
	u32** picture;
	bool** used;
	int height, weight;
};

void draw_sprite(float x, float y, float size,const sprite &object) {
	float half_size = size * .5f;
	float original_y = y;
	float original_x = x;

	for (int i = 0; i < object.height; i++) {
		for (int j = 0; j < object.weight; j++) {
			if (object.used[i][j]) {
				draw_rect(x, y, half_size, half_size, object.picture[i][j]);
			}
			x += size;
		}
		y -= size;
		x = original_x;
	}
}

// 0 - �����, 1 - �����, 2 - �������, 3 - �����, 4 - ������
void draw_spaceship(float x, float y, float size) {
	u32 colors[] = { 0x7a7a7a, 0xffffff, 0xff0000, 0x00acff, 0xfff600 };
	size *= .5f;
	float half_size = size * .5f;
	float original_y = y;
	float original_x = x;
	for (int i = 0; i < 27; i++) {
		const char* row = spaceship[i];
		while (*row) {
			if (*row != ' ') {
				draw_rect(x, y, half_size, half_size, colors[*row - '0']);
			}
			y += size;
			row++;
		}
		x -= size;
		y = original_y;
	}
}

void draw_star(float x, float y, float size, int type) {
	u32 star_colors[] = { 0xFFFF00, 0x808000 };
	size *= 0.09f;
	float half_size = size * .5f;
	float original_y = y;
	float original_x = x;
	for (int i = 0; i < 9; i++) {
		const char* row = star[i];
		while (*row) {
			if (*row != ' ') {
				draw_rect(x, y, half_size, half_size, star_colors[*row - '0']);
			}
			x += size;
			row++;
		}
		y -= size;
		x = original_x;
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

void draw_cursor(Point Mouse_position, float size, u32 color) {
	size *= .2f;
	float x = Mouse_position.x - 1, y = Mouse_position.y + 87;
	float half_size = size * .5f;
	for (int i = 0; i < 21; i++) {
		const char* row = cursor[i];
		while (*row) {
			if (*row == '1') {
				draw_rect(x, y, half_size, half_size, color);
			}
			x += size;
			row++;
		}
		y -= size;
		x = Mouse_position.x - 1;
	}
}

static void draw_number(int number, float x, float y, float size, u32 color) {
	float half_size = size * .5f;
	bool drew_number = false;

	while (number || !drew_number) {

		drew_number = true;

		int digit = number % 10;
		number = number / 10;

		switch (digit) {
		case 0: {
			draw_rect(x - size, y, half_size, 2.5f * size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x, y + size * 2.f, half_size, half_size, color);
			draw_rect(x, y - size * 2.f, half_size, half_size, color);
			x -= size * 4.f;
		}break;

		case 1: {
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			x -= size * 2.f;
		}break;

		case 2: {
			draw_rect(x, y + size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x, y, 1.5f * size, half_size, color);
			draw_rect(x, y - size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x + size, y + size, half_size, half_size, color);
			draw_rect(x - size, y - size, half_size, half_size, color);
			x -= size * 4.f;
		}break;

		case 3: {
			draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
			draw_rect(x - half_size, y, size, half_size, color);
			draw_rect(x - half_size, y - size * 2.f, size, half_size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			x -= size * 4.f;
		}break;

		case 4: {
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x - size, y + size, half_size, 1.5f * size, color);
			draw_rect(x, y, half_size, half_size, color);
			x -= size * 4.f;
		}break;

		case 5: {
			draw_rect(x, y + size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x, y, 1.5f * size, half_size, color);
			draw_rect(x, y - size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x - size, y + size, half_size, half_size, color);
			draw_rect(x + size, y - size, half_size, half_size, color);
			x -= size * 4.f;
		}break;

		case 6: {
			draw_rect(x + half_size, y + size * 2.f, size, half_size, color);
			draw_rect(x + half_size, y, size, half_size, color);
			draw_rect(x + half_size, y - size * 2.f, size, half_size, color);
			draw_rect(x - size, y, half_size, 2.5f * size, color);
			draw_rect(x + size, y - size, half_size, half_size, color);
			x -= size * 4.f;
		}break;

		case 7: {
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
			x -= size * 4.f;
		}break;

		case 8: {
			draw_rect(x - size, y, half_size, 2.5f * size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x, y + size * 2.f, half_size, half_size, color);
			draw_rect(x, y - size * 2.f, half_size, half_size, color);
			draw_rect(x, y, half_size, half_size, color);
			x -= size * 4.f;
		}break;

		case 9: {
			draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
			draw_rect(x - half_size, y, size, half_size, color);
			draw_rect(x - half_size, y - size * 2.f, size, half_size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x - size, y + size, half_size, half_size, color);
			x -= size * 4.f;
		}
		}
	}
}

static void draw_text(const char* text, float x, float y, float size, u32 color) {
	float half_size = size * .5f;
	float original_y = y;

	while (*text) {
		if (*text != 32) {
			if (*text >= '0' && *text <= '9') {
				draw_number(*text - '0', x, y - half_size * 6, half_size * 2.7, color);
			}
			else {
				const char** a_letter = letters[*text - 'A'];
				float original_x = x;

				for (int i = 0; i < 7; i++) {
					const char* row = a_letter[i];
					while (*row) {
						if (*row == '0') {
							draw_rect(x, y, half_size, half_size, color);
						}
						x += size;
						row++;
					}
					y -= size;
					x = original_x;
				}
			}
		}
		text++;
		x += size * 6.f;
		y = original_y;
	}
}