////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 15.06.2013
////////////////////////////////////////////////////////////////////////////////
// strgInitLED.h: EXECUTABLE FILE TO THE LED INIT STRATEGY
// this strategy is only one, which can be executed at the start
// this strategy provides 5 seconds waiting
// strategy name: 'L' ( = led init)
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

#include "strgInitLED.h"

///////////////////////////////////////////////////////////
//private definitions:

#define STEP_WAIT       175     //led period in ms
#define NEXT_STRG       'M'     //name of the next strategy, must be only one char !
                                //in this case, next strategy is strgInitMove

///////////////////////////////////////////////////////////

strgInitLED::strgInitLED(rbtHW* hw) : myStrg(hw)
{
    //ctor
    this->name     = 'L';
    this->type     = init_only;
    this->critical = false;
    this->enabled  = false;

    this->step = -1;
}

strgInitLED::~strgInitLED()
{
    //ctor
}

//start the strategy
void strgInitLED::Start()
{
    this->step      = 0;
    this->counter   = 0;
    this->hw->leds->set_all(LED_S_OFF);         //turn all leds off
    this->first = true;
}

//stop the strategy
void strgInitLED::Stop()
{
    this->step = -1;
}

//execution of the strategy
//called on 20ms
int strgInitLED::exec()
{
    if (this->step < 0) return 0;

    //increment counter
    if (STEP_WAIT > this->counter)
    {
        this->counter += 20;        //exec() is called on 20ms
        return 0;
    }

    //execute step
    switch (this->step)
    {
        case 0: this->hw->leds->set(LED_BR, LED_S_OFF);     //because of jumping to step 0 from step 7
                this->hw->leds->set(LED_FL, LED_S_ON);
                break;
        case 1: this->hw->leds->set(LED_FL, LED_S_OFF);
                this->hw->leds->set(LED_FR, LED_S_ON);
                break;
        case 2: this->hw->leds->set(LED_FR, LED_S_OFF);
                this->hw->leds->set(LED_FL, LED_S_ON);
                break;
        case 3: this->hw->leds->set(LED_FL, LED_S_OFF);
                this->hw->leds->set(LED_FR, LED_S_ON);
                break;

        case 4: this->hw->leds->set(LED_FR, LED_S_OFF);
                this->hw->leds->set(LED_BL, LED_S_ON);
                break;
        case 5: this->hw->leds->set(LED_BL, LED_S_OFF);
                this->hw->leds->set(LED_BR, LED_S_ON);
                break;
        case 6: this->hw->leds->set(LED_BR, LED_S_OFF);
                this->hw->leds->set(LED_BL, LED_S_ON);
                break;
        case 7: this->hw->leds->set(LED_BL, LED_S_OFF);
                this->hw->leds->set(LED_BR, LED_S_ON);
                if (this->first) this->step = -1;        //will be 0
                this->first = false;
                break;

        case 8: this->hw->leds->set(LED_BR, LED_S_OFF);
                this->hw->motor->unblock();
                this->callEvSD(NEXT_STRG);
                break;
        default:break;
    }//switch

    this->step++;
    this->counter = 0;

    return 0;
}

bool strgInitLED::canDisable()
{
    return false;
}
