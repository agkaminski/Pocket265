/* Pocket265 platform header
 * A.K. 2022
 */

#ifndef POCKET265_H_
#define POCKET265_H_

/* User memory */
#define ZP_USR_START  __ZP_USER_START__
#define ZP_USR_SIZE   __ZP_USER_SIZE__
#define RAM_USR_START __RAM_USER_START__
#define RAM_USR_SIZE  __RAM_USER_SIZE__

/* Memory map */
#define RAM_BASE 0x1000
#define RAM_SIZE 0x2000
#define EXT1_BASE 0x2000
#define EXT1_SIZE 0x2000
#define EXT2_BASE 0x4000
#define EXT2_SIZE 0x2000
#define EXT3_BASE 0x6000
#define EXT3_SIZE 0x2000
#define EXT4_BASE 0x8000
#define EXT4_SIZE 0x2000
#define EXT5_BASE 0xA000
#define EXT5_SIZE 0x2000
#define ROM_BASE 0xE000
#define ROM_SIZE 0x2000

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
#define KEY_NONE 24

/* Time base */
#define NMI_HZ 1024

#endif
