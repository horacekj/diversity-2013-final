////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 13.06.2013
////////////////////////////////////////////////////////////////////////////////
// strgSearch.h: EXECUTABLE FILE TO THE BORDERS STRATEGY
// the class defines borders startegy
// strategy name: 'B' ( = borders)
// the strategy is CRITICAL !
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

#ifndef STRGBOR_H_INCLUDED
#define STRGBOR_H_INCLUDED

#include "rbtStrg.h"

///////////////////////////////////////////////////////////

class strgBor : virtual public myStrg {
    private:
        byte step;                   //step in agorithm
        unsigned int counter;


    protected:
    public:
        strgBor(rbtHW* hw);         //ctor
        virtual ~strgBor();         //dtor

        virtual void Start();
        virtual void Stop();
        virtual int exec();
        virtual bool canDisable();
};


#endif // RBTSTRG_H_INCLUDED
