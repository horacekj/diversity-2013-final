////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 15.06.2013
////////////////////////////////////////////////////////////////////////////////
// strgInitMove.h: EXECUTABLE FILE TO THE MOVE INIT STRATEGY
// this strategy is executed after led init strategy and provides 1. step of whole strategy
// this strategy moves robot to the center of the circle according to the step 1 of the Jedovnice strategy
// strategy name: 'M' ( = moving init)
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

#include "strgInitMove.h"
#include <stdlib.h>     //rand()
#include <avr/io.h>     //TCNT0

///////////////////////////////////////////////////////////
//private definitions:

#define STRG_NEXT             'C'       // circ strategy is next
#define STRG_BOR_NEXT         'F'       // which startegy to execute after border 'interrupt'

// steps indexes is following defitions are taken from the Paper sketch, they are not simillar to the firmware steps !

#define ENEMY_STRG           'E'        // what strategy to call, when enemy is detected
#define ENEMY_DIST            10        // enemy is detected, when sonar_left = sonar_right and both <= ENEMY_DIST
#define ENEMY_TOLERANT         0        // tolerantion between left and right sonar value (ideally are equal)

// those both constants define radius of the circle in step 1A, 1B
//  ! STEP1_MOT1 must be < than STEP1_MOT2 !
//  is is not defined, which is left and which is right wheel, because 'direction' of a circle is randomly generated
#define STEP1_MOTL_MINUS   10000        // circle on the clockwise - speed L
#define STEP1_MOTR_MINUS    2400        // circle on the clockwise - speed R

#define STEP1_MOTL_PLUS     2600        // circle on the anticlockwise - speed L
#define STEP1_MOTR_PLUS    10000        // circle on the anticlockwise - speed R

#define STEP1_CIRC_LEN        62        // 'lenght' of circle circuit

#define STEP2_SP           10000        // go_front speed
#define STEP2_LEN              7        // go_front lenght

#define STEP3_SP_RAW        8500        // raw turning speed
#define STEP3_SP_PREC       5000        // precision turning speed
#define STEP3_LEN_PREC        20        // precision turning timeout
#define STEP3_LEN_RAW         33        // war turning timeout

///////////////////////////////////////////////////////////

strgInitMove::strgInitMove(rbtHW* hw) : myStrg(hw)
{
    //ctor
    this->name     = 'M';
    this->type     = normal;
    this->critical = false;
    this->enabled  = false;

    this->step = -1;
}

///////////////////////////////////////////////////////////

strgInitMove::~strgInitMove()
{
    //ctor
}

///////////////////////////////////////////////////////////

//start the strategy
void strgInitMove::Start()
{
    this->step = 0;
}

///////////////////////////////////////////////////////////

//stop the strategy
void strgInitMove::Stop()
{
    this->step = -1;
}

///////////////////////////////////////////////////////////

//execution of the strategy
//called on 20ms
int strgInitMove::exec()
{
    if (this->step < 0) return 0;

    //enemy control only in specific steps
    if ((this->step >= 0) && (this->step <= 6)) this->control_en1();

    //sttaregy main execution
    this->exec_moving();

    return 0;
}

///////////////////////////////////////////////////////////

bool strgInitMove::canDisable()
{
    return false;
}

///////////////////////////////////////////////////////////

//enemy control in the Paper steps 1A, 1B, 2
void strgInitMove::control_en1()
{
    byte left  = this->hw->s_sonar->getL();
    byte right = this->hw->s_sonar->getR();

    if ((left == right) && (left > 0) && (left <= ENEMY_DIST))
    {
        this->callEvSD(ENEMY_STRG);     //strategy to follow the enemy eis executed
    }
}

//strategy main execution
void strgInitMove::exec_moving()
{
    byte left, right;           //data from sonnar, assignet later

    if (this->step == 0)
    {
        //turn robot left / right
        srand(TCNT0 + rand());
        if (rand()%2 == 0)
        {
            //turn left
            this->hw->motor->rotate(-90);
            this->step = 1;
        } else {
            //turn right
            this->hw->motor->rotate(+90);
            this->step = 2;
        }

        return;
    }//case 0

    if ((this->step == 1) || (this->step == 2))
    {
        //wait for end of rotation
        if (this->hw->motor->getRotating()) return;

        //robot has stopped rotating itself
        //step 1 = turn right; step 2 = trn right
        if (this->step == 1)
            this->hw->motor->set_sp_raw(STEP1_MOTL_MINUS, STEP1_MOTR_MINUS); else this->hw->motor->set_sp_raw(STEP1_MOTL_PLUS, STEP1_MOTR_PLUS);

        this->step += 2;
        this->counter = STEP1_CIRC_LEN;
        return;
    }

    if ((this->step == 3) || (this->step == 4))
    {
        //move on the circle (near to the arena edge, but so fast, that robot cannot be found or destroyed)
        this->counter--;
        if (this->counter > 0) return;

        //counter overflow -> next step = turn robot to the center
        if (this->step == 3)
            this->hw->motor->rotate(90); else this->hw->motor->rotate(-90);

        this->step = 5;
        return;
    }

    if (this->step == 5)
    {
        //wait for end of rotation
        if (this->hw->motor->getRotating()) return;

        //end of rotation -> fd to the center of the circle
        this->hw->motor->go_front(STEP2_SP);

        this->step++;
        this->counter = STEP2_LEN;
        return;
    }

    if (this->step == 6)
    {
        //getting the robot to the center
        this->counter--;
        if (this->counter > 0) return;

        //counter overflow -> turn the robot and try to find the enemy
        if (rand()%2 == 0)
            this->hw->motor->turn_left(STEP3_SP_RAW); else this->hw->motor->turn_right(STEP3_SP_RAW);

        this->step++;
        this->counter = STEP3_LEN_RAW;
        return;
    }

    if (this->step == 7)
    {
        //raw finding of a target
        // ! this step is not treated with control_en1(), because it is irrelevant

        //timeout
        this->counter--;
        if (this->counter <= 0)
        {
            //timeout overflow
            this->hw->motor->stop();
            this->callEvSD(STRG_NEXT);
            return;
        }

        left = this->hw->s_sonar->getL();
        right = this->hw->s_sonar->getR();

        if ((left > ENEMY_DIST) || (right > ENEMY_DIST)) return;

        if ((left+ENEMY_TOLERANT >= right) && (right+ENEMY_TOLERANT >= left))
        {
            //raw found was successful -> precision finding
            if (this->hw->motor->getSpL() > this->hw->motor->getSpR())
                this->hw->motor->turn_left(STEP3_SP_PREC); else this->hw->motor->turn_right(STEP3_SP_PREC);

            this->counter = STEP3_LEN_PREC;
            this->step++;
        }

        return;
    }

    if (this->step == 7)
    {
        //precision finding of a target

        //timeout
        this->counter--;
        if (this->counter <= 0)
        {
            //timeout overflow
            this->hw->motor->stop();
            this->callEvSD(STRG_NEXT);
            return;
        }

        left = this->hw->s_sonar->getL();
        right = this->hw->s_sonar->getR();

        if ((left == 0) || (right == 0)) return;

        if ((left+ENEMY_TOLERANT >= right) && (right+ENEMY_TOLERANT >= left))
        {
            //enemy robot was found
            this->hw->motor->stop();
            this->callEvSD(ENEMY_STRG);
            return;
        }

        return;
    }

    if (this->step == 10)
    {
        //finding timeout overflow -> execute next strategy
        this->hw->motor->stop();
        this->callEvSD(STRG_NEXT);

        return;
    }
}

