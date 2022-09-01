/* Pocket265 software timer
 * A.K. 2022
 */

#ifndef TIMER_H_
#define TIMER_H_

unsigned char timer_get_jiffies(void);

unsigned char timer_get_seconds(void);

void timer_wait_ms(unsigned char ms);

void timer_wait_sec(unsigned char sec);

#endif
