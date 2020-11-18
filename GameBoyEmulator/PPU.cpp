#include "PPU.h"
#include "Bus.h"
#include <algorithm>
namespace GamerBoi {

	PPU::PPU() :
		SCY{ 0 },
		SCX{ 0 },
		curr_scanline{ 0 },
		LYC{ 0 },
		winX{ 0 },
		winY{ 0 }
	{
		palettes.background = 0xFC;
		palettes.ob0 = 0xFF;
		palettes.ob1 = 0xFF;

		LCDC_reg.val = 0x00;
		clock_cnt = 0;
		mode = STATE::VBLANK;
		frame_count = 0;

	}
	void PPU::connectBus(Bus* bus) {
		this->bus = bus;
		reset();
	}
	uint8_t PPU::get_mode() {
		return static_cast<int>(this->mode);
	}
	uint8_t PPU::get_coincidence() {
		return this->STAT_reg.coincidence_flag;
	}
	uint8_t PPU::read(uint16_t addr) {
		if (0x8000 <= addr && addr < 0xA000) {//VRAM read
			if (mode != STATE::LCD_TRANSFERE) {
				return vram[addr - 0x8000];
			}
			return 0xFF;
		}
		else if (0xFE00 <= addr && addr < 0xFEA0) {//OAM read
			if (mode == STATE::HBLANK || mode == STATE::VBLANK) {
				return oam[addr - 0xFE00];
			}
			return 0xFF;
		}
	}
	void PPU::write(uint16_t addr, uint8_t data) {
		if (0x8000 <= addr && addr < 0xA000) {//VRAM write
			if (mode != STATE::LCD_TRANSFERE) {
				vram[addr - 0x8000] = data;
			}
		}
		else if (0xFE00 <= addr && addr < 0xFEA0) {//OAM write
			if (mode == STATE::HBLANK || mode == STATE::VBLANK) {
				oam[addr - 0xFE00] = data;
			}
		}
	}
	uint8_t PPU::read_register(uint16_t addr) {
		auto ADDR = static_cast<ADRESSES>(addr);
		switch (ADDR)
		{
		case GamerBoi::PPU::ADRESSES::STAT:
			STAT_reg.mode_flag = static_cast<int>(this->mode);
			return STAT_reg.val;
		case GamerBoi::PPU::ADRESSES::LCDC:
			return LCDC_reg.val;
		case GamerBoi::PPU::ADRESSES::SCY:
			return SCY;
		case GamerBoi::PPU::ADRESSES::SCX:
			return SCX;
		case GamerBoi::PPU::ADRESSES::LY:
			return curr_scanline;
		case GamerBoi::PPU::ADRESSES::LYC:
			return LYC;
		case GamerBoi::PPU::ADRESSES::winY:
			return winY;
		case GamerBoi::PPU::ADRESSES::winX:
			return winX;
		case GamerBoi::PPU::ADRESSES::BGP:
			return palettes.background;
		case GamerBoi::PPU::ADRESSES::OBP0:
			return palettes.ob0;
		case GamerBoi::PPU::ADRESSES::OBP1:
			return palettes.ob1;
		default:
			break;
		}
	}
	void PPU::write_register(uint16_t addr, uint8_t data) {
		auto ADDR = static_cast<ADRESSES>(addr);
		switch (ADDR)
		{
		case GamerBoi::PPU::ADRESSES::STAT:
			STAT_reg.hblank_interrupt = ((data & (1 << 3)) > 0);
			STAT_reg.vblank_interrupt = ((data & (1 << 4)) > 0);
			STAT_reg.oam_interrupt = ((data & (1 << 5)) > 0);
			STAT_reg.coincidence_interrupt = ((data & (1 << 6)) > 0);
			break;
		case GamerBoi::PPU::ADRESSES::LCDC:
			LCDC_reg.val = data;
			if (LCDC_reg.bg_display == 0) LCDC_reg.WD_display = 0;
			if (!LCDC_reg.lcd_on && !isOff) {
				curr_scanline = 0;
				turnedOff();
			}
			break;
		case GamerBoi::PPU::ADRESSES::SCY:
			SCY = data;
			break;
		case GamerBoi::PPU::ADRESSES::SCX:
			SCX = data;
			break;
		case GamerBoi::PPU::ADRESSES::LY:
			curr_scanline = 0;
			break;
		case GamerBoi::PPU::ADRESSES::LYC:
			LYC = data;
			break;
		case GamerBoi::PPU::ADRESSES::winY:
			winY = data;
			break;
		case GamerBoi::PPU::ADRESSES::winX:
			winX = data;
			break;
		case GamerBoi::PPU::ADRESSES::BGP:
			palettes.background = data;
			break;
		case GamerBoi::PPU::ADRESSES::OBP0:
			palettes.ob0 = data;
			break;
		case GamerBoi::PPU::ADRESSES::OBP1:
			palettes.ob1 = data;
			break;
		default:
			break;
		}
	}
	void PPU::turnedOff() {
		if (isOff) return;
		isOff = true;
		
		STAT_reg.coincidence_flag = (curr_scanline == LYC);

		for (int i = 0; i < 144; i++) {
			for (int j = 0; j < 160; j++) {
				for (int k = 0; k < 3; k++) {
					screen[i][j][k] = 0xFF;
				}
			}
		}

	}
	void PPU::inc_LY() {
		++curr_scanline %= 154;
	}
	bool PPU::clock(uint8_t cycles) {
		bool frame_complete = false;
		uint8_t req = 0;
		if (!LCDC_reg.lcd_on) {
			clock_cnt = 0;
			mode = STATE::HBLANK;
			return frame_complete;
		}
		isOff = false;
		clock_cnt += cycles;
		switch (mode)
		{
		case PPU::STATE::HBLANK:
			if (clock_cnt >= HBLANK_DURATION) {
				drawScanline();
				clock_cnt -= HBLANK_DURATION;
				inc_LY();
				if (curr_scanline == LYC) {
					STAT_reg.coincidence_flag = 1;
					if (STAT_reg.coincidence_interrupt) {// if LYC=LC interrupt enable
						req |= LCD_STAT_INTERRUPT;
					}
				}
				else {
					STAT_reg.coincidence_flag = 0;
				}
				if (curr_scanline == VBLANK_START) {
					mode = STATE::VBLANK;
					//request V-Blank interrupt
					req |= VBLANK_INTERRUPT;
					if (STAT_reg.vblank_interrupt) {
						req |= LCD_STAT_INTERRUPT;
					}

				}
				else {
					mode = STATE::OAM_SEARCH;
					doOAM_search();
					if (STAT_reg.oam_interrupt) {
						req |= LCD_STAT_INTERRUPT;
					}
				}
			}
			break;
		case PPU::STATE::VBLANK:
			if (clock_cnt >= SCANLINE_DURATION) {
				clock_cnt -= SCANLINE_DURATION;
				inc_LY();
				if (curr_scanline == LYC) {
					STAT_reg.coincidence_flag = 1;
					if (STAT_reg.coincidence_interrupt) {
						req |= LCD_STAT_INTERRUPT;
					}
				}
				else {
					STAT_reg.coincidence_flag = 0;
				}
				if (curr_scanline == 0) {
					//render to screen
					if (frameCompleted_callback) frameCompleted_callback();
					frame_complete = true;

					mode = STATE::OAM_SEARCH;
					doOAM_search();
					if (STAT_reg.oam_interrupt) {
						req |= LCD_STAT_INTERRUPT;
					}
				}
			}
			break;
		case PPU::STATE::OAM_SEARCH:
			if (clock_cnt >= OAM_SEARCH_DURATION) {
				clock_cnt -= OAM_SEARCH_DURATION;
				mode = STATE::LCD_TRANSFERE;
			}
			break;
		case PPU::STATE::LCD_TRANSFERE:
			if (clock_cnt >= LCD_TRANSFER_DURATION) {
				clock_cnt -= LCD_TRANSFER_DURATION;
				mode = STATE::HBLANK;
				if (STAT_reg.hblank_interrupt) {
					req |= 2;
				}
			}
			break;
		default:
			break;
		}
		bus->interrupt_req(req);

		return frame_complete;
	}




	void PPU::drawScanline() {
		bool draw = false;
		if (LCDC_reg.bg_display) {
			draw = true;
			drawBackground();
			if (LCDC_reg.WD_display) {
				drawWindow();
			}
		}
		if (LCDC_reg.OBJ_display) {
			draw = true;
			drawSprite();
		}
		if (!draw) return;
		//Actual transfer to lcd
		for (int i = 0; i < 160; i++) {
			switch (lineBuffer[i]) {
			case COLOR::WHITE:
				screen[curr_scanline][i][0] = 0xFF; screen[curr_scanline][i][1] = 0xFF; screen[curr_scanline][i][2] = 0xFF; break;
			case COLOR::LIGHT_GRAY:
				screen[curr_scanline][i][0] = 0xA9; screen[curr_scanline][i][1] = 0xA9; screen[curr_scanline][i][2] = 0xA9; break;
			case COLOR::DARK_GRAY:
				screen[curr_scanline][i][0] = 0x69; screen[curr_scanline][i][1] = 0x69; screen[curr_scanline][i][2] = 0x69; break; //NICE
			case COLOR::BLACK:
				screen[curr_scanline][i][0] = 0; screen[curr_scanline][i][1] = 0; screen[curr_scanline][i][2] = 0; break;
			}
			lineBuffer[i] = COLOR::TRANSPARANT;
		}
	}
	void PPU::drawBackground() {
		uint8_t curr_line = SCY + curr_scanline;
		uint8_t curr_x = SCX;
		uint8_t tileY = curr_line / 8;
		uint8_t line_tiles[32];
		uint16_t tilemap_select = ((LCDC_reg.BG_tilemap) == 0) ? 0x9800 : 0x9C00;
		uint16_t tiledata_base = (LCDC_reg.BG_WD_tiledata) ? 0x8000 : 0x9000;

		for (int i = 0; i < 32; i++) {
			line_tiles[i] = read(tilemap_select + 32 * tileY + i);
		}

		uint8_t tileX = curr_x / 8;//which tile in the line

		uint8_t pixle_count = 0;//how many pixels is drawn until now
		uint8_t curr_row = curr_line % 8;//the row of the tile we currtly drawing
		while (pixle_count < 160) {
			uint8_t tile_id = line_tiles[tileX++];
			if (tileX >= 32) tileX = 0;//if off screen wrap
			uint16_t tile_addr;
			if (tiledata_base == 0x8000) tile_addr = tiledata_base + tile_id * 16 + curr_row * 2;//unsigned base
			else tile_addr = tiledata_base + (int8_t)tile_id * 16 + curr_row * 2;//signed base
			uint8_t byte1 = read(tile_addr++);
			uint8_t byte2 = read(tile_addr);
			uint8_t px_start = curr_x % 8;
			for (int i = px_start; i < 8 && pixle_count < 160; i++, pixle_count++, curr_x++) {
				uint8_t j = 7 - i;
				uint8_t c1 = ((byte1 & (1 << j)) > 0);
				uint8_t c2 = ((byte2 & (1 << j)) > 0);
				uint8_t color_num = ((c2 << 1) | c1);
				COLOR color;
				switch (color_num) {
				case 0: color = static_cast<COLOR>(palettes.background & 0x03); break;
				case 1: color = static_cast<COLOR>(((palettes.background & 0x0C) >> 2)); break;
				case 2: color = static_cast<COLOR>(((palettes.background & 0x30) >> 4)); break;
				case 3: color = static_cast<COLOR>(((palettes.background & 0xC0) >> 6)); break;
				}
				lineBuffer[pixle_count] = color;
			}
		}
	}

	void PPU::drawWindow() {
		uint8_t WX = winX;
		uint8_t WY = winY;

		uint8_t line_tiles[32];
		if (WY <= curr_scanline && WX > 6 && WX <= 166) {//visible line
			uint16_t tilemap_select = (LCDC_reg.WD_tilemap == 0) ? 0x9800 : 0x9C00;
			uint16_t tiledata_base = (LCDC_reg.BG_WD_tiledata) ? 0x8000 : 0x9000;

			uint8_t curr_row = curr_scanline - WY;//the row of the the window we are currently drawing
			uint8_t pixle_count = WX - 7;
			for (int i = 0; i < 32; i++) {
				line_tiles[i] = read(tilemap_select + 32 * curr_row + i);
			}
			uint8_t tileX = 0;
			while (pixle_count < 160) {
				uint8_t tile_id = line_tiles[tileX++];
				uint16_t tile_addr;
				if (tiledata_base == 0x8000) tile_addr = tiledata_base + tile_id * 16 + curr_row * 2;//unsigned base
				else tile_addr = tiledata_base + (int8_t)tile_id * 16 + curr_row * 2;//signed base
				uint8_t byte1 = read(tile_addr++);
				uint8_t byte2 = read(tile_addr);
				for (int i = 0; i < 8 && pixle_count < 160; i++, pixle_count++) {
					uint8_t j = 7 - i;
					uint8_t c1 = ((byte1 & (1 << j)) > 0);
					uint8_t c2 = ((byte2 & (1 << j)) > 0);
					uint8_t color_num = (c2 << 1) | c1;
					COLOR color;
					switch (color_num) {
					case 0: color = static_cast<COLOR>(palettes.background & 0x03); break;
					case 1: color = static_cast<COLOR>(((palettes.background & 0x0C) >> 2)); break;
					case 2: color = static_cast<COLOR>(((palettes.background & 0x30) >> 4)); break;
					case 3: color = static_cast<COLOR>(((palettes.background & 0xC0) >> 6)); break;
					}
					lineBuffer[pixle_count] = color;
				}
			}
		}
	}

	void PPU::drawSprite() {

		uint8_t height = (LCDC_reg.OBJ_size) ? 16 : 8;

		std::pair<COLOR, uint8_t> spriteLine[160];
		std::sort(spriteBuffer.begin(), spriteBuffer.end());

		for (int i = 0; i < 160; i++) {
			spriteLine[i].first = COLOR::TRANSPARANT;
			spriteLine[i].second = 1;
		}
		for (int i = 0; i < spriteBuffer.size(); i++) {
			uint8_t y = spriteBuffer[i].yPos - 16;
			uint8_t x = spriteBuffer[i].xPos - 8;

			uint8_t num = spriteBuffer[i].tileNumber;

			uint8_t row = curr_scanline - y;//the row of the tile that will be rendered in this scanline

			if (spriteBuffer[i].yFlip) {//Y flip
				row = height - row - 1;
			}
			if (height == 16) {// if 8x16 tiles then select the appropriate tile to display
				if (row >= 8) {
					num |= 0x01;
					row -= 8;
				}
				else {
					row &= 0xFE;
				}
			}
			uint16_t addr = 0x8000 + 16 * num + 2 * row;
			uint8_t byte1 = read(addr);
			uint8_t byte2 = read(addr + 1);
			uint8_t palette = (spriteBuffer[i].palette_num == 0) ? palettes.ob0 : palettes.ob1;//get the correct palette

			bool x_mirror = spriteBuffer[i].xFlip;//X flip

			vector<COLOR> spriteColors;

			for (int bit = 7; bit >= 0; bit--) {
				uint8_t c1 = ((byte1 & (1 << bit)) > 0);
				uint8_t c2 = ((byte2 & (1 << bit)) > 0);
				uint8_t color_num = ((c2 << 1) | c1);
				COLOR color;
				switch (color_num) {
				case 0: color = COLOR::TRANSPARANT; break;
				case 1: color = static_cast<COLOR>(((palette & 0x0C) >> 2)); break;
				case 2: color = static_cast<COLOR>(((palette & 0x30) >> 4)); break;
				case 3: color = static_cast<COLOR>(((palette & 0xC0) >> 6)); break;
				}
				spriteColors.push_back(color);
			}
			if (spriteBuffer[i].xFlip) {
				std::reverse(spriteColors.begin(), spriteColors.end());
			}

			//draw the pixles
			for (int pixle = 0; pixle < 8; pixle++) {
				COLOR color = spriteColors[pixle];
				uint8_t pixle_offset = x + pixle;
				if (pixle_offset < 0 || pixle_offset >= 160) continue;
				if (spriteLine[pixle_offset].first == COLOR::TRANSPARANT) {
					spriteLine[pixle_offset].first = color;
					spriteLine[pixle_offset].second = spriteBuffer[i].obj_to_bg_priority;
				}
			}
		}
		COLOR bg_color0 = static_cast<COLOR>(palettes.background & 0x03);
		spriteBuffer.clear();

		for (int i = 0; i < 160; i++) {
			if (spriteLine[i].first == COLOR::TRANSPARANT) {
				continue;
			}
			if (spriteLine[i].second) {
				//OBJ Behind BG color 1-3
				if (lineBuffer[i] == bg_color0) {
					lineBuffer[i] = spriteLine[i].first;
				}
			}
			else {
				//Draws on top
				lineBuffer[i] = spriteLine[i].first;
			}
		}


	}

	void PPU::doOAM_search() {
		uint8_t height = (LCDC_reg.OBJ_size) ? 16 : 8;

		for (int i = 0; i < 160 && spriteBuffer.size() < 10; i += 4) {
			Sprite sprite;

			sprite.yPos = oam[i];
			sprite.xPos = oam[i + 1];
			sprite.tileNumber = oam[i + 2];
			sprite.attribute_falg = oam[i + 3];

			if (sprite.xPos == 0 || sprite.xPos >= 168) continue;
			if (sprite.yPos == 0 || sprite.yPos >= 160) continue;


			if (curr_scanline + 16 >= sprite.yPos && sprite.yPos + height > curr_scanline + 16) {
				spriteBuffer.push_back(sprite);
			}
		}


	}


	void PPU::reset() {
		clock_cnt = 0;
		mode = STATE::VBLANK;
		STAT_reg.coincidence_flag = 1;
		LCDC_reg.val = 0x00;
		curr_scanline = 0;
		LYC = 0;
		palettes.background = 0xFC;
		palettes.ob0 = 0xFF;
		palettes.ob1 = 0xFF;
		winY = 0;
		winX = 0;
		SCX = 0;
		SCY = 0;
		turnedOff();
	}
}