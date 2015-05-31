////////////////////////////////////////////////////////////////////////////////
// Adam Sarzyñski, Jan Horacek
// adam@wobit.com.pl, adamusx@tlen.pl, jan.horacek@seznam.cz
// WObit 2009
// 08-05-13
////////////////////////////////////////////////////////////////////////////////
// Hardware configuration of MAOR-12T (especially processor pins)
///////////////////////////////////////////////////////////////////////////////

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

// Switch S1:
#define S1 	               3
#define S1_DDR 	           DDRD
#define S1_PORT 	       PORTD
#define S1_PIN 	           PIND

// LED:
// front-left:
#define LED_LD_FL 	       7
#define LED_LD_FL_DDR 	   DDRA
#define LED_LD_FL_PORT 	   PORTA
// front-right:
#define LED_LD_FR 	       3
#define LED_LD_FR_DDR 	   DDRC
#define LED_LD_FR_PORT 	   PORTC
// back-left:
#define LED_LD_BL 	       6
#define LED_LD_BL_DDR 	   DDRA
#define LED_LD_BL_PORT 	   PORTA
// back-right:
#define LED_LD_BR 	       1
#define LED_LD_BR_DDR 	   DDRC
#define LED_LD_BR_PORT 	   PORTC

//Motor drivers:
//pins A,B are used to set direction and/or stop/brake
//pin PWW is used to generate PWM pulzes
//pin SENS = ?

// Left motor driver:
#define MLA 	           6
#define MLA_DDR 	       DDRC
#define MLA_PORT 	       PORTC

#define MLB 	           5
#define MLB_DDR 	       DDRC
#define MLB_PORT 	       PORTC

#define ML_PWM 	           4
#define ML_PWM_DDR 	       DDRD
#define ML_PWM_PORT 	   PORTD

#define ML_SENS 	       4
#define ML_SENS_DDR 	   DDRA
#define ML_SENS_PORT 	   PORTA


// Right motor driver:
#define MRA 	           2
#define MRA_DDR 	       DDRC
#define MRA_PORT 	       PORTC

#define MRB 	           4
#define MRB_DDR 	       DDRC
#define MRB_PORT 	       PORTC

#define MR_PWM 	           5
#define MR_PWM_DDR 	       DDRD
#define MR_PWM_PORT 	   PORTD

#define MR_SENS 	       5
#define MR_SENS_DDR 	   DDRA
#define MR_SENS_PORT 	   PORTA


// distance IRs:
#define LINLPWR 	       6            //?
#define LINLPWR_DDR 	   DDRD         //?
#define LINLPWR_PORT 	   PORTD        //?

// front-left:
#define LINL_F 	           1
#define LINL_F_DDR 	       DDRB
#define LINL_F_PORT 	   PORTB
#define LINL_F_PIN 	       PINB
// front-right:
#define LINR_F 	           7
#define LINR_F_DDR 	       DDRD
#define LINR_F_PORT 	   PORTD
#define LINR_F_PIN 	       PIND
// back-left:
#define LINL_B 	           0
#define LINL_B_DDR 	       DDRB
#define LINL_B_PORT 	   PORTB
#define LINL_B_PIN 	       PINB
// back-right:
#define LINR_B 	           0
#define LINR_B_DDR 	       DDRC
#define LINR_B_PORT 	   PORTC
#define LINR_B_PIN 	       PINC

// Battery sensor:
#define BAT_SENS	       3
#define BAT_SENS_DDR 	   DDRA
#define BAT_SENS_PORT 	   PORTA
#define BAT_SENS_PIN 	   PINA

// Sonar communication:
#define S_CS	           7
#define S_CS_DDR 	       DDRC
#define S_CS_PORT 	       PORTC
#define S_CS_PIN 	       PINC
