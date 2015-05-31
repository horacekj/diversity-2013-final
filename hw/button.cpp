////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 14.05.2013
////////////////////////////////////////////////////////////////////////////////
// button.cpp: BUTTON CONTROL
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#include "button.h"
#include "main.h"
#include "hw_io_cfg.h"

///////////////////////////////////////////////////////////

//private definitions:
#define IS_S1              (!(S1_PIN&(1<<S1))) // sprawdzamy czy S1 wcisniety

///////////////////////////////////////////////////////////

//ctor
button::button(globalHW* globHW)
{
    //set default values:
    this->globHW    = globHW;
    this->index     = 0;
    this->clicked   = false;

    //clear history array:
    for(int i = 0;i<BUT_HISTORY;i++)
        this->history[i] = 0;

    //init hw:
    this->hw_init();
}//ctor

//dtor
button::~button()
{

}

///////////////////////////////////////////////////////////

void button::hw_init()
{
    S1_PORT|=(1<<S1);
    MCUCR|=(1<<ISC11); // falling edge on the port generates interrupt, but interrupt is not used because of unstability
    GICR|=(1<<INT1);
}

///////////////////////////////////////////////////////////

//get actual button state
bool button::get_but_state()
{
    return ((IS_S1)? false : true);
}

///////////////////////////////////////////////////////////

//update button values and save it to the history
void button::update()
{
    //get data
    this->history[this->index] = get_but_state();
    this->index++;

    //iterator overflow
    if (this->index >= BUT_HISTORY)
    {
        this->check_data();
        this->index = 0;
    }
}

///////////////////////////////////////////////////////////

//check data in this->history[], this function can execute event
void button::check_data()
{
    //event is called, when 100 % of values in history[] are true
    byte tru = 0, fals = 0;

    for(int i = 0;i<BUT_HISTORY;i++)
        if (this->history[i]) tru += 1; else fals += 1;

    //DO NOT change order of those conditions !

    //if all states false, clicked is set to false
    if (fals == BUT_HISTORY)
    {
        this->clicked = false;
        return;
    }

    //if clicked, event cannot be called
    if (this->clicked) return;

    //to call event, all states must be true
    if (tru < BUT_HISTORY) return;

    //call event
    this->callEvOnClick();
    this->clicked = true;
}//button::check_data()

///////////////////////////////////////////////////////////
//events:

//OnClick registration:
void button::evRegOnClick(int (*func)(button* sender))
{
    this->evOnClick = func;
}

//OnClick call:
int button::callEvOnClick()
{
    if (this->evOnClick != 0)
        return (*(this->evOnClick))(this);
    return -1;
}


///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
