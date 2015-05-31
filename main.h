////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 05.06.2013
////////////////////////////////////////////////////////////////////////////////
// main.cpp: HEADER FILE TO THE ROBOT MAIN FILE
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

#ifndef _main_H_
#define _main_H_

//logical values definiton
#define TRUE 1
#define FALSE 0
#define true 1
#define false 0

#define EEPROM  __attribute__((section(".eeprom")))

#define ID_ROBOTA_ADDR	(unsigned char *)60
#define FIRST_BOOT_ADDR	(unsigned char *)49


// settings of timer 0:
#define TIMER0_PRESK  1024

// timer0 frequency:
#define T0_FREKV    1000    //value in range 62-15625 Hz

//timer 0:
#define     WAIT_TIME (unsigned char)(1000/T0_FREKV)
#define 	MAIN_TIMER_ON  TIMSK |= (1<<OCIE0)
#define 	MAIN_TIMER_OFF TIMSK &=~~ (1<<OCIE0)

#define BYTE unsigned char
#define byte unsigned char

//#define DEBUG_S1    1       //final version - button is not working
//#define DEBUG_SON   1       //sonar debug leds
#define DEBUG_BAT   1       //battery level bebug leds
#define DEBUG_BOR   1       //borders debug leds
#define DEBUG_ROT   1       //rotating debug leds

#define hi(x)      ((x)>>8)
#define lo(x)      ((x) & 0xFF)

#endif //__MiniSumo_main
