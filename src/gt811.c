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

#include <libopencm3/gd32/i2c.h>
#include <libopencm3/gd32/gpio.h>
#include <libopencm3/gd32/rcc.h>
#include "usb_device.h"
#include "systick.h"
#include "gt811.h"

// this is the GT811 configuration that will be written
// to the chip during setup
// extracted from https://github.com/virajkanwade/rk3188_android_kernel/blob/e6afb80688d7f5a3628dce8d733f653abd7f4a53/drivers/input/touchscreen/gt811_ts.c
// annotations from the GT811 datasheet / google translated from chinese
static const uint8_t gt811_config[]=
{
    // [0 - 9] TS induction wire configuration
    0x12,0x10,0x0E,0x0C,0x0A,0x08,0x06,0x04,0x02,0x00,

    // [10 - 41] channels configuration
    0x05,0x55,0x15,0x55,0x25,0x55,0x35,0x55,0x45,0x55,0x55,
    0x55,0x65,0x55,0x75,0x55,0x85,0x55,0x95,0x55,0xA5,0x55,
    0xB5,0x55,0xC5,0x55,0xD5,0x55,0xE5,0x55,0xF5,0x55,

    // [42 - 43] Chip scan control parameters  
    0x1B,0x03,

    // [44 - 46] Drive pulse frequency 1, 2, 3
    0x00,0x00,0x00,

    // [47 - 49] Number of drive pulses 1, 2, 4
    0x13,0x13,0x13,
    
    // [50] The total number of drive channels used (screen drive line + key drive line)							
    0x0F,

    // [51] Use the drive wire on the screen    
    0x0F,

    // [52] Use a sense line on the screen
    0x0A,

    // [53] The screen key threshold
    0x50,
    
    // [54] Screen loose threshold
    0x30,
    
    // [55] 
    // b8 Reserved	
    // b7 DD2
    // b6 R1
    // b5 R0
    // b4 INT
    // b3 SITO
    // b2 RT
    // b1 ST
    0x05,
    
    // [56]
    // b8 Reserved
    // b7 Reserved
    // b6-b1 Auto No button to enter the low-power time, 0-63 effective to s as a unit	
    0x03,
    
    // [57] Touch Touch refresh rate control parameter (50Hz - 100Hz): 0-100 effective							
    0x64,
    
    // [58] Number of touch points (1 - 5)
    // b8-b4 reserved
    0x05,
    
    // [59 - 60] X coordinate outputs the maximum value							
    0xe0,0x01,  // 480

    // [61 - 62] The Y coordinate outputs the maximum value							
    0x20,0x03,  // 800
    
    // [63]  X coordinate output threshold: 0-255, with 4 original coordinate points as a unit 							
    0x00,

    // [64] Y coordinate output threshold: 0-255, with 4 original coordinate points as a unit							
    0x00,

    // [65] X direction smoothing control variable, 0-255 can be configured, 0 means off							
    0x32,
    
    // [66] Y direction smoothing control variable, 0-255 can be configured, 0 means off						
    0x2C,
    
    // [67] X direction Smooth upper limit speed: 0-255 can be configured, 0 means off							
    0x34,
    
    // [68] Y direction Smooth upper limit speed: 0-255 can be configured, 0 means off							
    0x2E,
    
    // Reserved
    0x00,0x00,
    
    // [71] Filter
    // b8-b5: Number of discarded data frames			
    // b4-b1: The coordinate window filter value, with 4 as the base			
    0x04,
    
    // [72] 0-255 effective: a single touch area contains more than the number of nodes will be judged as a large area touch							
    0x14,
    
    // [73] Shake_Cu
    // b8-b5: The Touch event is created to debounce						
    // b4-b1: The number of fingers from more to less to shake				
    0x22,
    
    // [74] Noise reduction
    // b8-b5: Reserved				
    // b4-b1: The white noise reduction amount (low nibble) is valid			
    0x04,
    
    // reserved
    0x00,0x00,0x00,0x00,0x00,

    // [80] Normal Update Time, 0-255 Configurable, Zero Off Base Update (Base Cycle Time)							
    0x20,
    
    // [81] 0-255 Configurable, Zero Closed Base Update (based on the main cycle time)							
    0x14,
    
    // [82 - 83] The baseline updates the control variables
    0xEC,0x01,
    
    // [84] Reserved
    0x00,
    
    // [85] 
    // b7: button command
    // b6: button independent parameter (007A follow-up version is valid)
    // b5-b0: reserved
    0x00,
    
    // [86] FPC button ADCCFG parameter (applies only to the drive as the button common)							
    0x00,
    
    // [87] FPC button drive frequency selection (only for the drive as the public key)							
    0x00,
    
    // [88] FPC key drive pulse number (only for driving the button common)
    0x00,
    
    // [89] Reserved
    0x00,
    
    // [90 - 93] Key 1 to 4 Position: 0-255 (independent keys when all key positions are multiples of 16)							
    0x00,0x00,0x00,0x00,
    
    // [95]
    // b7-b4: reserved
    // b3-b0: key effective width (one side) 
    0x0C,
    
    // [96] Key press threshold
    0x30,

    // [96] Key release threshold
    0x25,
    
    // [97] Independent key judgment, the second largest difference in the upper limit
    0x28,
    
    // [98] Independent key to determine the maximum and maximum gap between the lower limit							
    0x14,
    
    // [99 - 103] Reserved
    0x00,0x00,0x00,0x00,0x00,
    
    // [104] Configuration update flag, the master write configuration information to write to the location 1
    0x00,
    
    // [105] ??no clue??
    0x01   
    
}; 

// write n bytes to GT811 starting at
void gt811_write_register(uint16_t reg, uint8_t size, uint8_t *data)
{
    uint32_t reg32 __attribute__((unused));
    const uint32_t i2c = I2C2;

    /* Send START condition. */
	i2c_send_start(i2c);
    i2c_enable_ack(i2c);

	/* Waiting for START is send and switched to master mode. */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

    //send address write
    i2c_send_7bit_address(I2C2, GT811_ADDRESS >> 1, I2C_WRITE);
    
    /* Waiting for address is transferred. */
    while (!(I2C_SR1(I2C2) & I2C_SR1_ADDR));    

	/* Cleaning ADDR condition sequence. */
	reg32 = I2C_SR2(i2c);    

	i2c_send_data(i2c, reg >> 8); // register MSB
	while (!(I2C_SR1(I2C2) & (I2C_SR1_BTF | I2C_SR1_TxE)));

    i2c_send_data(i2c, reg & 0xFF); // register LSB
	while (!(I2C_SR1(I2C2) & (I2C_SR1_BTF | I2C_SR1_TxE)));

    int8_t i;

    for(i = 0; i < size; i++)
    {
        i2c_send_data(i2c, data[i]); // send data byte
	    while (!(I2C_SR1(I2C2) & (I2C_SR1_BTF | I2C_SR1_TxE)));
    }

    i2c_send_stop(i2c);
}

// read n bytes from GT811 given the start register
void gt811_read_register(uint16_t reg, uint8_t size, uint8_t *data)
{
	uint32_t reg32 __attribute__((unused));
	const uint32_t i2c = I2C2;

	/* Send START condition. */
	i2c_send_start(i2c);
    i2c_enable_ack(i2c);

	/* Waiting for START is send and switched to master mode. */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

    //send address write
    i2c_send_7bit_address(I2C2, GT811_ADDRESS >> 1, I2C_WRITE);
    
    /* Waiting for address is transferred. */
    while (!(I2C_SR1(I2C2) & I2C_SR1_ADDR));    

	/* Cleaning ADDR condition sequence. */
	reg32 = I2C_SR2(i2c);    

	i2c_send_data(i2c, reg >> 8); // register MSB
	while (!(I2C_SR1(I2C2) & (I2C_SR1_BTF | I2C_SR1_TxE)));

    i2c_send_data(i2c, reg & 0xFF); // register LSB
	while (!(I2C_SR1(I2C2) & (I2C_SR1_BTF | I2C_SR1_TxE)));

	/* RE-START */
	i2c_send_start(i2c);

	/* Waiting for START is send and switched to master mode. */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	/* Say to what address we want to talk to. */
	i2c_send_7bit_address(i2c, GT811_ADDRESS >> 1, I2C_READ);  

    if(size == 1)
    { 
        i2c_disable_ack(i2c);
    } 
    else if(size == 2)
    {
        /* 2-byte receive is a special case. See datasheet POS bit. */
        I2C_CR1(i2c) |= (I2C_CR1_POS | I2C_CR1_ACK);
    }

	/* Waiting for address is transferred. */
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

	/* Cleaning ADDR condition sequence. */
	reg32 = I2C_SR2(i2c);
  
    uint8_t i;

    if(size == 1)
    {
        while (!(I2C_SR1(i2c) & (I2C_SR1_RxNE))); 
        data[0] = i2c_get_data(i2c);
        
        i2c_send_stop(i2c);
    }
    else if(size == 2)
    {
        i2c_disable_ack(i2c); 

        while (!(I2C_SR1(i2c) & (I2C_SR1_BTF)));        
        data[0] = i2c_get_data(i2c);   
        
        i2c_send_stop(i2c);

        while (!(I2C_SR1(i2c) & (I2C_SR1_RxNE)));
        data[1] = i2c_get_data(i2c); 

        I2C_CR1(i2c) &= ~I2C_CR1_POS;    
    }
    else
    {

        for (i = 0; i < size; i++)
        {
            if(i == size - 2)
            {
                i2c_disable_ack(i2c);
                while (!(I2C_SR1(i2c) & (I2C_SR1_BTF)));
            }
            else if(i == size - 1)
            {
                i2c_send_stop(i2c);
                while (!(I2C_SR1(i2c) & (I2C_SR1_RxNE)));
            }
            else 
            {
                while (!(I2C_SR1(i2c) & (I2C_SR1_BTF)));
            }

            
            data[i] = i2c_get_data(i2c);      
        }
    }    
}

#define TOUCH_POINTS_MAX 5

///
// stores one touch report and the information if the
// "touch up" event was already send for this fingers
// current touch sequence
struct touch_report
{
    uint8_t report[11];
    bool touch_up_sent;   // remember if we already sent a touch up report for this finger
};

///
// buffers for all fingers worth of HID reports
struct touch_report touch_reports[TOUCH_POINTS_MAX];

///
// GT811 receive buffer
uint8_t data[34];

///
// variables for gt811_poll() processing loop
uint8_t i, currentFinger, validTouchPoints, offsetData, sentReports;
uint16_t y_value, inverted_y, scan_time;
struct touch_report *hid_report;

///
// setup routine
void setup_gt811(void)
{
    /** setup GPIO */
	rcc_periph_clock_enable(RCC_GPIOB);     // enable clock for IO port B  

    /** setup I2C2 */
    rcc_periph_clock_enable(RCC_I2C2);      // Enable clocks for I2C2
	rcc_periph_clock_enable(RCC_AFIO);      // and AFIO
	
    gpio_set_mode(GPIOB,                    // Set alternate functions for the SCL and SDA pins of I2C2. 
        GPIO_MODE_OUTPUT_50_MHZ,   
        GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
        GPIO_I2C2_SCL | GPIO_I2C2_SDA);            

	i2c_peripheral_disable(I2C2);           // Disable the I2C before changing any configuration.
	i2c_set_clock_frequency(I2C2,           // APB2 is running at 1/2 system clock = 36MHz
        I2C_CR2_FREQ_36MHZ); 
	i2c_set_fast_mode(I2C2);                // 400KHz - I2C Fast Mode 
	i2c_set_ccr(I2C2, 0x1e);                // CCR = tlow/tcycle
    i2c_set_trise(I2C2, 11);
    i2c_peripheral_enable(I2C2);            // finally start the periph'

    /** write the configuration array to the GT811 */
    gt811_write_register(GT811_REGISTERS_CONFIGURATION, sizeof(gt811_config), (uint8_t*)gt811_config);

    /** initialize the touch reports struct */
    for(currentFinger = 0; currentFinger < TOUCH_POINTS_MAX; currentFinger++)
    {
        // point to the current finger's report  
        hid_report = &touch_reports[currentFinger];

        // avoid being caught in a dumb infinite loop in the beginning...
        hid_report->touch_up_sent = true;
    }
}





///
// poll the GT811 for touch reports and
// split the result in HID reports to send to our controller
void gt811_poll(void)
{
    if (gpio_get(GPIOB, GPIO6) == 0) {

        // clear the data from previous loop iteration
        for(i = 0; i < sizeof(data); i++)
            data[i] = 0; 
        
        // read the current touch report
        gt811_read_register(0x721, 34, data);

        // init some variables
        validTouchPoints = 0;
        sentReports = 0;
        scan_time = get_scan_time();

        // find out how many touches we have:
        for(currentFinger = 0; currentFinger < TOUCH_POINTS_MAX; currentFinger++)
        {  
            // check if the "finger N" bit is set high in GT811 answer
            if((data[0] & (0b00000001 << currentFinger)) > 0) 
            {
                validTouchPoints++;
            }
            else if(hid_report->touch_up_sent == false)
            {
                // last frame contained a contact point for this finger;
                // => we must send a "touch up" report, which is considered
                // a valid touch point...
                validTouchPoints++;
            }
        }

        // we need to get rid of the stupid "reserved" (0x733 to 0x739) 
        // bytes in the GT811 touch reports for easier access later
        // so we shift them accordingly
        for(i = 24; i < 34; i++) 
        {
            data[i - 6] = data[i];
            data[i] = 0;
        }

        // send one report per finger found
        for(currentFinger = 0; currentFinger < TOUCH_POINTS_MAX; currentFinger++)
        {          
            // point to the current finger's report  
            hid_report = &touch_reports[currentFinger];

            // byte[0] = 0x01 (Report ID, for this application, always = 0x01)
            hid_report->report[0] = 0x01;

            // check if the "finger N" bit is set high in GT811 answer
            if((data[0] & (0b00000001 << currentFinger)) > 0) 
            {
                // byte[1] = state        
                // .......x: Tip switch
                // xxxxxxx.: Ignored
                hid_report->report[1] = 0b00000001;

                // this is still within a move sequence of reports
                hid_report->touch_up_sent = false;
            }
            else if(hid_report->touch_up_sent == false)
            {
                // we're required to send a "touch up" report when
                // a finger was lifted. This means that after the last
                // report with TIP = true was sent we need to issue
                // yet another report for the same finger with the 
                // last coordinates but TIP = false
                hid_report->report[1] = 0b00000000;
                
                // taking care of sending only one touch up report
                hid_report->touch_up_sent = true;
            }
            else 
                continue;   // skip this "finger"

            
            if(hid_report->touch_up_sent == false)
            {            
                // calculate the "data offset" in the GT811 data_align
                // for the current finger / at 5 bytes per finger
                offsetData = currentFinger * 5; 

                // byte[2] Contact index
                hid_report->report[2] = currentFinger;

                // byte[3] Tip pressure
                hid_report->report[3] = data[offsetData + 6];

                // NOTE: apparently the waveshare touch digitizer was mounted
                // mirrored in both directions, this means we "flip" X <-> Y
                // coordinates AND we have to mirror the Y axis

                // byte[4] X coordinate LSB                
                hid_report->report[4] = data[offsetData + 5];
                
                // byte[5] X coordinate MSB 
                hid_report->report[5] = data[offsetData + 4];

                // Y is inverted for some reason
                // so we have to do some math here...
                // basicall y max - value...
                y_value =  data[offsetData + 3] + (data[offsetData + 2] << 8);
                inverted_y = 480 - y_value;

                // byte[6] Y coordinate LSB
                hid_report->report[6] = inverted_y & 0xFF;

                // byte[7] Y coordinate MSB
                hid_report->report[7] = (inverted_y & 0xFF00) >> 8;    

                // byte[8] contact count  
                if(sentReports == 0) 
                {
                    // this is the first report in the frame: write touch count in report
                    hid_report->report[10] = validTouchPoints;   
                }                    
                else
                    hid_report->report[10] = 0;
            }

            // bytes[8 & 9] this report's scan time
            hid_report->report[8] = scan_time & 0xFF;
            hid_report->report[9] = (scan_time & 0xFF00) >> 8;

            // send that report!
            send_hid_report(hid_report->report, 11);

            // remember we sent a report
            sentReports++;
        }            
    }
}