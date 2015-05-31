////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 14.05.2013
////////////////////////////////////////////////////////////////////////////////
//adc.h: ADC CONTROL
// this file cares about Analog to Digital Comaparator (ADC)
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
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#include "adc.h"
#include "main.h"
#include "hw_io_cfg.h"

///////////////////////////////////////////////////////////

//private definitions:
#define ADC_OFFSET_MEAS_CNT 10  // = ADC offset measurements count

#define ADC_MEAS_CNT 38	        // = ADC measurements count
                                //by the number of measurements (with assumed mode ADC),
                                //we obtain samples from a single PWM period.
                                //This allows you to calculate the value of the average current during

#define ADC_start_conversion    ADCSRA|=(1<<ADSC)
#define ADC_stop_conversion     ADCSRA&=~(1<<ADSC)

#define ADC_turn_off            ADCSRA &= ~(1<<ADEN)
#define ADC_turn_on             ADCSRA |= (1<<ADEN)

///////////////////////////////////////////////////////////

adc* me;            //this is necessary to call class function from interrupt

//ctor
adc::adc()
{
    me = this;

    //default values:
    for (int i = 0;i<3;i++) this->sonar[i] = 0;
    for (int i = 0;i<2;i++) this->motor[i] = 0;
    this->battery = 0;

    this->actual_channel = 0;
    this->actual_meas    = ADC_MEAS_CNT;
    this->meas_souc      = 0;

    this->init();

    //start first conversion:
    ADC_start_conversion;
    //conversions will automatically continue
}

//dtor
adc::~adc()
{

}

///////////////////////////////////////////////////////////

//ADC initialization
void adc::init()
{
	ADMUX = (1<<REFS0)|(1<<REFS1) | 0b01101 ;
	ADCSRA|=(1<<ADEN)|(1<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	//prescaler 128 - 125kHz@16MHz, incorporated interrupt continuous conversion
}

///////////////////////////////////////////////////////////

//waiting for the ending conversion, used to can be measured without interruption
void adc::wait_conv_complete()
{
	while(!(ADCSRA & (1<<ADIF))); //Status flag ADIF is low As long as the ADC conversion has finished no
	ADCSRA |= (1<<ADIF); 	//reset the interrupt flag to the next call
							//flag has not been read from the previous conversion
}

///////////////////////////////////////////////////////////

//get offset error
//this function is not used
unsigned int adc::get_offset_error()
{
	unsigned int offset=0,i;

	ADMUX |= (1<<MUX4)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0); //GND to select the voltage measurement
	_delay_ms(1); //time to stabilize the voltage

	ADC_start_conversion;
	for(i=0;i<ADC_OFFSET_MEAS_CNT;i++) //by averaging the
	{
		this->wait_conv_complete(); //waiting for the end of conversion
		offset += ADC;
	}
	//ADC_stop_conversion();
	offset =offset / ADC_OFFSET_MEAS_CNT;
	ADMUX &= ~((1<<MUX4)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0));
	return offset;
}

///////////////////////////////////////////////////////////

//ADC Conversion Complete Interrupt
ISR(ADC_vect)
{
    me->new_value(ADC);
}

///////////////////////////////////////////////////////////

//get new value from ADC
void adc::new_value(unsigned int value)
{
    //we get value 0..1023
    //referrence: 2,56 V
    //voltage on battery measurement pin = battery_V*0,248
    //battery:

    //meas_souc      : sum of AD values is saved here
    //actual_meas    : count of measurements in saved here (in is decremented to the zero)
    //actual_channel : actual channel is savede here

	switch (this->actual_channel)
	{
		case 0:  // channel 0
		{
			this->meas_souc += value;
			if(--actual_meas == 0)
			{
			    //we made all measurements
				ADC_turn_off;
				this->actual_channel = 1;                       //set new channel

                ADMUX = (1<<REFS0)|(1<<REFS1) | 0b00001;        //select channel

                this->actual_meas=ADC_MEAS_CNT;                 //new measurement count
				this->sonar[0] = this->meas_souc / ADC_MEAS_CNT; //average of measurements
				this->meas_souc=0;                              //sum = 0

				ADC_turn_on;
				ADC_start_conversion;
			}

		}
		      break;

		case 1:  // channel 1
		{
			this->meas_souc += value;
			if(--this->actual_meas == 0)
			{
				ADC_turn_off;
				this->actual_channel = 2;

                ADMUX = (1<<REFS0)|(1<<REFS1) | 0b00010 ;

                this->actual_meas=ADC_MEAS_CNT;
				this->sonar[1] = this->meas_souc / ADC_MEAS_CNT;
                this->meas_souc=0;

				ADC_turn_on;
				ADC_start_conversion;
			}

		}
		      break;

		case 2:  // channel 2
		{
            meas_souc += value;
			if(--this->actual_meas == 0)
			{
				ADC_turn_off;
				this->actual_channel = 3;

				ADMUX = (1<<REFS0)|(1<<REFS1) | 0b00011 ;

                this->actual_meas=ADC_MEAS_CNT;
				this->sonar[2] = this->meas_souc / ADC_MEAS_CNT;
                this->meas_souc = 0;

				ADC_turn_on;
				ADC_start_conversion;
            }
		}
		    break;

        case 3:  // channel 3
		{
            meas_souc += value;
			if(--this->actual_meas == 0)
			{
				ADC_turn_off;
				actual_channel = 4;

				ADMUX = (1<<REFS0)|(1<<REFS1) | 0b00100 ;

                this->actual_meas=ADC_MEAS_CNT;
				this->battery = this->meas_souc / ADC_MEAS_CNT;
                this->meas_souc = 0;

                //count battery voltage
                this->battery_V = ((this->battery / 1024.0) * 2.56) / (3.3/(10+3.3));

				ADC_turn_on;
				ADC_start_conversion;
            }
		}
		    break;

        case 4:  // channel 4
		{
            meas_souc += value;
			if(--this->actual_meas == 0)
			{
				ADC_turn_off;
				this->actual_channel = 5;

				ADMUX = (1<<REFS0)|(1<<REFS1) | 0b00101 ;

                this->actual_meas=ADC_MEAS_CNT;
				this->motor[0] = this->meas_souc / ADC_MEAS_CNT;
                this->meas_souc = 0;

				ADC_turn_on;
				ADC_start_conversion;
            }
		}
		    break;

        case 5:  // channel 5
		{
            meas_souc += value;
			if(--this->actual_meas == 0)
			{
				ADC_turn_off;
				this->actual_channel = 0;

				ADMUX = (1<<REFS0)|(1<<REFS1) | 0b00000 ;

                this->actual_meas=ADC_MEAS_CNT;
				this->motor[1] = this->meas_souc / ADC_MEAS_CNT;
                this->meas_souc = 0;

				ADC_turn_on;
				ADC_start_conversion;
            }
		}
		    break;


	}//case
}// ADC Conversion Complete


///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
