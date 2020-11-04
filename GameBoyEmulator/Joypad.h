#pragma once
#include <cstdint>
class Bus;

class Joypad
{
public:
	enum class BUTTONS {
		UP,
		DOWN,
		LEFT,
		RIGHT,

		A,
		B,
		SELECT,
		START
	};
	Joypad();
	void reset();
	~Joypad();

	
	void write(uint8_t data);

	uint8_t read();
	
	void press_button(BUTTONS button);
	void release_button(BUTTONS button);

	void connectBus(Bus* bus);

	bool get_key_state(BUTTONS button);
private:
	void toggle_button(BUTTONS button, bool val);

	bool button_keys;
	bool direction_keys;
	Bus* bus;
	
	struct{
		uint8_t up : 1;
		uint8_t down : 1;
		uint8_t left : 1;
		uint8_t right : 1;

		uint8_t A : 1;
		uint8_t B : 1;
		uint8_t select : 1;
		uint8_t start : 1;
	} buttons_state;
};

