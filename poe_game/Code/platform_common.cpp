
struct Button_State { // ������ � ������������
	bool is_down;
	bool changed;
};

enum { // ������
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_W,
	BUTTON_S,
	BUTTON_A,
	BUTTON_D,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_ENTER,
	BUTTON_ESC,
	BUTTON_TAB,
	BUTTON_SPACE,
	BUTTON_MOUSE_L,

	BUTTON_COUNT, // Should be the last item
};

struct Input {
	Button_State buttons[BUTTON_COUNT];
	Point Mouse;
};