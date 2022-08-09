namespace Game_Object {
	enum Gamemode {
		GM_MENU,
		GM_GAMEPLAY,
		GM_PAUSE,
		GM_OPTIONS_MENU,
		GM_OPTIONS,
		GM_SHOP,
		GM_GAMEOVER,
		GM_SAVE,
		GM_WIN,
	};
	enum Optionsmode {
		OM_GAMEPLAY,
		OM_CONTROLS,
		OM_AUDIO,
	};
	struct STAR {
		dot pos;
		float size;
		sprite_type sp;
		STAR(dot _pos, float _size, sprite_type _sp) {
			pos = _pos;
			size = _size;
			sp = _sp;
		}
		STAR() {

		}
	};

	template<typename OBJECT>
	std::ostream& operator << (std::ostream& output, const std::vector<OBJECT> Arr) {
		output << Arr.size() << " ";
		for (size_t i = 0; i < Arr.size(); i++) {
			output << Arr[i] << " ";
		}
		return output;
	}
	template<typename OBJECT>
	std::istream& operator >> (std::istream& input, std::vector<OBJECT> Arr) {
		size_t length;
		input >> length;
		Arr.clear();
		Arr.resize(length);
		for (size_t i = 0; i < length; i++) {
			input >> Arr[i];
		}
		return input;
	}

	float random_bound(bool is_up, float y, float arena_half_size_y) {
		if (is_up) {
			return rnoas(y + 1, arena_half_size_y);
		}
		else {
			return rnoas(0, y - 1);
		}
	}

	struct laser {
		dot pos;
		float dp;
		u32 color;

		laser(dot position, float _dp, u32 _color) {
			pos = position;
			dp = _dp;
			color = _color;
		}
		laser() {

		}
		void update(float dt) {
			pos.x += 3.f * dp * dt * (dp < 0 ? -dp : dp);
		}
		void draw() {
			draw_rect(pos, 1.6f, .3f, color);
		}
	};
	std::ostream& operator << (std::ostream& output, const laser& las) {
		return output << las.color << " " << las.dp << " " << las.pos;
	}
	std::istream& operator >> (std::istream& input, laser& las) {
		return input >> las.color >> las.dp >> las.pos;
	}

	u32 laser_colors[] = { 0xFF0000, 0xF9FF00 , 0x0DFF00 , 0x00FFF3, 0x0D00FF, 0xF500FF };

	void next_laser_color(size_t &it) {
		it++;
		it = (it >= 6) ? 0 : it;
	}

	struct alien {
		object obj;
		dot dp;

		bool up;
		float bound; // [-13, arena_half_size_y]
		int health;

		std::vector<u32> laser_colors;
		int laser_color;

		float next_time_attack;

		float new_next_time_attack() {
			return rnoas(30, 70) * .1f;
		}

		alien() {}
		alien(sprite_type type, dot pos, float _size, bool _up, float arena_half_size_y, int _health) {
			up = _up;
			dp = dot();
			obj = object(type, pos, _size);
			bound = random_bound(_up, _up ? obj.coll.a.y : obj.coll.b.y, arena_half_size_y);
			health = _health;
			next_time_attack = new_next_time_attack();

			laser_colors = { 0xFF0000, 0xF9FF00 , 0x0DFF00 , 0x00FFF3, 0x0D00FF, 0xF500FF };
			laser_color = rnoas(0, 5);
		}
		void update_move(float dt, dot ddp) {
			obj.update_move2d(dt, dp, dot(ddp.x, !up ? -ddp.y : ddp.y));
		}
		void simulate_rebound(float arena_half_size_y) { // good
			/* ----------------------

						a-----+
						|	  |
						|	  |
						+-----b

			   ----------------------
			*/
			if (up && obj.coll.a.y > bound) { // upper bound
				float delta_y = bound - obj.coll.a.y;
				obj.coll.a.y -= delta_y;
				obj.coll.b.y -= delta_y;
				dp.y *= 0; // если будет отрицательное то будет происходить отскок 
				// обновить границу
				up = false;
				bound = random_bound(false, obj.coll.b.y, arena_half_size_y);
			}
			else if (!up && obj.coll.b.y < bound) {
				float delta_y = -bound + obj.coll.b.y;
				obj.coll.a.y -= delta_y;
				obj.coll.b.y -= delta_y;
				dp.y *= 0;
				// обновить границу
				up = true;
				bound = random_bound(true, obj.coll.a.y, arena_half_size_y);
			}
			// update draw_position
			{
				obj.draw_position = dot(obj.coll.a.x, obj.coll.b.y);
			}
		}
		void draw() {
			draw_sprite(obj.draw_position, obj.size, obj.sp_type);

			if (debug) {
				draw_rect(obj.coll.a, .5f, .5f, 0xff0000);
				draw_rect(obj.coll.b, .5f, .5f, 0xff0000);
			}
		}
		bool trigger(dot pt) {
			return obj.coll.trigger(pt);
		}
		void simulate_shot(float dt, std::vector<laser> &Lasers) {
			next_time_attack -= dt;
			if (next_time_attack <= 0) {
				next_time_attack = new_next_time_attack();
				// shot
				Lasers.push_back(laser(dot(obj.draw_position.x, obj.draw_position.y + Sprites[obj.sp_type].len.y * .5f), -5.f, laser_colors[laser_color]));
				laser_color++;
				if (laser_color >= 6) {
					laser_color = 0;
				}
			}
		}
	};
	std::ostream& operator << (std::ostream& output, const alien& ali) {
		return output << ali.obj << " " << ali.dp << " " << ali.bound << " " << ali.up << " " << ali.laser_color
			<< " " << ali.laser_colors << " " << ali.health << " " << ali.next_time_attack;
	}
	std::istream& operator >> (std::istream& input, alien& ali) {
		return input >> ali.obj >> ali.dp >> ali.bound >> ali.up >> ali.laser_color
			>> ali.laser_colors >> ali.health >> ali.next_time_attack;
	}

	struct enemies {
		std::vector<alien> Aliens;
		std::vector<laser> Lasers;

		// wawe
		double time_between_wawe;
		int wawe_count, wawe_alien_count;
		bool wawe_need_for_update;

		enemies() {
			Aliens.clear();
			Lasers.clear();
			wawe_count = wawe_alien_count = time_between_wawe = 0;
			wawe_need_for_update = true;
		}

		void update_move(double dt) {
			for (size_t i = 0; i < Aliens.size(); i++) {
				if (Aliens[i].obj.coll.a.x < -20) {
					Aliens.erase(Aliens.begin() + i);
					i--;
				}
				else {
					Aliens[i].update_move(dt, dot(-250, 400));
					Aliens[i].simulate_rebound(arena_half_size_y);
				}
			}
			for (size_t i = 0; i < Lasers.size(); i++) {
				Lasers[i].update(dt);
			}
		}
		void update_shot(double dt) {
			for (size_t i = 0; i < Aliens.size(); i++) {
				Aliens[i].simulate_shot(dt, Lasers);
			}
		}
		void update_wawe(double dt) {
			if (Aliens.empty()) {
				if (time_between_wawe > 5) {
					time_between_wawe = 0;
					// create new Enemies
					{
						wawe_alien_count += 1;
						bool is_up = false;
						for (int i = 0; i <= wawe_alien_count / 5; i++) {
							for (int j = 0; j < 5 && wawe_alien_count - i * 5 - j > 0; j++) {
								Aliens.push_back(alien(roas<sprite_type>({ SP_alien1, SP_alien2, SP_alien3, SP_alien4 }), dot(double(arena_half_size_x + 30 + i * 22), rnoas(10, arena_half_size_y - 15)), 1.1f, is_up, arena_half_size_y, 1 + my_clamp(0.f, wawe_count * 0.15f, 100000.f)));
								is_up = !is_up;
							}
						}
					}
					wawe_need_for_update = true;
				}
				else {
					wawe_count += wawe_need_for_update;
					wawe_need_for_update = false;

					time_between_wawe += dt;
					draw("%cThe next wave will be in %d", dot(47, 5), .5f, 0x5833ff, my_clamp(1, 5 - (int)time_between_wawe, 5));
				}
			}
		}
		void draw_enemies() {
			for (size_t i = 0; i < Aliens.size(); i++) {
				Aliens[i].draw();
			}
			for (size_t i = 0; i < Lasers.size(); i++) {
				Lasers[i].draw();
			}
		}
		void simulate(double dt) {
			update_wawe(dt);
			update_shot(dt);
			update_move(dt);
			draw_enemies();
		}
		
	};
	std::ostream& operator << (std::ostream& output, const enemies& enemy) {
		return output << enemy.Aliens << " " << enemy.Lasers << " " << enemy.time_between_wawe << " " << enemy.wawe_alien_count 
			<< " " << enemy.wawe_count << " " << enemy.wawe_need_for_update;
	}
	std::istream& operator >> (std::istream& input, enemies& enemy) {
		return input >> enemy.Aliens >> enemy.Lasers >> enemy.time_between_wawe >> enemy.wawe_alien_count
			>> enemy.wawe_count >> enemy.wawe_need_for_update;
	}
	struct planet {
		object plan;
		int health;
		planet() {}
		planet(object _plan, int _health) {
			plan = _plan;
			health = _health;
		}
		bool trigger(dot pos) {
			return plan.trigger(pos);
		}
	};
	std::ostream& operator << (std::ostream& output, const planet& plan) {
		return output << plan.plan << " " << plan.health;
	}
	std::istream& operator >> (std::istream& input, planet& plan) {
		return input >> plan.plan >> plan.health;
	}

	enum weapon_type {
		WP_blaster,
		WP_destroyer,

		WP_COUNT,
	};
	bool buy(int cost, int& money_count) {
		bool may_buy = cost <= money_count;
		if (may_buy) {
			money_count -= cost;
			Sounds[ST_buy].play();
		}
		else {
			Sounds[ST_failed_to_buy].play();
		}
		return may_buy;
	}
	struct weapon {
		float time_attack_cool_down; // задержка перед следующей атакой
		float time_between_attack; // время с прошлой атаки

		bool is_buy; // была куплена
		int cost; // стоимость

		weapon(float _time_attack_cool_down, int _cost) {
			time_between_attack = time_attack_cool_down = _time_attack_cool_down;
			cost = _cost;
			is_buy = false;
		}
		weapon() {

		}
		// если игрок навел и нажал на кнопку покупки
		bool buy_weapon(int& money_count) {
			bool may_buy = buy(cost, money_count);
			if (may_buy) {
				is_buy = true;
			}
			return may_buy;
		}
		void time_update(float dt) {
			time_between_attack += dt;
		}
		// можем выстрелить. Если можем, то функция обновит время
		bool may_shot() {
			bool may = time_between_attack >= time_attack_cool_down;
			if (may) {
				time_between_attack = 0;
			}
			return may;
		}
	};
	std::ostream& operator << (std::ostream& output, const weapon& weap) {
		return output << weap.cost << " " << weap.is_buy << " " << weap.time_attack_cool_down << " " << weap.time_between_attack;
	}
	std::istream& operator >> (std::istream& input, weapon& weap) {
		return input >> weap.cost >> weap.is_buy >> weap.time_attack_cool_down >> weap.time_between_attack;
		
	}

	struct shield {
		float recovery_time_cooldown, recovery_time;
		int const_health, health;
		bool golden;
		shield() {}
		shield(double _recovery_time_cooldown, size_t _health) {
			recovery_time_cooldown = _recovery_time_cooldown;
			const_health = health = _health;
			golden = false;
		}
		// return when need draw golden spaceship
		void simulate_shield(float dt, bool is_down) {
			if (health == 0) {
				recovery_time -= dt;
				if (recovery_time <= 0) {
					health = const_health;
					recovery_time = recovery_time_cooldown;
				}
			}
			golden = is_down && health != 0;
		}
		void update_shield() {
			if (health <= 0) { // защита спала
				golden = false; 
				recovery_time = recovery_time_cooldown;
			}
		}
	};
	std::ostream& operator << (std::ostream& output, const shield& sh) {
		return output << sh.const_health << " " << sh.golden << " " << sh.health << " " << sh.recovery_time << " " << sh.recovery_time_cooldown;
	}
	std::istream& operator >> (std::istream& input, shield& sh) {
		return input >> sh.const_health >> sh.golden >> sh.health >> sh.recovery_time >> sh.recovery_time_cooldown;
	}

	struct blaster {
		object obj;
		weapon weap;
		size_t laser_color_it;
		int damage;
		shield sh;

		std::vector<laser> lasers;

		blaster() {
			lasers.clear();
			laser_color_it = rnoas(0, 5);
			damage = 1;
		}
		blaster(object _obj, weapon _weap, shield _sh) {
			*this = blaster();
			obj = _obj;
			weap = _weap;
			sh = _sh;
		}

		void simulate_shot() {
			if (weap.may_shot()) {
				lasers.push_back(laser(dot(obj.coll.b.x, obj.coll.b.y + Sprites[obj.sp_type].len.y * obj.size * .5f), 5.f, laser_colors[laser_color_it]));
				next_laser_color(laser_color_it);

				Sounds[roas<sound_type>({ ST_blaster1, ST_blaster2 })].play();
			}
		}
		/* возвращает количество уничтоженных OBJ*/
		template<typename OBJ>
		int simulate_collision(std::vector<OBJ>& Arr) {
			int ret = 0;
			for (size_t i = 0; i < Arr.size(); i++) {
				for (size_t j = 0; j < lasers.size(); j++) {
					// снаряд попал в цель
					if ((Arr[i].trigger(lasers[j].pos + dot(1.6f, 0)))) {
						Arr[i].health -= damage;
						lasers.erase(lasers.begin() + j);
						j--;
						if (Arr[i].health <= 0) { // цель уничтожена
							Arr.erase(Arr.begin() + i);
							i--;
							ret++;
							break;
						}
					}
				}
			}
			return ret;
		}
		/*Симулирует движение лазеров, выходом за пределы*/
		void update(float dt, bool is_down) {
			for (size_t i = 0; i < lasers.size(); i++) {
				lasers[i].update(dt);
				if (lasers[i].pos.x > 177) {
					lasers.erase(lasers.begin() + i);
					i--;
				}
			}
			sh.simulate_shield(dt, is_down);
		}
		void draw(weapon_type hot_weapon) {
			if (hot_weapon == WP_blaster) {
				if (sh.golden) {
					obj.sp_type = SP_golden_blaster; // blaster golden !!!
					obj.draw();
					obj.sp_type = SP_blaster;
				}
				else {
					obj.draw();
				}
			}
			for (int i = 0; i < lasers.size(); i++) {
				lasers[i].draw();
			}
		}
	};
	std::ostream& operator << (std::ostream& output, const blaster& blas) {
		return output << blas.damage << " " << blas.lasers << " " << blas.laser_color_it << " " << blas.obj << " " << blas.sh << " " << blas.weap;
	}
	std::istream& operator >> (std::istream& input, blaster& blas) {
		return input >> blas.damage >> blas.lasers >> blas.laser_color_it >> blas.obj >> blas.sh >> blas.weap;
	}

	/*struct star_destroyer {
		weapon Weapon;
		float remaining_attack_time, // оставшееся время атаки 
			remaining_charge_time; // оставшееся время зарядки

		float attack_duration,// длительность атаки
			charging_time; // длительность зарядки

		enum attack_type {
			AT_charging,
			AT_attacking,
			AT_nothing,
		};

		attack_type Type;

		star_destroyer() {

		}
		star_destroyer(weapon _Weapon) {
			Weapon = _Weapon;
			attack_duration = 2;
			charging_time = 8;

			Type = AT_nothing;
		}
		void simulate_shot(dot pos) {
			if (Weapon.may_shot()) {
				remaining_charge_time = charging_time;
				remaining_attack_time = attack_duration;
				Type = AT_charging;
				// simulate sound
				Sounds[ST_star_desctroyer].play();
			}
		}
		void update_ray(dot pos) {

		}
		void time_update(float dt) {
			if (Type == AT_charging) {
				if (remaining_charge_time <= 0) {
					Type = AT_attacking;
				}
				else {
					remaining_charge_time -= dt;
				}
			}
			else if (Type == AT_attacking) {
				if (remaining_attack_time <= 0) {
					Type = AT_nothing;
				}
				else {
					remaining_attack_time -= dt;
				}
			}
		}
		void draw(dot pos, weapon_type hot_weapon) {
			if (hot_weapon == WP_destroyer) {
				//Weapon.draw();
			}
			if (Type == AT_attacking) {
				//draw_sprite(pos, 3, Sprites[SP_ray]);
				//draw_rectangle(pos, dot(177, pos.y - 6), 0xFF0068);
			}
		}
	};*/

	struct PLAYER {
		int money_count;
		weapon_type hot_weapon;
		dot pos, dp;
		PLAYER() {
			money_count = 0;
			pos = dot();
			dp = dot();
		}
	};
	std::ostream& operator << (std::ostream& output, const PLAYER& player) {
		return output << " " << player.money_count << " " << player.hot_weapon << " " << player.pos << " " << player.dp;
	}
	std::istream& operator >> (std::istream& input, PLAYER& player) {
		input >> player.money_count;
		int wp; input >> wp;
		player.hot_weapon = (weapon_type)wp;
		input >> player.pos >> player.dp;
		return input;
	}

	struct shop_object {
		Text description;
		button buy;
		shop_object(Text _description) {
			description = _description;
		}
		shop_object() {

		}
	};

	template<typename value>
	value new_val;

	template<typename upgrade_value>
	struct upgrade_button {
		button b;
		int cost;
		upgrade_value* Value;// = new_val<upgrade_value>;
		double upgrade;
		bool plus;

		upgrade_button() {
			//Value = new_val<upgrade_value>;
		}

		void simulate_buy(int &money_count) {
			if (buy(cost, money_count)) {
				*Value = plus ? *Value + upgrade : *Value * upgrade;
			}
		}
		
		void new_button(button _b, int _cost, upgrade_value& _Value, double _upgrade, bool _plus) {
			b = _b;
			cost = _cost;
			Value = &_Value;
			upgrade = _upgrade;
			plus = _plus;
		}
	};

	enum shop_object_type {
		SH_spaceship,
		SH_blaster,
		SH_COUNT,
	};

	shop_object Shop_objects[SH_COUNT];

	internal void init_shop_objects() {
		Shop_objects[SH_spaceship] = shop_object(
			Text("%s%x%hShield of the spaceship. Protects against damage.%nWhen it becomes zero you lose. Keep track of shield.",
				dot(50, 80), 1.f, SP_shield, .4f));
		Shop_objects[SH_blaster] = shop_object(Text("%s%x%hThe Blaster is the base weapon. He has an average%nrate of fire and damage.",
			 dot(50, 80), 1.f, SP_blaster, .4f));
	}
}

using namespace Game_Object;