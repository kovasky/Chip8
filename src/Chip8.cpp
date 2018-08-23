/* CHIP-8 INTERPRETER
 *
 * MADE BY: KOVASKY BUEZO, 2018
 *
 * LICENSE: MIT
 *
 * */

#include <fstream>
#include <iostream>
#include <random>
#include <stdio.h>
#include "Chip8.h"

std::pair<Chip8::Register8, Chip8::keyTrigger> getKey();

const std::array<Chip8::Register8,0x50> font = 
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
	:romLocation(someRom),delayTimer(0x00),soundTimer(0x00), opcode(0x00), index(0x00), stackPointer(0x00), programCounter(0x0200)
{
	this->keyState.fill(false);

	this->generalPurposeRegisters.fill(0x00); //reset GPRs

	for(auto i : this->displayMemory)
	{
		i.fill(0x00);
	}

	this->memory.fill(0x00);

	this->stack.fill(0x00);

	this->updateDisplay_ = false;

	//copy default fontset
	std::copy(font.begin(),font.end(),memory.begin());

	if(!this->loadRom())
	{
		exit(EXIT_FAILURE);
	}
}

bool Chip8::loadRom() const 
{
	std::ifstream someRom(this->romLocation.c_str(), std::ios::in | std::ios::binary);
	someRom.unsetf(std::ios::skipws);

	if(someRom)
	{
		someRom.seekg(0, std::ios::end);
		std::streampos fileSize = someRom.tellg();
		someRom.seekg(0, std::ios::beg);

		if(fileSize < 0x0E01)
		{
			someRom.read((char*)((this->memory.data())+0x0200),fileSize);

			someRom.close();

			return true;
		}
	}

	return false;
}

//will simulate one cpu cycle
void Chip8::run()
{
		this->fetch();

		this->decode();

		this->execute();
}

bool Chip8::updateKeys(std::pair<Register8,Chip8::keyTrigger> somePair)
{
	if(somePair.second == Chip8::keyTrigger::keyPress)
	{
		this->keyState[somePair.first] = true;
		
		return true;
	}
	else
	{
		this->keyState[somePair.first] = false;
	}

	return false;
}

//retrieve updateDisplay flag
bool Chip8::updateDisplay()
{
	bool temp = updateDisplay_;
	
	if(updateDisplay_)
	{
		updateDisplay_ = false;
	}

	return temp;
}

std::array<std::array<Chip8::Register8,0x20>,0x40> Chip8::displayMem()
{
	return this->displayMemory;
}

//retrieve opcode
void Chip8::fetch()
{
	this->opcode = this->memory[programCounter] << 0x08 | this->memory[programCounter + 0x01];
}

//save parts of the instruction that are necessary to make execution easier
void Chip8::decode()
{
	this->instruction.firstNibble = ((this->opcode >> 0x0C) & 0x0F);

	this->instruction.lastNibble = this->opcode & 0x0F;

	this->instruction.address = this->opcode & 0x0FFF;

	this->instruction.Vx = (this->opcode >> 0x08) & 0x0F;

	this->instruction.Vy = (this->opcode >> 0x04) & 0x0F;

	this->instruction.kk = this->opcode & 0x00FF;

	if(this->instruction.firstNibble == 0x01 || this->instruction.firstNibble == 0x02 || this->instruction.firstNibble == 0x0B)
	{
		this->instruction.performJump = true;
	}
	else
	{
		this->instruction.performJump = false;
	}
}

void Chip8::execute()
{
	switch(this->instruction.firstNibble)
	{
		case 0x00:
		{

			if(this->instruction.lastNibble == 0x00)
			{
				//clear the screen
				for(auto i : this->displayMemory)
				{
					i.fill(0x00);
				}

				this->updateDisplay_ = true;
			}
			else if (this->instruction.lastNibble == 0x0E)
			{

				this->programCounter = 0x00;
				
				this->programCounter |= this->stack[this->stackPointer] << 0x08;

				this->stackPointer--;

				this->programCounter |= this->stack[this->stackPointer];
				
				//perform a return from subroutine
				this->stackPointer--;
			}

			break;
		}

		case 0x01:
		{
			//perform a jump to specified address
			this->programCounter = this->instruction.address;

			break;
		}

		case 0x02:
		{
			this->stackPointer++;
			
			//increment stack pointer and save current program counter
			this->stack[this->stackPointer] = this->programCounter;

			this->stackPointer++;

			this->stack[this->stackPointer] = this->programCounter >> 0x08;

			//set program counter to specified address
			this->programCounter = this->instruction.address;

			break;
		}

		case 0x03:
		{
			//skip next instruction if
			if(this->generalPurposeRegisters[this->instruction.Vx] == this->instruction.kk)
			{
				this->programCounter += 0x02;
			}

			break;
		}

		case 0x04:
		{
			//skip next instruction if not
			if(this->generalPurposeRegisters[this->instruction.Vx] != this->instruction.kk)
			{
				this->programCounter += 0x02;
			}

			break;
		}

		case 0x05:
		{
			//skip next instruction if
			if(this->generalPurposeRegisters[this->instruction.Vx] == this->generalPurposeRegisters[this->instruction.Vy])
			{
				this->programCounter += 0x02;
			}

			break;
		}

		case 0x06:
		{
			//put kk into Vx
			this->generalPurposeRegisters[this->instruction.Vx] = this->instruction.kk;

			break;
		}

		case 0x07:
		{
			//set Vx = Vx + kk
			this->generalPurposeRegisters[this->instruction.Vx] += this->instruction.kk;

			break;
		}

		case 0x08:
		{
			switch(this->instruction.lastNibble)
			{
				case 0x00:
				{
					//store the value of Vy into Vx
					this->generalPurposeRegisters[this->instruction.Vx] = this->generalPurposeRegisters[this->instruction.Vy];
					break;
				}

				case 0x01:
				{
					//perform OR between Vx and Vy and store into Vx
					this->generalPurposeRegisters[this->instruction.Vx] |= this->generalPurposeRegisters[this->instruction.Vy];

					break;
				}

				case 0x02:
				{
					//perform AND with Vx and Vy and store into Vx
					this->generalPurposeRegisters[this->instruction.Vx] &= this->generalPurposeRegisters[this->instruction.Vy];

					break;
				}

				case 0x03:
				{
					//perform XOR with Vx and Vy and store into Vx
					this->generalPurposeRegisters[this->instruction.Vx] ^= this->generalPurposeRegisters[this->instruction.Vy];

					break;
				}

				case 0x04:
				{
					//if there is an overflow, set the overflow flag to 1
					this->generalPurposeRegisters[0x0F] = (this->generalPurposeRegisters[this->instruction.Vx] > UINT8_MAX - this->generalPurposeRegisters[this->instruction.Vy] ? 0x01 : 0x00);
					
					//perform ADD with Vx and Vy and store into Vx 
					this->generalPurposeRegisters[this->instruction.Vx] += this->generalPurposeRegisters[this->instruction.Vy];

					break;
				}

				case 0x05:
				{
					//check if borrow
					this->generalPurposeRegisters[0x0F] = (this->generalPurposeRegisters[this->instruction.Vx] < this->generalPurposeRegisters[this->instruction.Vy] ? 0x00 : 0x01);

					//perform SUB with Vx and Vy and store into Vx
					this->generalPurposeRegisters[this->instruction.Vx] -= this->generalPurposeRegisters[this->instruction.Vy];

					break;
				}

				case 0x06:
				{	
					//store last bit into flag register
					this->generalPurposeRegisters[0x0F] = this->generalPurposeRegisters[this->instruction.Vx] & 0x01;

					//store Vx shifted one to the right into Vx
					this->generalPurposeRegisters[this->instruction.Vx] = this->generalPurposeRegisters[this->instruction.Vx] >> 0x01;

					break;
				}

				case 0x07:
				{
					
					//if Vx > Vy flag is 1 else 0 
					this->generalPurposeRegisters[0x0F] = (this->generalPurposeRegisters[this->instruction.Vx] > this->generalPurposeRegisters[this->instruction.Vy] ? 0x00 : 0x01);

					//SUB Vy - Vx and store into Vx
					this->generalPurposeRegisters[this->instruction.Vx] = this->generalPurposeRegisters[this->instruction.Vy] - this->generalPurposeRegisters[this->instruction.Vx];
					
					break;
				}

				case 0x0E:
				{
					//store most significant bit of Vx
					this->generalPurposeRegisters[0x0F] = (this->generalPurposeRegisters[this->instruction.Vx] & 0x80)? 0x01 : 0x00;

					//shift Vx to the left by 1
					this->generalPurposeRegisters[this->instruction.Vx] <<= 0x01;

					break;
				}
			}

			break;
		}

		case 0x09:
		{
			//skip next instruction if Vx is not equal to Vy
			if(this->generalPurposeRegisters[this->instruction.Vx] != this->generalPurposeRegisters[this->instruction.Vy])
			{
				this->programCounter += 0x02;
			}

			break;
		}

		case 0x0A:
		{
			//set Index register to the address provided
			this->index = this->instruction.address;

			break;
		}

		case 0x0B:
		{
			//jump to address + V0
			this->programCounter = this->instruction.address + this->generalPurposeRegisters[0x00];

			break;
		}

		case 0x0C:
		{
			//generate a random number and store it into Vx
			std::random_device rand;
			std::mt19937 random(rand());
			std::uniform_int_distribution<Register8> rint(0x00,0xFF);

			this->generalPurposeRegisters[this->instruction.Vx] = rint(random) & this->instruction.kk;

			break;
		}

		/* draw sprite on screen
		 *
		 * position is Vx and Vy
		 * 
		 * height = n = last nibble
		 *
		 * help from Laurence Muller
		 * */
		case 0x0D:
		{
			this->generalPurposeRegisters[0x0F] = 0x00;	

			Register8 pixel = 0x00;

			for(std::size_t j = 0; j < this->instruction.lastNibble; ++j)
			{
				Register8 ypos = (this->generalPurposeRegisters[this->instruction.Vy] + j) % 0x20;

				pixel = this->memory[this->index + j];	

				for(std::size_t i = 0; i < 0x08; ++i)
				{
					if((pixel & (0x80 >> i)))
					{
						Register8 xpos = (this->generalPurposeRegisters[this->instruction.Vx] + i) % 0x40;

						this->generalPurposeRegisters[0x0F] |= this->displayMemory[xpos][ypos] == 0x01? 0x01 : 0x00;
					
						this->displayMemory[xpos][ypos] ^= 0x01;
					}
					
				}
			}

			//after this is executed we need to update the display
			this->updateDisplay_ = true;
			
			break;
		}

		case 0x0E:
		{	
			//if Vx key is pressed, skip next instruction
			if(this->instruction.kk == 0x9E)
			{
				if(this->keyState[this->generalPurposeRegisters[this->instruction.Vx]])
				{
					this->programCounter += 0x02;
				}	
			}
			else if(this->instruction.kk == 0xA1)//if Vx is not pressed skip next instruction
			{
				if(!this->keyState[this->generalPurposeRegisters[this->instruction.Vx]])
				{
					this->programCounter += 0x02;
				}
			}

			break;
		}

		case 0x0F:
		{
			switch(this->instruction.kk)
			{
				//save delayTimer into register Vx
				case 0x07:
				{
					this->generalPurposeRegisters[this->instruction.Vx] = this->delayTimer;

					break;
				}

				case 0x0A:
				{
					while(!this->updateKeys(getKey())){};
					
					//wait for keypress and store in register
					for(std::size_t i = 0; i < this->keyState.size(); ++i)
					{
						if(this->keyState[i])
						{
							this->generalPurposeRegisters[this->instruction.Vx] = i;
						}
					}

					break;
				}

				case 0x15:
				{
					//set delayTimer to the value stored in Vx
					this->delayTimer = this->generalPurposeRegisters[this->instruction.Vx];

					break;
				}

				case 0x18:
				{
					//set soundTimer to the value stored in Vx
					this->soundTimer = this->generalPurposeRegisters[this->instruction.Vx];

					break;
				}

				case 0x1E:
				{
					
					//add to the current index the value stored in Vx
					this->index += this->generalPurposeRegisters[this->instruction.Vx];

					break;
				}

				case 0x29:
				{
					//set index to the location of the sprite
					this->index = this->generalPurposeRegisters[this->instruction.Vx] * 0x05;
					
					break;
				}

				case 0x33:
				{
					//store BCD representation of Vx in index, index + 1, index + 2
					this->memory[this->index] = (this->generalPurposeRegisters[this->instruction.Vx] / 0x64);

					this->memory[this->index + 1] = (this->generalPurposeRegisters[this->instruction.Vx] % 0x64) / 0x0A;

					this->memory[this->index + 2] = (this->generalPurposeRegisters[this->instruction.Vx] % 0x64) % 0x0A;

					break;
				}

				case 0x55:
				{
					//store the values stored in the general purpose registers
					for(std::size_t i = 0x00; i < this->generalPurposeRegisters.size(); i++)
					{
						this->memory[this->index + i] = this->generalPurposeRegisters[i];
					}

					this->index += this->generalPurposeRegisters[this->instruction.Vx] + 0x01;

					break;
				}
				case 0x65:
				{
					//retrieve values stored in memory and set them to the general purpose registers
					for(std::size_t i = 0x00; i < this->generalPurposeRegisters.size(); i++)
					{
						this->generalPurposeRegisters[i] = this->memory[this->index + i];
					}

					this->index += this->generalPurposeRegisters[this->instruction.Vx] + 0x01;

					break;
				}
			}

			break;
		}
	}
	
	//if no jump was performed then the counter needs to be increased by 2
	if(!this->instruction.performJump)
	{
		this->programCounter += 0x02;
	}


	if(this->delayTimer > 0x00)
	{
		this->delayTimer--;
	}

	if(this->soundTimer > 0x00)
	{
		this->soundTimer--;
	}
}
