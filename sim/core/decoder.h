#ifndef _DECODER_H_
#define _DECODER_H_

#include "common/types.h"

typedef enum {
	ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI,
	BNE, BPL, BRK, BVC, BVS, CLC, CLD, CLI,
	CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR,
	INC, INX, INY, JMP, JSR, LDA, LDX, LDY,
	LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL,
	ROR, RTI, RTS, SBC, SEC, SED, SEI, STA,
	STX, STY, TAX, TAY, TSX, TXA, TXS, TYA
} opcode_t;

typedef enum {
	mode_acc, mode_abs, mode_abx, mode_aby,
	mode_imm, mode_imp, mode_ind, mode_inx,
	mode_iny, mode_rel, mode_zp, mode_zpx,
	mode_zpy
} addrmode_t;

typedef struct {
	opcode_t opcode;
	addrmode_t mode;
} opinfo_t;

opinfo_t decode(u8 opcode);

const char *opcodetostring(opcode_t opcode);

#endif
