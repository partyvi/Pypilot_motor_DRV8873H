/****************************************************
/* AMS 5601 class for Arduino
/* Author: Tom Denton
/* Edit from 5600 to 5601: David Criado
/* Date: 13 Mar 2017
/* File: AMS_5601.cpp
/* Version 2.00
/* www.ams.com
/*
/* Description:  This class has been designed to
/* access the AMS AS5601 sensor.
/*
/***************************************************/


#include "Arduino.h"
#include "AMS_5601.h"

#define  GPIO_I2C_IN_USE

#ifndef GPIO_I2C_IN_USE
	#include "Wire.h"
#else
	#include "GPIO_I2C.h"
  GPIO_I2C Wire;
#endif
  
/****************************************************
/* Method: AMS_5601
/* Constructor class for AMS 5601
/***************************************************/
AMS_5601::AMS_5601()
{
  _ams5601_Address = 0x36;

  _zmco         = 0x00;
  _zpos_hi      = 0x01;
  _zpos_lo      = 0x02;
  _conf_hi      = 0x07;
  _conf_lo      = 0x08;
  _abn          = 0x09;
  _pushthr      = 0x0a;
  _raw_ang_hi   = 0x0c;
  _raw_ang_lo   = 0x0d;
  _ang_hi       = 0x0e;
  _ang_lo       = 0x0f;
  _stat         = 0x0b;
  _agc          = 0x1a;
  _mag_hi       = 0x1b;
  _mag_lo       = 0x1c;
  _burn         = 0xff;
}
/****************************************************
/* Method: i2cInit (for gpio_i2c)
/* Returns i2c address of AMS 5601
/***************************************************/
void AMS_5601::init(void)
{
  Wire.init();
}
/****************************************************
/* Method: AMS_5601
/* Returns i2c address of AMS 5601
/***************************************************/
int AMS_5601::getAddress()
{
  return _ams5601_Address;
}

/***NEW FUNCTION**************************************
/* Method: getABN
/* Gets value of output positions and update rate.
/*
/* binary   positions   upd rate (Hz)
/* ----------------------------------
/* 0000     8           61
/* 0001     16          122
/* 0010     32          244
/* 0011     64          488
/* 0100     128         976
/* 0101     256         1900
/* 0110     512         3900
/* 0111     1024        7800
/* others   2048        15600
/*
/*******************************************************/
word AMS_5601::getABN()
{
  return readOneByte(_abn);
}

/***NEW FUNCTION**************************************
/* Method: setABN
/* New value of output positions and update rate
/*******************************************************/

word AMS_5601::setABN(word inputResolution)
{
  if(inputResolution == -1)
  {
    _resolution = 0;
  }
  else
    _resolution = inputResolution;

  writeOneByte(_abn, _resolution);  // write value
  delay(2);

  _updateRate = readOneByte(_abn);  // print result
  return(_updateRate);
}

/*******************************************************
/* Method: setStartPosition
/* In: new start angle position
/* Out: value of start position register
/* Description: sets a value in start position register.
/* If no value is provided, method will read position of
/* magnet.
/*******************************************************/
word AMS_5601::setZeroPosition(word zeroAngle)
{
  if(zeroAngle == -1)
  {
    _rawStartAngle = getRawAngle();
  }
  else
    _rawStartAngle = zeroAngle;

  writeOneByte(_zpos_hi, highByte(_rawStartAngle));
  delay(2);
  writeOneByte(_zpos_lo, lowByte(_rawStartAngle));
  delay(2);
  _zPosition = readTwoBytes(_zpos_hi, _zpos_lo);

  return(_zPosition);
}

/*******************************************************
/* Method: getStartPosition
/* Gets value of start position register.
/*******************************************************/
word AMS_5601::getZeroPosition()
{
  return readTwoBytes(_zpos_hi, _zpos_lo);
}


/*******************************************************
/* Method: getRawAngle
/* In: none
/* Out: value of raw angle register
/* Description: gets raw value of magnet position.
/* zero angle settings do not apply
/*******************************************************/
word AMS_5601::getRawAngle()
{
  return readTwoBytes(_raw_ang_hi, _raw_ang_lo);
}

/*******************************************************
/* Method: getScaledAngle
/* In: none
/* Out: value of scaled angle register
/* Description: gets scaled value of magnet position.
/* zero angle settings are used to determine value
/*******************************************************/
word AMS_5601::getScaledAngle()
{
  return readTwoBytes(_ang_hi, _ang_lo);
}

/*******************************************************
/* Method: detectMagnet
/* In: none
/* Out: 1 if magnet is detected, 0 if not
/* Description: reads status register and examines the
/* MH bit
/*******************************************************/
int AMS_5601::detectMagnet()
{
  int magStatus;
  int retVal = 0;
  /*0 0 MD ML MH 0 0 0*/
  /* MD high = AGC minimum overflow, Magnet to strong   */
  /* ML high = AGC Maximum overflow, magnet to weak     */
  /* MH high = magnet detected                          */

  magStatus = readOneByte(_stat);

  if(magStatus & 0x20)      // 32 (0010 0000)
    retVal = 1;

  return retVal;
}

/*******************************************************
/* Method: getMagnetStrength
/* In: none
/* Out: 0 if no magnet is detected
/*      1 if magnet is to weak
/*      2 if magnet is just right
/*      3 if magnet is to strong
/* Description: reads status register andexamins the MH,ML,MD bits
/*******************************************************/
int AMS_5601::getMagnetStrength()
{
  int magStatus;
  int retVal = 0;
  /*0 0 MD ML MH 0 0 0*/
  /* MD high = magnet detected */
  /* ML high = AGC Maximum overflow, magnet too weak*/
  /* MH high = AGC minimum overflow, Magnet too strong*/
  magStatus = readOneByte(_stat);
  if(detectMagnet() ==1)
  {
      retVal = 2; /*just right */
      if(magStatus & 0x10)      // 16 (0001 0000)
        retVal = 1; /*too weak */
      else if(magStatus & 0x08) // 8  (0000 1000)
        retVal = 3; /*too strong */
  }
//_resolution
  return retVal;
}

/*******************************************************
/* Method: get Agc
/* In: none
/* Out: value of AGC register
/* Description: gets value of AGC register.
/*******************************************************/
int AMS_5601::getAgc()
{
  return readOneByte(_agc);
}

/*******************************************************
/* Method: getMagnitude
/* In: none
/* Out: value of magnitude register
/* Description: gets value of magnitude register.
/*******************************************************/
word AMS_5601::getMagnitude()
{
  return readTwoBytes(_mag_hi, _mag_lo);
}

/*******************************************************
/* Method: getBurnCount
/* In: none
/* Out: value of zmco register
/* Description: determines how many times chip has been
/* permanently written to.
/*******************************************************/
int AMS_5601::getBurnCount()
{
  return readOneByte(_zmco);
}

/*******************************************************
/* Method: burnAngle
/* In: none
/* Out: 1 success
/*     -1 no magnet
/*     -2 burn limit exceeded
/*     -3 start position not set (useless burn)
/* Description: burns zero position to chip.
/* THIS CAN ONLY BE DONE 3 TIMES
/* MODIFIED FROM AS5600 burnAngle
/*******************************************************/
int AMS_5601::burnAngle()
{
  int retVal = 1;
  _zPosition = getZeroPosition();

  if(detectMagnet() == 1)
  {
    if(getBurnCount() < 3)
    {
      if((_zPosition == 0))
        retVal = -3;
      else
        writeOneByte(_burn, 0x80);
    }
    else
      retVal = -2;
  }
  else
    retVal = -1;

  return retVal;
}

/*******************************************************
/* Method: burnConfig
/* In: none
/* Out: 1 success
/*     -1 burn limit exceeded
/* Description: burns config data to chip.
/* THIS CAN ONLY BE DONE 1 TIME
/* MODIFIED FROM AS5600 burnMaxAngleandConfig
/*******************************************************/
int AMS_5601::burnConfig()
{
  int retVal = 1;

  if(getBurnCount() ==0)
  {
      writeOneByte(_burn, 0x40);
  }
  else
    retVal = -1;

  return retVal;
}


/*******************************************************
/* Method: readOneByte
/* In: register to read
/* Out: data read from i2c
/* Description: reads one byte register from i2c
/*******************************************************/
int AMS_5601::readOneByte(int in_adr)
{
  int retVal = -1;
  Wire.beginTransmission(_ams5601_Address);
  Wire.write(in_adr);
  Wire.endTransmission();
  Wire.requestFrom(_ams5601_Address, 1);
  while(Wire.available() == 0);
  retVal = Wire.read();

  return retVal;
}

/*******************************************************
/* Method: readOneByte
/* In: two registers to read
/* Out: data read from i2c as a word
/* Description: reads two bytes register from i2c
/*******************************************************/
word AMS_5601::readTwoBytes(int in_adr_hi, int in_adr_lo)
{
  word retVal = -1;

  /* Read Low Byte */
  Wire.beginTransmission(_ams5601_Address);
  Wire.write(in_adr_lo);
  Wire.endTransmission();
  Wire.requestFrom(_ams5601_Address, 1);
  while(Wire.available() == 0);
  int low = Wire.read();

  /* Read High Byte */
  Wire.beginTransmission(_ams5601_Address);
  Wire.write(in_adr_hi);
  Wire.endTransmission();
  Wire.requestFrom(_ams5601_Address, 1);

  while(Wire.available() == 0);

  word high = Wire.read();

  high = high << 8;
  retVal = high | low;

  return retVal;
}

/*******************************************************
/* Method: writeOneByte
/* In: address and data to write
/* Out: none
/* Description: writes one byte to a i2c register
/*******************************************************/
void AMS_5601::writeOneByte(int adr_in, int dat_in)
{
  Wire.beginTransmission(_ams5601_Address);
  Wire.write(adr_in);
  Wire.write(dat_in);
  Wire.endTransmission();
}

/**********  END OF AMS 5601 CLASS *****************/
