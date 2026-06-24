#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

void TIMER1_init(void);

uint16_t TIMER_get_ticks(void);

#endif