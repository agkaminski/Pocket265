/* SimAK65 instruction decoder
 * Copyright A.K. 2018, 2023
 */

#ifndef SIMAK65_DECODER_H_
#define SIMAK65_DECODER_H_

#include "types.h"

enum opcode {
	ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI,
	BNE, BPL, BRK, BVC, BVS, CLC, CLD, CLI,
	CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR,
	INC, INX, INY, JMP, JSR, LDA, LDX, LDY,
	LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL,
	ROR, RTI, RTS, SBC, SEC, SED, SEI, STA,
	STX, STY, TAX, TAY, TSX, TXA, TXS, TYA
};

enum addrmode{
	mode_acc, mode_abs, mode_abx, mode_aby,
	mode_imm, mode_imp, mode_ind, mode_inx,
	mode_iny, mode_rel, mode_zp, mode_zpx,
	mode_zpy
};

struct opinfo {
	enum opcode opcode;
	enum addrmode mode;
};

struct opinfo decode(u8 opcode);

const char *opcodetostring(enum opcode opcode);

#endif /* SIMAK65_DECODER_H_ */
