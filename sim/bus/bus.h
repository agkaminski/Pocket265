#ifndef _BUS_H_
#define _BUS_H_

#include "common/types.h"

typedef struct {
	u16 begin;
	u16 end;
	void (*write)(u16, u8);
	u8 (*read)(u16);
} busentry_t;

void bus_write(u16 addr, u8 data);

u8 bus_read(u16 addr);

void bus_register(busentry_t entry);

#endif
