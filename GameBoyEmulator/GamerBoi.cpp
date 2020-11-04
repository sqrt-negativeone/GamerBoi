#include "Debugger.h"
#include <SDL.h>
#include <iostream>
#include <chrono>
#include <thread>
#undef main

SDL_Renderer* renderer;
SDL_Window* window;
SDL_Texture* texture;

using BUTTONS = Joypad::BUTTONS;;

Bus* bus;

void updateKey(uint8_t key, BUTTONS button) {
	if (key != bus->joypad.get_key_state(button)) {
		if (key) {
			bus->joypad.press_button(button);
		}
		else {
			bus->joypad.release_button(button);
		}
	}
}

void render()
{
	SDL_UpdateTexture(texture, NULL, bus->ppu.screen, 160 * sizeof(uint8_t) * 3);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

int main() {
	//Initialize SDL
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	window = SDL_CreateWindow("Gamer Boi (Emulator)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 432, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, 160, 144);
	SDL_SetWindowResizable(window, SDL_TRUE);

	bus = new Bus();

	//Cartridge* cartridge = new Cartridge("ROMs/Super Mario Land (World).gb");
	//Cartridge* cartridge = new Cartridge("ROMs/Dr. Mario.gb");
	Cartridge* cartridge = new Cartridge("ROMs/Tetris.gb");
	Debugger db;
	bus->cartridge = cartridge;
	/*db.getBus(bus);
	db.start();*/
	int cycles = 0;
	auto start = std::chrono::high_resolution_clock::now();
	SDL_Event e;
	for (;;) {
		if (bus->clock()) {
			render();
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					goto stop;
				}
			}
			auto keys = SDL_GetKeyboardState(NULL);
			uint8_t up = keys[SDL_SCANCODE_UP];
			uint8_t down = keys[SDL_SCANCODE_DOWN];
			uint8_t right = keys[SDL_SCANCODE_RIGHT];
			uint8_t left = keys[SDL_SCANCODE_LEFT];

			uint8_t A = keys[SDL_SCANCODE_X];
			uint8_t B = keys[SDL_SCANCODE_C];
			uint8_t sl = keys[SDL_SCANCODE_V];
			uint8_t st = keys[SDL_SCANCODE_B];


			updateKey(up, BUTTONS::UP);
			updateKey(down, BUTTONS::DOWN);
			updateKey(left, BUTTONS::LEFT);
			updateKey(right, BUTTONS::RIGHT);
			updateKey(A, BUTTONS::A);
			updateKey(B, BUTTONS::B);
			updateKey(sl, BUTTONS::SELECT);
			updateKey(st, BUTTONS::START);

			auto end = std::chrono::high_resolution_clock::now();
			double wait = 16.667 - std::chrono::duration<double, std::milli>(end - start).count();
			std::this_thread::sleep_for(std::chrono::milliseconds((int)wait));
			start = std::chrono::high_resolution_clock::now();
		}
	}
stop:
	return 0;
}