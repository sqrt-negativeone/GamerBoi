#pragma once
#include <cstdint>
namespace GamerBoi {

	class Channel
	{
	public:
		Channel() :
			is_enable(false),
			timer(0),
			dac_enable(false)
		{
		}
		virtual uint8_t read(uint8_t reg_num) = 0;
		virtual void write(uint8_t reg_num, uint8_t data) = 0;
		virtual uint8_t clock() = 0;
		virtual void clock_length() = 0;
		bool Is_enable() {
			return is_enable;
		}
		virtual void Disable() = 0;
		virtual void reset() = 0;
	protected:
		virtual void trigger() = 0;
		bool is_enable;
		bool dac_enable;
		int timer;
	};

}