////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 08.05.2013
////////////////////////////////////////////////////////////////////////////////
// main.cpp: LEDs CONTROL
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

#include "LED.h"
#include "hw_io_cfg.h"
#include "../main.h"

#include <avr/io.h>
#include <util/delay.h>

///////////////////////////////////////////////////////////

// LEDs control: (this is private because of object theory)
#define LED_LD_FL_ON       LED_LD_FL_PORT|=(1<<LED_LD_FL)
#define LED_LD_FL_OFF      LED_LD_FL_PORT&=~(1<<LED_LD_FL)
#define LED_LD_FL_TOGGLE   LED_LD_FL_PORT^=(1<<LED_LD_FL)

#define LED_LD_FR_ON       LED_LD_FR_PORT|=(1<<LED_LD_FR)
#define LED_LD_FR_OFF      LED_LD_FR_PORT&=~(1<<LED_LD_FR)
#define LED_LD_FR_TOGGLE   LED_LD_FR_PORT^=(1<<LED_LD_FR)

#define LED_LD_BL_ON       LED_LD_BL_PORT|=(1<<LED_LD_BL)
#define LED_LD_BL_OFF      LED_LD_BL_PORT&=~(1<<LED_LD_BL)
#define LED_LD_BL_TOGGLE   LED_LD_BL_PORT^=(1<<LED_LD_BL)

#define LED_LD_BR_ON       LED_LD_BR_PORT|=(1<<LED_LD_BR)
#define LED_LD_BR_OFF      LED_LD_BR_PORT&=~(1<<LED_LD_BR)
#define LED_LD_BR_TOGGLE   LED_LD_BR_PORT^=(1<<LED_LD_BR)

#define LED_ALL_ON         LED_LD_FL_ON;LED_LD_FR_ON;LED_LD_BL_ON;LED_LD_BR_ON
#define LED_ALL_OFF        LED_LD_FL_OFF;LED_LD_FR_OFF;LED_LD_BL_OFF;LED_LD_BR_OFF
#define LED_ALL_TOGGLE     LED_LD_FL_TOGGLE;LED_LD_FR_TOGGLE;LED_LD_BL_TOGGLE;LED_LD_BR_TOGGLE

///////////////////////////////////////////////////////////

//ctor
LED::LED(globalHW* globHW)
{
    this->globHW = globHW;
    this->io_init();
}

//dtor
LED::~LED()
{

}

///////////////////////////////////////////////////////////

void LED::io_init()
{
    //set ports as outputs
    LED_LD_FL_DDR|=(1<<LED_LD_FL);
    LED_LD_FR_DDR|=(1<<LED_LD_FR);
    LED_LD_BL_DDR|=(1<<LED_LD_BL);
    LED_LD_BR_DDR|=(1<<LED_LD_BR);
}

///////////////////////////////////////////////////////////

void LED::set(byte index, byte value)
{
	switch(index)
	{
		case 0:
		{
			if (value == LED_S_ON) LED_LD_FL_ON;
			else if (value == LED_S_OFF) LED_LD_FL_OFF;
			else LED_LD_FL_TOGGLE;
			break;
		}
		case 1:
		{
			if (value == LED_S_ON) LED_LD_FR_ON;
			else if (value == LED_S_OFF) LED_LD_FR_OFF;
			else LED_LD_FR_TOGGLE;
			break;
		}
		case 2:
		{
			if (value == LED_S_ON) LED_LD_BL_ON;
			else if (value == LED_S_OFF) LED_LD_BL_OFF;
			else LED_LD_BL_TOGGLE;
			break;
		}
		case 3:
		{
			if (value == LED_S_ON) LED_LD_BR_ON;
			else if (value == LED_S_OFF) LED_LD_BR_OFF;
			else LED_LD_BR_TOGGLE;
			break;
		}
	}//swtich
}//LED::set(...)

///////////////////////////////////////////////////////////

//change state of all leds:
void LED::set_all(byte value)
{
    if (value == LED_S_ON) {LED_ALL_ON; }
    else if (value == LED_S_OFF) {LED_ALL_OFF; }
    else {LED_ALL_TOGGLE; }
}

///////////////////////////////////////////////////////////

void LED::flash_all(unsigned int sleep_ms)
{
    this->set_all(LED_S_ON);
    _delay_ms(sleep_ms);
    this->set_all(LED_S_OFF);
}


///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////

