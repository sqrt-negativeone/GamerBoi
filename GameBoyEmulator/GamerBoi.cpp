#include "Debugger.h"
#include "Bus.h"
#include <SDL.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <conio.h>
#undef main

using namespace GamerBoi;
using BUTTONS = Joypad::BUTTONS;;


namespace GamerBoi
{
	//void render(uint8_t* screen);
	class GameBoy
	{
	public:
		GameBoy() :
			debugging(false)
		{
			quit = false;
			SDL_Init(SDL_INIT_VIDEO);
			SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
			window = SDL_CreateWindow("Gamer Boi (Emulator)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 432, 0);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, 160, 144);
			SDL_SetWindowResizable(window, SDL_TRUE);

			bus = new Bus();
			bus->ppu.SetFrameCompleteCallback(FrameComplete);
			
			insertCartridge("ROMs/Super Mario Land (World).gb");
		}
		void StartWithDebug()
		{
			debugging = true;
			debugger.bus = bus;
			while (!quit)
			{
				UpdateDebug();
			}
		}
		void Start()
		{
			while (!quit)
			{
				Update();
			}
		}
		void StartWithSound()
		{
			SDL_Init(SDL_INIT_AUDIO);
			bus->apu.set_callback_function(sampleSoundReady);
			audio = { 0 };
			audio.freq = SAMPLE_FRAME_RATE;
			audio.channels = 2;
			audio.samples = MAX_BUFFER_SIZE;
			audio.format = AUDIO_F32SYS;
			audio.callback = audioCallback;
			audio.userdata = this;
			if (SDL_OpenAudio(&audio, NULL))
			{
				//error
				printf("Couldn't open audio: %s\n", SDL_GetError());
				return;
			}
			SDL_PauseAudio(0);

			while (!quit)
			{
				handle_events();
			}

			SDL_PauseAudio(1);
		}
	private:
		Debugger debugger;
		SDL_Renderer* renderer;
		SDL_Window* window;
		SDL_Texture* texture;
		SDL_AudioSpec audio;
		SDL_Event e;
		Bus* bus;
		std::atomic<bool> quit;
		bool debugging;

		const int SAMPLE_FRAME_RATE = 44100;
		const int MAX_BUFFER_SIZE = 1024;

		std::vector<float> audioBuffer;


		void insertCartridge(const char* path)
		{
			Cartridge* cartridge = new Cartridge(path);
			bus->cartridge = cartridge;
		}
		void UpdateDebug()
		{
			debugger.step();
		}
		void Update()
		{
			bus->clock();
		}

		void UpdateWithSound()
		{
			while (audioBuffer.size() < 2ll * MAX_BUFFER_SIZE)
			{
				bus->clock();
			}
		}

		void handle_events()
		{
			while (SDL_PollEvent(&e))
			{
				if (e.type == SDL_QUIT)
				{
					quit = true;
					return;
				}
				else if (e.type == SDL_DROPFILE)
				{
					auto filePath = e.drop.file;
					bus->removeCartridge();
					insertCartridge(filePath);
					SDL_free(filePath);
					bus->reset();
					return;
				}
			}
			if (debugging) return;
			auto keys = SDL_GetKeyboardState(NULL);
			if (keys[SDL_SCANCODE_R])
			{
				bus->reset();
				return;
			}
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

		}

		void updateKey(uint8_t key, BUTTONS button)
		{
			if (key != bus->joypad.get_key_state(button))
			{
				if (key)
				{
					bus->joypad.press_button(button);
				}
				else
				{
					bus->joypad.release_button(button);
				}
			}
		}

		std::function<void()> FrameComplete = [&]()
		{
			render();
			handle_events();
		};
		std::function<void(float, float)> sampleSoundReady = [&](float outLeft, float outRight)
		{
			audioBuffer.push_back(outLeft);
			audioBuffer.push_back(outRight);
		};
		static void audioCallback(void* userdata, Uint8* stream, int len)
		{
			auto gameBoy = (GameBoy*)userdata;
			gameBoy->UpdateWithSound();
			std::memcpy(stream, gameBoy->audioBuffer.data(), len);
			gameBoy->audioBuffer.clear();
		}
		void render()
		{
			SDL_UpdateTexture(texture, NULL, bus->ppu.screen, 160 * sizeof(uint8_t) * 3);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}
	};

}

int main()
{
	GamerBoi::GameBoy gb;
	gb.StartWithSound();
	return 0;
}
