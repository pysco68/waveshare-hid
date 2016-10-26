Open firmware for Waveshare 7" capacitive touchscreen
=====================================================

An open firmware of the Waveshare 7" Rev 2.1 capacitive touch display. The original firmware wasn't really HID compliant, so I decided to roll my own as Waveshare didn't reply to my support requests.
Most notably the touchscreen would not respond correctly when used with Windows 10 Anniversary Update and later because of some change in behavior of the USB HID driver (discarding malformed HID reports).

The hardware 
------------

(only things relevant for the project)

- GigaDevice GD32F103 mcu
- Goodix GT811 touchscreen controller
	- I2C port connected to I2C2 on GD32F103
	- I2C_SCL and I2C_SDA tied high with 10k SMT resistors
	- /INT pin connected to Port B / Pin 6
	- /RST tied high with 10k resistor
- SWDP port accessible on PCB
- 8 MHz quartz (positive point for stable USB)
- USB correctly terminated on D+/D- lines
	- no CPU controllable Pull-Up/Pull-Down, so no software controlled re-attach



