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

int main(void)
{
	// setup the CPU
	rcc_clock_setup_in_hse_8mhz_out_72mhz();    // 72MHz clock using on-board crystal / HSE

	// setup the i2c communication
    setup_gt811();

	uint8_t buf[34];

	while (1) {

		// read the full touch report
		gt811_read_register(0x721, 34, buf);

	}
}