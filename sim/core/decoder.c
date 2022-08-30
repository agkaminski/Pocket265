#include "common/error.h"
#include "decoder.h"

static const opinfo_t decoder_table[] = {
	{BRK, mode_imp}, {ORA, mode_inx}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {ORA, mode_zp},  {ASL, mode_zp},  {NOP, mode_imp},
	{PHP, mode_imp}, {ORA, mode_imm}, {ASL, mode_acc}, {NOP, mode_imp},
	{NOP, mode_imp}, {ORA, mode_abs}, {ASL, mode_abs}, {NOP, mode_imp},
	
	{BPL, mode_rel}, {ORA, mode_iny}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {ORA, mode_zpx}, {ASL, mode_zpx}, {NOP, mode_imp},
	{CLC, mode_imp}, {ORA, mode_aby}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {ORA, mode_abx}, {ASL, mode_abx}, {NOP, mode_imp},
	
	{JSR, mode_abs}, {AND, mode_inx}, {NOP, mode_imp}, {NOP, mode_imp},
	{BIT, mode_zp},  {AND, mode_zp},  {ROL, mode_zp},  {NOP, mode_imp},
	{PLP, mode_imp}, {AND, mode_imm}, {ROL, mode_acc}, {NOP, mode_imp},
	{BIT, mode_abs}, {AND, mode_abs}, {ROL, mode_abs}, {NOP, mode_imp},
	
	{BMI, mode_rel}, {AND, mode_iny}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {AND, mode_zpx}, {ROL, mode_zpx}, {NOP, mode_imp},
	{SEC, mode_imp}, {AND, mode_aby}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {AND, mode_abx}, {ROL, mode_abx}, {NOP, mode_imp},
	
	{RTI, mode_imp}, {EOR, mode_inx}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {EOR, mode_zp},  {LSR, mode_zp},  {NOP, mode_imp},
	{PHA, mode_imp}, {EOR, mode_imm}, {LSR, mode_acc}, {NOP, mode_imp},
	{JMP, mode_abs}, {EOR, mode_abs}, {LSR, mode_abs}, {NOP, mode_imp},
	
	{BVC, mode_rel}, {EOR, mode_iny}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {EOR, mode_zpx}, {LSR, mode_zpx}, {NOP, mode_imp},
	{CLI, mode_imp}, {EOR, mode_aby}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {EOR, mode_abx}, {LSR, mode_abx}, {NOP, mode_imp},
	
	{RTS, mode_imp}, {ADC, mode_inx}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {ADC, mode_zp},  {ROR, mode_zp},  {NOP, mode_imp},
	{PLA, mode_imp}, {ADC, mode_imm}, {ROR, mode_acc}, {NOP, mode_imp},
	{JMP, mode_ind}, {ADC, mode_abs}, {ROR, mode_abs}, {NOP, mode_imp},
	
	{BVS, mode_rel}, {ADC, mode_iny}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {ADC, mode_zpx}, {ROR, mode_zpx}, {NOP, mode_imp},
	{SEI, mode_imp}, {ADC, mode_aby}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {ADC, mode_abx}, {ROR, mode_abx}, {NOP, mode_imp},
	
	{NOP, mode_imp}, {STA, mode_inx}, {NOP, mode_imp}, {NOP, mode_imp},
	{STY, mode_zp},  {STA, mode_zp},  {STX, mode_zp},  {NOP, mode_imp},
	{DEY, mode_imp}, {NOP, mode_imp}, {TXA, mode_imp}, {NOP, mode_imp},
	{STY, mode_abs}, {STA, mode_abs}, {STX, mode_abs}, {NOP, mode_imp},
	
	{BCC, mode_rel}, {STA, mode_iny}, {NOP, mode_imp}, {NOP, mode_imp},
	{STY, mode_zpx}, {STA, mode_zpx}, {STX, mode_zpy}, {NOP, mode_imp},
	{TYA, mode_imp}, {STA, mode_aby}, {TXS, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {STA, mode_abx}, {NOP, mode_imp}, {NOP, mode_imp},
	
	{LDY, mode_imm}, {LDA, mode_inx}, {LDX, mode_imm}, {NOP, mode_imp},
	{LDY, mode_zp},  {LDA, mode_zp},  {LDX, mode_zp},  {NOP, mode_imp},
	{TAY, mode_imp}, {LDA, mode_imm}, {TAX, mode_imp}, {NOP, mode_imp},
	{LDY, mode_abs}, {LDA, mode_abs}, {LDX, mode_abs}, {NOP, mode_imp},
	
	{BCS, mode_rel}, {LDA, mode_iny}, {NOP, mode_imp}, {NOP, mode_imp},
	{LDY, mode_zpx}, {LDA, mode_zpx}, {LDX, mode_zpy}, {NOP, mode_imp},
	{CLV, mode_imp}, {LDA, mode_aby}, {TSX, mode_imp}, {NOP, mode_imp},
	{LDY, mode_abx}, {LDA, mode_abx}, {LDX, mode_aby}, {NOP, mode_imp},
	
	{CPY, mode_imm}, {CMP, mode_inx}, {NOP, mode_imp}, {NOP, mode_imp},
	{CPY, mode_zp},  {CMP, mode_zp},  {DEC, mode_zp},  {NOP, mode_imp},
	{INY, mode_imp}, {CMP, mode_imm}, {DEX, mode_imp}, {NOP, mode_imp},
	{CPY, mode_abs}, {CMP, mode_abs}, {DEC, mode_abs}, {NOP, mode_imp},
	
	{BNE, mode_rel}, {CMP, mode_iny}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {CMP, mode_zpx}, {DEC, mode_zpx}, {NOP, mode_imp},
	{CLD, mode_imp}, {CMP, mode_aby}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {CMP, mode_abx}, {DEC, mode_abx}, {NOP, mode_imp},
	
	{CPX, mode_imm}, {SBC, mode_inx}, {NOP, mode_imp}, {NOP, mode_imp},
	{CPX, mode_zp},  {SBC, mode_zp},  {INC, mode_zp},  {NOP, mode_imp},
	{INX, mode_imp}, {SBC, mode_imm}, {NOP, mode_imp}, {NOP, mode_imp},
	{CPX, mode_abs}, {SBC, mode_abs}, {INC, mode_abs}, {NOP, mode_imp},
	
	{BEQ, mode_rel}, {SBC, mode_iny}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {SBC, mode_zpx}, {INC, mode_zpx}, {NOP, mode_imp},
	{SED, mode_imp}, {SBC, mode_aby}, {NOP, mode_imp}, {NOP, mode_imp},
	{NOP, mode_imp}, {SBC, mode_abx}, {INC, mode_abx}, {NOP, mode_imp}
};

static const char *opcode_string[] = {
	"ADC", "AND", "ASL", "BCC", "BCS", "BEQ", "BIT", "BMI",
	"BNE", "BPL", "BRK", "BVC", "BVS", "CLC", "CLD", "CLI",
	"CLV", "CMP", "CPX", "CPY", "DEC", "DEX", "DEY", "EOR",
	"INC", "INX", "INY", "JMP", "JSR", "LDA", "LDX", "LDY",
	"LSR", "NOP", "ORA", "PHA", "PHP", "PLA", "PLP", "ROL",
	"ROR", "RTI", "RTS", "SBC", "SEC", "SED", "SEI", "STA",
	"STX", "STY", "TAX", "TAY", "TSX", "TXA", "TXS", "TYA",
	"ERR"
};

const char *opcodetostring(opcode_t opcode)
{
	if (opcode > sizeof(opcode_string) / sizeof(opcode_string[0]))
		opcode = sizeof(opcode_string) / sizeof(opcode_string[0]);

	return opcode_string[opcode];
}

opinfo_t decode(u8 opcode)
{
	opinfo_t info;
	
	info = decoder_table[opcode];
	
	if (info.opcode == NOP && opcode != 0xea)
		WARN("Invalid instruction 0x%02x interpreted as NOP", opcode);
	
	DEBUG("Decoded 0x%02x as %s", opcode, opcode_string[info.opcode]);
	
	return info;
}
