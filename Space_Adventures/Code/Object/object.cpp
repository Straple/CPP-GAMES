namespace Object {

	// спрайт мыши и указатель
	struct Mouse {
		sprite_type sp, focus_sp;
		float size;
		dot pos;
		bool focus;
		void draw() {
			if (!focus) {
				draw_sprite(dot(pos.x, pos.y - Sprites[sp].len.y * size), size, sp);
			}
			else {
				draw_sprite(dot(pos.x - 1.2f, pos.y - Sprites[focus_sp].len.y * size), size, focus_sp);
			}
			focus = false;

			if (debug) {
				draw_rect(pos, .35f, .35f, 0xff0000);
			}
		}
		Mouse() {}
		Mouse(float _size, sprite_type _sp, sprite_type _focus_sp) {
			size = _size;
			sp = _sp;
			focus_sp = _focus_sp;
			focus = false;
		}
	};

	// спрайт с колизией
	struct object {
		sprite_type sp_type;
		collision coll;
		dot draw_position;
		float size;

		void new_pos(dot pos) {
			draw_position = pos;
			float x_len = abs(coll.a.x - coll.b.x), y_len = abs(coll.a.y - coll.b.y);
			coll.a = dot(pos.x, pos.y + y_len);
			coll.b = dot(pos.x + x_len, pos.y);
		}
		object() {
		}
		object(sprite_type type, dot pos, float _size) {
			size = _size;
			sp_type = type;
			draw_position = pos;
			coll.a = dot(pos.x, pos.y + Sprites[type].len.y * size);
			coll.b = dot(pos.x + Sprites[type].len.x * size, pos.y);
		}
		void draw() {
			draw_sprite(draw_position, size, sp_type);

			if (debug) {
				draw_rect(coll.a, .5f, .5f, 0xff0000);
				draw_rect(coll.b, .5f, .5f, 0xff0000);
			}
		}
		bool trigger(dot pt) {
			return coll.trigger(pt);
		}
		bool trigger(const Mouse &mouse) {
			return coll.trigger(mouse.pos);
		}
		void update_move(float dt, float &dp, float ddp, bool is_x) {
			ddp -= dp * 15.f;
			float plus = dp * dt + ddp * dt * dt * .5f;
			if (is_x) {
				coll.a.x += plus;
				coll.b.x += plus;
				draw_position.x += plus;
			}
			else {
				coll.a.y += plus;
				coll.b.y += plus;
				draw_position.y += plus;
			}
			dp += ddp * dt;
		}
		void update_move2d(float dt, dot &dp, dot ddp) {
			update_move(dt, dp.x, ddp.x, true);
			update_move(dt, dp.y, ddp.y, false);
		}
	};
	std::ostream& operator << (std::ostream& output, const object& obj) {
		return output << obj.sp_type << " " << obj.coll.a << " " << obj.coll.b << " " << obj.draw_position << " " << obj.size;
	}
	std::istream& operator >> (std::istream& input, object& obj) {
		int sp;
		input >> sp;
		obj.sp_type = (sprite_type)sp;
		return input >> obj.coll.a >> obj.coll.b >> obj.draw_position >> obj.size;
	}

	// обьект с функциями слайдера
	struct sprite_slider {
		object obj;
		float delta_x;
		bool focus;
		float left, right;

		void update_focus(const Mouse& mouse) {
			if (this->trigger(mouse)) {
				delta_x = obj.coll.a.x - mouse.pos.x;
				focus = true;
			}
		}
		void update(bool is_down, Mouse mouse, float& volume) {
			if (!is_down) {
				focus = false;
			}
			if (focus) {
				volume = this->new_pos(dot(mouse.pos.x + delta_x, obj.draw_position.y));
				update_musics_volume();
			}
		}
		float new_pos(dot pos) {
			pos.x = my_clamp(left, pos.x, right);
			obj.new_pos(pos);
			float length = right - left;
			float slider_len = obj.coll.a.x - left;
			return slider_len / length;
		}
		sprite_slider() {

		}
		sprite_slider(sprite_type type, dot pos, float _size, float _left, float _right, float volume) {
			obj = object(type, pos, _size);
			left = _left;
			right = _right - abs(obj.coll.a.x - obj.coll.b.x);
			focus = false;
			delta_x = 0;

			float length = right - left;
			pos.x += length * volume;
			obj = object(type, pos, _size);
		}
		void draw() {
			obj.draw();
		}
		bool trigger(Mouse mouse) {
			return obj.trigger(mouse);
		}
	}; 
	// Not used

	// полоса с функциями слайдера
	struct slider {
		bool focus;
		float left, right;
		dot a, b;
		/* a------------+
		   |            |
		   +------------b
		*/

		void update_focus(const Mouse& mouse) {
			focus = this->trigger(mouse);
		}
		void simulate_slider(bool is_pressed, bool is_down, const Mouse& mouse, float& volume) {
			if (is_pressed) {
				update_focus(mouse);
			}
			if (!is_down) {
				focus = false;
			}
			if (focus) {
				volume = this->new_pos(mouse.pos.x);
				update_musics_volume();
			}
			draw();
		}
		float new_pos(float p) {
			b.x = my_clamp(left, p, right);
			float length = right - left;
			float slider_len = b.x - left;
			return slider_len / length;
		}
		slider() {

		}
		slider(dot _a, dot _b, float volume) {
			a = _a;
			b = dot(_a.x, _b.y);
			left = _a.x;
			right = _b.x;
			focus = false;

			float length = right - left;
			b.x += length * volume;
		}
		void draw() {
			draw_rectangle(a, dot(right, b.y), 0x474747);
			draw_rectangle(a, b, 0xffffff);

			if (debug) {
				draw_rect(a, .5f, .5f, 0xff0000);
				draw_rect(b, .5f, .5f, 0xff0000);
			}
		}
		bool trigger(const Mouse& mouse) {
			return is_between(a.x, mouse.pos.x, right) && is_between(b.y, mouse.pos.y, a.y);
		}
	};

	// текст с позицией и листом переменных для вывода с помощью draw()
	struct Text {
		string text;
		dot pos;
		float size;
		std::list<int> List;
		Text() {

		}
		Text(const char* _text, dot _pos, float _size, ...) {
			text = _text;
			size = _size;
			pos = _pos;
			va_list Args;
			va_start(Args, _size);

			while (*_text) {
				if (*_text == '%') {
					_text++;
					switch (*_text) {
						case 'c': {
							List.push_back(va_arg(Args, u32));
						}break;
						case 's': {
							List.push_back(va_arg(Args, sprite_type));
						}break;
						case 'd': {
							List.push_back(va_arg(Args, int));
						}break;
						case 'h': {
							List.push_back(va_arg(Args, double) * 100.f);
						}break;
					}
				}
				_text++;
			}
			va_end(Args);
		}
		void draw_Text() {
			string txt = text;
			thr::Tasks.push(thr::render_task(thr::RT_draw, txt, pos, dot(), size, 0.f, 0xff, true, 10, SP_COUNT, List, *thr::draw_id));
		}
	};

	// текст с колизией
	struct button { // кнопка
		const char* text;
		dot pos; // центр текста по x
		float size;
		collision coll;
		u32 color, // usual color
			focus_color; // color when the button doesn't focus
		bool is_align, is_focus;

		button() {

		}
		button(const char* _text, dot _pos, float _size, u32 _color, u32 _focus_color, bool align) {
			is_align = align;
			is_focus = false;
			text = _text;
			pos.x = _pos.x + 0.5f * _size;
			pos.y = _pos.y - 0.5f * _size;
			size = _size;
			color = _color;
			focus_color = _focus_color;
			// !создать коллизию!
			{
				int len = Render_realization::text_len(_text);

				coll.a = dot(_pos.x + (align ? -(size * len) *.5f : 0), _pos.y);
				coll.b.x = _pos.x + size * len - size + (align ? -(size * len) *.5f : 0);
				coll.b.y = _pos.y - size * 6 - size;
			}
		}

		void draw() {
			draw_text(text, pos, size, (is_focus ? focus_color : color), is_align);

			if (debug) {
				draw_rect(coll.a, 0.5, 0.5, 0x00ff00);
				draw_rect(coll.b, 0.5, 0.5, 0x00ff00);
			}
		}
		void update_focus(Mouse& mouse) {
			if (coll.trigger(mouse.pos)) {
				mouse.focus = true;
				if (!is_focus) {
					UI[UI_focus].play();
				}
				is_focus = true;
			}
			else {
				is_focus = false;
			}
		}
		// обновление фокуса и вывод
		void simulate_button(Mouse& mouse) {
			update_focus(mouse);
			draw();
		}
	};
}

using namespace Object;