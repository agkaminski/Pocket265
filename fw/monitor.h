/* Pocket265 monitor
 * A.K. 2022
 */

#ifndef MONITOR_H_
#define MONITOR_H_

void monitor_run(void);

void monitor_getu16param(const char *prompt, uint16_t *param, char type);

void monitor_displayProgress(unsigned int curr, unsigned int total);

#endif
