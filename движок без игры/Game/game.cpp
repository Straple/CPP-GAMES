static const dot world_half_size(500, 500);

#include "UI Objects/ui_objects.cpp"

// UI objects

Mouse mouse(SP_CURSOR, SP_FOCUS_CURSOR, 0.09);

void simulate_player(const Input& input, point_t delta_time) {

	// накопление вектора движения
	auto accum_ddp = [&input](button_t left, button_t right, button_t top, button_t bottom) -> dot {
		return dot(is_down(right) - is_down(left), is_down(top) - is_down(bottom));
	};

}

void simulate_physics(point_t delta_time) {

}

void render_game(u8 player_id) {

	clear_screen(0xff000000);

	draw_texture(dot(-world_half_size.x, world_half_size.y), 32, 32, 0.5, SP_GRASS_BACKGROUND);
	
}

template<typename func_t>
void simulate_input(const Input& input, func_t&& window_mode_callback) {
	if (pressed(BUTTON_ESC)) {
		running = false;
	}

	if (pressed(BUTTON_ENTER)) {
		window_mode_callback();
	}


	// update render_scale
	{
		if (is_down(BUTTON_UP)) {

			point_t pt_x = (mouse.pos.x + arena_half_size.x) * scale_factor;
			point_t pt_y = (mouse.pos.y + arena_half_size.y) * scale_factor;


			render_scale *= 0.95;

			// relax scaling
			scale_factor = render_state.height * render_scale;

			// relax arena
			arena_half_size = dot(static_cast<point_t>(render_state.width) / scale_factor, static_cast<point_t>(1) / render_scale) * 0.5;


			mouse.pos = dot(pt_x, pt_y)
				/ scale_factor - arena_half_size;
		}

		if (is_down(BUTTON_DOWN)) {
			point_t pt_x = (mouse.pos.x + arena_half_size.x) * scale_factor;
			point_t pt_y = (mouse.pos.y + arena_half_size.y) * scale_factor;


			render_scale /= 0.95;

			// relax scaling
			scale_factor = render_state.height * render_scale;

			// relax arena
			arena_half_size = dot(static_cast<point_t>(render_state.width) / scale_factor, static_cast<point_t>(1) / render_scale) * 0.5;


			mouse.pos = dot(pt_x, pt_y)
				/ scale_factor - arena_half_size;
		}
	}

	mouse.simulate(input);
}

template<typename func_t>
void simulate_game(const Input& input, point_t delta_time, func_t&& window_mode_callback) {

	simulate_input(input, window_mode_callback);

	// simulate players
	{
		simulate_player(input, delta_time);
	}

	simulate_physics(delta_time);
	mouse.simulate(input);

	render_game(0);
}
