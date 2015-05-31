////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 08.05.2013
////////////////////////////////////////////////////////////////////////////////
// main.cpp: LEDs CONTROL HEADER FILE
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

#ifndef LED_H
#define LED_H

#include "../main.h"

//function set(...) can be called with those constants as indexes
#define LED_FL      0
#define LED_FR      1
#define LED_BL      2
#define LED_BR      3

//led states:
#define LED_S_OFF       0
#define LED_S_ON        1
#define LED_S_TOGGLE    2

class globalHW;

class LED
{

    private:
        globalHW* globHW;

        void io_init();

    public:
        LED(globalHW* globHW);
        virtual ~LED();

        void set(byte index, byte value);
        void set_all(byte value);
        void flash_all(unsigned int sleep_ms);

    protected:
};

#endif // LED_H

