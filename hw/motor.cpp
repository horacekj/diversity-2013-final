////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 05.05.2013
////////////////////////////////////////////////////////////////////////////////
// motor.cpp: MOTOR CONTROL
// this class uses values specified in hw_io_cfg and drives 2 wheels
// this class is not universal (can drive only 2 specified wheels)
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
#include <stdlib.h>                 //abs() function

#include "motor.h"
#include "hw_io_cfg.h"
#include "../main.h"
#include "hardware.h"

//ctor
wheels::wheels(globalHW* globHW)
{
    this->globHW = globHW;

    //initialize variables:
    this->sp_l = 0;
    this->sp_r = 0;
    this->blocked = false;

    //initialization of motor drivers:

    //motor in mode 8 (PWM, Phase and Frequency Correct)
    TCCR1A = ((1<<COM1A1)|(1<<COM1A0)|(1<<COM1B1)|(1<<COM1B0)|(0<<WGM11)|(0<<WGM10));
    TCCR1B = ((1<<WGM13)|(0<<WGM12)|(1<<CS10));

    ICR1 = PWM_MAX; // maximum frequency
        // f = (16 MHz / 2* ICR1) =
        //PWM_MAX = 10000 -> f = 800 Hz

    //OCR1A = 1. output value (0..ICR1)
    //OCR1B = 2. output value (0..ICR1)

    MLA_DDR|=(1<<MLA);
    MLB_DDR|=(1<<MLB);
    MRA_DDR|=(1<<MRA);
    MRB_DDR|=(1<<MRB);

    MLA_PORT&=~(1<<MLA);
    MLB_PORT&=~(1<<MLB);
    MRA_PORT&=~(1<<MRA);
    MRA_PORT&=~(1<<MRB);

    ML_PWM_DDR|=(1<<ML_PWM);
    MR_PWM_DDR|=(1<<MR_PWM);
}

//dtor
wheels::~wheels()
{

}

///////////////////////////////////////////////////////////

//initialization of io ports:
void wheels::io_init()
{
    //Motor drivers:
    MLA_DDR|=(1<<MLA);
    MLB_DDR|=(1<<MLB);
    MRA_DDR|=(1<<MRA);
    MRB_DDR|=(1<<MRB);

    ML_PWM_DDR|=(1<<ML_PWM);
    MR_PWM_DDR|=(1<<MR_PWM);
}

///////////////////////////////////////////////////////////

//set both motors speed:
void wheels::set_sp(signed long left, signed long right)
{
    this->set_sp_r(right * MOT_OFFSET_R);
    this->set_sp_l(left * MOT_OFFSET_L);

    //send info about speed to the PC (via uart)
    //this->globHW->serial->send_motor_ad_info(left, right, this->globHW->ad_c->motor[0], this->globHW->ad_c->motor[1]);
}

//set both motors speed:
void wheels::set_sp_raw(signed long left, signed long right)
{
    this->set_sp_r(right);
    this->set_sp_l(left);

    //send info about speed to the PC (via uart)
    //this->globHW->serial->send_motor_ad_info(left, right, this->globHW->ad_c->motor[0], this->globHW->ad_c->motor[1]);
}

//set left motor speeed:
//parameter value is in range 0..MAX_PWM_SPEED
void wheels::set_sp_l(signed long left)
{
    if (this->blocked) return;

    unsigned int tmpSpeed;

    //max speed control:
    //max speed depens on the battery voltage
	//tmpSpeed = abs(left) * (this->globHW->bat_motor_mul);
	tmpSpeed = (unsigned int)(abs(left));
	if(tmpSpeed > PWM_MAX)
		OCR1B = (unsigned int)0;
	else
        OCR1B = (unsigned int)PWM_MAX - tmpSpeed;

	if ((signed long)(left*(this->sp_l)) < 0)
	{
	    //change motor direction:
        MLA_PORT|=(1<<MLA);
        MLB_PORT|=(1<<MLB);
        _delay_ms(1);
    }

    if(left > 0)
	{
	    //forward
        MLA_PORT|=(1<<MLA);
        MLB_PORT&=~(1<<MLB);
        TCCR1A |= (1<<COM1B1);
    }

	else if(left < 0)
	{
	    //reverse
        MLA_PORT&=~(1<<MLA);
        MLB_PORT|=(1<<MLB);
        TCCR1A |= (1<<COM1B1);
    }
	else
	{
	    //break
        TCCR1A &= ~(1<<COM1B1);
        MLA_PORT&=~(1<<MLA);
        MLB_PORT&=~(1<<MLB);
    }

    this->sp_l = tmpSpeed;
}

//set right motor speed:
//parameter value is in range 0..MAX_PWM_SPEED
void wheels::set_sp_r(signed long right)
{
    if (this->blocked) return;

    unsigned int tmpSpeed;

    //max speed control:
    //max speed depens on the battery voltage
    tmpSpeed = (unsigned int)(abs(right));
	if(tmpSpeed > PWM_MAX)
		OCR1A = (unsigned int)0;
	else
        OCR1A = (unsigned int)PWM_MAX - tmpSpeed;

	if ((signed long)(right*(this->sp_r)) < 0)
	{
	    //change motor direction
        MRA_PORT|=(1<<MRA);
        MRB_PORT|=(1<<MRB);
        _delay_ms(1);
    }

	if(right > 0)// gdy wartos dodatnia zmiana kierunku obrotow silnika
	{
	    //forward
        MRA_PORT|=(1<<MRA);
        MRB_PORT&=~(1<<MRB);
        TCCR1A |= (1<<COM1A1);
    }
	else if(right < 0)
	{
	    //reverse
        MRA_PORT&=~(1<<MRA);
        MRB_PORT|=(1<<MRB);
        TCCR1A |= (1<<COM1A1);
    }
	else
	{
	    //break
        TCCR1A &= ~(1<<COM1A1);
        MRA_PORT&=~(1<<MRA);
        MRB_PORT&=~(1<<MRB);
    }

    this->sp_r = tmpSpeed;
}

///////////////////////////////////////////////////////////
//get speed values:

signed long wheels::getSpL()
{
    return this->sp_l;
}

signed long wheels::getSpR()
{
    return this->sp_r;
}

///////////////////////////////////////////////////////////
//easy access to setting of a motor speed:

void wheels::go_back(unsigned int speed)
{
    //uses offsets

    //mathematics function dependency:
    //'speed' is set to the right wheel
    //left wheel speed is calculated

    //to-do: implementation of mathematics function dependency

	this->set_sp(-(signed int)speed * MOT_OFFSET_L, -(signed int)speed * MOT_OFFSET_R);
}

void wheels::go_front(unsigned int speed)
{
    //uses offsets

    //mathematics function dependency:
    //'speed' is set to the right wheel
    //left wheel speed is calculated

    //to-do: implementation of mathematics function dependency

	this->set_sp(speed * MOT_OFFSET_L, speed * MOT_OFFSET_R);
}

void wheels::turn_left(unsigned int speed)
{
	this->set_sp( -(signed int)speed, (signed int)speed);
}

void wheels::turn_right(unsigned int speed)
{
	this->set_sp((signed int)speed, -(signed int)speed);
}

void wheels::stop()
{
    this->rotating.time = 0;
    this->set_sp(0, 0);
}

///////////////////////////////////////////////////////////

void wheels::update()
{
    this->rotation_update();
}

///////////////////////////////////////////////////////////

//rotate the robot:
//angle in degrees
//speed cannot be set, because in not linear
void wheels::rotate(signed int angle)
{
    this->rotating.time = ((abs(angle) * this->rotating.OneDegree) + this->rotating.Intertia) / (PWM_MAX) * this->rotating.TimerCorrection;

    if (angle > 0)
    {
        //turn right
        this->set_sp(PWM_MAX, -PWM_MAX);
    } else {
        //turn left
        if (angle < 0) this->set_sp(-PWM_MAX, PWM_MAX);
    }

}

///////////////////////////////////////////////////////////

//control robot rotation:
void wheels::rotation_update()
{
    if (this->rotating.time < 1) return;

    this->rotating.time--;
    if (this->rotating.time <= 0) this->endOfRotation();
}

///////////////////////////////////////////////////////////

//is called, when rotating ends
void wheels::endOfRotation()
{
    //end of turn
    this->stop();
    this->rotating.time = 0;

}
///////////////////////////////////////////////////////////

bool wheels::getRotating()
{
    if (this->rotating.time > 0) return true; else return false;
}

///////////////////////////////////////////////////////////
//motor blocking

bool wheels::getBlocked() {return this->blocked; }

void wheels::block() {this->stop(); this->blocked = true; }

void wheels::unblock() {this->blocked = false; }

///////////////////////////////////////////////////////////
