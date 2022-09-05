/* Pocket265 interrupts
 * A.K. 2022
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

extern volatile uint16_t g_nmi_callback;
extern volatile uint16_t g_irq_callback;
extern volatile uint8_t g_nmi_valid;
extern volatile uint8_t g_irq_valid;
extern volatile uint8_t g_nmi_disable;

void nmi_clr(void);

#endif
