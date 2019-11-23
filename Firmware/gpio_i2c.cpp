/* Copyright (C) 2019 Viktor Partyshev <partyvi@gmail.com>
 *
 * This Program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 */

#include "Arduino.h"
#include "GPIO_I2C.h"

GPIO_I2C::GPIO_I2C()
{
  I2C_INIT;
}

void GPIO_I2C::init()
{
  I2C_INIT;
}

word GPIO_I2C::write(word data)
{
  word i;
  word ack;
  for (i=0; i<8; i++)
  {
    i2c_bit_delay;
    // Data setup
    if (data&0x80) { SDA_SET_HIGH; } else {SDA_SET_LOW;}
    data <<=1;
    i2c_bit_delay;
    
    SCL_SET_HIGH;  // SCL PULSE
    i2c_bit_delay;
    SCL_SET_LOW;  
    
  }
  SDA_SET_READ;
  i2c_bit_delay;
  i2c_bit_delay;
  i2c_bit_delay;
  i2c_bit_delay;
  i2c_bit_delay;
   SCL_SET_HIGH;  // SCL PULSE
  // ack check
  ack = 1;
  for (i=0; i<10; i++)
  {
    i2c_bit_delay;
    if (!SDA_DATA) {ack = 0 ; break; }
  }
  SCL_SET_LOW;  
  return ack;
}

word GPIO_I2C::beginTransmission(word address)
{
  SDA_SET_HIGH;
  i2c_bit_delay;
  SCL_SET_HIGH;
  i2c_bit_delay;
  SDA_SET_LOW;  // start condition
  i2c_bit_delay;
  SCL_SET_LOW;
  i2c_bit_delay;
  return write(address<<1);  //0 at bit R/W
}


void GPIO_I2C::endTransmission()
{
  SDA_SET_LOW; 
  i2c_bit_delay;
  SCL_SET_HIGH;
  i2c_bit_delay;
  SDA_SET_HIGH; 
  i2c_bit_delay;
}



word GPIO_I2C::requestFrom(word address, word len)
{
  word res;
   _i2cLenCo = len;
  
  SDA_SET_HIGH;
  i2c_bit_delay;
  SCL_SET_HIGH;
  i2c_bit_delay;
  SDA_SET_LOW;  // start condition
  i2c_bit_delay;
  SCL_SET_LOW;
  i2c_bit_delay;
  res = write((address<<1)+1);  //1 at bit R/W
  SDA_SET_READ;
  i2c_bit_delay;
  return res;
}

word GPIO_I2C::available(void)
{
  return 1;
}

word GPIO_I2C::read(void)
{
  word res=0;
  word i;
  for (i=0;i<8;i++)
  {
    res <<= 1 ;
    SCL_SET_HIGH;
    i2c_bit_delay;
    if (SDA_DATA) res |= 1;
     
    SCL_SET_LOW;
    i2c_bit_delay;
  }
 

  _i2cLenCo-- ;
  if (_i2cLenCo)  { SDA_SET_LOW; } else { SDA_SET_HIGH; }
    i2c_bit_delay;
    i2c_bit_delay;
    i2c_bit_delay;
    i2c_bit_delay;
    
    SCL_SET_HIGH;
    i2c_bit_delay;
    SCL_SET_LOW;
    i2c_bit_delay;

    if (!_i2cLenCo) endTransmission();
    return res;
}

