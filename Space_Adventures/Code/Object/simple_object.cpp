namespace Simple_Object {
	struct dot {
		float x, y;

		dot() {
			x = y = 0;
		}
		dot(float _x, float _y) {
			x = _x;
			y = _y;
		}

		dot operator + (const dot& Rhs) const {
			return dot(x + Rhs.x, y + Rhs.y);
		}
		dot operator - (const dot& Rhs) const {
			return dot(x - Rhs.x, y - Rhs.y);
		}
		bool operator == (const dot& Rhs)const {
			return x == Rhs.x && y == Rhs.y;
		}
		void operator *= (float multi) {
			x *= multi;
			y *= multi;
		}
	};
	std::ostream& operator << (std::ostream& output, const dot& point) {
		return output << point.x << " " << point.y;
	}
	std::istream& operator >> (std::istream& input, dot& point) {
		return input >> point.x >> point.y;
	}

	struct int_dot {
		int x, y;
		int_dot() {
			x = y = 0;
		}
		int_dot(int _x, int _y) {
			x = _x;
			y = _y;
		}

		int_dot operator + (const int_dot& Rhs) const {
			return int_dot(x + Rhs.x, y + Rhs.y);
		}
		int_dot operator - (const int_dot& Rhs) const {
			return int_dot(x - Rhs.x, y - Rhs.y);
		}
		bool operator == (const int_dot& Rhs)const {
			return x == Rhs.x && y == Rhs.y;
		}
	};

	struct sprite {
		unsigned int** picture;
		bool** used;
		int height, width;
		dot len;

		sprite() {

		}
		sprite(string file_path) {
			ifstream input(file_path);
			input >> height >> width;
			len = dot(width * 0.5f, height * 0.5f);

			picture = new unsigned int* [height];
			used = new bool* [height];
			for (int i = 0; i < height; i++) {
				picture[i] = new unsigned int[width];
				used[i] = new bool[width];
				for (int j = 0; j < width; j++) {
					long long pixel; 
					input >> pixel;
					if (pixel == -1) { // пиксель прозрачный
						used[i][j] = false;
					}
					else {
						used[i][j] = true;
						picture[i][j] = pixel;
					}
				}
			}
		}
	};

	struct Render_State {
		int height, width;
		void* memory; // pixels
		void* drawing_id;

		BITMAPINFO bitmap_info;
	};

	struct collision {
		/*  a----d
			|	 |
			|    |
			|    |
			c----b
		*/
		dot a, b;

		bool trigger(const dot& pt) {
			return is_between(a.x, pt.x, b.x) &&
				   is_between(b.y, pt.y, a.y);
		}
	};

	internal void simulate_move2d(dot& p, dot& dp, dot ddp, float dt) {
		simulate_move(p.x, dp.x, ddp.x, dt);
		simulate_move(p.y, dp.y, ddp.y, dt);
	}
}
using namespace Simple_Object;