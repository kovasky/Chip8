#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include<string>
#include <SDL2/SDL.h>

//forward declaration of Chip8 cpu
class Chip8;

class Display
{

public:

	Display(std::string someName, uint8_t someHeight, uint8_t someWidth);

	~Display();

	void drawImage(bool someBool);

private:

	uint8_t displayHeight;

	uint8_t displayWidth;

	SDL_Window *myWindow = nullptr;

	SDL_Surface *mySurface = nullptr;

	SDL_Renderer *myRenderer = nullptr;

	SDL_Texture *myTexture = nullptr;

	SDL_Event myEvent;
};

#endif
