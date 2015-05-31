////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 13.06.2013
////////////////////////////////////////////////////////////////////////////////
// IRs.cpp: IR SENSORS
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
#include <util/delay.h>

#include "IRs.h"
#include "hw_io_cfg.h"
#include "hardware.h"
#include "main.h"

///////////////////////////////////////////////////////////

//ctor
IRs::IRs(globalHW* globHW)
{
    this->globHW = globHW;

    // init IR sensors:
    LINL_F_DDR&=~(1<<LINL_F);
    LINR_F_DDR&=~(1<<LINR_F);
    LINL_B_DDR&=~(1<<LINL_B);
    LINR_B_DDR&=~(1<<LINR_B);

    LINLPWR_DDR|=(1<<LINLPWR);
    LINLPWR_PORT|=(1<<LINLPWR); // wyl ???
}

//dtor
IRs::~IRs()
{

}

///////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// checking status of IR sensors
// param: id = id of the sensor (read header file, where types are defined)
////////////////////////////////////////////////////////////////////////////////
bool IRs::get(byte id)
{
    bool uc_temp;

	switch(id)
	{
		case IRID_FL:
		{
			LINLPWR_ON;
			_delay_ms(1);       //IR sensitivity depends on the time
			uc_temp = ((IS_LINL_F)? 0 : 1);
			LINLPWR_OFF;
			return (uc_temp);
			break;
		}
		case IRID_FR:
		{
			LINLPWR_ON;
			_delay_ms(1);
			uc_temp = ((IS_LINR_F)? 0 : 1);
			LINLPWR_OFF;
			return (uc_temp);
			break;
		}

		case IRID_BL:
		{
			LINLPWR_ON;
			_delay_ms(1);
			uc_temp = ((IS_LINL_B)? 0 : 1);
			LINLPWR_OFF;
			return (uc_temp);
			break;
		}

		case IRID_BR:
		{
			LINLPWR_ON;
			_delay_ms(1);
			uc_temp = ((IS_LINR_B)? 0 : 1);
			LINLPWR_OFF;
			return (uc_temp);
			break;
		}

		default:
            return 0xffff;
            break;
	}//switch
}//IRs::get(...)
