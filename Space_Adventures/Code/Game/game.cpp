#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
#define is_down(b) input->buttons[b].is_down

#include "Game\\game_variable.cpp"
#include "Game\\game_utils.cpp"

internal void
simulate_game(Input* input, double dt) {
	draw_background();

	update_time_between_attack(dt);
	update_musics(dt);

	// update fps
	{
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

	// update important buttons
	{
		if (debug) {
			if (pressed(BUTTON_MOUSE_R)) {
				running = false;
			}
		}
	}

	switch (current_gamemode) {
	case GM_GAMEPLAY: {
		
		// Simulate player
		{
			if (pressed(BUTTON_ESC)) {
				current_gamemode = GM_PAUSE;
				change_of_music(MT_gameplay, MT_menu);
			}
			// update attack
			if (is_down(BUTTON_J)) {
				switch (player.hot_weapon) {
					case WP_blaster: {
						Blaster.simulate_shot();
					}break;
					case WP_destroyer: {
						//Destroyer.simulate_shot(player.pos);
					}break;
				}
			}

			// update move
			{
				double player_ddp = 0.f;
				if (is_down(BUTTON_W) || is_down(BUTTON_UP)) {
					player_ddp += 2000; //speed* dt
				}
				if (is_down(BUTTON_S) || is_down(BUTTON_DOWN)) {
					player_ddp -= 2000; //speed* dt
				}
				simulate_player_move(player.pos.y, player.dp.y, (Blaster.sh.golden ? player_ddp * .35f : player_ddp), dt);
				Blaster.obj.new_pos(dot(27, player.pos.y));
			}

			// update weapons
			{
				Blaster.update(dt, is_down(BUTTON_K));
			}
			// simulate collsion
			{
				for (size_t i = 0; i < Enemy.Lasers.size(); i++) {
					// попали в корабль
					if (Blaster.obj.trigger(Enemy.Lasers[i].pos)) {
						Enemy.Lasers.erase(Enemy.Lasers.begin() + i);
						if (Blaster.sh.golden) {
							Blaster.sh.health--;
							Blaster.sh.update_shield();
						}
						else {
							game_over(); // мы проиграли
						}
					}
				}
			}
		}

		// Simulate enemy
		{
			Enemy.simulate(dt);
			player.money_count += Blaster.simulate_collision(Enemy.Aliens) * 2.f;
		}

		//simulate planet
		{
			// new planet
			{
				if (time_between_planet <= 0) {
					Planets.push_back(planet(object(roas<sprite_type>({ SP_planet2, SP_planet3, SP_planet4, SP_planet5 }), dot(arena_half_size_x + 10, rnoas(0, arena_half_size_y - 15)), rnoas(2, 4) * .1f), 3));
					time_between_planet = rnoas(7, 15);
				}
				else {
					time_between_planet -= dt;
				}
			}

			// update collision
			{
				player.money_count += Blaster.simulate_collision(Planets) * 3;
			}

			// update move
			{
				for (int i = 0; i < Planets.size(); i++) {
					dot Null;
					Planets[i].plan.update_move2d(dt, Null, dot(-1000, 0));
					if (Planets[i].plan.draw_position.x < -30) {
						Planets.erase(Planets.begin() + i);
					}
				}
			}
		}

		// WIN
		{
			if (Enemy.wawe_count == 20) {
				current_gamemode = GM_WIN;
				change_of_music(MT_gameplay, MT_boss);
				Save::delete_save();
			}
		}


		// Rendering
		{
			draw("%s%c%d", dot(1, arena_half_size_y - 10), 1.f, (Blaster.sh.golden ? SP_golden_shield : SP_shield), (Blaster.sh.golden ? 0xffff00 : 0x0049FF), (Blaster.sh.golden ? Blaster.sh.health : 1));
			draw("%s%c%d", dot(1, arena_half_size_y - 20), 1.f, SP_money, 0xffff00, player.money_count);

			Blaster.draw(player.hot_weapon);

			for (size_t i = 0; i < Planets.size(); i++) {
				Planets[i].plan.draw();
			}
		}
	}break;
	case GM_PAUSE: {
		// Rendering
		{
			draw_text("PAUSE", dot(arena_center_x, arena_half_size_y - 20), .5f, 0x6a6a6a, true);
			continue_pause.simulate_button(mouse);
			shop_pause.simulate_button(mouse);
			exit_pause.simulate_button(mouse);
		}

		if (pressed(BUTTON_MOUSE_L)) {
			if (continue_pause.is_focus) {
				current_gamemode = GM_GAMEPLAY;
				change_of_music(MT_menu, MT_gameplay);
			}
			else if (exit_pause.is_focus) {
				current_gamemode = GM_MENU;
				Save::make_save(1);
			}
			else if (shop_pause.is_focus) {
				current_gamemode = GM_SHOP;
				change_of_music(MT_menu, MT_shop);
			}
		}
	}break;
	case GM_MENU: {
		// Rendering
		{
			play_menu.simulate_button(mouse);
			exit_menu.simulate_button(mouse);
			options_menu.simulate_button(mouse);

			draw_sprite(dot(75, 65), .6f, SP_planet2);
			draw_sprite(dot(50, 0), .7f, SP_planet5);
			draw_sprite(dot(120, -5), 1.f, SP_planet4);

			draw_text("beta", dot(151, 45), .7f, 0x5833FF, false);

			draw_text("SPACE ADVENTURES", dot(arena_center_x, arena_half_size_y * .5f + 10), 1.7f, 0x5833FF, true);
		}

		if (pressed(BUTTON_MOUSE_L)) {
			if (play_menu.is_focus) {
				current_gamemode = GM_SAVE;
				// init saves
				{
					if (!Save::have_save(1)) {
						save_slot = button("New Game", dot(arena_center_x, 60), 1, 0x6a6a6a, 0xb6b6b6, true);
					}
					else {
						save_slot = button("Continue Game", dot(arena_center_x, 60), 1, 0x6a6a6a, 0xb6b6b6, true);
					}
				}
			}
			else if (exit_menu.is_focus) {
				running = false;
			}
			else if (options_menu.is_focus) {
				current_gamemode = GM_OPTIONS_MENU;
			}
		}
	}break;
	case GM_OPTIONS_MENU: {
		// Rendering
		{
			draw_sprite(dot(5, 30), 1, SP_planet4);
			draw_sprite(dot(110, 0), 1, SP_planet2);

			draw_text("OPTIONS", dot(arena_center_x, 70), .5f, 0x6a6a6a, true);
			options_menu_gameplay.simulate_button(mouse);
			options_menu_controls.simulate_button(mouse);
			options_menu_audio.simulate_button(mouse);
		}

		if (pressed(BUTTON_ESC)) {
			current_gamemode = GM_MENU;
		}
		if (pressed(BUTTON_MOUSE_L)) {
			current_gamemode = GM_OPTIONS;
			if (options_menu_gameplay.is_focus) {
				current_optionsmode = OM_GAMEPLAY;
				// saves
				{
					save_debug = debug;
				}
			}
			else if (options_menu_controls.is_focus) {
				current_optionsmode = OM_CONTROLS;
			}
			else if (options_menu_audio.is_focus) {
				current_optionsmode = OM_AUDIO;
				// saves
				{
					save_global_volume = global_volume;
					save_sound_volume = sound_volume;
					save_music_volume = music_volume;
					save_UI_volume = UI_volume;
				}
			}
			else {
				current_gamemode = GM_OPTIONS_MENU;
			}
		}
	}break;
	case GM_OPTIONS: {
		simulate_options(input, mouse);
	}break;
	case GM_GAMEOVER: {
		draw_text("GAME OVER", dot(arena_center_x, arena_half_size_y - 10), 2, 0xffff00, true);
		draw_text("Press SPACE to menu", dot(30, 5), .5f, 0x6D6D6D, false);

		if (pressed(BUTTON_SPACE)) {
			current_gamemode = GM_MENU;
			change_of_music(MT_gameover, MT_menu);
		}
	}break;
	case GM_SHOP: {
		//renderer
		{
			draw("%cThe shop is maintained by the developer%nbut you can buy or upgrade basic items.", dot(10, 20), .7f, 0x5833FF);
			{
				spaceship_shop.simulate_button(mouse);
				blaster_shop.simulate_button(mouse);
			}
			draw_text("SHOP", dot(arena_center_x, arena_half_size_y - 4), .6f, 0x6a6a6a, true);
			draw("%cITEMS", dot(2, arena_half_size_y - 12), 1.f, 0x6a6a6a);

			// draw description
			Shop_objects[hot_shop_object].description.draw_Text();

			// draw buttons
			switch (hot_shop_object) {
				case SH_blaster: {
					if (!Blaster.weap.is_buy) {
						buy_shop.simulate_button(mouse);
						draw_number(Blaster.weap.cost, dot(132, 66.5f), .7f, 0xffff00);
					}
					else {
						blaster_upgrade_speed.b.simulate_button(mouse);
						blaster_upgrade_damage.b.simulate_button(mouse);

						draw_text("Upgrades", dot(arena_center_x, 65), .7f, 0x6a6a6a, false);
					}
				}break;
				case SH_spaceship: {
					spaceship_upgrade_cooldown_speed.b.simulate_button(mouse);
					spaceship_upgrade_shield_count.b.simulate_button(mouse);

					draw_text("Upgrades", dot(arena_center_x, 65), .7f, 0x6a6a6a, false);
				}break;
			}
		}

		if (pressed(BUTTON_ESC)) {
			current_gamemode = GM_PAUSE;
			change_of_music(MT_shop, MT_menu);
		}

		if (pressed(BUTTON_MOUSE_L)) {
			if (spaceship_shop.is_focus) {
				hot_shop_object = SH_spaceship;
			}
			else if (blaster_shop.is_focus) {
				hot_shop_object = SH_blaster;
			}
			else if (blaster_upgrade_speed.b.is_focus) {
				blaster_upgrade_speed.simulate_buy(player.money_count);
			}
			else if (blaster_upgrade_damage.b.is_focus) {
				blaster_upgrade_damage.simulate_buy(player.money_count);
			}
			else if (spaceship_upgrade_cooldown_speed.b.is_focus) {
				spaceship_upgrade_cooldown_speed.simulate_buy(player.money_count);
			}
			else if (spaceship_upgrade_shield_count.b.is_focus) {
				spaceship_upgrade_shield_count.simulate_buy(player.money_count);
			}
		}
	}break;
	case GM_SAVE: {
		{
			save_slot.simulate_button(mouse);
		}
		if (pressed(BUTTON_MOUSE_L)) {
			if (save_slot.is_focus) {
				if (Save::have_save(1)) {
					Save::get_save(1);
				}
				else {
					Save::new_save();
				}
				change_of_music(MT_menu, MT_gameplay);
				current_gamemode = GM_GAMEPLAY;
			}
		}
		if (pressed(BUTTON_ESC)) {
			current_gamemode = GM_MENU;
		}
	}break;
	case GM_WIN: {
		draw_text("Congratulations you won", dot(arena_center_x, 70), 1.4f, 0x1BFF00, true);
		draw_text("Press SPACE to menu", dot(30, 5), .5f, 0x6D6D6D, false);

		if (pressed(BUTTON_SPACE)) {
			current_gamemode = GM_MENU;
			change_of_music(MT_boss, MT_menu);
		}
	}break;
	}

	if (debug) {
		draw_number(mouse.pos.x, dot(10, 5), .4f, 0x6D6D6D);
		draw_number(mouse.pos.y, dot(20, 5), .4f, 0x6D6D6D);

		draw_number(global_fps, dot(arena_half_size_x - 7, arena_half_size_y - 3), .4f, 0x6D6D6D);
	}

	if (current_gamemode != GM_GAMEPLAY) {
		mouse.draw();
	}
}