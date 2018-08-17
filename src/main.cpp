#include <iostream>
#include <string>
#include <SDL2/SDL.h> 
#include "Chip8.h"
#include "Display.h"

int main(int argv, char *args[])
{
	Chip8 *myChip = new Chip8("/home/kova/Google Drive/School/PROJECTS/CHIP8/src/pong.c8");

	Display *myDisplay = new Display((std::string)"Chip8",32,64);

/*	while(true)
	{
		myChip->run();

		//myDisplay->drawImage(myChip->updateDisplay());
		//myChip->updateDisplay(false);
	}

	delete myChip;
	delete myDisplay;
*/

	while(true){};
	return 0;
}
