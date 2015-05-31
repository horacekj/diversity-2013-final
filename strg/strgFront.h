////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 17.06.2013
////////////////////////////////////////////////////////////////////////////////
// strgFront.h: HEADER FILE TO THE FRONT MOVE STRATEGY
// this strategy is executed when there is no else specific strategy
// this strategy moves the robot in the arena on circles with big radiuses
//  and tries to find an enemy (is rotates the robot and searches in the neighborhood)
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

#ifndef STRGFRONT_H_INCLUDED
#define STRGFRONT_H_INCLUDED

#include "rbtStrg.h"

///////////////////////////////////////////////////////////

class strgFront : virtual public myStrg {
    private:
        unsigned int counter, counter2;       //global counter
        byte step;                  //algorithm current step

    protected:
    public:
        strgFront(rbtHW* hw);       //ctor
        virtual ~strgFront();       //dtor

        //described in rbtStrg.h
        virtual void Start();
        virtual void Stop();
        virtual int exec();
        virtual bool canDisable();
};

#endif // STRGFRONT_H_INCLUDED
