#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include<string>
#include <SDL2/SDL.h>

//forward declaration of Chip8 cpu
class Chip8;

class Display
{
	using Register8 = uint8_t;

public:

	Display(std::string someName);

	~Display();

	void drawImage(std::array<std::array<Register8,0x20>,0x40> displayMem);

private:

	const Register8 displayHeight = 0x20;

	const Register8 displayWidth = 0x40;

	const Register8 pixelSize = 0x10;

	SDL_Window *myWindow = nullptr;

	SDL_Renderer *myRenderer = nullptr;
};

#endif
