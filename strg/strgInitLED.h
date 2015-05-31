////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 31.05.2013
////////////////////////////////////////////////////////////////////////////////
// strgInit.h: HEADER FILE TO THE LED INIT STRATEGY
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

#ifndef STRGINITLED_H_INCLUDED
#define STRGINITLED_H_INCLUDED

#include "rbtStrg.h"

///////////////////////////////////////////////////////////

class strgInitLED : virtual public myStrg {
    private:
        signed char step;                                       //step counter
        unsigned short counter;                                 //timer counter
        bool first;                                             //used in exec()

    protected:
    public:
        strgInitLED(rbtHW* hw);                                 //ctor
        virtual ~strgInitLED();                                 //dtor

        //methods described in rbtStrg.h
        virtual void Start();
        virtual void Stop();
        virtual int exec();
        virtual bool canDisable();
};

#endif // STRGINITLED_H_INCLUDED
