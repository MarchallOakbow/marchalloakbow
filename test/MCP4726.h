//**********************
// MCP4726.h for mbed
//
// (C)Copyright 2014 All rights reserved by Y.Onodera
// http://einstlab.web.fc2.com
//**********************

#ifndef MCP4726_H_
#define MCP4726_H_

#define MCP4726_ADDR                0xC0

#include "mbed.h"
#include "typedef.h"


class MCP4726{
public:
    MCP4726 (PinName sda, PinName scl);
    MCP4726 (I2C& p_i2c);
    void init();
    void put(unsigned short a);

protected:
    
    I2C _i2c;

    WORD_VAL da;
    char buf[3];

};

#endif /* MCP4726_H_ */



