#include <array>
#include <cstdint>
#include <string>
#include <SDL2/SDL.h>
#include "Display.h"

Display::Display()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	this->myWindow = SDL_CreateWindow("Chip8 Interpreter by Kovasky Buezo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, this->displayWidth  * this->pixelSize, this->displayHeight * this->pixelSize, SDL_WINDOW_SHOWN);

	this->myRenderer = SDL_CreateRenderer(this->myWindow, -1, SDL_RENDERER_ACCELERATED);

	SDL_RenderSetScale(this->myRenderer, this->pixelSize, this->pixelSize);
}

Display::~Display()
{
	SDL_DestroyWindow(this->myWindow);

	SDL_DestroyRenderer(this->myRenderer);

	SDL_Quit();
}


void Display::drawImage(std::array<std::array<Register8,0x20>,0x40> displayMem)
{
	 SDL_SetRenderDrawColor(this->myRenderer, 0x00, 0x00, 0x00, 0xFF);

	 SDL_RenderClear(this->myRenderer);

	 SDL_SetRenderDrawColor(this->myRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	 for(int j = 0; j < this->displayHeight; ++j)
	 {
		 for(int i = 0; i < this->displayWidth; ++i)
		 {
			 if(displayMem[i][j])
			 {
				 SDL_RenderDrawPoint(this->myRenderer, i, j);
			 }
		 }
	 }

	  SDL_RenderPresent(this->myRenderer);
}
