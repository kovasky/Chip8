/* CHIP-8 INTERPRETER
 *
 * MADE BY: KOVASKY BUEZO, 2018
 *
 * LICENSE: MIT
 *
 * */

#include <map>
#include <string>
#include <SDL2/SDL.h> 
#include "Chip8.h"
#include "Display.h"

using Register8 = uint8_t;

std::pair<Register8, Chip8::keyTrigger> getKey();

std::map<SDL_Keycode,Register8> myKeys = 
{
  {SDLK_1, 0x1},
  {SDLK_2, 0x2},
  {SDLK_3, 0x3},
  {SDLK_4, 0xC},
  {SDLK_q, 0x4},
  {SDLK_w, 0x5},
  {SDLK_e, 0x6},
  {SDLK_r, 0xD},
  {SDLK_a, 0x7},
  {SDLK_s, 0x8},
  {SDLK_d, 0x9},
  {SDLK_f, 0xE},
  {SDLK_z, 0xA},
  {SDLK_x, 0x0},
  {SDLK_c, 0xB},
  {SDLK_v, 0xF},
};

int main(int argv, char *args[])
{
	std::string romLocation = args[1];

	Chip8 *myChip = new Chip8(romLocation);

	Display *myDisplay = new Display();

	while(true)
	{
		auto start = SDL_GetTicks();
		
		myChip->run();

		myChip->updateKeys(getKey());

		if(myChip->updateDisplay())
		{
			myDisplay->drawImage(myChip->displayMem());
		}
		
		auto end = SDL_GetTicks();

		if(SDL_GetTicks() - start < 2)
		{
			SDL_Delay(2  - (end - start));
		}
	}

	delete myChip;

	delete myDisplay;

	return 0;
}

std::pair<Register8, Chip8::keyTrigger> getKey()
{
	SDL_Event someEvent;	
	
	Register8 mappedValue = 0x00;

	Chip8::keyTrigger someTrigger = Chip8::keyTrigger::keyRelease;

	while(SDL_PollEvent(&someEvent))
	{
		auto someKey = someEvent.key.keysym.sym;
		
		if(myKeys.find(someKey) != myKeys.end())
		{
			mappedValue = myKeys[someKey];

			someTrigger = someEvent.type == SDL_KEYDOWN? Chip8::keyTrigger::keyPress : Chip8::keyTrigger::keyRelease;
		}

	}

	return {mappedValue,someTrigger};
}
