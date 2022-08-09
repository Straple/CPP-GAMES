namespace thr {
	struct thread_safe_variable {
		u64 var;
		mutex locker;

		thread_safe_variable() {
			var = 0;
		}
		thread_safe_variable(u64 _var) {
			var = _var;
		}
		void operator ++ (int) {
			locker.lock();

			var++;

			locker.unlock();
		}
		u64 operator * () {
			locker.lock();

			u64 ret = var;
			var++;

			locker.unlock();

			return var;
		}
	};

	global_variable thread_safe_variable draw_id = 1;

	enum render_type {
		RT_draw,
		RT_draw_text,
		RT_draw_number,
		RT_draw_sprite,
		RT_draw_rectangle,
		RT_draw_rect,
		RT_clear_screen,

		RT_none,

		RT_COUNT,
	};
	struct render_task {
		string text;
		dot pos0, pos1;
		float size0, size1;
		u32 color;
		bool is_align;
		int number;
		sprite_type sprite;
		std::list<int> List;

		render_type rt;
		u64 draw_id;

		render_task(render_type _rt, string _text, dot _pos0, dot _pos1, float _size0,
			float _size1, u32 _color, bool _is_align, int _number, sprite_type _sp, std::list<int> &_List, u64 _draw_id) {
			rt = _rt;
			text = _text;
			pos0 = _pos0;
			pos1 = _pos1;
			size0 = _size0;
			size1 = _size1;
			color = _color;
			is_align = _is_align;
			number = _number;
			sprite = _sp;

			List = _List;
			
			draw_id = _draw_id;
		}
		render_task() {
			rt = RT_none;
		}

		void solve() {
			switch (rt) {
			case RT_clear_screen: {
				Render_realization::clear_screen(color, draw_id);
			}break;
			case RT_draw_rect: {
				Render_realization::draw_rect(pos0, size0, size1, color, draw_id);
			}break;
			case RT_draw_rectangle: {
				Render_realization::draw_rectangle(pos0, pos1, color, draw_id);
			}break;
			case RT_draw_sprite: {
				Render_realization::draw_sprite(pos0, size0, sprite, draw_id);
			}break;
			case RT_draw_number: {
				Render_realization::draw_number(number, pos0, size0, color, draw_id);
			}break;
			case RT_draw_text: {
				Render_realization::draw_text(text.c_str(), pos0, size0, color, is_align, draw_id);
			}break;
			case RT_draw: {
				Render_realization::draw(text.c_str(), pos0, size0, draw_id, List);
			}break;
			}
		}
	};


	template<typename T>
	struct container {
		mutex locker;
		queue<T> Q;
		T front() {
			locker.lock();
			T ret = Q.front();
			locker.unlock();

			return ret;
		}
		T get() {
			locker.lock();

			T ret;
			if (!Q.empty()) {
				ret = Q.front();
				Q.pop();
			}

			locker.unlock();
			return ret;
		}
		void push(T element) {
			locker.lock();
			Q.push(element);
			locker.unlock();
		}
		bool empty() {
			locker.lock();
			bool ret = Q.empty();
			locker.unlock();
			return ret;
		}
	};

	container<render_task> Tasks;

	namespace Render {
		/*fills the screen with color*/
		void clear_screen(u32 color) {
			string str;
			std::list<int> list;
			thr::Tasks.push(render_task(RT_clear_screen, str, dot(), dot(), 0.f, 0.f, color, true, 10, SP_COUNT, list, *draw_id));
		}
		void draw_rect(dot pos, float half_size_x, float half_size_y, u32 color) {
			std::list<int> list;
			string str;
			thr::Tasks.push(render_task(RT_draw_rect, str, pos, dot(), half_size_x, half_size_y, color, true, 10, SP_COUNT, list, *draw_id));
		}
		/*
			a - top left dot
			b - down right dot
			a----+
			|	 |
			+----b
		*/
		void draw_rectangle(dot a, dot b, u32 color) {
			std::list<int> list;
			string str;
			thr::Tasks.push(render_task(RT_draw_rectangle, str, a, b, 0.f, 0.f, color, true, 10, SP_COUNT, list, *draw_id));
		}
		void draw_sprite(dot pos, float size, sprite_type sprite) {
			std::list<int> list;
			string str;
			thr::Tasks.push(render_task(RT_draw_sprite, str, pos, dot(), size, 0.f, 0xff, true, 10, sprite, list, *draw_id));
		}
		// draw number. Example: -1409
		void draw_number(int number, dot pos, float size, u32 color) {
			std::list<int> list;
			string str;
			thr::Tasks.push(render_task(RT_draw_number, str, pos, dot(), size, 0.f, color, true, number, SP_COUNT, list, *draw_id));
		}
		/* if is_align == true: then align center on x
	       else no align
		*/
		void draw_text(const char* text, dot pos, float size, u32 color, bool is_align) {
			string txt = text;
			std::list<int> list;
			thr::Tasks.push(render_task(RT_draw_text, txt, pos, dot(), size, 0.f, color, is_align, 10, SP_COUNT, list, *draw_id));
		}
		/*
		Специальные символы, которым нужны входные данные:
		%c - color, задает цвет для всех следующих букв и цифр
		%s - sprite, рисует спрайт
		%d - рисует число
		%h - half_size - символ обновления size
		XXXXXXXXXXXXXXXXXXXXXXX
		Специальные символы, которым НЕ нужны входные данные:
		%n - символ перевода строки
		%x - new original_x - символ присвоения original_x к текущему x
		*/
		void draw(const char* text, dot pos, float default_size, ...) {
			string txt = text;
			va_list Arguments;         //указатель va_list
			va_start(Arguments, default_size);    // устанавливаем указатель
			std::list<int> List;

			{
				for (size_t i = 0; i < strlen(text); i++) {
					if (text[i] == '%') {
						i++;
						switch (text[i]) {
						case 'c': {
							List.push_back(va_arg(Arguments, u32));
						}break;
						case 's': {
							List.push_back(va_arg(Arguments, sprite_type));
						}break;
						case 'd': {
							List.push_back(va_arg(Arguments, int));
						}break;
						case 'h': {
							double h = va_arg(Arguments, double);
							List.push_back(h * 100.f);
						}
						}
					}
				}
			}
			va_end(Arguments);
			thr::Tasks.push(render_task(RT_draw, txt, pos, dot(), default_size, 0.f, 0xff, true, 10, SP_COUNT, List, *draw_id));
		}
	}

	void join() {
		if (running) {
			while (!Tasks.empty()) {}
			for (int i = 0; i < current_thread; i++) {
				while (Thread_busy[i]) {}
			}
		}
	}

	void simulate_thread(size_t thread_id) {
		render_task task;
		while (running) {
			task = Tasks.get();
			if (task.rt != RT_none) {
				Thread_busy[thread_id] = true;
				task.solve();
			}
			else {
				Thread_busy[thread_id] = false;
			}
		}
	}

	void simulate_render_thread() {
		thr::render_task task(thr::Tasks.get());
		while (task.rt != thr::RT_none) {
			task.solve();
			task = thr::Tasks.get();
		}
		thr::join();
	}
}
using namespace thr::Render;