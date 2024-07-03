#include "Cpu.hpp"

#define INPUT_ADDRESS 0x030000

void Impostor2::Cpu::ReduceBankRegs() {
	regs[6] = regs[6] & 0xff;
	regs[7] = regs[7] & 0xff;
}

unsigned int Impostor2::Cpu::GetPrgAddress() {

	return ((regs[6] << 16) | pc) & 0xffff;

}

int Impostor2::Cpu::ExecInstr(unsigned char* io, bool* accessedInput) {

    *accessedInput = false;

	ReduceBankRegs();

	unsigned int currentAddress = GetPrgAddress();

	unsigned short fullInstr = io[currentAddress] | (io[currentAddress + 1] << 8);
	unsigned char opcode = fullInstr & 0b111111;	//The first 6 bits contain the opcode
	unsigned int reg1 = (fullInstr >> 6) & 0b111;
	unsigned int reg2 = (fullInstr >> 9) & 0b111;

	unsigned short arg1 = io[currentAddress + 2] | (io[currentAddress + 3] << 8);
	unsigned short arg2 = io[currentAddress + 4] | (io[currentAddress + 5] << 8);

	//std::cout << "pc: " << std::hex << pc << ", opcode: " << std::hex << (int)opcode << ", arg1: " << std::hex << arg1 << '\n';

	pc += 2;

	unsigned int clockTicks = 2;

	//A switch case containing every single opcode
	switch (opcode) {
	case NOP: {
		clockTicks += 1;
		break;
	}
	case ADD: {
		clockTicks += 1;
		regs[reg1] += regs[reg2];
		break;
	}
	case SUB: {
		clockTicks += 1;
		regs[reg1] -= regs[reg2];
		break;
	}
	case SHL: {
		clockTicks += 1;
		regs[reg1] = regs[reg1] << (regs[reg2] & 0b1111);
		break;
	}
	case SHR: {
		clockTicks += 1;
		regs[reg1] = regs[reg1] >> (regs[reg2] & 0b1111);
		break;
	}
	case XOR: {
		clockTicks += 1;
		regs[reg1] ^= regs[reg2];
		break;
	}
	case LOR: {
		clockTicks += 1;
		regs[reg1] |= regs[reg2];
		break;
	}
	case AND: {
		clockTicks += 1;
		regs[reg1] &= regs[reg2];
		break;
	}
	case CMP: {
		clockTicks += 1;
		if (regs[reg1] == regs[reg2]) zero = true;
		else zero = false;
		if (regs[reg1] >= regs[reg2]) carry = true;
		else carry = false;
		if (regs[reg1] - regs[reg2] >= 32768U) negative = true;
		else negative = false;
		break;
	}
	case MOV: {
		clockTicks += 1;
		regs[reg1] = regs[reg2];
		break;
	}
	case LDW: {
		clockTicks += 3;
		pc += 2;
		int loadAddress = arg1 + regs[reg2] + (regs[7] << 16);
		regs[reg1] = io[loadAddress] | (io[loadAddress + 1] << 8);
      
        if (loadAddress == INPUT_ADDRESS) *accessedInput = true;

		break;
	}
	case STW: {
		clockTicks += 3;
		pc += 2;
		int storeAddress = arg1 + regs[reg2] + (regs[7] << 16);
		io[storeAddress] = regs[reg1] & 0xff;
		io[storeAddress + 1] = (regs[reg1] >> 8) & 0xff;
		break;
	}
	case LIW: {
		clockTicks += 3;
		pc += 2;
		int loadAddress = arg1 + (regs[7] << 16);
		regs[reg1] = io[loadAddress] | (io[loadAddress + 1] << 8);
      
        if (loadAddress == INPUT_ADDRESS) *accessedInput = true;

		break;
	}
	case SIW: {
		clockTicks += 3;
		pc += 2;
		int storeAddress = arg1 + (regs[7] << 16);
		io[storeAddress] = regs[reg1] & 0xff;
		io[storeAddress + 1] = (regs[reg1] >> 8) & 0xff;
		break;
	}
	case PSH: {
		clockTicks += 2;
		regs[4] -= 2;
		int storeAddress = regs[4] + (regs[7] << 16);
		io[storeAddress] = regs[reg1] & 0xff;
		io[storeAddress + 1] = (regs[reg1] >> 8) & 0xff;
		break;
	}
	case POP: {
		clockTicks += 3;
		int loadAddress = regs[4] + (regs[7] << 16);
		regs[reg1] = io[loadAddress] | (io[loadAddress + 1] << 8);
		regs[4] += 2;
      
        if (loadAddress == INPUT_ADDRESS) *accessedInput = true;

		break;
	}
	case JMP: {
		clockTicks += 2;
		pc = arg1;
		break;
	}
	case JIE: {
		clockTicks += 2;
		if (zero)
			pc = arg1;
		else
			pc += 2;
		break;
	}
	case JNE: {
		clockTicks += 2;
		if (!zero)
			pc = arg1;
		else
			pc += 2;
		break;
	}
	case JIC: {
		clockTicks += 2;
		if (carry)
			pc = arg1;
		else
			pc += 2;
		break;
	}
	case JNC: {
		clockTicks += 2;
		if (!carry)
			pc = arg1;
		else
			pc += 2;
		break;
	}
	case JIN: {
		clockTicks += 2;
		if (negative)
			pc = arg1;
		else
			pc += 2;
		break;
	}
	case JNN: {
		clockTicks += 2;
		if (!negative)
			pc = arg1;
		else
			pc += 2;
		break;
	}
	case CMI: {
		clockTicks += 2;
		pc += 2;
		if (regs[reg1] == arg1) zero = true;
		else zero = false;
		if (regs[reg1] >= arg1) carry = true;
		else carry = false;
		if (regs[reg1] - arg1 >= 32768U) negative = true;
		else negative = false;
		break;
	}
	case JAB: {
		clockTicks += 5;
		regs[6] = arg1 & 0xff;
		pc = arg2;
		break;
	}
	case JMS: {
		clockTicks += 4;

		regs[4] -= 2;
		int storeAddress = regs[4] + (regs[7] << 16);
		io[storeAddress] = pc & 0xff;
		io[storeAddress + 1] = (pc >> 8) & 0xff;
		
		pc = arg1;
		break;
	}
	case RET: {
		clockTicks += 3;

		int loadAddress = regs[4] + (regs[7] << 16);
		pc = (io[loadAddress] | (io[loadAddress + 1] << 8)) + 2;
		regs[4] += 2;
      
        if (loadAddress == INPUT_ADDRESS) *accessedInput = true;

		break;
	}
	case ADI: {
		clockTicks += 2;
		pc += 2;
		regs[reg1] += arg1;
		break;
	}
	case SBI: {
		clockTicks += 2;
		pc += 2;
		regs[reg1] -= arg1;
		break;
	}
	case SLI: {
		clockTicks += 2;
		pc += 2;
		regs[reg1] = regs[reg1] << (arg1 & 0b1111);
		break;
	}
	case SRI: {
		clockTicks += 2;
		pc += 2;
		regs[reg1] = regs[reg1] >> (arg1 & 0b1111);
		break;
	}
	case XRI: {
		clockTicks += 2;
		pc += 2;
		regs[reg1] ^= arg1;
		break;
	}
	case ORI: {
		clockTicks += 2;
		pc += 2;
		regs[reg1] |= arg1;
		break;
	}
	case ANI: {
		clockTicks += 2;
		pc += 2;
		regs[reg1] &= arg1;
		break;
	}
	case MVI: {
		clockTicks += 2;
		pc += 2;
		regs[reg1] = arg1;
		break;
	}
	}

	ReduceBankRegs();

	return clockTicks;

}
