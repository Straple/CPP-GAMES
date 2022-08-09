#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

float player_1_p, player_1_dp, player_2_p, player_2_dp;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 12;
float ball_p_x, ball_p_y, ball_dp_x = 130, ball_dp_y, ball_half_size = 1;


// dp - ускорение

u32 player_1_score, player_2_score;


internal void
simulate_player(float *p, float *dp, float ddp, float dt) {
	ddp -= *dp * 10.f;
	
	*p = *p + *dp * dt + ddp * dt * dt * .5f;
	*dp = *dp + ddp * dt;

	if (*p + player_half_size_y > arena_half_size_y) {
		*p = arena_half_size_y - player_half_size_y;
		*dp *= 0; // если будет отрицательное то будет происходить отскок 
	}
	else if (*p - player_half_size_y < -arena_half_size_y) {
		*p = -arena_half_size_y + player_half_size_y;
		*dp *= 0;
	}
}


internal bool
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {

	return (p1x + hs1x > p2x - hs2x &&
		p1x - hs1x < p2x + hs2x &&
		p1y + hs1y > p2y - hs2y &&
		p1y - hs1y < p2y + hs2y);
}


enum Gamemode {
	GM_MENU,
	GM_GAMEMENU,
	GM_SETTINGSMENU,
	GM_GAMEPLAY,
	GM_GAMEPLAY_PAUSE,
};

enum fpsmode {
	FPS_OFF,
	FPS_ON,
};

fpsmode current_fps;
Gamemode current_gamemode;
int hot_button;
bool enemy_is_ai;

internal void
simulate_game(Input* input, float dt, float time) {
	//clear_screen(0xff5500); // заполняет экран цветом  0xff5500
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x005f4b);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x009d4b);

	// fps
	if (current_fps == FPS_ON)
	{
		draw_number(global_fps, -75, -48, .7f, 0xaaaaaa);
	}
	// fps
	
	if (pressed(BUTTON_SPACE)) // возвращаюсь в начальное меню
	{
		current_gamemode = GM_MENU;
		player_1_score = player_2_score = 0;
		player_1_dp = player_2_dp = 0;
		player_1_p = player_2_p = 0;

		ball_p_x = ball_p_y = 0;
		ball_dp_x = 130;
		ball_dp_y = 0;
	}
	else if(pressed(BUTTON_ESC))
	{
		running = false;
	}

	if (current_gamemode == GM_GAMEPLAY)
	{

		if (player_1_score == 20)
		{
			if (!enemy_is_ai)
			{// multiplayer
				draw_text("FIRST PLAYER WIN", -50, 40, 1, 0x00dd4b);// 0xffffff
			}
			else
			{
				draw_text("YOU WIN", -45, 40, 2, 0x00dd4b);// 0xff0000
			}

			draw_text("PRESS SPACE TO MENU", -70, -10, 0.5, 0xaaaaaa);
			return;
		}
		else if (player_2_score == 20)
		{
			if (!enemy_is_ai)
			{// multiplayer
				draw_text("SECOND PLAYER WIN", -50, 40, 1, 0x00dd4b);
			}
			else
			{
				draw_text("YOU LOSE", -45, 40, 2, 0xff0000);
			}

			draw_text("PRESS SPACE TO MENU", -70, -10, 0.5, 0xaaaaaa);
			return;
		}

		if (pressed(BUTTON_TAB))
		{
			current_gamemode = current_gamemode == GM_GAMEPLAY ? GM_GAMEPLAY_PAUSE : GM_GAMEPLAY;
		}
		
		float player_1_ddp = 0.f;

		if (!enemy_is_ai){
			if (is_down(BUTTON_UP)) player_1_ddp += 2000;// speed*dt
			if (is_down(BUTTON_DOWN)) player_1_ddp -= 2000;
		}
		else{
			player_1_ddp = (ball_p_y - player_1_p) * 100;
			if (player_1_ddp > 1300) player_1_ddp = 1300;
			if (player_1_ddp < -1300)player_1_ddp = -1300;
		}


		float player_2_ddp = 0.f;
		if (is_down(BUTTON_W)) player_2_ddp += 2000;// speed*dt
		if (is_down(BUTTON_S)) player_2_ddp -= 2000;

		// Simulate Player
		simulate_player(&player_1_p, &player_1_dp, player_1_ddp, dt);
		simulate_player(&player_2_p, &player_2_dp, player_2_ddp, dt);
		//


		// Simulate Ball
		{
			ball_p_x += ball_dp_x * dt;
			ball_p_y += ball_dp_y * dt;


			if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 80, player_1_p, player_half_size_x, player_half_size_y)) {
				ball_p_x = 80 - player_half_size_x - ball_half_size;
				ball_dp_x *= -1;

				ball_dp_y = (ball_p_y - player_1_p) * 2 + player_1_dp * .75f;
			}
			else if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, -80, player_2_p, player_half_size_x, player_half_size_y)) {
				ball_p_x = -80 + player_half_size_x + ball_half_size;
				ball_dp_x *= -1;

				ball_dp_y = (ball_p_y - player_2_p) * 2 + player_2_dp * .75f;
			}


			if (ball_p_y + ball_half_size > arena_half_size_y) {
				ball_p_y = arena_half_size_y - ball_half_size;
				ball_dp_y *= -1;
			}
			else if (ball_p_y - ball_half_size < -arena_half_size_y) {
				ball_p_y = -arena_half_size_y + ball_half_size;
				ball_dp_y *= -1;
			}

			if (ball_p_x + ball_half_size > arena_half_size_x) {
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;
				player_1_score++; // увеличение счета
			}
			else if (ball_p_x - ball_half_size < -arena_half_size_x) {
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;
				player_2_score++;
			}
		}

		draw_number(player_1_score, -10, 40, 1.f, 0xbbffbb);
		draw_number(player_2_score, 10, 40, 1.f, 0xbbffbb);

		/*// cout score beta
		float at_x = -80;
		for (int i = 0; i < player_1_score; i++) {
			draw_rect(at_x, 47.f, 1.f, 1.f, 0xaaaaaa);
			at_x += 2.5f;
		}

		float at_x_2 = 80;
		for (int i = 0; i < player_2_score; i++) {
			draw_rect(at_x_2, 47.f, 1.f, 1.f, 0xaaaaaa);
			at_x_2 -= 2.5f;
		}*/

		// Rendering 
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);

		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xff0000);
		draw_rect(-80, player_2_p, player_half_size_x, player_half_size_y, 0xff0000);

	}
	else if (current_gamemode == GM_GAMEPLAY_PAUSE)
	{
		if (pressed(BUTTON_TAB))
		{
			current_gamemode = current_gamemode == GM_GAMEPLAY ? GM_GAMEPLAY_PAUSE : GM_GAMEPLAY;
		}
		draw_text("PAUSE", -30, 0, 2, 0x00dd4b);

		// draw all
		draw_number(player_1_score, -10, 40, 1.f, 0xbbffbb);
		draw_number(player_2_score, 10, 40, 1.f, 0xbbffbb);

		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);

		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xff0000);
		draw_rect(-80, player_2_p, player_half_size_x, player_half_size_y, 0xff0000);

		return; // stop
	}
	else if (current_gamemode == GM_MENU)
	{
		if (pressed(BUTTON_LEFT) || pressed(BUTTON_RIGHT)) {
			hot_button = !hot_button;
		}

		if (pressed(BUTTON_ENTER))
		{
			current_gamemode = hot_button ? GM_SETTINGSMENU : GM_GAMEMENU;
		}

		if (hot_button == 0) {
			draw_text("GAME", -55, -10, 1, 0xff0000);
			draw_text("SETTINGS", 15, -10, 1, 0xaaaaaa);
		}
		else {
			draw_text("GAME", -55, -10, 1, 0xaaaaaa);
			draw_text("SETTINGS", 15, -10, 1, 0xff0000);
		}


		draw_text("BALL GAME", -73, 40, 2, 0xffffff);

		draw_text("CREATED BY EGOR", -73, 15, 1, 0xffffff);
	}
	else if(current_gamemode == GM_GAMEMENU){

		if (pressed(BUTTON_LEFT) || pressed(BUTTON_RIGHT)) {
			hot_button = !hot_button;
		}

		if (pressed(BUTTON_ENTER)) 
		{
			current_gamemode = GM_GAMEPLAY;
			enemy_is_ai = hot_button ? 0 : 1;
		}

		if (hot_button == 0) {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xff0000);
			draw_text("MULTI PLAYER", 10, -10, 1, 0xaaaaaa);
		}
		else {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xaaaaaa);
			draw_text("MULTI PLAYER", 10, -10, 1, 0xff0000);
		}
	}
	else if (current_gamemode == GM_SETTINGSMENU)
	{
		if (pressed(BUTTON_LEFT) || pressed(BUTTON_RIGHT)) {
			hot_button = !hot_button;
		}

		if (pressed(BUTTON_ENTER))
		{
			current_fps = hot_button ? FPS_OFF : FPS_ON;
		}

		if (hot_button == 0) {
			draw_text("FPS ON", -80, -10, 1, 0xff0000);
			draw_text("FPS OFF", 10, -10, 1, 0xaaaaaa);
		}
		else {
			draw_text("FPS ON", -80, -10, 1, 0xaaaaaa);
			draw_text("FPS OFF", 10, -10, 1, 0xff0000);
		}
	}

	if (time_between_fps >= time_fps_update_cooldown) {
		global_fps = fps * (1.f / time_fps_update_cooldown);

		fps = 0;
		time_between_fps = 0;
	}
	else {
		fps++;
		time_between_fps += dt;
	}
}
