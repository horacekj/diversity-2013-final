////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 17.06.2013
////////////////////////////////////////////////////////////////////////////////
// main.cpp: MAIN PROGRAM EXECUTABLE FILE
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
#include <math.h>
#include <stdlib.h>
#include <avr/eeprom.h>

#include "main.h"
#include "obj.h"

#include "hw/hardware.h"
#include "hw/motor.h"
#include "hw/LED.h"
#include "hw/IRs.h"
#include "hw/button.h"

#include "strg/rbtStrg.h"
#include "strg/StrgDb.h"
#include "strg/StrgInitLED.h"
#include "strg/StrgInitMove.h"
#include "strg/StrgBor.h"
#include "strg/StrgEnemy.h"
#include "strg/StrgFront.h"
#include "strg/StrgCirc.h"

//function headers:
int main();                             //main function
void timer0_init();                     //init timer 0
void led_flash(LED* leds);              //init flash with all leds

//button event
int butS1OnClick(button* sender);

//uart events
int uartOnMotStop(uart* sender);
int uartOnMotSetSp(uart* sender, signed int left, signed int right);
int uartOnGetMotAD(uart* sender);
int uartOnMotFD(uart* sender, signed int fd);

//low battery event
int batLow(globalHW* sender);

///////////////////////////////////////////////////////////

//robot hardware is public, because some functions are called in interrupts
//but is accessible only from this file !
rbtHW* hardware;
strgDb* strgs;
bool clicked;           //button was clicked

///////////////////////////////////////////////////////////

int main()
{
    srand(TCNT0 + rand());
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    timer0_init();
    sei();      //allow interrupts

    hardware = new rbtHW;

    //button event registration:
    hardware->but->evRegOnClick(butS1OnClick);

    //serial (uart) event registration
    hardware->glob_hw->serial->evRegOnStop(uartOnMotStop);
    hardware->glob_hw->serial->evRegOnSetSp(uartOnMotSetSp);
    hardware->glob_hw->serial->evRegOnGetMotAD(uartOnGetMotAD);
    hardware->glob_hw->serial->evRegOnSetMotFD(uartOnMotFD);

    //low battery event registration
    hardware->glob_hw->evRegOnBatLow(batLow);

    //led flash
    hardware->leds->flash_all(100);

    hardware->motor->block();
    _delay_ms(10);     //first get data from sonar

    //create the strategy database
    strgs = new strgDb();
    strgs->Add(new strgInitLED(hardware));
    strgs->Add(new strgInitMove(hardware));
    strgs->Add(new strgEnemy(hardware));
    strgs->Add(new strgBor(hardware));
    strgs->Add(new strgFront(hardware));
    strgs->Add(new strgCirc(hardware));

    clicked = false;
    //init a random strategy
    // -> moved to the button click
    //strgs->InitRandomEnable();

    while (true)
    {
        //--- main program loop on 20ms: ---
        _delay_ms(20);

        //execute strategies:
        strgs->exec();

        #ifdef DEBUG_SON
        //this part of code is used only in debug mode:

        byte left, right;           //data from sonar

        //front LEDs shows sonar status
        left  = hardware->s_sonar->getL();
        right = hardware->s_sonar->getR();
        if ((left == right) && (left > 0) && (right > 0) && (left < 255) && (right < 255))
            hardware->leds->set(LED_BL, LED_S_ON); else hardware->leds->set(LED_BL, LED_S_OFF);
        #endif

        #ifdef DEBUG_BAT
        //show battery voltage
        if (hardware->glob_hw->ad_c->battery_V < LOW_BATERY) hardware->leds->set(LED_BR, LED_S_OFF); else hardware->leds->set(LED_BR, LED_S_ON);
        #endif

    }//while true

    delete strgs;
    delete hardware;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Timer 0 initialization
// configured to interrupt every X milliseconds - value in the configuration file
// main.h - T0_FREKV
////////////////////////////////////////////////////////////////////////////////
void timer0_init()
{
	OCR0 = (byte)(((F_CPU/TIMER0_PRESK) / T0_FREKV) - 1);
	TCCR0 |= (1<<WGM01)|(1<<CS00)|(1<<CS02);  //timer0 in mode CTC, prescaler 1024
	TIMSK |= (1<<OCIE0);                      //interrupt the match OC0
}

////////////////////////////////////////////////////////////////////////////////
// Button S1 on click (event called from hardware)
////////////////////////////////////////////////////////////////////////////////
int butS1OnClick(button* sender)
{
    //button is used only when debugging

    if (!clicked)
    {
        clicked = true;
        strgs->InitRandomEnable();
    } else {
        #ifdef DEBUG_S1
        if (hardware->motor->getBlocked()) hardware->motor->unblock(); else hardware->motor->block();
        #endif
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Button S1 interrupt
////////////////////////////////////////////////////////////////////////////////
ISR(SIG_INTERRUPT1)
{
    //this interrupt is not used
    //we use event function butS1click();
}

////////////////////////////////////////////////////////////////////////////////
// Timer 0 interrupt
////////////////////////////////////////////////////////////////////////////////
//main loop on 1ms
ISR(TIMER0_COMP_vect) //interruption of Timer/Counter0 Compare Match
{
    // --- BATTERY CONTROL : ---
    hardware->glob_hw->batCheck();

    // --- HARDWARE UPDATE: ---
    hardware->update();
}

////////////////////////////////////////////////////////////////////////////////
//events from uart:

//set motor stop
int uartOnMotStop(uart* sender)
{
    hardware->motor->stop();
    return 0;
}

//set motor speed
int uartOnMotSetSp(uart* sender, signed int left, signed int right)
{
    hardware->motor->set_sp(left, right);
    return 0;
}

int uartOnGetMotAD(uart* sender)
{
    hardware->glob_hw->serial->send_motor_only_ad_info(hardware->glob_hw->ad_c->motor[0], hardware->glob_hw->ad_c->motor[1]);
    return 0;
}

int uartOnMotFD(uart* sender, signed int fd)
{
    hardware->motor->go_front(fd);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

//battery low event:
int batLow(globalHW* sender)
{
    //to-do: what to do ?

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
