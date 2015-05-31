////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 10.05.2013
////////////////////////////////////////////////////////////////////////////////
// sonar.h: HEADER FILE TO THE SONAR CPP FILE
// how it works: this class continually takes sonar data and when left or right distance is wanted, average is made
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

#ifndef SONAR_H
#define SONAR_H

#include "main.h"

#define SON_HISTORY 10                      //median / average is made from 3 values

class globalHW;                             //bacause of cross-references

class sonar
{
    private:
        globalHW* globHW;
        byte historyL[SON_HISTORY];                             //history of left sonar
        byte historyR[SON_HISTORY];                             //history of right sonar
        byte hist_pos;                                          //position in history arrays (same for both)

        void spi_init();                                        //init of SPI bus
        void get_data_spi(byte* left, byte* right);             //get data from ATmega8 (SPI)

        byte spi_readwrite(byte data);                    //SPI bus data read / write
        byte spi_read(void);                                 //SPI bus adta read

    public:
        sonar(globalHW* globHW);                                //ctor
        virtual ~sonar();                                       //update

        void update();                                          //updates sonar values

        byte getL();                                            //get left value ( = distance)
        byte getR();                                            //get right sonar value ( = distance)

    protected:
};

#endif // SONAR_H
