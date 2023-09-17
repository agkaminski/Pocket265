/* 24xx I2C memory simulator
 * Copyright A.K. 2018, 2023
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <stddef.h>

struct i2c_bus {
	int sda;
	int scl;
};

enum i2c_state {
	i2c_idle,
	i2c_dev_addr,
	i2c_addr,
	i2c_read,
	i2c_write,
	i2c_ack
};

struct i2c_ctx {
	struct i2c_bus previn;
	struct i2c_bus prevout;

	uint8_t *mem;
	size_t memsz;

	uint16_t address;

	uint8_t devaddr;

	uint8_t reg;
	uint8_t regbit;

	enum i2c_state state;
	enum i2c_state next;
};

void i2c_step(struct i2c_ctx *ctx, const struct i2c_bus *in, struct i2c_bus *out);

void i2c_destroy(struct i2c_ctx *ctx);

int i2c_init(struct i2c_ctx *ctx, size_t memsz, uint8_t devaddr);

#endif
