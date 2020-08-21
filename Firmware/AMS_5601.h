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

#ifndef _AMS_5601_H_
#define _AMS_5601_H_


#include <Arduino.h>


class AMS_5601
{
  public:

    AMS_5601();
    
    void i2cinit();

    int     getAddress();                           // get AMS5601 address, set and fixed to 0x36

    word    setZeroPosition(word zeroAngle = -1);   // set zero position to the angle determined
    word    getZeroPosition();                      // get value of zero position

    word    setABN(word inputResolution = -1);      // set update rate and resolution of measurement
    word    getABN();                               // get such values

    word    getRawAngle();                          // get angle without zero position
    word    getScaledAngle();                       // get angle referred to zero position

    int     detectMagnet();                         // looks for MD/ML/MH in the status register
    int     getMagnetStrength();                    // checks if magnet too weak/strong/just right
    int     getAgc();                               // indicates the gain
    word    getMagnitude();                         // indicates the internal CORDIC output

    int     getBurnCount();                         // retrieves number of burns from zmco
    int     burnAngle();                            // MOD from 5600 burnAngle              burns zero Angle
    int     burnConfig();                           // MOD from 5600 burnMaxAngleAndConfig  burns settings

    void i2cEndTransmission();  
    word i2cRequestFrom(word address, word len);
    word i2cAvailable(void);
    word i2cRead(void);
    word i2cBeginTransmission(word address);
    word i2cWrite(word data);
    void init(void);
 

  private:

    int     _ams5601_Address;   // chip i2c address

    word    _resolution;        // ABN mapping (0x09)
    word    _updateRate;        // ABN mapping (0x09)
    word    _rawStartAngle;     //
    word    _zPosition;         // zero position
    
    word    _i2cLenCo;

    /* Registers */
    int     _zmco;
    int     _zpos_hi;           // zpos[11:8] high nibble  START POSITION
    int     _zpos_lo;           // zpos[7:0]
    int     _conf_hi;
    int     _conf_lo;
    int     _raw_ang_hi;
    int     _raw_ang_lo;
    int     _ang_hi;
    int     _ang_lo;
    int     _stat;
    int     _agc;
    int     _mag_hi;
    int     _mag_lo;
    int     _burn;
    int     _abn;
    int     _pushthr;

    

    int     readOneByte (int in_adr);
    word    readTwoBytes(int in_adr_hi, int in_adr_lo);
    void    writeOneByte(int adr_in,    int dat_in);
};
#endif
