namespace Sprite {
	// sprites
	enum sprite_type {
		SP_alien1,
		SP_alien2,
		SP_alien3,
		SP_alien4,

		SP_alien1_boss,
		SP_alien2_boss,
		SP_alien3_boss,
		SP_alien4_boss,

		SP_planet1,
		SP_planet2,
		SP_planet3,
		SP_planet4,
		SP_planet5,

		SP_cursor,
		SP_focus_cursor,

		SP_galaxy,

		SP_star1,
		SP_star2,


		SP_blaster,
		SP_golden_blaster,

		SP_money,

		SP_shield,
		SP_golden_shield,


		SP_COUNT, // Should be the last item
	};

	sprite Sprites[SP_COUNT];

	void init_sprite(sprite_type Sprite_type, string name) {
		Sprites[Sprite_type] = sprite("Sprite\\" + name);
	}

	void init_all_sprites() {
		init_sprite(SP_alien1, "alien1");
		init_sprite(SP_alien2, "alien2");
		init_sprite(SP_alien3, "alien3");
		init_sprite(SP_alien4, "alien4");

		init_sprite(SP_alien1_boss, "alien1_boss");
		init_sprite(SP_alien2_boss, "alien2_boss");
		init_sprite(SP_alien3_boss, "alien3_boss");
		init_sprite(SP_alien4_boss, "alien4_boss");

		//init_sprite(SP_planet1, "planet1");
		init_sprite(SP_planet2, "planet2");
		init_sprite(SP_planet3, "planet3");
		init_sprite(SP_planet4, "planet4");
		init_sprite(SP_planet5, "planet5");

		init_sprite(SP_cursor, "cursor");
		init_sprite(SP_focus_cursor, "focus_cursor");

		init_sprite(SP_star1, "star1");
		init_sprite(SP_star2, "star2");

		init_sprite(SP_blaster, "blaster");
		init_sprite(SP_golden_blaster, "golden_blaster");

		init_sprite(SP_money, "money");

		init_sprite(SP_shield, "shield");
		init_sprite(SP_golden_shield, "golden_shield");


		//init_sprite(SP_galaxy, "galaxy");
	}

	/*		+------+ 	       +------+
			|sprite|	---->  |object|
			+------+           +------+
	*/
}

using namespace Sprite;