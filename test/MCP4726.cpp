//**********************
// MCP4726.cpp for mbed
//
// MCP4726 mcp4726(P0_5,P0_4);
// or
// I2C i2c(P0_5,P0_4);
// MCP4726 mcp4726(i2c);
//
// Max Frequency with I2C mode
// Standard mode   3.4KHz=100KHz / 29bits
// Fast mode      13.8KHz=400KHz / 29bits
// Fast mode Plus 34.5KHz=1MHz / 29bits
// HS mode        117KHz =3.4MHz / 29bits
//
// (C)Copyright 2014 All rights reserved by Y.Onodera
// http://einstlab.web.fc2.com
//**********************

#include "mbed.h"
#include "MCP4726.h"

MCP4726::MCP4726 (PinName sda, PinName scl) : _i2c(sda, scl) {
    init();
}
MCP4726::MCP4726 (I2C& p_i2c) : _i2c(p_i2c) {
    init();
}


void MCP4726::put(unsigned short a)
{

    da.Val = a;
    buf[0]=0x0F & da.byte.HB;
    buf[1]=da.byte.LB;
    _i2c.write(MCP4726_ADDR, buf, 2);
 
}


void MCP4726::init()
{

    // Set fast mode
    _i2c.frequency(400000);  // default 100Kbps

    // wait POR 60us+
    wait_us(100);

    // reset
//    buf[0]=0x06;
//    _i2c.write(0x00, buf, 1);
    // wakeup
//    buf[0]=0x09;
//    _i2c.write(0x00, buf, 1);
        
    // Set Configuration
    buf[0]=0x80;
    _i2c.write(MCP4726_ADDR, buf, 1);

}

