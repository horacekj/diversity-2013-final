////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 17.06.2013
////////////////////////////////////////////////////////////////////////////////
// strgFront.cpp: EXECUTABLE FILE TO THE FRONT MOVE STRATEGY
// this strategy is executed when there is no else specific strategy
// this strategy moves the robot in the arena on circles with big radiuses
//  and tries to find an enemy (is rotates the robot and searches in the neighborhood)
// it is called front, but the robot is not actually moving front :-)
// strategy name: 'F' ( = front)
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

#include "strgFront.h"
#include <stdlib.h>     //rand()

///////////////////////////////////////////////////////////
//private definitions:

#define ROT_FIRST           30      // first rotation (measured from the start of the strategy)
#define ROT_PERIOD          50      // every next rotation period
#define ROT_SP_RAW        7500      // raw rotation speed
#define ROT_SP_PREC       5000      // precision rotation speed
#define ROT_TIME_RAW        35      // raw rotation time (should rotate 360°)
#define ROT_TIME_PREC       30      // prec rotation timeout

#define BK_TIME             10      // back moving time
#define BK_SPEED          8000      // back moving speed

#define ENEMY_STRG         'E'      // what strategy to call, when enemy is detected
#define ENEMY_DIST          10      // enemy is detected, when sonar_left = sonar_right and both <= ENEMY_DIST
#define ENEMY_TOLERANT       3      // tolerantion between left and right sonar value (ideally are equal)

#define CIRC_START_TIME     10      // how long to go with those speeds:
#define CIRC_MOT1_START   8000      // speed 1 - set after start - in start()
#define CIRC_MOT2_START   9000      // speed 2 - set after start - in start()

// those both constants define radius of the circle in step 1A, 1B
//  ! CIRC_MOT1 must be < than CIRC_MOT2 !
//  is is not defined, which is left and which is right wheel, because 'direction' of a circle is randomly generated
#define CIRC_MOT1         5000      // speed 1 - is set CIRC_START_TIME after initialization
#define CIRC_MOT2         7000      // speed 2 - is set CIRC_START_TIME after initialization

///////////////////////////////////////////////////////////

strgFront::strgFront(rbtHW* hw) : myStrg(hw)
{
    //ctor
    this->name     = 'F';
    this->type     = normal;
    this->critical = false;
    this->enabled  = false;
    this->step     = -1;
}

strgFront::~strgFront()
{
    //ctor
}

///////////////////////////////////////////////////////////

//start the strategy
void strgFront::Start()
{
    this->step = 0;
    this->counter = 0;

    //start moving on the circle with START speed
    if (rand()%2 == 0)
        this->hw->motor->set_sp(CIRC_MOT2_START, CIRC_MOT1_START); else this->hw->motor->set_sp(CIRC_MOT1_START, CIRC_MOT2_START);
}

//stop the strategy
void strgFront::Stop()
{
    this->step = -1;
}

///////////////////////////////////////////////////////////

//execution of the strategy
//called on 20ms
int strgFront::exec()
{
    //main strategy execution:

    byte left, right;       //used to store the data from the sonar

    if (this->step == 0)
    {
        #ifdef DEBUG_ROT
        this->hw->leds->set(LED_BL, LED_S_OFF);
        this->hw->leds->set(LED_BR, LED_S_OFF);
        #endif

        //first init: != 0, every next init: == 0
        if (this->hw->motor->getSpL() != 0)
        {
            this->counter2 = CIRC_START_TIME;       //max speed counter
        } else {
            this->counter2 = -1;     //no speed is set, because START speed is already set (in start())
            if (this->hw->motor->getSpL() > this->hw->motor->getSpR())      //set normal speed
                this->hw->motor->set_sp(CIRC_MOT2, CIRC_MOT1); else this->hw->motor->set_sp(CIRC_MOT1, CIRC_MOT2);
        }

        this->step++;
        if (this->counter == 0) this->counter = ROT_FIRST;
        return 0;
    }

    if (this->step == 1)
    {
        //wait for end of max speed

        if (this->counter2 == 0)
        {
            //set slower speed
            if (this->hw->motor->getSpL() > this->hw->motor->getSpR())
                this->hw->motor->set_sp(CIRC_MOT2, CIRC_MOT1); else this->hw->motor->set_sp(CIRC_MOT1, CIRC_MOT2);

            this->counter2--;
        } else {
            if (this->counter2 > 0) this->counter2--;
        }

        //wait to rotation interrupt
        this->counter--;
        if (this->counter > 0) return 0;

        //timer overflow -> move back (because of borders)
        this->hw->motor->go_back(BK_SPEED);

        #ifdef DEBUG_ROT
        this->hw->leds->set(LED_BL, LED_S_ON);
        #endif

        this->step++;
        this->counter = BK_TIME;
        return 0;
    }

    if (this->step == 2)
    {
        this->counter--;
        if (this->counter > 0) return 0;

        //timer overflow -> raw rotation and raw finding of the target
        if (rand()%2 == 0)
            this->hw->motor->turn_left(ROT_SP_RAW); else this->hw->motor->turn_right(ROT_SP_RAW);

        this->step++;
        this->counter = ROT_TIME_RAW;
        return 0;
    }

    if (this->step == 3)
    {
        //raw rotation and finding of the target

        this->counter--;
        if (this->counter <= 0)
        {
            // ! raw rotation timeout
            this->step = 0;     //reset the strategy
            this->hw->motor->stop();
            this->counter = ROT_PERIOD;
            return 0;
        }

        left = this->hw->s_sonar->getL();
        right = this->hw->s_sonar->getR();

        if ((left > ENEMY_DIST) || (right > ENEMY_DIST)) return 0;

        if ((left+ENEMY_TOLERANT >= right) && (right+ENEMY_TOLERANT >= left))
        {
            //raw found was successful -> precision finding
            if (this->hw->motor->getSpL() > this->hw->motor->getSpR())
                this->hw->motor->turn_left(ROT_SP_PREC); else this->hw->motor->turn_right(ROT_SP_PREC);

            #ifdef DEBUG_ROT
            this->hw->leds->set(LED_BR, LED_S_ON);
            #endif

            this->counter = ROT_TIME_PREC;
            this->step++;
        }
    }

    if (this->step == 4)
    {
        this->counter--;
        if (this->counter <= 0)
        {
            // ! precision rotation timeout
            this->step = 0;         //reset the strategy
            this->counter = ROT_PERIOD;
            this->hw->motor->stop();
            return 0;
        }

        left = this->hw->s_sonar->getL();
        right = this->hw->s_sonar->getR();

        if ((left > ENEMY_DIST) || (right > ENEMY_DIST)) return 0;

        if (left == right)
        {
            //enemy robot was found

            #ifdef DEBUG_ROT
            this->hw->leds->set(LED_BL, LED_S_OFF);
            this->hw->leds->set(LED_BR, LED_S_OFF);
            #endif

            this->hw->motor->stop();
            this->callEvSD(ENEMY_STRG);
            return 0;
        }

    }

    return 0;
}

///////////////////////////////////////////////////////////

bool strgFront::canDisable()
{
    if (this->step == 0) return true; else return false;
}



