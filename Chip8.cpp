#include <fstream>
#include "Chip8.h"

const std::array<Register8,0x50> font = 
{ 
	0xF0, 0x90, 0x90, 0x90, 0xF0, 
	0x20, 0x60, 0x20, 0x20, 0x70,
	0xF0, 0x10, 0xF0, 0x80, 0xF0,
	0xF0, 0x10, 0xF0, 0x10, 0xF0,
	0x90, 0x90, 0xF0, 0x10, 0x10,
	0xF0, 0x80, 0xF0, 0x10, 0xF0,
	0xF0, 0x80, 0xF0, 0x90, 0xF0,
	0xF0, 0x10, 0x20, 0x40, 0x40,
	0xF0, 0x90, 0xF0, 0x90, 0xF0,
	0xF0, 0x90, 0xF0, 0x10, 0xF0,
	0xF0, 0x90, 0xF0, 0x90, 0x90,
	0xE0, 0x90, 0xE0, 0x90, 0xE0,
	0xF0, 0x80, 0x80, 0x80, 0xF0,
	0xE0, 0x90, 0x90, 0x90, 0xE0,
	0xF0, 0x80, 0xF0, 0x80, 0xF0,
	0xF0, 0x80, 0xF0, 0x80, 0x80 
};

Chip8::Chip8(std::string someRom)
	:romLocation(someRom),delayTimer(0x00),soundTimer(0x00),flag(0x00), opcode(0x00), index(0x00), stackPointer(0x00), operand(0x00),programCounter(0x02)
{
	this->keyState.fill(0x00);

	this->generalPurposeRegisters.fill(0x00); //reset GPRs

	this->displayMemory.fill(0x00);

	this->memory.fill(0x00);

	this->stack.fill(0x00);

	//copy default fontset
	std::copy(font.begin(),font.end(),memory.begin());

	if(!this->loadRom())
	{
		exit(EXIT_FAILURE);
	}
}

bool Chip8::loadRom() const 
{
	std::ifstream someRom(this->romLocation.c_str(), std::ios::binary);

	if(someRom)
	{
		std::streamoff fileSize = someRom.tellg();

		if(fileSize < 0x0E01)
		{
			someRom.read((char*)(this->memory.data()+0x0200),fileSize);

			return true;
		}
	}

	return false;
}

void Chip8::run()
{
	while(true)
	{
		this->fetch();

		this->decode();

		this->execute();
	}
}

void Chip8::fetch()
{
	this->opcode = this->memory[programCounter] << 0x08 | this->memory[programCounter + 0x01];
}

void Chip8::decode()
{
	this->instruction.firstNibble = (this->opcode >> 0x12) & 0x0F;

	this->instruction.lastNibble = this->opcode & 0x0F;

	this->instruction.address = this->opcode & 0x0FFF;

	this->instruction.Vx = (this->opcode >> 0x08) & 0x0F;

	this->instruction.Vy = (this->opcode >> 0x04) & 0x0F;

	this->instruction.kk = this->opcode & 0x00FF;

	if(this->instruction.firstNibble == 0x01 || this->instruction.firstNibble == 0x02 || this->instruction.firstNibble == 0x0B)
	{
		this->instruction.performJump = true;
	}
}

void Chip8::execute()
{
	switch(this->instruction.firstNibble)
	{
		case 0x00:

			if(this->instruction.kk == 0x00E0)
			{
				//clear the screen
				this->displayMemory.fill(0x00);
			}
			else
			{
				//perform a return from subroutine
				this->programCounter = this->stack[this->stackPointer];

				this->stackPointer--;	
			}

			break;

		case 0x01:

			//perform a jump to specified address
			this->programCounter = this->instruction.address;

			break;

		case 0x02:

			//increment stack pointer and save current program counter
			this->stack[this->stackPointer++] = this->programCounter;
			
			//set program counter to specified address
			this->programCounter = this->instruction.address;

			break;

		case 0x03:

			//skip next instruction if
			if(this->generalPurposeRegisters[this->instruction.Vx] == this->instruction.kk)
			{
				this->programCounter += 0x02;
			}

			break;

		case 0x04:

			//skip next instruction if not
			if(this->instruction.Vx != this->instruction.kk)
			{
				this->programCounter += 0x02;
			}

			break;

		case 0x05:

			//skip next instruction if
			if(this->instruction.Vx == this->instruction.Vy)
			{
				this->programCounter += 0x02;
			}

			break;

		case 0x06:
			break;
		case 0x07:
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0A:
			break;
		case 0x0B:
			break;
		case 0x0C:
			break;
		case 0x0D:
			break;
		case 0x0E:
			break;
		case 0x0F:
			break;
	}
}
