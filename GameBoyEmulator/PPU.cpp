#include "PPU.h"
#include "Bus.h"
#include <algorithm>

PPU::PPU() {
	clock_cnt = 0;
	mode = STATE::VBLANK;
	coincidenceFlag = 1;
	frame_count = 0;
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("GB Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 432, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 160, 144);
	SDL_SetWindowResizable(window, SDL_TRUE);
}
void PPU::connectBus(Bus* bus) {
	this->bus = bus;
	dma = new DMA(bus);
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
		//return vram[addr - 0x8000];
		if (mode != STATE::LCD_TRANSFERE) {
			return vram[addr - 0x8000];
		}
		return 0xFF;
	}
	else if (0xFE00 <= addr && addr < 0xFEA0) {//OAM read
		//return oam[addr - 0xFE00];
		if (mode == STATE::HBLANK || mode == STATE::VBLANK) {
			return oam[addr - 0xFE00];
		}
		return 0xFF;
	}
}
void PPU::write(uint16_t addr, uint8_t data) {
	if (0x8000 <= addr && addr < 0xA000) {//VRAM write
		//vram[addr - 0x8000] = data;
		if (mode != STATE::LCD_TRANSFERE) {
			vram[addr - 0x8000] = data;
		}
	}
	else if (0xFE00 <= addr && addr < 0xFEA0) {//OAM write
		//oam[addr - 0xFE00] = data;
		if (mode == STATE::HBLANK || mode == STATE::VBLANK) {
			oam[addr - 0xFE00] = data;
		}
	}
}

void PPU::clock() {
	uint8_t lcd = bus->read(LCDC);
	uint8_t stat = bus->read(STAT);
	uint8_t req = 0;
	if (!isLcdOn()) {
		clock_cnt = 0;
		mode = STATE::HBLANK;
		return;
	}
	clock_cnt++;
	switch (mode)
	{
	case PPU::STATE::HBLANK:
		if (clock_cnt == HBLANK_DURATION) {
			drawScanline(bus->read(LY));
			clock_cnt = 0;
			bus->io_registers.inc_LY();
			if (bus->read(LY) == bus->read(LYC)) {
				coincidenceFlag = 1;
				if (stat & (1 << 6)) {// if LYC=LC interrupt enable
					req |= 2;
				}
			}
			else {
				coincidenceFlag = 0;
			}
			if (bus->read(LY) == VBLANK_START) {
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
		if (clock_cnt == VBLANK_DURATION) {
			//render to screen
			SDL_UpdateTexture(texture, NULL, screen, 160 * sizeof(uint8_t) * 3);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			clock_cnt = 0;
			bus->write(LY, 0);
			mode = STATE::OAM_SEARCH;
			doOAM_search();
			if (stat & (1 << 2)) {
				req |= 2;
			}
		}
		else if (clock_cnt % SCANLINE_DURATION == 0) {
			bus->io_registers.inc_LY();
			if (bus->read(LY) == bus->read(LYC)) {
				coincidenceFlag = 1;
				if (stat & (1 << 6)) {// if LYC=LC interrupt enable
					req |= 2;
				}
			}
			else {
				coincidenceFlag = 0;
			}
		}
		break;
	case PPU::STATE::OAM_SEARCH:
		if (clock_cnt == OAM_SEARCH_DURATION) {
			clock_cnt = 0;
			mode = STATE::LCD_TRANSFERE;
		}
		break;
	case PPU::STATE::LCD_TRANSFERE:
		if (clock_cnt == LCD_TRANSFER_DURATION) {
			clock_cnt = 0;
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

	{

		//if (clock_cnt == 0 ) {
		//	//start of new scanline
		//	
		//	uint8_t curr_scanline = bus->read(LY);

		//	//a line on the screen that needs to be drawn
		//	if (curr_scanline < 144) {
		//		mode = 2;
		//		if (stat & (1 << 5)) {//if OAM interrupt is enable
		//			req |= 2;
		//		}
		//		drawScanline();
		//	}
		//}
		//if (clock_cnt == 20) {
		//	//mode3, transfering data to lcd
		//	mode = 3;
		//}
		//if (clock_cnt == 63) {
		//	//mode0, H-blank period
		//	mode = 0;
		//	if (stat & (1 << 3)) {//if H-Blank interrupt is enable
		//		req |= 2; //request interrupt
		//	}
		//}
		//clock_cnt++;
		////scanline completed
		//if (clock_cnt == 114) {
		//	clock_cnt = 0;
		//	//increment the LY
		//	bus->io_registers.inc_LY();

		//	if (bus->read(LY) == 144) { //enter V-Blank
		//		mode = 1;
		//		//request V-Blank interrupt
		//		req |= 1;
		//		//render to screen
		//		SDL_UpdateTexture(texture, NULL, screen, 160 * sizeof(uint8_t) * 3);
		//		SDL_RenderCopy(renderer, texture, NULL, NULL);
		//		SDL_RenderPresent(renderer);
		//		frame_count++;
		//	}

		//	//start of new frame
		//	if (bus->read(LY) == 154) {
		//		bus->write(LY, 0);
		//	}
		//	if (bus->read(LY) == bus->read(LYC)) {
		//		coincidenceFlag = 1;
		//		if (stat & (1 << 6)) {// if LYC=LC interrupt enable
		//			req |= 2;
		//		}
		//	}
		//	else {
		//		coincidenceFlag = 0;
		//	}
		//}
	}

}
bool PPU::isLcdOn()
{
	uint8_t lcd = bus->read(LCDC);
	return (lcd & (1 << 7)) > 0;
}
bool PPU::isBackgroundEnable() {
	uint8_t lcd = bus->read(LCDC);
	return (lcd & (1 << 0)) > 0;
}
bool PPU::isWindowEnable() {
	uint8_t lcd = bus->read(LCDC);
	return (lcd & (1 << 5)) > 0;
}
bool PPU::isSpriteEnable() {
	uint8_t lcd = bus->read(LCDC);
	return (lcd & (1 << 1)) > 0;
}

//void PPU::drawScanline(uint8_t curr_scanline) {
//	bool draw = false;
//	if (isBackgroundEnable()) {
//		draw = true;
//		drawBackground(curr_scanline);
//		if (isWindowEnable()) {
//			drawWindow(curr_scanline);
//		}
//	}
//	if (isSpriteEnable()) {
//		draw = true;
//		drawSprite(curr_scanline);
//	}
//	if (!draw) return;
//	//Actual transfer to lcd
//	for (int i = 0; i < 160; i++) {
//		switch (lineBuffer[i]) {
//		case COLOR::WHITE:
//			screen[curr_scanline][i][0] = 0xFF; screen[curr_scanline][i][1] = 0xFF; screen[curr_scanline][i][2] = 0xFF; break;
//		case COLOR::LIGHT_GRAY:
//			screen[curr_scanline][i][0] = 0xA9; screen[curr_scanline][i][1] = 0xA9; screen[curr_scanline][i][2] = 0xA9; break;
//		case COLOR::DARK_GRAY:
//			screen[curr_scanline][i][0] = 0x69; screen[curr_scanline][i][1] = 0x69; screen[curr_scanline][i][2] = 0x69; break; //NICE
//		case COLOR::BLACK:
//			screen[curr_scanline][i][0] = 0; screen[curr_scanline][i][1] = 0; screen[curr_scanline][i][2] = 0; break;
//		}
//		lineBuffer[i] = COLOR::TRANSPARANT;
//	}
//}
void PPU::drawBackground(uint8_t curr_scanline) {
	uint8_t lcd = bus->read(LCDC);
	uint8_t curr_line = bus->read(SCY) + curr_scanline;
	uint8_t curr_x = bus->read(SCX);
	uint8_t tileY = curr_line / 8;
	uint8_t line_tiles[32];
	uint8_t palette = bus->read(0xFF47);

	uint16_t tilemap_select = ((lcd & (1 << 3)) == 0) ? 0x9800 : 0x9C00;
	uint16_t tiledata_base = ((lcd & (1 << 4)) > 0) ? 0x8000 : 0x9000;

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

void PPU::drawWindow(uint8_t curr_scanline) {
	uint8_t lcd = bus->read(LCDC);
	uint8_t WX = bus->read(winX);
	uint8_t WY = bus->read(winY);
	uint8_t palette = bus->read(0xFF47);

	uint8_t line_tiles[32];
	if (WY <= curr_scanline && WX > 6 && WX <= 166) {//visible line
		uint16_t tilemap_select = ((lcd & (1 << 6)) == 0) ? 0x9800 : 0x9C00;
		uint16_t tiledata_base = ((lcd & (1 << 4)) > 0) ? 0x8000 : 0x9000;

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

void PPU::drawSprite(uint8_t curr_scanline) {
	uint8_t lcd = bus->read(LCDC);
	uint8_t height = ((lcd & (1 << 2)) == 0) ? 8 : 16;
	std::pair<COLOR, uint8_t> spriteLine[160];
	std::sort(spriteBuffer.begin(), spriteBuffer.end());
	for (int i = 0; i < 160; i++) {
		spriteLine[i].first = COLOR::TRANSPARANT;
		spriteLine[i].second = 1;
	}
	for (int i = 0; i < spriteBuffer.size(); i++) {
		uint8_t y = spriteBuffer[i].yPos;
		uint8_t x = spriteBuffer[i].xPos;
		uint8_t num = spriteBuffer[i].palette_num;

		if (x == 0 || x >= 168) continue;
		uint8_t row = curr_scanline - y + 16;//the row of the tile that will be rendered in this scanline
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

		for (int j = 7; j >= 0; j--) {//foreach bit in the 2 bytes
			uint8_t pixle_pos = (x_mirror) ? (x - 1 - j) : (x - 8 + j);
			if (pixle_pos < 0) {//pixle Off screen
				continue;
			}
			uint8_t c1 = ((byte1 & (1 << j)) > 0);
			uint8_t c2 = ((byte2 & (1 << j)) > 0);
			uint8_t color_num = ((c2 << 1) | c1);
			COLOR color;
			switch (color_num) {
			case 0: color = COLOR::TRANSPARANT; break;
			case 1: color = static_cast<COLOR>(((palette & 0x0C) >> 2)); break;
			case 2: color = static_cast<COLOR>(((palette & 0x30) >> 4)); break;
			case 3: color = static_cast<COLOR>(((palette & 0xC0) >> 6)); break;
			}
			if (color == COLOR::TRANSPARANT) {
				continue;
			}
			if (spriteLine[pixle_pos].first == COLOR::TRANSPARANT) {
				spriteLine[pixle_pos].first = color;
				spriteLine[pixle_pos].second = spriteBuffer[i].obj_to_bg_priority;

			}
		}
	}
	COLOR bg_color0 = static_cast<COLOR>(bus->read(0xFF47) & 0x03);

	for (int i = 0; i < 160; i++) {
		if (spriteLine[i].first == COLOR::TRANSPARANT) {
			continue;
		}
		if (spriteLine[i].second) {
			//OBJ Behind BG color 1-3
			if (spriteLine[i].first == bg_color0) {
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
	uint8_t curr_scanline = bus->read(LY);
	uint8_t height = ((bus->read(LCDC) & (1 << 2)) == 0) ? 8 : 16;
	for (int i = 0; i < 160; i += 4) {
		Sprite sprite;
		sprite.xPos = oam[4 * i];
		sprite.yPos = oam[4 * i + 1];
		sprite.palette_num = oam[4 * i + 2];
		sprite.attribute_falg = oam[4 * i + 3];
		if (curr_scanline + 16 >= sprite.yPos && sprite.yPos > curr_scanline + 16 + height) {
			spriteBuffer.push_back(sprite);
		}
	}
}

void PPU::drawScanline(uint8_t curr_scanline) {

	uint8_t lcd = bus->read(LCDC);
	uint8_t buffer[4 * 10];
	uint8_t sprite_found = 0;
	uint8_t height = ((lcd & (1 << 2)) == 0) ? 8 : 16;
	COLOR line[160];
	for (int i = 0; i < 160; i++) {
		line[i] = COLOR::WHITE;
	}
	//oam search
	for (uint8_t i = 0; i < 40 && sprite_found <= 10; i++) {
		uint8_t y = oam[4 * i];
		uint8_t x = oam[4 * i + 1];
		uint8_t num = oam[4 * i + 2];
		uint8_t flags = oam[4 * i + 3];
		if (curr_scanline + 16 >= y && y > curr_scanline + 16 + height) {
			buffer[4 * sprite_found] = oam[4 * i];
			buffer[4 * sprite_found + 1] = oam[4 * i + 1];
			buffer[4 * sprite_found + 2] = oam[4 * i + 2];
			buffer[4 * sprite_found + 3] = oam[4 * i + 3];
			sprite_found++;
		}
	}

	// if background and window display enable
	if ((lcd & 1) > 0) {

		//render background
		uint8_t curr_line = bus->read(SCY) + curr_scanline;
		uint8_t curr_x = bus->read(SCX);
		uint8_t tileY = curr_line / 8;
		uint8_t line_tiles[32];
		uint8_t palette = bus->read(0xFF47);

		uint16_t tilemap_select = ((lcd & (1 << 3)) == 0) ? 0x9800 : 0x9C00;
		uint16_t tiledata_base = ((lcd & (1 << 4)) > 0) ? 0x8000 : 0x9000;

		for (int i = 0; i < 32; i++) {
			line_tiles[i] = read(tilemap_select + 32 * tileY + i);
		}

		uint8_t tileX = curr_x / 8;//which tile in the line

		uint8_t pixle_count = 0;//how many pixels is displayed until now
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
				uint8_t color_num = (c2 << 1) | c1;
				COLOR color;
				switch (color_num) {
				case 0: color = static_cast<COLOR>(palette & 0x03); break;
				case 1: color = static_cast<COLOR>(((palette & 0x0C) >> 2)); break;
				case 2: color = static_cast<COLOR>(((palette & 0x30) >> 4)); break;
				case 3: color = static_cast<COLOR>(((palette & 0xC0) >> 6)); break;
				}
				line[pixle_count] = color;
			}
		}
		//render window
		if (lcd & (1 << 5)) {// if window display enable
			uint8_t WX = bus->read(winX);
			uint8_t WY = bus->read(winY);
			if (WY <= curr_scanline && WX > 6 && WX <= 166) {//visible line
				uint16_t tilemap_select = ((lcd & (1 << 6)) == 0) ? 0x9800 : 0x9C00;
				curr_row = curr_scanline - WY;//the row of the the window we are currently drawing
				pixle_count = WX - 7;
				for (int i = 0; i < 32; i++) {
					line_tiles[i] = read(tilemap_select + 32 * curr_row + i);
				}
				tileX = 0;
				while (pixle_count < 160) {
					uint8_t tile_id = line_tiles[tileX++];
					uint16_t tile_addr;
					if (tiledata_base == 0x8000) tile_addr = tiledata_base + tile_id * 16 + curr_row * 2;//unsigned base
					else tile_addr = tiledata_base + (int8_t)tile_id * 16 + curr_row * 2;//signed base
					uint8_t byte1 = read(tile_addr++);
					uint8_t byte2 = read(tile_addr);
					for (int i = 0; i < 8 && pixle_count < 160; i++, pixle_count++, curr_x++) {
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
						line[pixle_count] = color;
					}
				}
			}
		}
	}
	if ((lcd & (1 << 1))) {//if sprites display is enable
		//render sprites
		for (int i = 0; i < sprite_found; i++) {
			uint8_t y = buffer[4 * i];
			uint8_t x = buffer[4 * i + 1];
			uint8_t num = buffer[4 * i + 2];
			uint8_t flags = buffer[4 * i + 3];
			if (x == 0 || x >= 168) continue;
			uint8_t row = curr_scanline - y + 16;//the row of the tile that will be rendered in this scanline
			if ((flags & (1 << 6))) {//Y flip
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
			uint8_t palette = ((flags & (1 << 4)) > 0) ? bus->read(0xFF48) : bus->read(0xFF49);//get the correct palette
			bool x_mirror = ((flags & (1 << 5)) > 0);//X flip

			for (int j = 7; j >= 0; j--) {//foreach bit in the 2 bytes
				uint8_t pixle_pos = (x_mirror) ? (x - 1 - j) : (x - 8 + j);
				if (pixle_pos < 0) {//pixle Off screen
					continue;
				}
				uint8_t c1 = ((byte1 & (1 << j)) > 0);
				uint8_t c2 = ((byte2 & (1 << j)) > 0);
				uint8_t color_num = (c2 << 1) | c1;
				COLOR color;
				switch (color_num) {
				case 0: color = COLOR::TRANSPARANT; break;
				case 1: color = static_cast<COLOR>(((palette & 0x0C) >> 2)); break;
				case 2: color = static_cast<COLOR>(((palette & 0x30) >> 4)); break;
				case 3: color = static_cast<COLOR>(((palette & 0xC0) >> 6)); break;
				}
				if (color == COLOR::TRANSPARANT) {
					continue;
				}
				if (flags & (1 << 7)) {
					if (line[pixle_pos] == COLOR::WHITE) line[pixle_pos] = color;
				}
				else {
					line[pixle_pos] = color;
				}
			}
		}
	}
	for (int i = 0; i < 160; i++) {
		switch (line[i])
		{
		case COLOR::WHITE:
			screen[curr_scanline][i][0] = 0xFF; screen[curr_scanline][i][1] = 0xFF; screen[curr_scanline][i][2] = 0xFF; break;
		case COLOR::LIGHT_GRAY:
			screen[curr_scanline][i][0] = 0xA9; screen[curr_scanline][i][1] = 0xA9; screen[curr_scanline][i][2] = 0xA9; break;
		case COLOR::DARK_GRAY:
			screen[curr_scanline][i][0] = 0x69; screen[curr_scanline][i][1] = 0x69; screen[curr_scanline][i][2] = 0x69; break; //NICE
		case COLOR::BLACK:
			screen[curr_scanline][i][0] = 0; screen[curr_scanline][i][1] = 0; screen[curr_scanline][i][2] = 0; break;
		}
	}
}

void PPU::reset() {
	dma->is_running = false;
}