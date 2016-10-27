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

#ifndef USB_SETUP_H_INCLUDED
#define USB_SETUP_H_INCLUDED

#include <libopencm3/usb/usbd.h>

#define ENDPOINT_ADDRESS 0x81

void setup_usb(void);
void poll_usb(void);
void hid_set_config(usbd_device *cb_usbd_dev, uint16_t wValue);
int hid_control_request(usbd_device *cb_usbd_dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete);

#endif