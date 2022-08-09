
struct Button_State { // Õ¿∆¿“¿ » »—œŒÀ‹«”≈“—ﬂ
	bool is_down;
	bool changed;
};

enum { //  ÕŒœ »
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_ENTER,
	BUTTON_ESC,
	BUTTON_TAB,
	BUTTON_SPACE,
	BUTTON_MOUSE_L,
	BUTTON_MOUSE_R,
	BUTTON_DEL,

	BUTTON_A,
	BUTTON_B,
	BUTTON_C,
	BUTTON_D,
	BUTTON_E,
	BUTTON_F,
	BUTTON_W,
	BUTTON_S,

	BUTTON_0,
	BUTTON_1,
	BUTTON_2,
	BUTTON_3,
	BUTTON_4,
	BUTTON_5,
	BUTTON_6,
	BUTTON_7,
	BUTTON_8,
	BUTTON_9,

	BUTTON_COUNT, // Should be the last item
};

struct Input {
	Button_State buttons[BUTTON_COUNT];
	Point Mouse;
};