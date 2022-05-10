Open firmware for Waveshare 7" capacitive touchscreen
=====================================================

An open firmware of the Waveshare 7" Rev 2.1 capacitive touch display. The original firmware wasn't really HID compliant, so I decided to roll my own as Waveshare didn't reply to my support requests.
Most notably the touchscreen would not respond correctly when used with Windows 10 Anniversary Update and later because of some change in behavior of the USB HID driver (discarding malformed HID reports).

Hardware versions & other vendors
---------------------------------

Since I published this firmware and blogged about it, a few people around the world tried to flash the firmware on diffenrent screen models with similar hardware; here are a screens that are known to work so far:

- Waveshare 7" capacitive touch screen model 800x480
- Waveshare 7" capacitive touch screen model (B / 800x600 pixel / rev 2.1)
- Waveshare 7" capacitive touch screen model (C / 1024x600 pixel / rev 1.1 and rev 2.1)
- Eleduino 7" HDMI capacitive touch screen (1024x600 pixel / version 150830)

LCD Requirements 
-----------------

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

SW Requirements
---------------
- GCC / ARM toolchain (arm-none-eabi-gcc) - Failed to compile on Pi or MacOS - Ubuntu works great. 
- SWDP/JTAG for STM32: eg: StLink v2 & friends.

Instructions
------------

WARNING: there is no cheap was of backing up the original firmware from Waveshare so it'll be gone when you flash this firmware. I can and will not take any responsibility for a bricked screen, you must know what you are doing starting from here!! To say it clearly once again: I'm not liable for any damage to your computer system or touch screen!

The first thing you will have to do is solder some connection wires to the SWDP "header".
When you look at the screen's PCB (looking at silkscreen upright) the contacts are at the bottom right corner, close to the GD32F103.
Top to bottom:

	- SWDIO
	- SWCLK
	- GND

You can eitheir download the pre-compiled ELF or compile manually:

	git clone https://github.com/filipenobrerc/waveshare-hid-xAxisFix.git
	cd waveshare-hid/libopencm3
	make 	# compile the libopencm3/GD32 fork
	cd ../src
	make	# compile the new touchscreen firmware


The easiest way to flash resulting main.elf is with STM32CubeProgrammer.

After flashing the touchscreen should be recognized as a classic HID touchscreen and work in Linux/MacOS/Windows out of the box.


Status
------

What works:
- Five point multitouch
- working under Ubuntu 16.10
- working under Raspbian Jessie
- working under Windows 10 Anniversary Update (didn't really work with original firmware)
- **working under Windows 10 IoT Anniversary Update on RaspberryPi (didn't work with original firmware either)**
- working under Android 4.2.2


Many thanks to pysco68!!
