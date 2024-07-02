#pragma once

#include <iostream>

constexpr unsigned char NOP = 0x0;
constexpr unsigned char ADD = 0x1;
constexpr unsigned char SUB = 0x2;
constexpr unsigned char SHL = 0x3;
constexpr unsigned char SHR = 0x4;
constexpr unsigned char XOR = 0x5;
constexpr unsigned char LOR = 0x6;
constexpr unsigned char AND = 0x7;
constexpr unsigned char CMP = 0x8;
constexpr unsigned char MOV = 0x9;
constexpr unsigned char LDW = 0xa;
constexpr unsigned char STW = 0xb;
constexpr unsigned char LIW = 0xc;
constexpr unsigned char SIW = 0xd;
constexpr unsigned char PSH = 0xe;
constexpr unsigned char POP = 0xf;
constexpr unsigned char JMP = 0x10;
constexpr unsigned char JIE = 0x11;
constexpr unsigned char JNE = 0x12;
constexpr unsigned char JIC = 0x13;
constexpr unsigned char JNC = 0x14;
constexpr unsigned char JIN = 0x15;
constexpr unsigned char JNN = 0x16;
constexpr unsigned char CMI = 0x17;
constexpr unsigned char JAB = 0x18;
constexpr unsigned char JMS = 0x19;
constexpr unsigned char RET = 0x1a;
constexpr unsigned char ADI = 0x1b;
constexpr unsigned char SBI = 0x1c;
constexpr unsigned char SLI = 0x1d;
constexpr unsigned char SRI = 0x1e;
constexpr unsigned char XRI = 0x1f;
constexpr unsigned char ORI = 0x20;
constexpr unsigned char ANI = 0x21;
constexpr unsigned char MVI = 0x22;
constexpr unsigned int NUMBEROFOPCODES = 0x23;

constexpr unsigned char opcodes[NUMBEROFOPCODES] = { NOP, ADD, SUB, SHL, SHR, XOR, LOR, AND, CMP, MOV, LDW, STW, LIW, SIW,
													PSH, POP, JMP, JIE, JNE, JIC, JNC, JIN, JNN, CMI, JAB, JMS, RET, ADI,
													SBI, SLI, SRI, XRI, ORI, ANI, MVI };

namespace Impostor2 {
	class Cpu {

		void ReduceBankRegs();	//Set the MSB of the bank regs to 0

		unsigned int GetPrgAddress();

	public:
		unsigned short regs[8] = { 0 };

		unsigned short pc = 0;				//Program counter

		//Flags
		bool zero = false;
		bool carry = false;
		bool negative = false;

		int ExecInstr(unsigned char* io);

	};
}