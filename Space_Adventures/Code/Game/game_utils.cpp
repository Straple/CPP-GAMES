internal void init_weapons() {
	// init blaster
	{
		Blaster = blaster(object(SP_blaster, dot(27, 0), 1.3f), weapon(.7f, 0), shield(4.f, 3));
		Blaster.weap.is_buy = true;
		player.hot_weapon = WP_blaster;
	}
	// init Destroyer
	{
		//Destroyer = star_destroyer(weapon(SP_blaster, 1, 11, 300));
	}
}

internal void draw_stars() {
	for (int i = 0; i < 100; i++) {
		draw_sprite(Stars[i].pos, Stars[i].size *.05f, Stars[i].sp);
	}
}

internal void simulate_player_move(float& p, float& dp, float ddp, float dt) {
	simulate_move(p, dp, ddp, dt);

	int player_half_size_y;

	switch (player.hot_weapon) {
		case WP_blaster: {
		player_half_size_y = Sprites[SP_blaster].len.y * Blaster.obj.size;
	}break;
	}

	if (p + player_half_size_y > arena_half_size_y) {
		p = arena_half_size_y - player_half_size_y;
		dp *= 0; // если будет отрицательное то будет происходить отскок 
	}
	else if (p < 0) {
		p = 0;
		dp *= 0;
	}
}

// init all game objects
internal void initialization(Mouse& mouse) {
	// init stars
	{
		for (int i = 0; i < 100; i++) {
			Stars[i] = STAR(
				dot(rnoas(0, arena_half_size_x), rnoas(0, arena_half_size_y)),
				rnoas(1, 3) * 2.5f, roas<sprite_type>({ SP_star2, SP_star1 }));
		}
	}
	// !init all sprites!
	init_all_sprites();

	// init objects
	mouse = Mouse(0.15f, SP_cursor, SP_focus_cursor);

	// init buttons
	{
		// menu
		{
			play_menu = button("PLAY", dot(5, 24), .5f, 0x6a6a6a, 0xb6b6b6, false);
			options_menu = button("OPTIONS", dot(5, 17), .5f, 0x6a6a6a, 0xb6b6b6, false);
			exit_menu = button("QUIT", dot(5, 10), .5f, 0x6a6a6a, 0xb6b6b6, false);
		}
		// pause
		{
			continue_pause = button("Continue", dot(arena_center_x, 70), .5f, 0x6a6a6a, 0xb6b6b6, true);
			shop_pause = button("Shop", dot(arena_center_x, 60), .5f, 0x6a6a6a, 0xb6b6b6, true);
			exit_pause = button("Quit to Menu", dot(arena_center_x, 50), .5f, 0x6a6a6a, 0xb6b6b6, true);
		}
		// options menu
		{
			options_menu_gameplay = button("GAMEPLAY", dot(arena_center_x, 60), .5f, 0x6a6a6a, 0xb6b6b6, true);
			options_menu_controls = button("CONTROLS", dot(arena_center_x, 50), .5f, 0x6a6a6a, 0xb6b6b6, true);
			options_menu_audio = button("AUDIO", dot(arena_center_x, 40), .5f, 0x6a6a6a, 0xb6b6b6, true);
			// gameplay options
			{
				options_debug_mode = button(debug ? "Enable Debug Mod" : "Disable Debug Mod", dot(arena_center_x, 60), .6f, 0x6a6a6a, 0xb6b6b6, true);
			}
			// controls options
			{

			}
			// audio options
			{
				global_volume_slider = slider(dot(arena_center_x - 30, 70), dot(arena_center_x + 30, 67), global_volume);
				music_volume_slider = slider(dot(arena_center_x - 30, 55), dot(arena_center_x + 30, 52), music_volume);
				sound_volume_slider = slider(dot(arena_center_x - 30, 40), dot(arena_center_x + 30, 37), sound_volume);
				UI_volume_slider = slider(dot(arena_center_x - 30, 25), dot(arena_center_x + 30, 22), UI_volume);
			}
			// options
			{
				options_cancel = button("CANCEL", dot(arena_center_x - 30, 10), .5f, 0x6a6a6a, 0xb6b6b6, true);
				options_confirm = button("CONFIRM", dot(arena_center_x + 30, 10), .5f, 0x6a6a6a, 0xb6b6b6, true);
			}
		}
		// shop
		{
			buy_shop = button("BUY", dot(130, 67), .7f, 0xffff00, 0xF88401, false);
			spaceship_shop = button("SPACESHIP", dot(2, 75), .7f, 0x6a6a6a, 0xb6b6b6, false);
			blaster_shop = button("BLASTER", dot(2, 65), .7f, 0x6a6a6a, 0xb6b6b6, false);
			init_shop_objects();

			blaster_upgrade_speed.new_button(button("Upgrade attack reload time 15", dot(50, 55), .7f, 0x00B9FF, 0x0070FF, false),
				15, Blaster.weap.time_attack_cool_down, .95f, false);
			blaster_upgrade_damage.new_button(button("Upgrade attack damage 30", dot(50, 40), .7f, 0x00B9FF, 0x0070FF, false),
				30, Blaster.damage, 1, true);

			spaceship_upgrade_cooldown_speed.new_button(button("Upgrade shield repair speed 15", dot(50, 55), .7f, 0x00B9FF, 0x0070FF, false),
				15, Blaster.sh.recovery_time_cooldown, .92f, false);
			spaceship_upgrade_shield_count.new_button(button("Upgrade number of shields 20", dot(50, 40), .7f, 0x00B9FF, 0x0070FF, false),
				20, Blaster.sh.const_health, 1, true);
		}
	}
}

internal void update_time_between_attack(float dt) {
	Blaster.weap.time_update(dt);
	{
		//Destroyer.Weapon.time_update(dt);
		//Destroyer.time_update(dt);
	}
}

internal void draw_background() {
	clear_screen(0x090010);
	draw_stars();
}

internal void draw_loading_screen() {
	clear_screen(0x000000);
	draw_text("Green Game Developer", dot(arena_center_x, 50 + 10), 1.f, 0x00ff00, true);
	draw_text("Loading...", dot(arena_center_x, 7), .6f, 0x6a6a6a, true);
}

namespace Level {
	void simulate_options(Input* input, Mouse& mouse) {
		draw_sprite(dot(4, 0), .7f, SP_planet5);
		draw_sprite(dot(120, 50), 1, SP_planet2);

		switch (current_optionsmode) {
		case OM_GAMEPLAY: {
			// Rendering
			{
				draw_text("OPTIONS", dot(arena_center_x, 80), .5f, 0x6a6a6a, true);
				options_debug_mode.simulate_button(mouse);
			}

			if (pressed(BUTTON_MOUSE_L)) {
				if (options_debug_mode.is_focus) {
					debug = !debug;
					options_debug_mode = button(debug ? "Enable Debug Mod" : "Disable Debug Mod", dot(arena_center_x, 60), .6f, 0x6a6a6a, 0xb6b6b6, true);
					options_debug_mode.is_focus = true;
				}
			}
		}break;
		case OM_CONTROLS: {
			draw_text("The controls is maintained by the developer", dot(arena_center_x, 50), .5f, 0x5833FF, true);
		}break;
		case OM_AUDIO: {
			// Rendering
			{
				draw_text("OPTIONS", dot(arena_center_x, 85), .5f, 0x6a6a6a, true);

				draw_text("Global Volume", dot(arena_center_x, 75), .5f, 0x6a6a6a, true);

				draw_text("Music Volume", dot(arena_center_x, 60), .5f, 0x6a6a6a, true);

				draw_text("SFX Volume", dot(arena_center_x, 45), .5f, 0x6a6a6a, true);

				draw_text("UI Volume", dot(arena_center_x, 30), .5f, 0x6a6a6a, true);
			}

			// simulate sliders
			{
				music_volume_slider.simulate_slider(pressed(BUTTON_MOUSE_L), is_down(BUTTON_MOUSE_L), mouse, music_volume);
				sound_volume_slider.simulate_slider(pressed(BUTTON_MOUSE_L), is_down(BUTTON_MOUSE_L), mouse, sound_volume);
				global_volume_slider.simulate_slider(pressed(BUTTON_MOUSE_L), is_down(BUTTON_MOUSE_L), mouse, global_volume);
				UI_volume_slider.simulate_slider(pressed(BUTTON_MOUSE_L), is_down(BUTTON_MOUSE_L), mouse, UI_volume);
			}
		}break;
		}

		// simulate buttons
		{
			options_cancel.simulate_button(mouse);
			options_confirm.simulate_button(mouse);
		}

		if (pressed(BUTTON_MOUSE_L)) {
			if (options_cancel.is_focus) {
				current_gamemode = GM_OPTIONS_MENU;

				switch (current_optionsmode) {
				case OM_GAMEPLAY: {
					debug = save_debug;
					options_debug_mode = button(debug ? "Enable Debug Mod" : "Disable Debug Mod", dot(arena_center_x, 60), .6f, 0x6a6a6a, 0xb6b6b6, true);
				}break;
				case OM_AUDIO: {
					Audio::global_volume = save_global_volume;
					Audio::sound_volume = save_sound_volume;
					Audio::music_volume = save_music_volume;
					Audio::UI_volume = save_UI_volume;
					// audio options
					{
						global_volume_slider = slider(dot(arena_center_x - 30, 70), dot(arena_center_x + 30, 67), global_volume);
						music_volume_slider = slider(dot(arena_center_x - 30, 55), dot(arena_center_x + 30, 52), music_volume);
						sound_volume_slider = slider(dot(arena_center_x - 30, 40), dot(arena_center_x + 30, 37), sound_volume);
						UI_volume_slider = slider(dot(arena_center_x - 30, 25), dot(arena_center_x + 30, 22), UI_volume);
					}
					update_musics_volume();
				}break;
				}
			}
			else if (options_confirm.is_focus) {
				switch (current_optionsmode) {
				case OM_GAMEPLAY: {
					save_debug = debug;
				}break;
				case OM_AUDIO: {
					save_global_volume = global_volume;
					save_sound_volume = sound_volume;
					save_music_volume = music_volume;
					save_UI_volume = UI_volume;
				}break;
				}
			}
		}
	}
}
using namespace Level;

namespace Save {
	void make_save(size_t save_number) {
		std::string file = "Save\\save_slot";
		file += save_number + '0';
		ofstream output(file);

		output << player << "\n" << Enemy << "\n" << Planets << "\n" << Blaster << "\n" << time_between_planet;
		output.close();
	}
	void get_save(size_t save_number) {
		std::string file = "Save\\save_slot";
		file += save_number + '0';
		ifstream input(file);
		
		input >> player >> Enemy >> Planets >> Blaster >> time_between_planet;

		input.close();
	}
	bool have_save(size_t save_number) {
		std::string file = "Save\\save_slot";
		file += save_number + '0';
		ifstream input_save_slot(file);
		std::string str;
		input_save_slot >> str;
		input_save_slot.close();
		return str != "none";
	}
	void new_save() {
		player = PLAYER();
		init_weapons();
		Planets.clear();
		Enemy = enemies();
	}
	void delete_save() {
		ofstream output("Save\\save_slot1");
		output << "none";
		output.close();
	}
}

void game_over() {
	current_gamemode = GM_GAMEOVER;
	change_of_music(MT_gameplay, MT_gameover);

	Save::delete_save();
}
