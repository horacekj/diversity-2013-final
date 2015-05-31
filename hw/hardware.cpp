////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 16.05.2013
////////////////////////////////////////////////////////////////////////////////
// hardware.cpp: methods to work with robot hardware
// this class is used as a poiner given to the strategies (strtegies casn control HW)
////////////////////////////////////////////////////////////////////////////////

/*
   Copyright 2013 Jan Horacek

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <avr/io.h>

#include "hardware.h"
#include "obj.h"
#include "hw_io_cfg.h"
#include "adc.h"

////////////////////////////////////////////////////////////////////////////////
// rbtHW class methods
////////////////////////////////////////////////////////////////////////////////

//ctor
rbtHW::rbtHW()
{
    //DO NOT change order: io_init() and THEN class ctors (is necessary)

    //init hardware
    this->io_init();

    //create hardware instances
    this->glob_hw = new globalHW();

    this->motor     = new wheels(this->glob_hw);
    this->leds      = new LED(this->glob_hw);
    this->ir_sens   = new IRs(this->glob_hw);
    this->s_sonar   = new sonar(this->glob_hw);
    this->but       = new button(this->glob_hw);
}

//dtor
rbtHW::~rbtHW()
{
    //delete hardware instances
    delete (this->glob_hw);

    delete (this->motor);
    delete (this->leds);
    delete (this->ir_sens);
    delete (this->s_sonar);
    delete (this->but);
}

///////////////////////////////////////////////////////////

//intialization of io ports:
void rbtHW::io_init(void)
{
    DDRA = 0x00;
    DDRB = 0x00;
    DDRC = 0x00;
    DDRD = 0x00;

    DDRD&=~(1<<PD2);
    PORTD|=(1<<PD2);  // RC5 input

    // Unused pins set as outputs:
    DDRB|=(1<<PB2)|(1<<PB3)|(1<<PB4);
}


///////////////////////////////////////////////////////////

//updates all hardarare (especially sonar), called in timer (10 ms)
void rbtHW::update()
{
    this->motor->update();
    this->s_sonar->update();
    this->but->update();
}

// ------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////
// globalHW class methods
///////////////////////////////////////////////////////////////////////////////

//ctor
globalHW::globalHW()
{
    this->low_bat_cnt = 0;
    this->evOnBatLow  = 0;

    this->ad_c      = new adc();
    this->serial    = new uart();
}

//dtor
globalHW::~globalHW()
{
    delete (this->ad_c);
    delete (this->serial);
}

///////////////////////////////////////////////////////////

//battery level control:
void globalHW::batCheck()
{
    if (this->ad_c->battery_V <= LOW_BATERY)
    {
        this->low_bat_cnt++;
        if (this->low_bat_cnt > BAT_TIMES)
        {
            this->callEvBatLow();
            this->low_bat_cnt = 0;
        }
    } else
        this->low_bat_cnt = 0;

}

///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//events:

//OnBatLow registration:
void globalHW::evRegOnBatLow(int (*func)(globalHW* sender))
{
    this->evOnBatLow = func;
}

//OnBatLow call:
int globalHW::callEvBatLow()
{
    if (this->evOnBatLow != 0)
        return (*(this->evOnBatLow))(this);
    return -1;
}


///////////////////////////////////////////////////////////
