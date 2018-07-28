/*
 *
 * */

#include <cstdint> //for uint8
#include <array>

using Register8 = uint8_t; //used to represent an 8-bit register
using Register16 = uint16_t; //used to represent a 16-bit register

class Chip8
{
public:
	Chip8(std::string rom);

private:

	std::string fileName;

	Register8 delayTimer;

	Register8 soundTimer;

	Register8 flag; 

	Register16 opcode; //the opcode is 16 bits long and is used for the instructions

	Register16 index;

	Register16 stackPointer;

	Register16 operand;

	Register16 programCounter;

	std::array<Register8,0x10> keyState;
	
	std::array<Register16,0x10> stack;

	std::array<Register8,0x10> generalPurposeRegisters;  //the Chip-8 has 16 General Purpose Registers

	std::array<Register8,0x100> displayMemory;

	std::array<Register8,0x1000> memory;
		
};
