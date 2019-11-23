/* Copyright (C) 2019 Viktor Partyshev <partyvi@gmail.com>
 *
 * This Program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 */

#ifndef _GPIO_I2C_H_
#define _GPIO_I2C_H_


#include <Arduino.h>

        
#define sda_port_mask (1<<2)
#define scl_port_mask (1<<1)
 
#define gpio_i2c_DDR DDRB
#define gpio_i2c_PORT PORTB
#define gpio_i2c_PIN PINB
 
#define I2C_INIT  \
gpio_i2c_DDR |= scl_port_mask;\
gpio_i2c_PORT &= ~sda_port_mask;\
gpio_i2c_DDR &= ~sda_port_mask;\
gpio_i2c_PORT |= scl_port_mask;\
gpio_i2c_DDR |= sda_port_mask;PORTB |= sda_port_mask;



#define SCL_SET_HIGH   	gpio_i2c_PORT |= scl_port_mask;
#define SCL_SET_LOW    	gpio_i2c_PORT &= ~scl_port_mask;
#define SDA_SET_HIGH   	gpio_i2c_DDR |= sda_port_mask;PORTB |= sda_port_mask
#define SDA_SET_LOW   	gpio_i2c_DDR |= sda_port_mask;PORTB &= ~sda_port_mask
#define SDA_SET_READ  	gpio_i2c_DDR &= ~sda_port_mask;PORTB |= sda_port_mask
#define SDA_DATA   		(gpio_i2c_PIN & sda_port_mask)

#define i2c_bit_delay  \
    asm volatile ("nop"); \
    asm volatile ("nop"); \
    asm volatile ("nop"); \
    asm volatile ("nop"); \
    asm volatile ("nop"); \
    asm volatile ("nop"); \
    asm volatile ("nop");


class GPIO_I2C
{
  public:

    GPIO_I2C(void);
    void init();  
    void endTransmission();  
    word requestFrom(word address, word len);
    word available(void);
    word read(void);
    word beginTransmission(word address);
    word write(word data);
 

  private:
    word  _i2cLenCo;

};
#endif
