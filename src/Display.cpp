#include <cstdint>
#include <string>
#include <SDL2/SDL.h>
#include "Display.h"
#include "Chip8.h"


Display::Display(std::string someName, uint8_t someHeight, uint8_t someWidth)
{
	SDL_Init(SDL_INIT_VIDEO);

	this->myWindow = SDL_CreateWindow(someName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, someWidth, someHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

	this->myRenderer = SDL_CreateRenderer(this->myWindow, -1, 0);
}
