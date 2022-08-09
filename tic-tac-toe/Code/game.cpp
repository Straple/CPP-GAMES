#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
#define is_down(b) input->buttons[b].is_down

/*	Egor.Co LTD
	developed by Egor
	drawn by Egor
*/

#include<set>
#include<vector>
#include <ctime>

enum fieldmod {
	undefind,
	player,
	enemy
};
struct dot {
	int x, y; // x, y !!!!!!
	void operator += (dot other) {
		x += other.x; y += other.y;
	}
};

struct line {
	dot begin, vector;
	int cntX, cntO;
	bool operator < (const line& other)const {
		if (cntX == other.cntX) {
			return cntO > other.cntO;
		}
		return cntX < other.cntX;
	}
};

fieldmod field[3][3];

internal void
clear_field() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			field[i][j] = undefind;
		}
	}
}

internal bool
all_line(dot pos, dot vector) {
	fieldmod This = field[pos.y][pos.x];
	for(int i = 0; i < 3; i++, pos += vector){
		if (This != field[pos.y][pos.x]) {
			return false;
		}
	}
	return This != undefind;
}

internal fieldmod
is_winner() {// dot : x, y 
	for (int y = 0; y < 3; y++) {
		if (all_line(dot{ 0, y }, dot{ 1, 0 })) {
			return field[y][0];
		}
	}
	for (int x = 0; x < 3; x++) {
		if (all_line(dot{ x, 0 }, dot{ 0, 1 })) {
			return field[0][x];
		}
	}
	if (all_line(dot{ 0, 0 }, dot{ 1, 1 })) {
		return field[0][0];
	}
	if (all_line(dot{ 0, 2 }, dot{ 1, -1 })) {
		return field[0][2];
	}
	return undefind;
}


enum Gamemode {
	GM_MENU,
	GM_GAMEPLAY,
};


Gamemode current_gamemode;
int hot_button, now_playing;
dot choice;
dot dot_begin;

internal void
draw_grid(dot p, u32 color) {
	draw_rect(p.x + 6, p.y + 3, 12.5, 1, color);
	draw_rect(p.x + 6, p.y - 22, 12.5, 1, color);
	draw_rect(p.x - 5.5, p.y - 9, 1, 12.5, color);
	draw_rect(p.x + 17.5, p.y - 9, 1, 12.5, color);
}

internal line
get_line(dot pos, dot vector) {
	line New;
	New.begin = pos;
	New.vector = vector;
	New.cntO = New.cntX = 0;

	for (int i = 0; i < 3; i++, pos += New.vector) {
		if (field[pos.y][pos.x] == player) {
			New.cntX++;
		}
		else if (field[pos.y][pos.x] == enemy) {
			New.cntO++;
		}
	}
	return New;
}

internal void
put_X(const line &a) {
	dot pos = a.begin, vector = a.vector;
	for (int i = 0; i < 3; i++, pos += vector) {
		if (field[pos.y][pos.x] == undefind) { // я могу сюда поставить
			field[pos.y][pos.x] = enemy;
			return;
		}
	}
}


internal void update_enemy(line This, line &TwoO, line &TwoX, std::vector<line> &noX, std::vector<line> &other) {
	if (This.cntO == 2 && This.cntX != 1) {
		TwoO = This;
	}
	if (This.cntX == 2 && This.cntO != 1) {
		TwoX = This;
	}
	if (This.cntX == 0 && (noX.size() > 0 && This.cntO >= noX[0].cntO || noX.size() == 0)) {
		if (noX.size() > 0 && This.cntO > noX[0].cntO) {
			noX.clear();
		}
		noX.push_back(This);
	}
	if (This.cntO + This.cntX < 3) {
		other.push_back(This);
	}
}

internal void
simulate_game(Input* input, float dt) {
	clear_screen(0x005f4b); // заполняет экран цветом  0xff5500

	//draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x005f4b);
	draw_arena_borders(85, 45, 0x00ffaa); 


	//update important keys
	{
		if (pressed(BUTTON_SPACE)) { // возвращаюсь в начальное меню
			current_gamemode = GM_MENU;
		}
		if (pressed(BUTTON_ESC)) {
			running = false;
		}
	}

	if (current_gamemode == GM_GAMEPLAY) { // если мы играем

		if (is_winner() != undefind || now_playing == 9) { // кто-то выйграл или ходы закончились
			draw_text("PRESS SPACE TO MENU", -70, -40, 0.5, 0xaaaaaa);
			if (now_playing == 9) {
				draw_text("NOBODY WON", -60, 40, 2, 0x00dd4b);// 0xff0000
			}
			else if (is_winner() == player) {
				draw_text("YOU WIN", -45, 40, 2, 0x00dd4b);// 0xff0000
			}
			else{
				draw_text("YOU LOSE", -45, 40, 2, 0xff0000);
			}	
			return;
		}
		

		if (now_playing % 2 == 0) { // Simulate player
			if (pressed(BUTTON_LEFT) || pressed(BUTTON_A)) {
				choice.x = (choice.x + 2) % 3;
			}
			if (pressed(BUTTON_RIGHT) || pressed(BUTTON_D)) {
				choice.x = (choice.x + 1) % 3;
			}
			if (pressed(BUTTON_UP) || pressed(BUTTON_W)) {
				choice.y = (choice.y + 1) % 3;
			}
			if (pressed(BUTTON_DOWN) || pressed(BUTTON_S)) {
				choice.y = (choice.y + 2) % 3;
			}

			if (pressed(BUTTON_ENTER) && field[choice.y][choice.x] == undefind) { // player chose и там ничего не лежит
				field[choice.y][choice.x] = player;
				now_playing++; // теперь ходит наш опонент
				choice = dot{ 1, 1 };
			}
		}
		else { // Simulate enemy
			// Логика ai\
				1.если есть два O и нет X,то мы 100% должны поставить туда 0, т.к. иначе мы упустим свой шанс выйграть\
			   2.иначе Если у линии есть два Х и нет O, то мы 100% должны поставить туда 0, т.к. иначе мы проиграем\
			   3.иначе Если есть линия у которой нет X. взять ту, у которой большее количество O(закинуть в вектор и взять рандомную линиюпри ответе), то нам нужно туда поставить 0, \
			   4.иначе если у линии cntO + cntX < 3(закинуть в вектор и взять рандомную линию при ответе), поставить в любую линию для поддержания идеального баланса

			std::vector<line> other, noX;
			line TwoX, TwoO, This;
			TwoX.cntO = TwoO.cntO = -1;

			{ // filling
				for (int y = 0; y < 3; y++) {
					This = get_line(dot{ 0, y }, dot{ 1, 0 });
					update_enemy(This, TwoO, TwoX, noX, other);
				}
				for (int x = 0; x < 3; x++) {
					This = get_line(dot{ x, 0 }, dot{ 0, 1 });
					update_enemy(This, TwoO, TwoX, noX, other);
				}
				This = get_line(dot{ 0, 0 }, dot{ 1, 1 });
				update_enemy(This, TwoO, TwoX, noX, other);
				This = get_line(dot{ 0, 2 }, dot{ 1, -1 });
				update_enemy(This, TwoO, TwoX, noX, other);
			}

			
			if (TwoO.cntO != -1) {
				put_X(TwoO);
			}
			else if (TwoX.cntO != -1) {
				put_X(TwoX);
			}
			else if (noX.size() > 0) {
				put_X(noX[std::rand() % noX.size()]);
			}
			else{
				put_X(other[std::rand() % other.size()]);
			}
			now_playing++;
		}

		// draw field
		{
			dot p = dot_begin;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					
					if (field[i][j] != undefind) { // draw
						draw_text((field[i][j] == player ? "X" : "O"), p.x, p.y, 3, 0xffaa33);
					}
					draw_grid(p, 0xaaaaaa);
					p.x += 25;
				}
				p.y += 25;
				p.x = -30;
			}
			draw_grid(dot{ dot_begin.x + choice.x * 25, dot_begin.y + choice.y * 25 }, 0xff0000);
		}
	}
	else if (current_gamemode == GM_MENU) {
		if (pressed(BUTTON_ENTER)){ // игрок выбрал
			current_gamemode = GM_GAMEPLAY;
			choice = dot{ 1, 1 };
			clear_field();
			now_playing = 0;
			dot_begin = dot{ -30, -15 };
		}
		draw_text("PLAY", -22, -20, 2, 0xff0000);
		draw_text("TIC TAC TOE GAME", -73, 40, 1, 0xffffff);
		draw_text("CREATED BY EGOR", -73, 15, 0.5, 0xffffff);
	}
}