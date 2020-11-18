#include "Joypad.h"
#include "Bus.h"
namespace GamerBoi {
	Joypad::Joypad() {
		reset();
	}

	void Joypad::reset()
	{
		button_keys = false;
		direction_keys = false;

		release_button(BUTTONS::A);
		release_button(BUTTONS::B);
		release_button(BUTTONS::SELECT);
		release_button(BUTTONS::START);
		release_button(BUTTONS::LEFT);
		release_button(BUTTONS::RIGHT);
		release_button(BUTTONS::UP);
		release_button(BUTTONS::DOWN);
	}

	Joypad::~Joypad() {

	}
	void Joypad::connectBus(Bus* bus) {
		this->bus = bus;
	}
	void Joypad::write(uint8_t data) {
		button_keys = (~data & (1 << 5));
		direction_keys = (~data & (1 << 4));
	}
	void Joypad::press_button(BUTTONS button) {
		toggle_button(button, true);
	}

	void Joypad::release_button(BUTTONS button) {
		toggle_button(button, false);
	}

	void Joypad::toggle_button(BUTTONS button, bool val) {
		switch (button)
		{
		case Joypad::BUTTONS::UP:
			if (!buttons_state.up && val) {
				bus->interrupt_req(JOYPAD_INTERRUPT);
			}
			buttons_state.up = val;
			break;
		case Joypad::BUTTONS::DOWN:
			if (!buttons_state.down && val) {
				bus->interrupt_req(JOYPAD_INTERRUPT);
			}
			buttons_state.down = val;
			break;
		case Joypad::BUTTONS::LEFT:
			if (!buttons_state.left && val) {
				bus->interrupt_req(JOYPAD_INTERRUPT);
			}
			buttons_state.left = val;
			break;
		case Joypad::BUTTONS::RIGHT:
			if (!buttons_state.right && val) {
				bus->interrupt_req(JOYPAD_INTERRUPT);
			}
			buttons_state.right = val;
			break;
		case Joypad::BUTTONS::A:
			if (!buttons_state.A && val) {
				bus->interrupt_req(JOYPAD_INTERRUPT);
			}
			buttons_state.A = val;
			break;
		case Joypad::BUTTONS::B:
			if (!buttons_state.B && val) {
				bus->interrupt_req(JOYPAD_INTERRUPT);
			}
			buttons_state.B = val;
			break;
		case Joypad::BUTTONS::SELECT:
			if (!buttons_state.select && val) {
				bus->interrupt_req(JOYPAD_INTERRUPT);
			}
			buttons_state.select = val;
			break;
		case Joypad::BUTTONS::START:
			if (!buttons_state.start && val) {
				bus->interrupt_req(JOYPAD_INTERRUPT);
			}
			buttons_state.start = val;
			break;
		default:
			break;
		}
	}

	uint8_t Joypad::read() {
		uint8_t res = 0xFF;

		if (button_keys) {
			res &= ~(buttons_state.A * (1 << 0));
			res &= ~(buttons_state.B * (1 << 1));
			res &= ~(buttons_state.select * (1 << 2));
			res &= ~(buttons_state.start * (1 << 3));
			res &= ~(button_keys << 5);
		}
		if (direction_keys) {
			res &= ~(buttons_state.right * (1 << 0));
			res &= ~(buttons_state.left * (1 << 1));
			res &= ~(buttons_state.up * (1 << 2));
			res &= ~(buttons_state.down * (1 << 3));
			res &= ~(direction_keys << 4);
		}

		return res;
	}
	bool Joypad::get_key_state(BUTTONS button) {
		switch (button)
		{
		case Joypad::BUTTONS::UP:
			return buttons_state.up;
			break;
		case Joypad::BUTTONS::DOWN:
			return buttons_state.down;
			break;
		case Joypad::BUTTONS::LEFT:
			return buttons_state.left;
			break;
		case Joypad::BUTTONS::RIGHT:
			return buttons_state.right;
			break;
		case Joypad::BUTTONS::A:
			return buttons_state.A;
			break;
		case Joypad::BUTTONS::B:
			return buttons_state.B;
			break;
		case Joypad::BUTTONS::SELECT:
			return buttons_state.select;
			break;
		case Joypad::BUTTONS::START:
			return buttons_state.start;
			break;
		default:
			break;
		}
	}
}