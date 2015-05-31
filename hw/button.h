////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 14.05.2013
////////////////////////////////////////////////////////////////////////////////
// button.h: HEADER FILE TO THE BUTTON CPP FILE
// this file defines class button, which cares about S1 button
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

#ifndef BUTTON_H
#define BUTTON_H

#include "main.h"

#define BUT_HISTORY 10                      //length history of button states

class globalHW;                             //bacause of cross-references

//this class contains 1 event function: when button is pushed, the function is called
class button
{
    private:
        globalHW* globHW;                                       //pointer to the hardware

        bool history[BUT_HISTORY];                              //history of button states
        byte index;                                             //index in history[]

        bool clicked;                                           //if the button was pushed (used to call event only once), more info in implementation of check_data()

        //event pointers
        int (*evOnClick)(button* sender);                       //event function pointer

        //when we want to call event, we call those methods (from this class) and they call event function
        int callEvOnClick();                                    //call on click event

        void hw_init();                                         //hardware intialization (set outputs)
        bool get_but_state();                                   //get actual button state
        void check_data();                                      //checks data in history[], this function calls event

    public:
        button(globalHW* globHW);                               //ctor
        virtual ~button();                                      //dtor

        void update();                                          //updates values (called in main timer)

        //event registration, called to register event (callback) functions
        void evRegOnClick(int (*func)(button* sender));         //on click event registration

    protected:
};

#endif // SONAR_H
