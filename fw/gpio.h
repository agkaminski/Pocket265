/* Pocket265 GPIO driver
 * A.K. 2022
 */

#ifndef GPIO_H_
#define GPIO_H_

#define GPIO_TX 0
#define GPIO_RX 0

#define GPIO_SDA 1
#define GPIO_SCL 2

#define GPIO_ROM_RDY 3
#define GPIO_ROM_WP  3

void gpio_setPin(char pin, char state);

char gpio_getPin(char pin);

void gpio_init(void);

#endif

