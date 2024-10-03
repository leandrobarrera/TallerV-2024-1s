/*
 * systick_driver_hal.h
 *
 *  Created on: 27/09/2024
 *      Author: lbarreras
 */

#ifndef SYSTICKDRIVER_H_
#define SYSTICKDRIVER_H_

#include<stm32f4xx.h>

#define SYSTICK_LOAD_VALUE_16Mhz_1ms 	16000
#define SYSTICK_LOAD_VALUE_100Mhz_1ms 	100000


void config_SysTick_ms(uint8_t systemClock);
uint64_t getTicks_ms(void);
void systick_Delay_ms(uint32_t wait_time_ms);

#endif /*SYSTICKDRIVER_H_*/
