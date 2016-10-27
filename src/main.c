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
#include <libopencm3/gd32/i2c.h>
#include <libopencm3/gd32/gpio.h>
#include <libopencm3/gd32/rcc.h>

// I2C read and write addresses as per datasheet
#define GT811_ADDRESS 0xBA 

// read n bytes from GT811 given the start register
static void gt811_read_register(uint16_t reg, uint8_t size, uint8_t *data)
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

int main(void)
{
	// setup the CPU
	rcc_clock_setup_in_hse_8mhz_out_72mhz();    // 72MHz clock using on-board crystal / HSE

	// setup the i2c communication
    rcc_periph_clock_enable(RCC_I2C2);     
	rcc_periph_clock_enable(RCC_AFIO);    
	
    gpio_set_mode(GPIOB,                  
        GPIO_MODE_OUTPUT_50_MHZ,   
        GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
        GPIO_I2C2_SCL | GPIO_I2C2_SDA);            

	i2c_peripheral_disable(I2C2);     
	i2c_set_clock_frequency(I2C2,     
        I2C_CR2_FREQ_36MHZ); 
	i2c_set_fast_mode(I2C2);        
	i2c_set_ccr(I2C2, 0x1e);          
    i2c_set_trise(I2C2, 11);
    i2c_peripheral_enable(I2C2);

    // try to read some stuff
	uint8_t buf[32];

	// read version
	gt811_read_register(0x717, 2, buf);

	// read the full touch report
	gt811_read_register(0x721, 34, data);


	while (1) {
	}
}