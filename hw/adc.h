////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 14.05.2013
////////////////////////////////////////////////////////////////////////////////
//adc.h: HEADER FILE TO THE ADC CPP FILE
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

#ifndef ADC_H
#define ADC_H

#include "main.h"

class adc
{
    private:
        byte actual_channel;
        byte actual_meas;
        unsigned int meas_souc;

        void init();
        void wait_conv_complete(void);

        unsigned int get_offset_error(void);

    public:
        adc();                                      //ctor
        virtual ~adc();                             //dtor

        //ADC data
        unsigned int sonar[3];
        unsigned int battery;
        float battery_V;
        unsigned int motor[2];

        void new_value(unsigned int value);         //CAN BE CALLED ONLY IN INTERRUPT! get new value from ADC

    protected:
};

#endif // SONAR_H
