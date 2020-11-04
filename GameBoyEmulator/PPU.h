#pragma once
#include <SDL.h>
#include <vector>
using std::vector;
#undef main
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
private:
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
	uint16_t STAT = 0xFF41; //stat register (R/W)
	uint16_t LCDC = 0XFF40; //LCD Control register (R/W)
	uint16_t SCY = 0xFF42; // scroll Y (R/W)
	uint16_t SCX = 0xFF43; // scroll X (R/W)
	uint16_t LY = 0xFF44; // LCD current Scanline (R)
	uint16_t LYC = 0xFF45; // LY compare (R/W)
	uint16_t winY = 0xFF4A; // window Y position 
	uint16_t winX = 0xFF4B; // window X position (+7 so substruct 7 to get the X pos)

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
	uint8_t coincidenceFlag;
	uint16_t clock_cnt;
	void drawScanline();

	SDL_Renderer* renderer;
	SDL_Window* window;
	SDL_Texture* texture;

	Bus* bus;

	COLOR lineBuffer[160];
	vector<Sprite> spriteBuffer;

	void doOAM_search();
	void drawBackground();
	void drawWindow();
	void drawSprite();

	void render();
	bool isOff;
};

