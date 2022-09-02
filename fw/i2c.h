/* Pocket265 I2C bus 24xx memory driver
 * A.K. 2022
 */

#ifndef I2C_H_
#define I2C_H_

int i2c_write(uint16_t address, uint8_t *data, uint16_t len);

int i2c_read(uint16_t address, uint8_t *data, uint16_t len);

#endif
