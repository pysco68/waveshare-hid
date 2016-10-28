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

#include <stdlib.h>
#include <libopencm3/gd32/rcc.h>

#include "usb_device.h"
#include "systick.h"
#include "gt811.h"

int main(void)
{
	uint32_t delayCnt;

	// setup the CPU
	rcc_clock_setup_in_hse_8mhz_out_72mhz();    // 72MHz clock using on-board crystal / HSE

    // start the clock
    setup_systick();

    // setup the I2C bus and configure the GT811 touchscreen controller
    setup_gt811();

    // setup the USB
    setup_usb();

    // wait for the USB port to come up...
    for(delayCnt = 0; delayCnt < 100000; delayCnt++)
        __asm__("nop");       


	while (1) {
        poll_usb();
        gt811_poll();
	}
}