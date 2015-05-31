////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 20.06.2013
////////////////////////////////////////////////////////////////////////////////
// strgSearch.h: EXECUTABLE FILE TO THE BORDERS STRATEGY
// strategy name: 'B' ( = borders)
// this strategy controls borders
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

#include "strgBor.h"

///////////////////////////////////////////////////////////
//private definitions:

#define STRG_BOR_NEXT        'F'        //which strategy to execute after border 'interrupt'
#define STRG_BOR_BACK_NEXT   'F'        //which strategy to execute after back IR border 'interrupt'

#define BOR1_BACK_SP       10000        //dynamic brake speed
#define BOR1_BACK_LEN          4        //dynamnic brake lenght

//radius of the circle in step 2
// ! BOR2_CIRC_MOT1 must be < than BOR2_CIRC_MOT2 !
#define BOR2_CIRC_MOT1      5000        //speed of motor 1
#define BOR2_CIRC_MOT2    -10000        //speed of motor 2
#define BOR2_CIRC_LEN         15        //lenght of the turning

#define BOR3_ROT_ANGLE       120        //rotation angle in border step 3

///////////////////////////////////////////////////////////

strgBor::strgBor(rbtHW* hw) : myStrg(hw)
{
    //ctor
    this->name     = 'B';
    this->type     = normal;
    this->critical = true;          // ! is critical
    this->enabled  = true;          // ! is necessary

    this->step = 0;                 //bacause of critical
}

strgBor::~strgBor()
{
    //dtor
}

//start the strategy
void strgBor::Start()
{
    this->step = 0;
}

//stop the strategy
void strgBor::Stop()
{
    this->step = -1;
}

//execution of the strategy
int strgBor::exec()
{
    //when motor is blocked, strategy execution is interrupted
    //used on the start
    if (this->hw->motor->getBlocked()) return 0;

    if (this->step == 0)
    {
        //did we just find the border -> activate dynamic brake

        //front left
        if (this->hw->ir_sens->get(IRID_FL))
        {
            this->step = 1;
            this->hw->motor->go_back(BOR1_BACK_SP);
            this->counter = BOR1_BACK_LEN;
            this->callEvECS();      //enter critical section
            this->callEvSC();       //take a control

            #ifdef DEBUG_BOR
            this->hw->leds->set(LED_FL, LED_S_ON);
            #endif

            return 0;
        }

        //front right
        if (this->hw->ir_sens->get(IRID_FR))
        {
            this->step = 2;
            this->hw->motor->go_back(BOR1_BACK_SP);
            this->counter = BOR1_BACK_LEN;
            this->callEvSC();
            this->callEvECS();

            #ifdef DEBUG_BOR
            this->hw->leds->set(LED_FR, LED_S_ON);
            #endif

            return 0;
        }

        //back sensors
        if ((this->hw->ir_sens->get(IRID_BL)) || (this->hw->ir_sens->get(IRID_BR)))
        {
            this->callEvSD(STRG_BOR_BACK_NEXT);

            // ! is neccessary
            this->enabled = true;
            this->critical = true;
            this->step = 0;

            return 0;
        }
    }

    if ((this->step == 1) || (this->step == 2))
    {
        //wait until dynamic brake brakes the robot

        this->counter--;
        if (this->counter > 0) return 0;

        //counter overflow -> turn on the circle
        if (this->step == 1)
            this->hw->motor->set_sp(BOR2_CIRC_MOT2, BOR2_CIRC_MOT1); else this->hw->motor->set_sp(BOR2_CIRC_MOT1, BOR2_CIRC_MOT2);

        this->counter = BOR2_CIRC_LEN;
        this->step += 2;
        return 0;
    }

    if ((this->step == 3) || (this->step == 4))
    {
        //turning on the circle...

        this->counter--;
        if (this->counter > 0) return 0;

        //counter overflow -> rotate the robot

        if (this->step == 3)
            this->hw->motor->rotate(-BOR3_ROT_ANGLE); else this->hw->motor->rotate(BOR3_ROT_ANGLE);

        this->step = 5;
        return 0;
    }

    if (this->step == 5)
    {
        //rotating the robot...
        if (this->hw->motor->getRotating()) return 0;

        //end of turning -> control to the STRG_BOR_NEXT strategy
        #ifdef DEBUG_BOR
        this->hw->leds->set(LED_FL, LED_S_OFF);
        this->hw->leds->set(LED_FR, LED_S_OFF);
        #endif

        this->callEvLCS();
        this->callEvSD(STRG_BOR_NEXT);
        this->enabled = true;
        this->critical = true;
        this->step = 0;
        return 0;
    }

    return 0;
}

bool strgBor::canDisable()
{
    if (this->step == 0) return true; else return false;
}
