#include <string>
#include <SDL2/SDL.h> 
#include "Chip8.h"
#include "Display.h"

int main(int argv, char *args[])
{
	std::string romName = args[1];

	Chip8 *myChip = new Chip8("src/" + romName);

	Display *myDisplay = new Display();

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
