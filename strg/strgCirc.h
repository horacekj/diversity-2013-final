////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 13.06.2013
////////////////////////////////////////////////////////////////////////////////
// strgCirc.h: HEADER FILE TO THE CIRC MOVE STRATEGY
// startegy, which moves robots on the 'spirala' (! translate to english)
// this strategy is known as step 4 in the Paper
// this strategy periodically tries to find the enemy via rotating
// this startegy is very similar to the strgFront
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

#ifndef STRGCIRC_H_INCLUDED
#define STRGCIRC_H_INCLUDED

#include "rbtStrg.h"

///////////////////////////////////////////////////////////

class strgCirc : virtual public myStrg {
    private:
        byte rotCnt;                //number of rotations
        unsigned int counter;       //general counter
        byte step;                  //algorithm current step
        byte dir;

    protected:
    public:
        strgCirc(rbtHW* hw);        //ctor
        virtual ~strgCirc();        //dtor

        //desribed in rbtStrg.h
        virtual void Start();
        virtual void Stop();
        virtual int exec();
        virtual bool canDisable();
};

#endif // STRGCIRC_H_INCLUDED
