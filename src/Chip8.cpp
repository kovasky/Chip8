#include <fstream>
#include <stdio.h>
#include <iostream>
#include <random>
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
	:romLocation(someRom),delayTimer(0x00),soundTimer(0x00), opcode(0x00), index(0x00), stackPointer(0x00), operand(0x00),programCounter(0x0200)
{
	this->keyState.fill(0x00);

	this->generalPurposeRegisters.fill(0x00); //reset GPRs

	this->displayMemory.fill(0x00);

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

			return true;
		}
	}

	return false;
}

//will simulate one cpu cycle
void Chip8::run()
{
		this->fetch();
		std::cout << "fetch" << "\n";

		this->decode();
		std::cout << "decode" << "\n";

		this->execute();
		std::cout << "execute" << "\n";
}

//set updateDisplay flag
void Chip8::updateDisplay(bool someBool)
{
	updateDisplay_ = someBool;
}

//retrieve updateDisplay flag
bool Chip8::updateDisplay() const
{
	return updateDisplay_;
}

std::array<Register8,0x100> Chip8::displayMem() const
{
	return this->displayMemory;
}

//retrieve opcode
void Chip8::fetch()
{
	this->opcode = this->memory[programCounter] << 8 | this->memory[programCounter + 1];
	printf("%#010x\n", this->opcode); 
}

//save parts of the instruction that are necessary to make execution easier
void Chip8::decode()
{
	this->instruction.firstNibble = ((this->opcode >> 0x0C) & 0x0F);
	std::cout << "first nibble = ";
       	printf("%#010x\n", this->instruction.firstNibble);	
	std::cout << "\n";

	this->instruction.lastNibble = this->opcode & 0x0F;

	this->instruction.address = this->opcode & 0x0FFF;

	this->instruction.Vx = (this->opcode >> 0x08) & 0x0F;
	std::cout << "Vx = " << this->instruction.Vx << "\n";

	this->instruction.Vy = (this->opcode >> 0x04) & 0x0F;
	std::cout << "Vy = " << this->instruction.Vy << "\n";

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

			if(this->instruction.lastNibble == 0x0E)
			{
				//clear the screen
				this->displayMemory.fill(0x00);
				this->updateDisplay_ = true;
			}
			else
			{
				//perform a return from subroutine
				this->programCounter = this->stack[this->stackPointer];

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
			//increment stack pointer and save current program counter
			this->stack[this->stackPointer++] = this->programCounter;
			
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
					//perform ADD with Vx and Vy and store into Vx; also checks for overflow
					this->generalPurposeRegisters[this->instruction.Vx] += this->generalPurposeRegisters[this->instruction.Vy];

					//if there is an overflow, set the overflow flag to 1
					this->generalPurposeRegisters[0x0F] = (this->generalPurposeRegisters[this->instruction.Vx] > UINT8_MAX - this->generalPurposeRegisters[this->instruction.Vy] ? 0x01 : 0x00);
					
					break;
				}

				case 0x05:
				{
					//perform SUB with Vx and Vy and store into Vx; also check for borrow
					this->generalPurposeRegisters[this->instruction.Vx] -= this->generalPurposeRegisters[this->instruction.Vy];

					//check if borrow
					this->generalPurposeRegisters[0x0F] = (this->generalPurposeRegisters[this->instruction.Vx] < this->generalPurposeRegisters[this->instruction.Vy] ? 0x00 : 0x01);

					break;
				}

				case 0x06:
				{	
					//store last bit into flag register
					this->generalPurposeRegisters[0x0F] = this->generalPurposeRegisters[this->instruction.Vy] >> 0x07;

					//store Vy shifted one to the right into Vx
					this->generalPurposeRegisters[this->instruction.Vx] = this->generalPurposeRegisters[this->instruction.Vy] >> 0x01;

					break;
				}

				case 0x07:
				{
					//SUB Vy - Vx and store into Vx
					this->generalPurposeRegisters[this->instruction.Vx] = this->generalPurposeRegisters[this->instruction.Vy] - this->generalPurposeRegisters[this->instruction.Vx];

					//if Vx > Vy flag is 1 else 0 
					this->generalPurposeRegisters[0x0F] = (this->generalPurposeRegisters[this->instruction.Vx] > this->generalPurposeRegisters[this->instruction.Vy] ? 0x01 : 0x00);

					break;
				}

				case 0x0E:
				{
					//store most significant bit of Vx
					this->generalPurposeRegisters[0x0F] = this->generalPurposeRegisters[this->instruction.Vx] >> 0x07;

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
			std::uniform_int_distribution<Register8> rint(0,255);

			this->generalPurposeRegisters[this->instruction.Vx] = rint(random);

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

			uint8_t pixel = 0x00;

			for(std::size_t j = 0; j < this->instruction.lastNibble; j++)
			{
				pixel = this->memory[this->index + j];	

				for(std::size_t i = 0; i < 0x08; i++)
				{
					if((pixel & (0x80 >> i)) != 0x00)
					{
						if(this->displayMemory[(this->instruction.Vx + i + ((this->instruction.Vy + j) * 0x40))] == 0x01)
						{
							this->generalPurposeRegisters[0x0F] = 0x01;

							this->displayMemory[(this->instruction.Vx + i + ((this->instruction.Vy + j) * 0x40))] ^= 0x01;
						}
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
			if(this->instruction.lastNibble == 0x0E)
			{
				if(keyState[this->instruction.Vx])
				{
					this->programCounter += 0x02;
				}	
			}
			else //if Vx is not pressed skip next instruction
			{
				if(!this->keyState[this->instruction.Vx])
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
					//wait for kepress and store in register

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
					this->memory[this->index] = this->generalPurposeRegisters[this->instruction.Vx] / 100;

					this->memory[this->index + 1] = (this->generalPurposeRegisters[this->instruction.Vx] % 100) / 10;

					this->memory[this->index + 2] = this->generalPurposeRegisters[this->instruction.Vx] % 10;

					break;
				}

				case 0x55:
				{
					//store the values stored in the general purpose registers
					for(std::size_t i = 0; i < this->generalPurposeRegisters.size(); i++)
					{
						this->memory[this->index + i] = this->generalPurposeRegisters[i];
					}

					break;
				}
				case 0x65:
				{
					//retrieve values stored in memory and set them to the general purpose registers
					for(std::size_t i = 0; i < this->generalPurposeRegisters.size(); i++)
					{
						this->generalPurposeRegisters[i] = this->memory[this->index + i];
					}

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
