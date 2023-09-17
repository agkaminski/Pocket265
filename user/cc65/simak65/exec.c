/* SimAK65 instruction execution
 * Copyright A.K. 2018, 2023
 */

#include "bus.h"
#include "exec.h"
#include "alu.h"
#include "flags.h"
#include "simak65.h"

#define IRQ_VECTOR 0xfffe
#define RST_VECTOR 0xfffc
#define NMI_VECTOR 0xfffa

static void exec_push(struct simak65_cpustate *cpu, u8 data)
{
	u16 addr;

	addr = 0x0100 | cpu->sp;
	--cpu->sp;
	
	bus.write(addr, data);
}

static u8 exec_pop(struct simak65_cpustate *cpu)
{
	u16 addr;
	u8 data;

	++cpu->sp;
	addr = 0x0100 | cpu->sp;

	data = bus.read(addr);

	return data;
}

static void exec_adc(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	cpu->a = alu_add(cpu->a, arg, &cpu->flags);
}

static void exec_and(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	cpu->a = alu_and(cpu->a, arg, &cpu->flags);
}

static void exec_asl(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;
	u8 result;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	result = alu_asl(arg, 0, &cpu->flags);

	if (argtype == arg_addr) {
		bus.write(addr, result);
		*cycles += 1;
	}
	else {
		cpu->a = result;
	}
}

static void exec_bcc(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = (args[1] << 8) | args[0];

	if (!(cpu->flags & FLAG_CARRY)) {
		cpu->pc = addr;
		*cycles += 1;
	}
}

static void exec_bcs(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = (args[1] << 8) | args[0];

	if (cpu->flags & FLAG_CARRY) {
		cpu->pc = addr;
		*cycles += 1;
	}
}

static void exec_beq(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = (args[1] << 8) | args[0];

	if (cpu->flags & FLAG_ZERO) {
		cpu->pc = addr;
		*cycles += 1;
	}
}

static void exec_bit(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	alu_bit(cpu->a, arg, &cpu->flags);
}

static void exec_bmi(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = (args[1] << 8) | args[0];

	if (cpu->flags & FLAG_SIGN) {
		cpu->pc = addr;
		*cycles += 1;
	}
}

static void exec_bne(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = (args[1] << 8) | args[0];

	if (!(cpu->flags & FLAG_ZERO)) {
		cpu->pc = addr;
		*cycles += 1;
	}
}

static void exec_bpl(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = (args[1] << 8) | args[0];

	if (!(cpu->flags & FLAG_SIGN)) {
		cpu->pc = addr;
		*cycles += 1;
	}
}

static void exec_brk(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u8 flags;
	u16 addr;

	(void)argtype;
	(void)args;

	cpu->pc += 1;
	exec_push(cpu, (cpu->pc >> 8) & 0xff);
	exec_push(cpu, cpu->pc & 0xff);

	flags = cpu->flags;
	flags |= FLAG_ONE | FLAG_BRK;
	exec_push(cpu, flags);

	cpu->flags |= FLAG_IRQD;

	addr = bus.read(IRQ_VECTOR);
	addr |= ((u16)bus.read(IRQ_VECTOR + 1) << 8);

	cpu->pc = addr;

	*cycles += 4;
}

static void exec_bvc(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = (args[1] << 8) | args[0];

	if (!(cpu->flags & FLAG_OVRF)) {
		cpu->pc = addr;
		*cycles += 1;
	}
}

static void exec_bvs(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = (args[1] << 8) | args[0];

	if (cpu->flags & FLAG_OVRF) {
		cpu->pc = addr;
		*cycles += 1;
	}
}

static void exec_clc(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->flags &= ~FLAG_CARRY;

	*cycles += 1;
}

static void exec_cld(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->flags &= ~FLAG_BCD;

	*cycles += 1;
}

static void exec_cli(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->flags &= ~FLAG_IRQD;

	*cycles += 1;
}

static void exec_clv(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->flags &= ~FLAG_OVRF;

	*cycles += 1;
}

static void exec_cmp(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	alu_cmp(cpu->a, arg, &cpu->flags);
}

static void exec_cpx(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	alu_cmp(cpu->x, arg, &cpu->flags);
}

static void exec_cpy(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	alu_cmp(cpu->y, arg, &cpu->flags);
}

static void exec_dec(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;
	u8 result;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	result = alu_dec(arg, 0, &cpu->flags);

	if (argtype == arg_addr) {
		bus.write(addr, result);
		*cycles += 1;
	}
	else {
		cpu->a = result;
	}
}

static void exec_dex(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->x = alu_dec(cpu->x, 0, &cpu->flags);

	*cycles += 1;
}

static void exec_dey(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->y = alu_dec(cpu->y, 0, &cpu->flags);

	*cycles += 1;
}

static void exec_eor(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	cpu->a = alu_eor(cpu->a, arg, &cpu->flags);
}

static void exec_inc(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;
	u8 result;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	result = alu_inc(arg, 0, &cpu->flags);

	if (argtype == arg_addr) {
		bus.write(addr, result);
		*cycles += 1;
	}
	else {
		cpu->a = result;
	}
}

static void exec_inx(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->x = alu_inc(cpu->x, 0, &cpu->flags);

	*cycles += 1;
}

static void exec_iny(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->y = alu_inc(cpu->y, 0, &cpu->flags);

	*cycles += 1;
}

static void exec_jmp(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = (args[1] << 8) | args[0];

	cpu->pc = addr;

	*cycles += 1;
}

static void exec_jsr(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = cpu->pc - 1;

	exec_push(cpu, (addr >> 8) & 0xff);
	exec_push(cpu, addr & 0xff);

	addr = (args[1] << 8) | args[0];

	cpu->pc = addr;

	*cycles += 2;
}

static void exec_lda(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	cpu->a = alu_load(arg, 0, &cpu->flags);
}

static void exec_ldx(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	cpu->x = alu_load(arg, 0, &cpu->flags);
}

static void exec_ldy(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	cpu->y = alu_load(arg, 0, &cpu->flags);
}

static void exec_lsr(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;
	u8 result;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	result = alu_lsr(arg, 0, &cpu->flags);

	if (argtype == arg_addr) {
		bus.write(addr, result);
		*cycles += 1;
	}
	else {
		cpu->a = result;
	}
}

static void exec_nop(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)cpu;
	(void)argtype;
	(void)args;

	*cycles += 1;
}

static void exec_ora(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	cpu->a = alu_or(cpu->a, arg, &cpu->flags);
}

static void exec_pha(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	exec_push(cpu, cpu->a);

	*cycles += 2;
}

static void exec_php(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u8 flags;

	(void)argtype;
	(void)args;

	flags = cpu->flags;
	flags |= FLAG_ONE | FLAG_BRK;

	exec_push(cpu, flags);

	*cycles += 2;
}

static void exec_pla(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->a = alu_load(exec_pop(cpu), 0, &cpu->flags);

	*cycles += 2;
}

static void exec_plp(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->flags = exec_pop(cpu);
	cpu->flags &= FLAG_CARRY | FLAG_ZERO | FLAG_IRQD | FLAG_BCD | FLAG_OVRF | FLAG_SIGN;

	*cycles += 2;
}

static void exec_rol(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;
	u8 result;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	result = alu_rol(arg, 0, &cpu->flags);

	if (argtype == arg_addr) {
		bus.write(addr, result);
		*cycles += 1;
	}
	else {
		cpu->a = result;
	}
}

static void exec_ror(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;
	u8 result;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	result = alu_ror(arg, 0, &cpu->flags);

	if (argtype == arg_addr) {
		bus.write(addr, result);
		*cycles += 1;
	}
	else {
		cpu->a = result;
	}
}

static void exec_rti(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;
	(void)args;

	cpu->flags = exec_pop(cpu);
	cpu->flags &= FLAG_CARRY | FLAG_ZERO | FLAG_IRQD | FLAG_BCD | FLAG_OVRF | FLAG_SIGN;

	addr = exec_pop(cpu);
	addr |= (u16)exec_pop(cpu) << 8;

	cpu->pc = addr;

	cycles += 3;
}

static void exec_rts(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;
	(void)args;

	addr = exec_pop(cpu);
	addr |= (u16)exec_pop(cpu) << 8;
	addr += 1;

	cpu->pc = addr;

	cycles += 2;
}

static void exec_sbc(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;
	u8 arg;

	if (argtype == arg_addr) {
		addr = ((u16)args[1] << 8) | args[0];
		arg = bus.read(addr);
		*cycles += 2;
	}
	else {
		arg = args[0];
		*cycles += 1;
	}

	cpu->a = alu_sub(cpu->a, arg, &cpu->flags);
}

static void exec_sec(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->flags |= FLAG_CARRY;

	*cycles += 1;
}

static void exec_sed(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->flags |= FLAG_BCD;

	*cycles += 1;
}

static void exec_sei(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->flags |= FLAG_IRQD;

	*cycles += 1;
}

static void exec_sta(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = ((u16)args[1] << 8) | args[0];

	bus.write(addr, cpu->a);

	*cycles += 2;
}

static void exec_stx(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = ((u16)args[1] << 8) | args[0];

	bus.write(addr, cpu->x);

	*cycles += 2;
}

static void exec_sty(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	u16 addr;

	(void)argtype;

	addr = ((u16)args[1] << 8) | args[0];

	bus.write(addr, cpu->y);

	*cycles += 2;
}

static void exec_tax(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->x = alu_load(cpu->a, 0, &cpu->flags);

	*cycles += 1;
}

static void exec_tay(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->y = alu_load(cpu->a, 0, &cpu->flags);

	*cycles += 1;
}

static void exec_tsx(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->x = alu_load(cpu->sp, 0, &cpu->flags);

	*cycles += 1;
}

static void exec_txa(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->a = alu_load(cpu->x, 0, &cpu->flags);

	*cycles += 1;
}

static void exec_txs(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->sp = cpu->x;

	*cycles += 1;
}

static void exec_tya(struct simak65_cpustate *cpu, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	(void)argtype;
	(void)args;

	cpu->a = alu_load(cpu->y, 0, &cpu->flags);

	*cycles += 1;
}

typedef void (*exec_func_t)(struct simak65_cpustate*, enum argtype, u8*, unsigned int*);
static const exec_func_t exec_instr[] = {
	exec_adc, exec_and, exec_asl, exec_bcc, exec_bcs, exec_beq, exec_bit, exec_bmi,
	exec_bne, exec_bpl, exec_brk, exec_bvc, exec_bvs, exec_clc, exec_cld, exec_cli,
	exec_clv, exec_cmp, exec_cpx, exec_cpy, exec_dec, exec_dex, exec_dey, exec_eor,
	exec_inc, exec_inx, exec_iny, exec_jmp, exec_jsr, exec_lda, exec_ldx, exec_ldy,
	exec_lsr, exec_nop, exec_ora, exec_pha, exec_php, exec_pla, exec_plp, exec_rol,
	exec_ror, exec_rti, exec_rts, exec_sbc, exec_sec, exec_sed, exec_sei, exec_sta,
	exec_stx, exec_sty, exec_tax, exec_tay, exec_tsx, exec_txa, exec_txs, exec_tya
};

void exec_execute(struct simak65_cpustate *cpu, enum opcode instruction, enum argtype argtype, u8 *args, unsigned int *cycles)
{
	if (instruction > sizeof(exec_instr) / sizeof(exec_instr[0])) {
		exec_nop(cpu, argtype, args, cycles);
	}

	exec_instr[instruction](cpu, argtype, args, cycles);
}

void exec_irq(struct simak65_cpustate *cpu, unsigned int *cycles)
{
	u8 flags;

	exec_push(cpu, (cpu->pc >> 8) & 0xff);
	exec_push(cpu, cpu->pc & 0xff);

	flags = cpu->flags;
	flags |= FLAG_ONE;
	flags &= ~FLAG_BRK;
	exec_push(cpu, flags);

	cpu->pc = bus.read(IRQ_VECTOR);
	cpu->pc |= (u16)bus.read(IRQ_VECTOR + 1) << 8;

	cpu->flags |= FLAG_IRQD;

	*cycles += 7;
}

void exec_nmi(struct simak65_cpustate *cpu, unsigned int *cycles)
{
	u8 flags;

	exec_push(cpu, (cpu->pc >> 8) & 0xff);
	exec_push(cpu, cpu->pc & 0xff);

	flags = cpu->flags;
	flags |= FLAG_ONE;
	flags &= ~FLAG_BRK;
	exec_push(cpu, flags);

	cpu->pc = bus.read(NMI_VECTOR);
	cpu->pc |= (u16)bus.read(NMI_VECTOR + 1) << 8;

	cpu->flags |= FLAG_IRQD;

	*cycles += 7;
}

void exec_rst(struct simak65_cpustate *cpu, unsigned int *cycles)
{
	cpu->a = 0;
	cpu->x = 0;
	cpu->y = 0;
	cpu->flags = FLAG_ONE;
	cpu->sp = 0xff;

	cpu->pc = bus.read(RST_VECTOR);
	cpu->pc |= (u16)bus.read(RST_VECTOR + 1) << 8;

	*cycles += 4;
}
