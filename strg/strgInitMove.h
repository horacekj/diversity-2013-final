////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 13.06.2013
////////////////////////////////////////////////////////////////////////////////
// strgInitMove.h: HEADER FILE TO THE MOVE INIT STRATEGY
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

#ifndef STRGINITMOVE_H_INCLUDED
#define STRGINITMOVE_H_INCLUDED

#include "rbtStrg.h"

///////////////////////////////////////////////////////////

class strgInitMove : virtual public myStrg {
    private:
        signed char step;                                       //step counter
                            //stopped: -1; moving steps: 0..31; border steps: 32..63;
        unsigned short counter;                                 //timer counter

        void control_en1();          //enemy control in steps 1A, 1B, 2
        void exec_moving();

    protected:
    public:
        strgInitMove(rbtHW* hw);                                //ctor
        virtual ~strgInitMove();                                //dtor

        //described in rbtStrg.h
        virtual void Start();
        virtual void Stop();
        virtual int exec();
        virtual bool canDisable();
};

#endif // STRGINITMOVE_H_INCLUDED
