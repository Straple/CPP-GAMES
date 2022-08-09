Gamemode current_gamemode = GM_MENU;
Optionsmode current_optionsmode;

double time_between_planet = 0; // times

button play_menu, exit_menu, options_menu, // menu
continue_pause, shop_pause, exit_pause, // pause
options_menu_gameplay, options_menu_controls, options_menu_audio, // options
options_debug_mode,
spaceship_shop, blaster_shop, buy_shop,
options_cancel, options_confirm,
save_slot;

upgrade_button<float> blaster_upgrade_speed, spaceship_upgrade_cooldown_speed;
upgrade_button<int> blaster_upgrade_damage, spaceship_upgrade_shield_count;

slider global_volume_slider, sound_volume_slider, music_volume_slider, UI_volume_slider;

STAR Stars[100];

enemies Enemy;

PLAYER player;

std::vector<planet> Planets;

// weapons
blaster Blaster;
//star_destroyer Destroyer;

shop_object_type hot_shop_object;
//

// saves
bool save_debug;
double save_global_volume, save_sound_volume, save_music_volume, save_UI_volume;
//
