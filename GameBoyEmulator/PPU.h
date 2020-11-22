#pragma once
#include <vector>
#include <functional>
using std::vector;

class GameBoy;
namespace GamerBoi {
	class Bus;
	class PPU
	{
	public :
		struct Sprite {
			union {
				uint8_t attribute_falg;
				//working on a little endian machine
				struct {
					uint8_t unused : 4; // not used in DMG
					uint8_t palette_num : 1;//Non CGB
					uint8_t xFlip : 1;
					uint8_t yFlip : 1;
					uint8_t obj_to_bg_priority : 1;
				};
			};
			uint8_t tileNumber;
			uint8_t xPos;
			uint8_t yPos;
			bool operator<(const Sprite& rhs) const {return this->xPos < rhs.xPos; }
		};

		PPU();

		uint8_t screen[144][160][3];
		void clock(uint8_t cycles);

	
	
		void connectBus(Bus* bus);
		void reset();

		uint8_t vram[0x2000];
		uint8_t oam[160];
		//read and write to the VRAM or OAM tables
		uint8_t read(uint16_t addr);
		void write(uint16_t addr, uint8_t data);
		unsigned int frame_count;
		uint8_t get_mode();
		uint8_t get_coincidence();
		void turnedOff();
		void SetFrameCompleteCallback(std::function<void()> callback) {
			frameCompleted_callback = callback;
		}
		uint8_t read_register(uint16_t addr);
		void write_register(uint16_t addr, uint8_t data);
	private:
		union {
			uint8_t val;
			struct {
				uint8_t mode_flag : 2;
				uint8_t coincidence_flag : 1;
				uint8_t hblank_interrupt : 1;
				uint8_t vblank_interrupt : 1;
				uint8_t oam_interrupt : 1;
				uint8_t coincidence_interrupt : 1;
				uint8_t _ : 1;
			};
		} STAT_reg;
		union {
			uint8_t val;
			struct {
				uint8_t bg_display : 1;
				uint8_t OBJ_display : 1;
				uint8_t OBJ_size : 1;
				uint8_t BG_tilemap : 1;
				uint8_t BG_WD_tiledata : 1;
				uint8_t WD_display : 1;
				uint8_t WD_tilemap : 1;
				uint8_t lcd_on : 1;
			};
		} LCDC_reg;
		uint8_t curr_scanline;
		enum class STATE {
			HBLANK,
			VBLANK,
			OAM_SEARCH,
			LCD_TRANSFERE
		};
		enum class ADRESSES
		{
			STAT = 0xFF41, //stat register (R/W)
			LCDC = 0XFF40, //LCD Control register (R/W)
			SCY = 0xFF42, // scroll Y (R/W)
			SCX = 0xFF43, // scroll X (R/W)
			LY = 0xFF44, // LCD current Scanline (R)
			LYC = 0xFF45, // LY compare (R/W)
			winY = 0xFF4A, // window Y position 
			winX = 0xFF4B,// window X position (+7 so substruct 7 to get the X pos)
			BGP = 0xFF47,
			OBP0 = 0xFF48,
			OBP1 = 0xFF49
		};
		struct {
			uint8_t background;
			uint8_t ob0;
			uint8_t ob1;
		} palettes;
		uint8_t SCY;
		uint8_t SCX;
		uint8_t LYC;
		uint8_t winY;
		uint8_t winX;

		const uint8_t HBLANK_DURATION = 51;
		const uint8_t LCD_TRANSFER_DURATION = 43;
		const uint8_t OAM_SEARCH_DURATION = 20;
		const uint8_t SCANLINE_DURATION = HBLANK_DURATION + LCD_TRANSFER_DURATION + OAM_SEARCH_DURATION;
		const uint16_t VBLANK_DURATION = 10 * SCANLINE_DURATION;
		const uint8_t VBLANK_START = 144;
		enum class COLOR {
			WHITE,
			LIGHT_GRAY,
			DARK_GRAY,
			BLACK,
			TRANSPARANT
		};
		STATE mode;
		//uint8_t coincidenceFlag;
		uint16_t clock_cnt;
		void drawScanline();

		std::function<void()> frameCompleted_callback;

		Bus* bus;

		COLOR lineBuffer[160];
		vector<Sprite> spriteBuffer;

		void doOAM_search();
		void drawBackground();
		void drawWindow();
		void drawSprite();
		void inc_LY();
		bool isOff;
	};

}
