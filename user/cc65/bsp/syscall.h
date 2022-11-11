/* Pocket265 system calls prototypes
 * A.K. 2022
 */

#ifndef POCKET265_SYSCALL_H_
#define POCKET265_SYSCALL_H_

#include <stdint.h>

extern void svc_irqRegister(void (*callback)(void));

extern void svc_nmiRegister(void (*callback)(void));

extern void svc_putch(char c);

extern void svc_puts(char *s);

extern void svc_putb(unsigned char b);

extern void svc_printxU16(uint16_t val);

extern void svc_printdU16(uint16_t val);

extern void svc_printdS16(uint16_t val);

extern uint16_t svc_scanxU16(char *prompt);

extern uint16_t svc_scandU16(char *prompt);

extern unsigned char svc_getKey(void);

extern unsigned char svc_getKeyNb(void);

extern unsigned char svc_getJiffies(void);

extern unsigned char svc_getSeconds(void);

extern void svc_msleep(unsigned char ms);

extern void svc_sleep(unsigned char s);

extern void svc_nmiDisable(void);

extern void svc_nmiStart(void);

#endif

