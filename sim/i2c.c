/* 24xx I2C memory simulator
 * Copyright A.K. 2023
 */

#include <stdlib.h>
#include <string.h>
#include "i2c.h"

void i2c_step(struct i2c_ctx *ctx, const struct i2c_bus *in, struct i2c_bus *out)
{
	*out = ctx->prevout;

	if (in->scl) {
		if (ctx->previn.sda && !in->sda) {
			/* Start condition */
			ctx->state = i2c_dev_addr;
			ctx->regbit = 0;
		}
		else if (!ctx->previn.sda && in->sda) {
			/* Stop condtion */
			ctx->state = i2c_idle;
		}
	}

	if (in->scl && !ctx->previn.scl) {
		/* Rising edge */
		switch (ctx->state) {
			case i2c_dev_addr:
				ctx->reg = (ctx->reg << 1) | !!in->sda;
				if (++ctx->regbit > 7) {
					ctx->regbit = 0;
					if (ctx->devaddr == (ctx->reg >> 1)) {
						if (ctx->reg & 1)
							ctx->next = i2c_read;
						else
							ctx->next = i2c_addr;
						ctx->state = i2c_ack;
					}
					else {
						ctx->state = i2c_idle;
					}
				}
				break;

			case i2c_addr:
				ctx->address = (ctx->address << 1) | !!in->sda;
				++ctx->regbit;
				if (ctx->regbit == 8) {
					ctx->state = i2c_ack;
					ctx->next = i2c_addr;
				}
				else if (ctx->regbit > 15) {
					ctx->regbit = 0;
					ctx->next = i2c_write;
					ctx->state = i2c_ack;
				}
				break;

			case i2c_write:
				ctx->reg = (ctx->reg << 1) | !!in->sda;
				if (++ctx->regbit > 7) {
					ctx->regbit = 0;
					ctx->mem[ctx->address % ctx->memsz] = ctx->reg;
					++ctx->address;
					ctx->next = i2c_write;
					ctx->state = i2c_ack;
				}
				break;

			case i2c_read:
				++ctx->regbit;
				if (ctx->regbit == 9) {
					if (in->sda) {
						ctx->state = i2c_idle;
						ctx->regbit = 0;
					}
					else {
						ctx->regbit = 0;
						++ctx->address;
					}
				}
				break;

			case i2c_ack:
				ctx->state = ctx->next;
				break;

			default:
				break;
		}
	}
	else if (!in->scl && ctx->previn.scl) {
		/* Falling ege */
		switch (ctx->state) {
			case i2c_ack:
				out->sda = 0;
				break;

			case i2c_read:
				if (ctx->regbit == 0)
					ctx->reg = ctx->mem[ctx->address % ctx->memsz];

				if (ctx->regbit <= 7) {
					out->sda = !!(ctx->reg & 0x80);
					ctx->reg <<= 1;
				}
				else {
					out->sda = 1;
				}
				break;

			default:
				out->sda = 1;
				break;
		}
	}

	ctx->previn = *in;
	ctx->prevout = *out;
}

void i2c_destroy(struct i2c_ctx *ctx)
{
	free(ctx->mem);
	ctx->mem = NULL;
}

int i2c_init(struct i2c_ctx *ctx, size_t memsz, uint8_t devaddr)
{
	ctx->previn = (struct i2c_bus){ .sda = 1, .scl = 1 };
	ctx->prevout = (struct i2c_bus){ .sda = 1, .scl = 1 };

	ctx->state = i2c_idle;
	ctx->address = 0;
	ctx->reg = 0;
	ctx->regbit = 0;
	ctx->devaddr = devaddr;

	ctx->mem = malloc(memsz);
	if (ctx->mem == NULL) {
		return -1;
	}
	ctx->memsz = memsz;

	return 0;
}
