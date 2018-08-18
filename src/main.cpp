#include <iostream>
#include <string>
#include <SDL2/SDL.h> 
#include <unistd.h>
#include "Chip8.h"
#include "Display.h"

int main(int argv, char *args[])
{
	Chip8 *myChip = new Chip8("/home/kova/Google Drive/School/PROJECTS/CHIP8/src/PONG");

	Display *myDisplay = new Display("Chip8");

	while(true)
	{
		myChip->run();

		if(myChip->updateDisplay())
		{
			myDisplay->drawImage(myChip->displayMem());
			myChip->updateDisplay(false);
		}
	}

	delete myChip;

	delete myDisplay;

	return 0;
}
