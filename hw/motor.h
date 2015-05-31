////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 05.05.2013
////////////////////////////////////////////////////////////////////////////////
// motor.h: HEADER FILE TO TOHE MOTOR CLASS
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

#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

//maximum values:
#define PWM_MAX         10000
#define MAX_PWM_SPEED   8500        //because of corrections

//definition of directions:
#define DIR_LEFT        0
#define DIR_RIGHT       1

//motor ofsets oin %
#define MOT_OFFSET_L    1.05    //used only in function go_front() and go_back() - function set_sp(...) must accept original speeds (because of robot rotating)
#define MOT_OFFSET_R    1

#include "../main.h"

//because of cross reference
class globalHW;

class wheels {
    private:
        globalHW* globHW;

        long sp_l, sp_r;
        void io_init();
        bool blocked;

        struct {                //rotating data
            static const float OneDegree = 2450;     //how much degree in rotated in 10ms (timer loop)
            static const float Intertia  = 15000;    //intertia correction
            static const float TimerCorrection  = 10;//timer correction
            unsigned int time;                       //there is (OneDefree*degree), decremeting in rotation_update(), when turning stopped, there is zero
        } rotating;

        void rotation_update();
        void endOfRotation();

    public:

        wheels(globalHW* globHW);                           //ctor
        virtual ~wheels();                                  //dtor

        void set_sp_l(signed long left);
        void set_sp_r(signed long right);
        void set_sp(signed long left, signed long right);
        void set_sp_raw(signed long left, signed long right);
        signed long getSpL();
        signed long getSpR();

        void rotate(signed int angle);                      //angle in degrees
        void update();                                      //called every 10ms
        bool getRotating();

        //blocking of the motor
        bool getBlocked();
        void block();
        void unblock();

        //easy setting of max values
        void go_front(unsigned int speed = MAX_PWM_SPEED);
        void go_back(unsigned int speed = MAX_PWM_SPEED);
        void turn_left(unsigned int speed = MAX_PWM_SPEED);
        void turn_right(unsigned int speed = MAX_PWM_SPEED);
        void stop();

    protected:
};

#endif // MOTOR_H_INCLUDED
