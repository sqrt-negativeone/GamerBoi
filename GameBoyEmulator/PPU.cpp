#include "PPU.h"
#include "Bus.h"
#include <algorithm>

PPU::PPU() {
	clock_cnt = 0;
	mode = STATE::VBLANK;
	coincidenceFlag = 1;
	frame_count = 0;
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	window = SDL_CreateWindow("Gamer Boi (Emulator)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 432, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, 160, 144);
	SDL_SetWindowResizable(window, SDL_TRUE);
}
void PPU::connectBus(Bus* bus) {
	this->bus = bus;
	reset();
}
uint8_t PPU::get_mode() {
	return static_cast<int>(this->mode);
}
uint8_t PPU::get_coincidence() {
	return this->coincidenceFlag;
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

void PPU::turnedOff() {
	if (isOff) return;
	isOff = true;

	curr_scanline = 0;
	coincidenceFlag = (curr_scanline == bus->read(LYC));

	for (int i = 0; i < 144; i++) {
		for (int j = 0; j < 160; j++) {
			for (int k = 0; k < 3; k++) {
				screen[i][j][k] = 0xFF;
			}
		}
	}
	render();
}

void PPU::clock(uint8_t cycles) {
	LCDC_reg.val = bus->read(LCDC);
	uint8_t stat = bus->read(STAT);
	curr_scanline = bus->read(LY);
	uint8_t req = 0;
	if (!LCDC_reg.lcd_on) {
		clock_cnt = 0;
		mode = STATE::HBLANK;
		return;
	}
	isOff = false;
	clock_cnt += cycles;
	switch (mode)
	{
	case PPU::STATE::HBLANK:
		if (clock_cnt >= HBLANK_DURATION) {
			drawScanline();
			clock_cnt -= HBLANK_DURATION;
			bus->io_registers.inc_LY();
			curr_scanline++;
			if (curr_scanline == bus->read(LYC)) {
				coincidenceFlag = 1;
				if (stat & (1 << 6)) {// if LYC=LC interrupt enable
					req |= 2;
				}
			}
			else {
				coincidenceFlag = 0;
			}
			if (curr_scanline == VBLANK_START) {
				mode = STATE::VBLANK;
				//request V-Blank interrupt
				req |= 1;
				if (stat & (1 << 4)) {
					req |= 2;
				}

			}
			else {
				mode = STATE::OAM_SEARCH;
				doOAM_search();
				if (stat & (1 << 5)) {//if OAM interrupt is enable
					req |= 2;
				}
			}
		}
		break;
	case PPU::STATE::VBLANK:
		if (clock_cnt >= SCANLINE_DURATION) {
			clock_cnt -= SCANLINE_DURATION;
			bus->io_registers.inc_LY();
			curr_scanline++;
			curr_scanline %= 154;
			if (curr_scanline == bus->read(LYC)) {
				coincidenceFlag = 1;
				if (stat & (1 << 6)) {// if LYC=LC interrupt enable
					req |= 2;
				}
			}
			else {
				coincidenceFlag = 0;
			}
			if (curr_scanline == 0) {
				//render to screen
				render();
				frame_count = 1 - frame_count;
				mode = STATE::OAM_SEARCH;
				doOAM_search();
				if (stat & (1 << 2)) {
					req |= 2;
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
			if (stat & (1 << 3)) {//if OAM interrupt is enable
				req |= 2;
			}
		}
		break;
	default:
		break;
	}
	bus->interrupt_req(req);


}

void PPU::render()
{
	SDL_UpdateTexture(texture, NULL, screen, 160 * sizeof(uint8_t) * 3);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
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
	uint8_t curr_line = bus->read(SCY) + curr_scanline;
	uint8_t curr_x = bus->read(SCX);
	uint8_t tileY = curr_line / 8;
	uint8_t line_tiles[32];
	uint8_t palette = bus->read(0xFF47);

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
			case 0: color = static_cast<COLOR>(palette & 0x03); break;
			case 1: color = static_cast<COLOR>(((palette & 0x0C) >> 2)); break;
			case 2: color = static_cast<COLOR>(((palette & 0x30) >> 4)); break;
			case 3: color = static_cast<COLOR>(((palette & 0xC0) >> 6)); break;
			}
			lineBuffer[pixle_count] = color;
		}
	}
}

void PPU::drawWindow() {
	uint8_t WX = bus->read(winX);
	uint8_t WY = bus->read(winY);
	uint8_t palette = bus->read(0xFF47);

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
				case 0: color = static_cast<COLOR>(palette & 0x03); break;
				case 1: color = static_cast<COLOR>(((palette & 0x0C) >> 2)); break;
				case 2: color = static_cast<COLOR>(((palette & 0x30) >> 4)); break;
				case 3: color = static_cast<COLOR>(((palette & 0xC0) >> 6)); break;
				}
				lineBuffer[pixle_count] = color;
			}
		}
	}
}

void PPU::drawSprite() {
	
	uint8_t height = (LCDC_reg.OBJ_size)? 16 : 8;

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
		uint8_t palette = (spriteBuffer[i].palette_num == 0) ? bus->read(0xFF48) : bus->read(0xFF49);//get the correct palette

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
	COLOR bg_color0 = static_cast<COLOR>(bus->read(0xFF47) & 0x03);
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


		if (curr_scanline + 16 >= sprite.yPos && sprite.yPos + height > curr_scanline + 16 ) {
			spriteBuffer.push_back(sprite);
		}
	}
	
	
}


void PPU::reset() {
	clock_cnt = 0;
	mode = STATE::VBLANK;
	coincidenceFlag = 1;
	turnedOff();
}