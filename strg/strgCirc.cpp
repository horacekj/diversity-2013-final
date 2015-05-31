////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 16.06.2013
////////////////////////////////////////////////////////////////////////////////
// strgCirc.cpp: EXECUTABLE FILE TO THE CIRCLE MOVE STRATEGY
// startegy, which moves robots on the 'spirala' (! translate to english)
// this strategy is known as step 4 in the Paper
// this strategy periodically tries to find the enemy via rotating
// strategy name: 'C' ( = circle)
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

#include "strgCirc.h"
#include <stdlib.h>     //rand()

///////////////////////////////////////////////////////////
//private definitions:

#define ROT_FIRST           50      // first rotation (measured from the start of the strategy)
#define ROT_PERIOD         200      // every next rotation period
#define ROT_SP_RAW        8000      // raw rotation speed
#define ROT_SP_PREC       5000      // precision rotation speed
#define ROT_TIME_RAW        35      // raw rotation time (should rotate 360°)
#define ROT_TIME_PREC       50      // prec rotation timeout

#define ROT_MAXCNT           6      // max count of rotations
#define ROT_NEXT           'F'      // after rotaion count overflow, this strategy is executed

#define ENEMY_STRG         'E'      // what strategy to call, when enemy is detected
#define ENEMY_DIST          10      // enemy is detected, when sonar_left = sonar_right and both <= ENEMY_DIST
#define ENEMY_TOLERANT       3      // tolerantion between left and right sonar value (ideally are equal)

// those both constants define radius of the circle in step 1A, 1B
//  ! CIRC_MOT1 must be < than CIRC_MOT2 !
//  is is not defined, which is left and which is right wheel, because 'direction' of a circle is randomly generated
#define CIRC_MOT1             0     // speed 1
#define CIRC_MOT2          9000     // speed 2

///////////////////////////////////////////////////////////

strgCirc::strgCirc(rbtHW* hw) : myStrg(hw)
{
    //ctor
    this->name     = 'C';
    this->type     = normal;
    this->critical = false;
    this->enabled  = false;
    this->step     = -1;
}

strgCirc::~strgCirc()
{
    //ctor
}

///////////////////////////////////////////////////////////

//start the strategy
void strgCirc::Start()
{
    this->step   = 0;
    this->rotCnt = 0;
    this->dir    = -1;
}

//stop the strategy
void strgCirc::Stop()
{
    this->step = -1;
}

///////////////////////////////////////////////////////////

//execution of the strategy
//called on 20ms
int strgCirc::exec()
{
    //main strategy execution:
    byte left, right;       //data from sonar

    //main execution:
    if (this->step == 0)
    {
        //control of rotation cnt overflow:
        if (this->rotCnt > ROT_MAXCNT)
        {
            this->callEvSD(ROT_NEXT);
            return 0;
        }

        #ifdef DEBUG_ROT
        this->hw->leds->set(LED_BL, LED_S_OFF);
        this->hw->leds->set(LED_BR, LED_S_OFF);
        #endif

        //random direction:
        if (this->dir < 0) this->dir = rand()%2;
        if (this->dir == 0)
            this->hw->motor->set_sp(CIRC_MOT2, CIRC_MOT1); else this->hw->motor->set_sp(CIRC_MOT1, CIRC_MOT2);

        this->step++;
        this->counter = ROT_FIRST;
        return 0;
    }

    if (this->step == 1)
    {
        //finding an enemy
        left = this->hw->s_sonar->getL();
        right = this->hw->s_sonar->getR();

        if ((left > ENEMY_DIST) || (right > ENEMY_DIST)) return 0;

        if ((left+ENEMY_TOLERANT >= right) && (right+ENEMY_TOLERANT >= left))
        {
            //enemy was found
            this->hw->motor->stop();
            this->callEvSD(ENEMY_STRG);
        }

        //counter decrementing
        this->counter--;
        if (this->counter > 0) return 0;

        //timer overflow -> raw rotation and find of the target
        if (rand()%2 == 0)
            this->hw->motor->turn_left(ROT_SP_RAW); else this->hw->motor->turn_right(ROT_SP_RAW);

        #ifdef DEBUG_ROT
        this->hw->leds->set(LED_BL, LED_S_ON);
        #endif

        this->rotCnt++;         //new rotation -> increment counter
        this->step++;
        this->counter = ROT_TIME_RAW;
        return 0;
    }

    if (this->step == 2)
    {
        //raw rotation and find of the target

        this->counter--;
        if (this->counter <= 0)
        {
            this->step = 0;     //reset the strategy
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

    if (this->step == 3)
    {
        //robot was found -> precision finding

        this->counter--;
        if (this->counter <= 0)
        {
            // precision rotation timeout
            this->step = 0;         //reset the strategy
            return 0;
        }

        left = this->hw->s_sonar->getL();
        right = this->hw->s_sonar->getR();

        if ((left > ENEMY_DIST) || (right > ENEMY_DIST)) return 0;

        if (left == right)
        {
            //the enemy robot was found -> give the control to the ENEMY_STRG strategy
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

bool strgCirc::canDisable()
{
    if (this->step == 0) return true; else return false;
}
