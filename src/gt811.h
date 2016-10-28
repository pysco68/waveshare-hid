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

#ifndef GT811_H_INCLUDED
#define GT811_H_INCLUDED

// I2C read and write addresses as per datasheet
#define GT811_ADDRESS 0xBA 
#define GT811_REGISTERS_CONFIGURATION 0x6A2
#define GT811_REGISTERS_READ 0x721

void setup_gt811(void);
void gt811_read_register(uint16_t reg, uint8_t size, uint8_t *data);
void gt811_write_register(uint16_t reg, uint8_t size, uint8_t *data);
void gt811_poll(void);

#endif /* GT811_H_INCLUDED */
