/* CHIP-8 INTERPRETER
 *
 * MADE BY: KOVASKY BUEZO, 2018
 *
 * LICENSE: MIT
 *
 * */

#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <cstdint> //for uint8
#include <string>
#include <SDL2/SDL.h>


class Chip8
{
public:

	using Register8 = uint8_t; //used to represent an 8-bit register

	using Register16 = uint16_t; //used to represent a 16-bit register

	Chip8(std::string someRom); //constructor that will initialize variables

	void run(); //will fetch decode and execute the program

	bool updateDisplay();

	enum class keyTrigger
	{
		keyPress,
		keyRelease
	};

	bool updateKeys(std::pair<Register8, Chip8::keyTrigger> somePair);

	std::array<std::array<Register8,0x20>,0x40> displayMem();

private:

	bool loadRom() const;

	void fetch();

	void decode();
	
	void execute();

	bool updateDisplay_;

	struct Instruction
	{
		Register8 firstNibble = 0x00; //first nibble of the opcode to check what will be executed

		Register8 lastNibble = 0x00;

		Register16 address = 0x00; //address jump to 
		
		Register8 Vx = 0x00; //register used for operation

		Register8 Vy = 0x00; 

		Register8 kk = 0x00; //lowest 8 bits of the instruction

		bool performJump = false; //will this make a jump?
	};

	Instruction instruction;

	std::string romLocation;

	Register8 delayTimer;

	Register8 soundTimer;

	Register16 opcode; //the opcode is 16 bits long and is used for the instructions

	Register16 index;

	Register16 stackPointer;

	Register16 programCounter;

	std::array<bool,0x10> keyState;

	std::array<Register8,0x10> generalPurposeRegisters;  //the Chip-8 has 16 General Purpose Registers

	std::array<Register8,0x10> stack;

	std::array<Register8,0x1000> memory;
	
	std::array<std::array<Register8,0x20>,0x40> displayMemory; //array of [64d][32d]
};

#endif
