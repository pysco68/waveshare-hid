/**
	Open firmware for Waveshare 7" capacitive touchscreen 
	Copyright (C) 2016 Yannic Staudt / Staudt Engineering

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.

 */	

#include <libopencm3/gd32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include "systick.h"

// we define ticks as 1ms
uint32_t ticks = 0;

void sys_tick_handler(void)
{
    // simply increment that!
	ticks++;
}

void setup_systick(void)
{
    // 72MHz / 8 => 9000000 counts per second 
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

	// 9000000/9000 = 1000 overflows per second - every 1ms one interrupt
	// SysTick interrupt every N clock pulses: set reload to N-1
	systick_set_reload(8999);

    // Start counting
	systick_interrupt_enable();
    systick_counter_enable();
}

// sleep for t milliseconds
void msleep(uint32_t delay)
{
	uint64_t wake = ticks + delay;
	while (wake > ticks);
}

uint16_t get_scan_time(void)
{
    return (uint16_t)(ticks & 0xFFFF);
}