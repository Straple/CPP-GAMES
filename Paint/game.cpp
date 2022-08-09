#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
#define is_down(b) input->buttons[b].is_down


struct LAH{ //length and height
	int length, height;
};

struct field {
	LAH size;// 28, 50
	u32 **A;
	bool **used;
	void new_field(int height, int length) {
		size.length = length;
		size.height = height;
		A = new u32 * [size.height];
		used = new bool*[size.height];
		for (int i = 0; i < size.height; i++) {
			A[i] = new u32[size.length];
			used[i] = new bool[size.length];
			for (int j = 0; j < size.length; j++) {
				used[i][j] = false;
			}
		}
	}
};

field picture;
sprite check;

int choicex, choicey;

void draw_picture(int x, int y) {
	for (int i = 0; i < picture.size.height; i++) {
		for (int j = 0; j < picture.size.length; j++) {
			if (picture.used[i][j]) {
				draw_rect(x + j * 1.5, y + i * 1.5, .7f, .7f, picture.A[i][j]);
			}
			else {
				draw_rect(x + j * 1.5, y + i * 1.5, .7f, .7f, 0x555555);
			}
		}
	}
}

void draw_choice(int chx, int chy) {
	//draw_rect(x + j * 1.5, y + i * 1.5, .7f, .7f, picture.A[i][j]);
	int x = 15 + chx * 1.5;
	int y = -10 + chy * 1.5;
	draw_rect(x, y - .7f, 1, 0.15, 0x00ff00);
	draw_rect(x, y + .7f, 1, 0.15, 0x00ff00);
	draw_rect(x - .7f, y, 0.15, 1, 0x00ff00);
	draw_rect(x + .7f, y, 0.15, 1, 0x00ff00);
}

char Hex[6];
int hex_ind;

#include<iostream>
#include<map>
#include<stdio.h>
//#include"output.cpp"

void simulate_game(Input* input, float dt) {
	clear_screen(0x000000);
	//draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x005f4b);
	//draw_arena_borders(85, 45, 0x00ffaa); 

	if (first_screen) {
		first_screen = false;
		picture.new_field(70, 100);
		hex_ind = 0;
		for (int i = 0; i < 6; i++) {
			Hex[i] = ' ';
		}
		//check = init_check();
	}
	//update important keys
	{
		if (pressed(BUTTON_ESC)) {
			running = false;
		}
		if (pressed(BUTTON_S)) { // save picture
			//1. найти самую левую точку\
			  2. найти самую верхнюю точку
			int leftmost, topmost;
			topmost = leftmost = 1e8; // INF
			// init leftmost and rightmost
			for (int i = 0; i < picture.size.height; i++) {
				for (int j = 0; j < picture.size.length; j++) {
					if (picture.used[i][j]) {
						if (j < leftmost) {
							leftmost = j;
						}
						if (i < topmost) {
							topmost = i;
						}
					}
				}
			}

#pragma warning(suppress : 4996) // warning на fopen
			FILE* output = fopen("output.cpp", "w");

			int height = picture.size.height - topmost,
				weight = picture.size.length - leftmost;

			fprintf(output, "sprite Sprite;\n");
			fprintf(output, "Sprite.height = %d;\n", height);
			fprintf(output, "Sprite.weight = %d;\n", weight);
			fprintf(output, "Sprite.picture = new u32*[%d];\n", height);
			fprintf(output, "Sprite.used = new bool*[%d];\n", height);

			for (int i = topmost; i < picture.size.height; i++) {

				fprintf(output, "Sprite.picture[%d] = new u32[%d];\n", i - topmost, weight);
				fprintf(output, "Sprite.used[%d] = new bool[%d];\n", i - topmost, weight);

				for (int j = leftmost; j < picture.size.length; j++) {
					if (picture.used[i][j]) {

						fprintf(output, "Sprite.picture[%d][%d] = %d;\n", i - topmost, j - leftmost, picture.A[i][j]);
						fprintf(output, "Sprite.used[%d][%d] = true;\n", i - topmost, j - leftmost);

					}
					else {
						fprintf(output, "Sprite.used[%d][%d] = false;\n", i - topmost, j - leftmost);
					}
				}
			}

			fclose(output); // обязательно
		}
	}

	if (pressed(BUTTON_DEL)) {
		hex_ind = 0;
		for (int i = 0; i < 6; i++) {
			Hex[i] = ' ';
		}
	}

	// update hex
	{ 
		if (pressed(BUTTON_0)) {
			Hex[hex_ind] = '0';
			hex_ind++;
		}
		else if (pressed(BUTTON_1)) {
			Hex[hex_ind] = '1';
			hex_ind++;
		}
		else if (pressed(BUTTON_2)) {
			Hex[hex_ind] = '2';
			hex_ind++;
		}
		else if (pressed(BUTTON_3)) {
			Hex[hex_ind] = '3';
			hex_ind++;
		}
		else if (pressed(BUTTON_4)) {
			Hex[hex_ind] = '4';
			hex_ind++;
		}
		else if (pressed(BUTTON_5)) {
			Hex[hex_ind] = '5';
			hex_ind++;
		}
		else if (pressed(BUTTON_6)) {
			Hex[hex_ind] = '6';
			hex_ind++;
		}
		else if (pressed(BUTTON_7)) {
			Hex[hex_ind] = '7';
			hex_ind++;
		}
		else if (pressed(BUTTON_8)) {
			Hex[hex_ind] = '8';
			hex_ind++;
		}
		else if (pressed(BUTTON_9)) {
			Hex[hex_ind] = '9';
			hex_ind++;
		}
		else if (pressed(BUTTON_A)) {
			Hex[hex_ind] = 'A';
			hex_ind++;
		}
		else if (pressed(BUTTON_B)) {
			Hex[hex_ind] = 'B';
			hex_ind++;
		}
		else if (pressed(BUTTON_C)) {
			Hex[hex_ind] = 'C';
			hex_ind++;
		}
		else if (pressed(BUTTON_D)) {
			Hex[hex_ind] = 'D';
			hex_ind++;
		}
		else if (pressed(BUTTON_E)) {
			Hex[hex_ind] = 'E';
			hex_ind++;
		}
		else if (pressed(BUTTON_F)) {
			Hex[hex_ind] = 'F';
			hex_ind++;
		}
	}

	{// update choice
		choicex = (input->Mouse.x) / 1.5;
		choicey = (input->Mouse.y - 10 + 87 + 9 + 1.5) / 1.5;
		choicex = clamp(0, choicex - 10, picture.size.length - 1);
		choicey = clamp(0, choicey + 7, picture.size.height - 1);
	}

	if (is_down(BUTTON_MOUSE_L)) {
		picture.A[choicey][choicex] = strtol(Hex, NULL, 16);
		picture.used[choicey][choicex] = true;
	}
	if (is_down(BUTTON_MOUSE_R)) { // clear pixel
		picture.used[choicey][choicex] = false;
	}
	
	//draw_sprite(10, 50, 1, check);
	

	// render
	draw_picture(15, -10);
	draw_choice(choicex, choicey);
	//draw_cursor(input->Mouse, 1, strtol(Hex, NULL, 16));
	draw_text(Hex, 80, 85, 1, 0xff0000);
}