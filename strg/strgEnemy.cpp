////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 15.06.2013
////////////////////////////////////////////////////////////////////////////////
// strgEnemy.h: EXECUTABLE FILE TO THE ENEMY SEARCH STRATEGY
// this strategy is executed when enemy is found and the robot is positioned direct to the enemy
// this strategy is trying to follow the enemy and to push hum out of the arena
// strategy name: 'E' ( = enemy searching)
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

#include "strgEnemy.h"

///////////////////////////////////////////////////////////
//private definitions:

#define FOL_CORRECTION      500         //when one of the sonar has not the same value, how much to change the speed of the wheel
#define FD_SP              7500         //speed of fd

///////////////////////////////////////////////////////////

strgEnemy::strgEnemy(rbtHW* hw) : myStrg(hw)
{
    //ctor
    this->name     = 'E';
    this->type     = normal;
    this->critical = false;
    this->enabled  = false;
    this->step     = -1;
}

strgEnemy::~strgEnemy()
{
    //ctor
}

///////////////////////////////////////////////////////////

//start the strategy
void strgEnemy::Start()
{
    this->hw->leds->set(LED_FL, LED_S_ON);
    this->hw->leds->set(LED_FR, LED_S_ON);

    this->hw->motor->go_front(FD_SP);

    this->step = 0;
}

//stop the strategy
void strgEnemy::Stop()
{
    this->hw->leds->set(LED_FL, LED_S_OFF);
    this->hw->leds->set(LED_FR, LED_S_OFF);

    this->step = -1;
}

///////////////////////////////////////////////////////////

//execution of the strategy
//called on 20ms
int strgEnemy::exec()
{
    this->exec_follow();

    return 0;
}

///////////////////////////////////////////////////////////

bool strgEnemy::canDisable()
{
    if (this->step == 0) return true; else return false;
}

//exection of the following 'strategy'
void strgEnemy::exec_follow()
{
    byte left, right;

    if (this->step == 0)
    {
        //approach to the target and correction of direction
        // ! only left wheel is set, because we always must be in the speed interval

        left = this->hw->s_sonar->getL();
        right = this->hw->s_sonar->getR();

        //if ((left > 5) && (right > 5)) return 0;

        if (left > right)
        {
            this->hw->motor->set_sp_l(this->hw->motor->getSpL()-FOL_CORRECTION);
        } else {
            this->hw->motor->set_sp_l(this->hw->motor->getSpL()+FOL_CORRECTION);
        }

        return;
    }

}

