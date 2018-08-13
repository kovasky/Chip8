#include <iostream>
#include "Chip8.h"

int main(int argc, char *argv[])
{
	Chip8 *myChip = new Chip8("");

	while(true)
	{
		myChip->run();

		if(myChip->updateDisplay())
		{
			drawGraphics();
		}
	}
	return 0;
}
