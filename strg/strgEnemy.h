////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 13.06.2013
////////////////////////////////////////////////////////////////////////////////
// strgEnemy.h: HEADER FILE TO THE ENEMY SEARCH STRATEGY
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

#ifndef STRGENEMY_H_INCLUDED
#define STRGENEMY_H_INCLUDED

#include "rbtStrg.h"

///////////////////////////////////////////////////////////

class strgEnemy : virtual public myStrg {
    private:
        unsigned int counter;
        byte step;                  //algorithm current step
                    //stopped: -1; moving steps: 0..31; border steps: 32..63;

        void exec_follow();         //exection of the following 'strategy'

    protected:
    public:
        strgEnemy(rbtHW* hw);       //ctor
        virtual ~strgEnemy();       //dtor

        //described in rbtStrg.h
        virtual void Start();
        virtual void Stop();
        virtual int exec();
        virtual bool canDisable();
};

#endif // STRGENEMY_H_INCLUDED
