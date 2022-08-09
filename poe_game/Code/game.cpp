#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
#define is_down(b) input->buttons[b].is_down

/*	Paint over everything
	Egor.Co LTD
	developed by Egor
	drawn by Egor
*/

#include<queue>

enum Gamemode {
	GM_MENU,
	GM_GAMEPLAY,
	GM_CHOOSE,
};

enum winner {
	nobody,
	player,
	enemy
};
Gamemode current_gamemode;
int hot_button, now_playing, cnt_players;
float choicey, choicex;

const int arena_half_size_y = 10, arena_half_size_x = 20;
int field[arena_half_size_y][arena_half_size_x];

internal void
field_clear() {
	for (int i = 0; i < arena_half_size_y; i++) {
		for (int j = 0; j < arena_half_size_x; j++) {
			field[i][j] = rand() % cnt_players;
		}
	}
}

internal void
draw_field() {
	u32 vec_color[] = { 0x00ff00, 0x0000ff, 0xff0000, 0xffff00, 0xff1493 };
	for (int i = 0; i < arena_half_size_y; i++) {
		for (int j = 0; j < arena_half_size_x; j++) {
			draw_rect(-75 + j * 6, - 25 + i * 6, 2.5, 2.5, vec_color[field[i][j]]);
		}
	}
}

internal void
draw_grid(int x, int y, u32 color) {
	x = -75 + x * 6;
	y = -25 + y * 6;
	draw_rect(x, y - 2.8, 3, 0.3, color);
	draw_rect(x , y + 2.8, 3, 0.3, color);
	draw_rect(x - 2.8, y , 0.3, 3, color);
	draw_rect(x + 2.8, y , 0.3, 3, color);
}

internal winner
we_have_a_winner() {
	int color = field[0][0];
	int cnt_player = 0;
	bool only_color = true;
	for (int i = 0; i < arena_half_size_y; i++) {
		for (int j = 0; j < arena_half_size_x; j++) {
			if (color != field[i][j]) {
				only_color = false;
			}
			if (field[i][j] == 0) {
				cnt_player++;
			}
		}
	}
	if (cnt_player == arena_half_size_x * arena_half_size_y) {
		return player;
	}
	else if (cnt_player == 0) {
		return enemy;
	}
	else {
		return nobody;
	}
}

internal bool 
arena_clamp(int x, int y) {
	return x >= 0 && x < arena_half_size_x && y >= 0 && y < arena_half_size_y;
}

internal int
paint(int x, int y, int color) {
	struct pos {
		int x, y;
	};
	bool vis[arena_half_size_y][arena_half_size_x] = { false };
	int This_color = field[y][x];
	std::queue<pos> Q;
	Q.push(pos{ x, y });
	int cnt = 0;
	while (!Q.empty()) {
		x = Q.front().x; y = Q.front().y;
		Q.pop();
		if (vis[y][x]) {
			continue;
		}
		field[y][x] = color;
		vis[y][x] = true;
		if (arena_clamp(x, y - 1) && !vis[y - 1][x] && field[y - 1][x] == This_color) { // up
			Q.push(pos{ x, y - 1 });
		}
		if (arena_clamp(x, y + 1) && !vis[y + 1][x] && field[y + 1][x] == This_color) { // down
			Q.push(pos{ x, y + 1 });
		}
		if (arena_clamp(x - 1, y) && !vis[y][x - 1] && field[y][x - 1] == This_color) { // left
			Q.push(pos{ x - 1, y});
		}
		if (arena_clamp(x + 1, y) && !vis[y][x + 1] && field[y][x + 1] == This_color) { // left
			Q.push(pos{ x + 1, y });
		}
		cnt++;
	}
	return cnt;
}

// x, y
internal std::vector<std::pair<int, int>>
bfs(int x, int y, int color) {
	struct pos {
		int x, y;
	};
	bool vis[arena_half_size_y][arena_half_size_x] = { false };
	int This_color = field[y][x];
	std::queue<pos> Q;
	Q.push(pos{ x, y });
	std::vector<std::pair<int, int>> V;
	while (!Q.empty()) {
		x = Q.front().x; y = Q.front().y;
		Q.pop();
		if (vis[y][x]) {
			continue;
		}
		vis[y][x] = true;
		if (arena_clamp(x, y - 1) && !vis[y - 1][x] && field[y - 1][x] == This_color) { // up
			Q.push(pos{ x, y - 1 });
		}
		if (arena_clamp(x, y + 1) && !vis[y + 1][x] && field[y + 1][x] == This_color) { // down
			Q.push(pos{ x, y + 1 });
		}
		if (arena_clamp(x - 1, y) && !vis[y][x - 1] && field[y][x - 1] == This_color) { // left
			Q.push(pos{ x - 1, y });
		}
		if (arena_clamp(x + 1, y) && !vis[y][x + 1] && field[y][x + 1] == This_color) { // left
			Q.push(pos{ x + 1, y });
		}
		V.push_back(std::make_pair(x, y));
	}
	return V;
}

internal bool
contiguous(int x, int y, int color) { //contiguous(choicex, choicey, 0)) 
	if (arena_clamp(x, y - 1) && field[y - 1][x] == color) {
		return true;
	}
	if (arena_clamp(x, y + 1) && field[y + 1][x] == color) {
		return true;
	}
	if (arena_clamp(x - 1, y) && field[y][x - 1] == color) {
		return true;
	}
	if (arena_clamp(x + 1, y) && field[y][x + 1] == color) {
		return true;
	}
	return false;
}

internal bool
good(int x, int y) {
	int xysize = paint(x, y, field[y][x]);
	std::vector<int> Sizes;
	if (arena_clamp(x, y - 1) && field[y - 1][x] == 0) {
		Sizes.push_back(paint(x, y - 1, field[y - 1][x]));
	}
	if (arena_clamp(x, y + 1) && field[y + 1][x] == 0) {
		Sizes.push_back(paint(x, y + 1, field[y + 1][x]));
	}
	if (arena_clamp(x - 1, y) && field[y][x - 1] == 0) {
		Sizes.push_back(paint(x - 1, y, field[y][x - 1]));
	}
	if (arena_clamp(x + 1, y) && field[y][x + 1] == 0) {
		Sizes.push_back(paint(x + 1, y, field[y][x + 1]));
	}
	sort(Sizes.begin(), Sizes.end());
	return Sizes.size() > 0 && Sizes[Sizes.size() - 1] >= xysize;
}

internal bool 
is_good(int x, int y) {
	std::vector<std::pair<int, int>> V = bfs(x, y, 0x000000);
	while (!V.empty()) {
		if (good(V.back().first,V.back().second)) {
			return true;
		}
		V.pop_back();
	}
	return false;
}

struct str_star {
	float x, y, size;
};

str_star Stars[200] = {
str_star{66, 88, .2f},
str_star{-40, -15, .2f},
str_star{-69, 94, .1f},
str_star{72, 90, .1f},
str_star{4, -29, .1f},
str_star{-59, 69, .1f},
str_star{-59, 26, .3f},
str_star{57, -80, .2f},
str_star{-84, -30, .3f},
str_star{30, 31, .2f},
str_star{80, -75, .3f},
str_star{60, -3, .2f},
str_star{40, 92, .1f},
str_star{55, 0, .2f},
str_star{12, 20, .1f},
str_star{-84, 45, .3f},
str_star{15, 70, .2f},
str_star{-72, -78, .3f},
str_star{-77, -47, .1f},
str_star{56, -40, .2f},
str_star{18, -99, .3f},
str_star{71, 30, .2f},
str_star{82, -62, .3f},
str_star{-3, 67, .1f},
str_star{71, 62, .1f},
str_star{85, -32, .2f},
str_star{10, 68, .3f},
str_star{-94, -40, .2f},
str_star{70, 39, .2f},
str_star{4, 67, .2f},
str_star{56, 58, .1f},
str_star{59, -12, .3f},
str_star{-45, -5, .1f},
str_star{45, 41, .2f},
str_star{98, 66, .3f},
str_star{30, -64, .2f},
str_star{-67, -85, .3f},
str_star{83, 12, .3f},
str_star{80, 74, .1f},
str_star{-26, 15, .1f},
str_star{75, -55, .1f},
str_star{-53, -26, .1f},
str_star{-72, -82, .1f},
str_star{43, 9, .1f},
str_star{77, 22, .2f},
str_star{-3, 28, .1f},
str_star{-70, 53, .2f},
str_star{-26, 92, .3f},
str_star{-30, 82, .2f},
str_star{-66, -14, .2f},
str_star{28, 86, .2f},
str_star{-48, 79, .3f},
str_star{67, -78, .2f},
str_star{-51, -2, .2f},
str_star{-30, 56, .3f},
str_star{6, -55, .2f},
str_star{41, -24, .1f},
str_star{-8, -49, .3f},
str_star{-22, 29, .3f},
str_star{-12, -100, .1f},
str_star{-100, 38, .3f},
str_star{-37, 14, .3f},
str_star{-79, 72, .2f},
str_star{-67, 12, .2f},
str_star{91, 71, .2f},
str_star{-29, 23, .3f},
str_star{58, -84, .3f},
str_star{76, 55, .3f},
str_star{-21, -58, .3f},
str_star{-19, 41, .1f},
str_star{-56, -35, .3f},
str_star{90, 8, .3f},
str_star{-54, 86, .1f},
str_star{-100, -79, .1f},
str_star{-46, 43, .3f},
str_star{30, -20, .2f},
str_star{-50, 71, .1f},
str_star{79, -93, .3f},
str_star{71, -43, .2f},
str_star{-1, -91, .3f},
str_star{-1, -33, .3f},
str_star{-69, 66, .3f},
str_star{-77, -99, .1f},
str_star{53, -78, .1f},
str_star{90, -86, .1f},
str_star{55, -97, .1f},
str_star{76, 35, .1f},
str_star{2, 74, .2f},
str_star{-27, -64, .2f},
str_star{9, -59, .3f},
str_star{99, -97, .2f},
str_star{6, -87, .2f},
str_star{49, 28, .3f},
str_star{-45, -4, .2f},
str_star{60, 36, .1f},
str_star{70, -20, .2f},
str_star{94, -45, .2f},
str_star{94, 43, .2f},
str_star{-9, -95, .2f},
str_star{15, 90, .3f},
str_star{ -89, 28, .1f },
str_star{ 57, -16, .1f },
str_star{ 43, 35, .3f },
str_star{ 27, -31, .3f },
str_star{ -47, -14, .3f },
str_star{ 49, 7, .3f },
str_star{ 23, -40, .1f },
str_star{ 41, 77, .3f },
str_star{ -36, 82, .2f },
str_star{ -46, 92, .3f },
str_star{ -74, 50, .1f },
str_star{ -71, 94, .1f },
str_star{ -73, -56, .2f },
str_star{ 27, -1, .1f },
str_star{ 83, 49, .1f },
str_star{ 91, -84, .3f },
str_star{ 47, 6, .3f },
str_star{ 18, 0, .2f },
str_star{ -100, 82, .1f },
str_star{ 42, -94, .3f },
str_star{ -20, -84, .1f },
str_star{ -75, 21, .3f },
str_star{ 88, -49, .2f },
str_star{ -24, -60, .3f },
str_star{ -91, -89, .3f },
str_star{ 99, 38, .3f },
str_star{ 66, -92, .2f },
str_star{ 97, 99, .2f },
str_star{ 76, -2, .2f },
str_star{ 41, -33, .1f },
str_star{ 49, 65, .3f },
str_star{ -1, -87, .3f },
str_star{ 95, -21, .2f },
str_star{ 99, 14, .2f },
str_star{ 17, -91, .3f },
str_star{ 16, 65, .3f },
str_star{ 1, -87, .2f },
str_star{ -68, -38, .1f },
str_star{ 1, -43, .2f },
str_star{ 71, 94, .1f },
str_star{ 79, -1, .2f },
str_star{ 17, 67, .1f },
str_star{ -48, -76, .2f },
str_star{ -18, -77, .1f },
str_star{ -22, -54, .2f },
str_star{ -19, 8, .2f },
str_star{ 6, -6, .2f },
str_star{ -1, -54, .1f },
str_star{ 17, -67, .1f },
str_star{ 20, 30, .1f },
str_star{ -36, -25, .3f },
str_star{ -89, -5, .2f },
str_star{ 23, 61, .3f },
str_star{ -67, -28, .2f },
str_star{ 12, 68, .2f },
str_star{ -42, 84, .2f },
str_star{ 30, 97, .3f },
str_star{ -48, -72, .1f },
str_star{ -33, 82, .3f },
str_star{ 18, -91, .2f },
str_star{ -100, 0, .3f },
str_star{ 26, 6, .2f },
str_star{ -3, 9, .2f },
str_star{ 55, 77, .1f },
str_star{ 51, 19, .3f },
str_star{ -53, -59, .3f },
str_star{ -43, 2, .2f },
str_star{ -33, 22, .2f },
str_star{ -6, -97, .3f },
str_star{ 79, -71, .3f },
str_star{ 5, -95, .2f },
str_star{ -8, 36, .3f },
str_star{ 77, -76, .1f },
str_star{ 93, -85, .2f },
str_star{ -22, 96, .3f },
str_star{ 78, -88, .1f },
str_star{ -65, 7, .3f },
str_star{ 44, 76, .2f },
str_star{ 2, -57, .2f },
str_star{ -65, -44, .1f },
str_star{ -72, -79, .2f },
str_star{ -13, 89, .2f },
str_star{ 55, 5, .1f },
str_star{ 82, 57, .1f },
str_star{ 46, -62, .1f },
str_star{ 67, 84, .1f },
str_star{ 25, 42, .3f },
str_star{ 61, 82, .2f },
str_star{ 23, 87, .3f },
str_star{ 7, 43, .1f },
str_star{ 32, -6, .3f },
str_star{ 2, -43, .1f },
str_star{ 90, 41, .3f },
str_star{ 58, -74, .3f },
str_star{ -25, -48, .1f },
str_star{ -30, 48, .2f },
str_star{ 59, -86, .2f },
str_star{ -20, 39, .3f },
str_star{ -3, -94, .3f },
str_star{ 6, -98, .3f },
};

internal void
draw_stars() {
	for (int i = 0; i < 200; i++) {
		draw_star(Stars[i].x, Stars[i].y, Stars[i].size, 0xffff00);
	}
}

internal void
clear_place_for_field() {
	for (int i = 0; i < arena_half_size_y; i++) {
		for (int j = 0; j < arena_half_size_x; j++) {
			draw_rect(-75 + j * 6, -25 + i * 6, 3, 3, 0x000000);
		}
	}
}

internal void
simulate_game(Input* input, float dt) {
	clear_screen(0x000000); // заполн€ет экран цветом  0xff5500
	draw_stars();
	//draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x005f4b);
	//draw_arena_borders(85, 45, 0x00ffaa); 
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

		if (we_have_a_winner() != nobody) { // кто-то выйграл
			draw_text("PRESS SPACE TO MENU", -70, -40, 0.5, 0xaaaaaa);
			if (we_have_a_winner() == player) {
				draw_text("YOU WIN", -45, 40, 2, 0x00dd4b);// 0xff0000
			}
			else{
				draw_text("YOU LOSE", -45, 40, 2, 0xff0000);
			}	
			return;
		}

		if (now_playing % cnt_players == 0) { // Simulate player
			draw_text("PRESS TAB TO CONTINUE", -70, -40, 0.5, 0xaaaaaa);

			choicex = (input->Mouse.x + 77) / 6;
			choicey = (input->Mouse.y + 27) / 6;

			choicex = clamp(0, choicex, arena_half_size_x - 1);
			choicey = clamp(0, choicey, arena_half_size_y - 1);

			if (pressed(BUTTON_TAB)) {// игрок пропускает ход
				now_playing++;
			}

			if (pressed(BUTTON_MOUSE_L) && field[int(choicey)][int(choicex)] != 0 && is_good(choicex, choicey)){ // player chose и там не лежит наш цвет и \
				если мы нападаем на цвет который смежен с нашим
				paint(choicex, choicey, 0);
				now_playing++; // теперь ходит наш опонент
			}
		}
		else { // Simulate enemy
			struct enemy {
				int x, y, size;
			};
			enemy A = enemy{-1, -1, -1};
			for (int i = 0; i < arena_half_size_y; i++) {
				for (int j = 0; j < arena_half_size_x; j++) {
					if (field[i][j] == now_playing % cnt_players) { // эта €чейка наша
						int xysize = paint(j, i, field[i][j]);
						if (arena_clamp(j, i - 1) && field[i - 1][j] != field[i][j]) {
							int val = paint(j, i - 1, field[i - 1][j]);
							if (xysize > val && A.size < val) {
								A = enemy{ j, i - 1,  val};
							}
						}
						if (arena_clamp(j, i + 1) && field[i + 1][j] != field[i][j]) {
							int val = paint(j, i + 1, field[i + 1][j]);
							if (xysize > val && A.size < val) {
								A = enemy{ j, i + 1,  val };
							}
						}
						if (arena_clamp(j - 1, i) && field[i][j - 1] != field[i][j]) {
							int val = paint(j - 1, i, field[i][j - 1]);
							if (xysize > val &&  A.size < val) {
								A = enemy{ j - 1, i,  val };
							}
						}
						if (arena_clamp(j + 1, i) && field[i][j + 1] != field[i][j]) {
							int val = paint(j + 1, i, field[i][j + 1]);
							if (xysize > val && A.size < val) {
								A = enemy{ j + 1, i,  val };
							}
						}
					}
				}
			}
			if (A.size != -1) {
				paint(A.x, A.y, now_playing % cnt_players);
			}
			now_playing++;
		}

		clear_place_for_field();
		draw_field();
		draw_grid(choicex, choicey, 0xffffff);
	}
	else if (current_gamemode == GM_MENU) {
		if (pressed(BUTTON_ENTER)){ // игрок выбрал
			current_gamemode = GM_CHOOSE;
			cnt_players = 2;
		}
		draw_text("PLAY", -22, -20, 2, 0xff0000);
		draw_text("PAINT OVER EVERYTHING", -73, 40, 1, 0x5833FF);
		draw_text("CREATED BY EGOR", -73, 10, 0.5, 0x5833FF);//0xffffff
	}
	else if (current_gamemode == GM_CHOOSE) {
		if (pressed(BUTTON_ENTER)) { // игрок выбрал
			current_gamemode = GM_GAMEPLAY;
			field_clear();
			now_playing = 0;
		}

		if (pressed(BUTTON_UP) || pressed(BUTTON_W)) {
			cnt_players++;
			if (cnt_players > 5) {
				cnt_players--;
			}
		}
		if (pressed(BUTTON_DOWN) || pressed(BUTTON_S)) {
			cnt_players--;
			if (cnt_players < 2) {
				cnt_players++;
			}
		}
		draw_number(cnt_players, -3, -5, 4, 0xff0000);
		draw_text("SELECT THE NUMBER OF PLAYERS", -73, 40, 0.7, 0x5833FF);
		draw_text("USE THESE BUTTONS TO SELECT  W S UP DOWN", -70, -40, 0.5, 0xaaaaaa);
	}

	draw_cursor(input->Mouse, 1, 0xffffff);
}