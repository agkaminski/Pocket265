#include "common/error.h"
#include "alu.h"
#include "flags.h"

void alu_flags(u8 result, u8 *flags, u8 mask)
{
	if (mask & flag_zero) {
		if ((result & 0xff) == 0)
			*flags |= flag_zero;
		else
			*flags &= ~flag_zero;
	}

	if (mask & flag_sign) {
		if (result & 0x80)
			*flags |= flag_sign;
		else
			*flags &= ~flag_sign;
	}
}

u8 alu_add(u8 a, u8 b, u8 *flags)
{
	u16 result, ai, bi;
	u8 carry_in = !!(*flags & flag_carry);

	ai = (u16)a;
	bi = (u16)b;

	result = ai + bi + carry_in;

	if (*flags & flag_bcd) {
		if ((ai & 0xf) + (bi & 0xf) + carry_in > 9)
			result += 0x06;

		if ((result >> 4) > 9)
			result += 0x60;
	}

	if (result < ai || result < bi || (result & 0xff00) != 0)
		*flags |= flag_carry;
	else
		*flags &= ~flag_carry;

	alu_flags(result & 0xff, flags, flag_sign | flag_zero);

	if ((ai ^ result) & (bi ^ result) & 0x80)
		*flags |= flag_ovrf;
	else
		*flags &= ~flag_ovrf;

	DEBUG("0x%02x + 0x%02x + %d = 0x%02x, flags 0x%02x", ai & 0xff, bi & 0xff, carry_in, result & 0xff, *flags);

	return result & 0xff;
}

u8 alu_sub(u8 a, u8 b, u8 *flags)
{
	u16 result, ai, bi;
	u8 carry_in = !!(*flags & flag_carry);

	ai = (u16)a;

	if (*flags & flag_bcd)
		bi = (u16)((0x99 - b) & 0xff);
	else
		bi = (u16)(~b & 0xff);

	result = ai + bi + carry_in;

	if (*flags & flag_bcd) {
		if ((ai & 0xf) + (bi & 0xf) + carry_in > 9)
			result += 0x06;

		if ((result >> 4) > 9)
			result += 0x60;
	}

	if (result < ai || result < bi || (result & 0xff00) != 0)
		*flags |= flag_carry;
	else
		*flags &= ~flag_carry;

	alu_flags(result & 0xff, flags, flag_sign | flag_zero);

	if ((ai ^ result) & (bi ^ result) & 0x80)
		*flags |= flag_ovrf;
	else
		*flags &= ~flag_ovrf;

	DEBUG("0x%02x - 0x%02x + %d = 0x%02x, flags 0x%02x", ai & 0xff, bi & 0xff, carry_in, result & 0xff, *flags);

	return result & 0xff;
}

u8 alu_inc(u8 a, u8 b, u8 *flags)
{
	u8 result;

	result = a + 1;

	alu_flags(result, flags, flag_sign | flag_zero);

	DEBUG("0x%02x + 1 = 0x%02x, flags 0x%02x", a & 0xff, result & 0xff, *flags);

	return result;
}

u8 alu_dec(u8 a, u8 b, u8 *flags)
{
	u8 result;

	result = a - 1;

	alu_flags(result, flags, flag_sign | flag_zero);

	DEBUG("0x%02x - 1 = 0x%02x, flags 0x%02x", a & 0xff, result & 0xff, *flags);

	return result;
}

u8 alu_and(u8 a, u8 b, u8 *flags)
{
	u8 result;

	result = a & b;

	alu_flags(result, flags, flag_sign | flag_zero);

	DEBUG("0x%02x & 0x%02x = 0x%02x, flags 0x%02x", a & 0xff, b & 0xff, result & 0xff, *flags);

	return result;
}

u8 alu_or(u8 a, u8 b, u8 *flags)
{
	u8 result;

	result = a | b;

	alu_flags(result, flags, flag_sign | flag_zero);

	DEBUG("0x%02x | 0x%02x = 0x%02x, flags 0x%02x", a & 0xff, b & 0xff, result & 0xff, *flags);

	return result;
}

u8 alu_eor(u8 a, u8 b, u8 *flags)
{
	u8 result;

	result = a ^ b;

	alu_flags(result, flags, flag_sign | flag_zero);

	DEBUG("0x%02x ^ 0x%02x = 0x%02x, flags 0x%02x", a & 0xff, b & 0xff, result & 0xff, *flags);

	return result;
}

u8 alu_rol(u8 a, u8 b, u8 *flags)
{
	u8 result;

	result = a << 1;

	if (*flags & flag_carry)
		result |= 1;

	alu_flags(result, flags, flag_sign | flag_zero);

	if (a & 0x80)
		*flags |= flag_carry;
	else
		*flags &= ~flag_carry;

	DEBUG("c << 0x%02x << c = 0x%02x, flags 0x%02x", a & 0xff, result & 0xff, *flags);

	return result;
}

u8 alu_ror(u8 a, u8 b, u8 *flags)
{
	u8 result;

	result = a >> 1;

	if (*flags & flag_carry)
		result |= 0x80;

	alu_flags(result, flags, flag_sign | flag_zero);

	if (a & 0x01)
		*flags |= flag_carry;
	else
		*flags &= ~flag_carry;

	DEBUG("c >> 0x%02x >> c = 0x%02x, flags 0x%02x", a & 0xff, result & 0xff, *flags);

	return result;
}

u8 alu_asl(u8 a, u8 b, u8 *flags)
{
	u8 result;

	result = a << 1;

	alu_flags(result, flags, flag_sign | flag_zero);

	if (a & 0x80)
		*flags |= flag_carry;
	else
		*flags &= ~flag_carry;

	DEBUG("c << 0x%02x << 0 = 0x%02x, flags 0x%02x", a & 0xff, result & 0xff, *flags);

	return result;
}

u8 alu_lsr(u8 a, u8 b, u8 *flags)
{
	u8 result;

	result = a >> 1;

	alu_flags(result, flags, flag_sign | flag_zero);

	if (a & 0x01)
		*flags |= flag_carry;
	else
		*flags &= ~flag_carry;

	DEBUG("0 >> 0x%02x >> c = 0x%02x, flags 0x%02x", a & 0xff, result & 0xff, *flags);

	return result;
}

u8 alu_bit(u8 a, u8 b, u8 *flags)
{
	u8 result;

	result = a & b;

	alu_flags(result, flags, flag_zero);

	if (b & 0x40)
		*flags |= flag_ovrf;
	else
		*flags &= ~flag_ovrf;

	if (b & 0x80)
		*flags |= flag_sign;
	else
		*flags &= ~flag_sign;

	DEBUG("0x%02x & 0x%02x = 0x%02x, flags 0x%02x", a & 0xff, b & 0xff, result & 0xff, *flags);

	return result;
}

u8 alu_cmp(u8 a, u8 b, u8 *flags)
{
	u16 result, ai, bi;

	ai = (u16)a;
	bi = (u16)(~b & 0xff);

	result = ai + bi + 1;

	if (result < ai || result < bi || (result & 0xff00) != 0)
		*flags |= flag_carry;
	else
		*flags &= ~flag_carry;

	alu_flags(result & 0xff, flags, flag_sign | flag_zero);

	DEBUG("0x%02x - 0x%02x = 0x%02x, flags 0x%02x", ai & 0xff, bi & 0xff, result & 0xff, *flags);
	DEBUG("0x%04x - 0x%04x = 0x%04x, flags 0x%02x", ai, bi, result, *flags);

	return result;
}

u8 alu_load(u8 a, u8 b, u8 *flags)
{
	alu_flags(a, flags, flag_zero | flag_sign);

	DEBUG("0x%02x, flags 0x%02x", a & 0xff, *flags);

	return a;
}
