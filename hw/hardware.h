////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 31.05.2013
////////////////////////////////////////////////////////////////////////////////
// hardware.h: HEADER FILE TO THE HARDWARE CLASS
// class which contains robot`s hardware and methods directly communicagiong with HW
// this class is used as a poiner given to the strategies (strtegies casn control HW)
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

#ifndef HARDWARE_H_INCLUDED
#define HARDWARE_H_INCLUDED

#include "motor.h"
#include "LED.h"
#include "IRs.h"
#include "sonar.h"
#include "button.h"
#include "adc.h"
#include "uart.h"

#define LOW_BATERY  7.0     //in volts
#define BAT_TIMES   50      //when battery status is 50x mesared as low, batLow() event is called

//this class contains global

//robot hardware global methods and properties:
//reference to the instance of globalHW is given to every hardware class (wheels, leds, ...) in ctor
class globalHW {
    private:
        byte low_bat_cnt;                                       //when the low battery level in lowBatCheck() is measured, this value is incremented

        int (*evOnBatLow)(globalHW* sender);                    //event function pointer
        int callEvBatLow();                                     //low battery event calling

    public:
        globalHW();                                             //ctor
        virtual ~globalHW();                                    //dtor

        void batCheck();                                        //periodically checks battery, called in main timer

        adc* ad_c;
        uart* serial;

        void evRegOnBatLow(int (*func)(globalHW* sender));      //low battery event registration

    protected:
};


// robot hardware class:
// all hw is public, because strategies must be able to use the hardware
class rbtHW {
    private:
        void io_init();

    public:
        rbtHW();                        //ctor
        virtual ~rbtHW();               //dtor

        globalHW* glob_hw;

        wheels* motor;
        LED* leds;
        IRs* ir_sens;
        sonar* s_sonar;
        button* but;

        void update();                  //used to update data in all hardware classes (especially sonar), called in timer

    protected:
};

#endif // HARDWARE_H_INCLUDED
