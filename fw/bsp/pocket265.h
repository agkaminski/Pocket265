/* Pocket265 platform header
 * A.K. 2022
 */

#ifndef POCKET265_H_
#define POCKET265_H_

/* I/O map */
#define KEYBOARD_BASE  0xC000
#define SCREEN_BASE    0xC400
#define NMI_CLEAR_BASE 0xC800
#define GPIO_BASE      0xCC00

/* Key codes */
#define KEY_INC 16
#define KEY_DEC 17
#define KEY_SEL 18
#define KEY_GO 19
#define KEY_F1 20
#define KEY_F2 21
#define KEY_F3 22
#define KEY_F4 23
#define KEY_NONE 0xff

/* Time base */
#define NMI_HZ 1024

#endif
