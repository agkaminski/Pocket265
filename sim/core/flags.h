#ifndef _FLAGS_H_
#define _FLAGS_H_

enum {
	flag_carry = 0x01,
	flag_zero =  0x02,
	flag_irqd =  0x04,
	flag_bcd =   0x08,
	flag_brk =   0x10,
	flag_one =   0x20,
	flag_ovrf =  0x40,
	flag_sign =  0x80
};

#endif
