////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 05.05.2013
////////////////////////////////////////////////////////////////////////////////
// sonar.cpp: SONAR CONTROL
// this file is maintaining sonar (receiving data from ATmega8)
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

#include <stdlib.h>

#include "sonar.h"
#include "main.h"
#include "hw_io_cfg.h"

///////////////////////////////////////////////////////////

//private definitions:
#define SPIPORT PORTB
#define SPIPIN  PINB
#define SPIDDR  DDRB

#define SPISCK   PB7
#define SPIMISO  PB6

#define SPI_SS_HIGH() (S_CS_PORT |= (1<<S_CS))
#define SPI_SS_LOW() (S_CS_PORT &= ~(1<<S_CS))

#define IRQ_DELAY _delay_us(5);
#define TRANSFER_DATA (0x00)

///////////////////////////////////////////////////////////

sonar::sonar(globalHW* globHW)
{
    //ctor
    this->globHW    = globHW;
    this->hist_pos  = 0;

    //set default values
    for(int i = 0;i<SON_HISTORY;i++)
    {
        this->historyL[i] = 0;
        this->historyR[i] = 0;
    }

    //init spi
    this->spi_init();
}//ctor

sonar::~sonar()
{
    //dtor
}

///////////////////////////////////////////////////////////

//initializator the SPI bus
//SPI bus is used to connect to he ATmega8 (sonar processor)
void sonar::spi_init()
{
    S_CS_DDR|=(1<<S_CS); // CS pin as output
	// SCK, SS!!, MOSI as outputs
	SPIDDR |= (1<<SPISCK);
	// MISO as input
	SPIDDR &= ~(1<<SPIMISO);	// cbi(SPIDDR,SPIMISO);
	// Inicialization, prescaler 128
	//SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1);
	SPCR = 0;
}

///////////////////////////////////////////////////////////

//update sonar values and save it to the history
void sonar::update()
{
    //iterator overflow
    if (this->hist_pos >= SON_HISTORY) this->hist_pos = 0;

    //get data
    byte left, right;
    get_data_spi(&left, &right);
    this->historyL[this->hist_pos] = left;
    this->historyR[this->hist_pos] = right;
    this->hist_pos++;
}

///////////////////////////////////////////////////////////
//spi bus communication functions

byte sonar::spi_readwrite(byte data)
{
	SPDR = data;
	// Czekaj na koniec transmisji
	while(!(SPSR & (1<<SPIF)));
	// Zwroc odebrana wartosc
	return SPDR;
}

byte sonar::spi_read(void)
{
	return spi_readwrite(TRANSFER_DATA);
}

///////////////////////////////////////////////////////////
//compare function, need to the median:

#define MyType byte

/*int compare(const void * a, const void * b)
{
  if ( *(MyType*)a <  *(MyType*)b ) return -1;
  if ( *(MyType*)a == *(MyType*)b ) return 0;
  if ( *(MyType*)a >  *(MyType*)b ) return 1;
}*/

/*int compare(const void * a, const void * b)
{
    return ( *(byte*)a - *(byte*)b );
}*/

///////////////////////////////////////////////////////////
//public functions, which returns sensors values ( = distancies)
//they make average of measured data

byte sonar::getL()
{
    //copy data
    //byte my_histL[SON_HISTORY];
    //for (int i = 0;i<SON_HISTORY;i++) my_histL[i] = this->historyL[i];

    //get median
    //qsort(my_histL, SON_HISTORY, sizeof(byte), compare);
    //return (my_histL[(SON_HISTORY-1)/2]);

    //get average
    int souc = 0;
    for (int i = 0;i<SON_HISTORY;i++) souc += this->historyL[i];
    return (souc / SON_HISTORY);
}

byte sonar::getR()
{
    //copy data
    //byte my_histR[SON_HISTORY];
    //for (int i = 0;i<SON_HISTORY;i++) my_histR[i] = this->historyR[i];

    //get median
    //qsort(my_histR, SON_HISTORY, sizeof(byte), compare);
    //return (my_histR[(SON_HISTORY-1)/2]);

    //get average
    int souc = 0;
    for (int i = 0;i<SON_HISTORY;i++) souc += this->historyR[i];
    return (souc / SON_HISTORY);
}

///////////////////////////////////////////////////////////

void sonar::get_data_spi(byte* left, byte* right)
{
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1);  // SPI ON

    SPI_SS_LOW();   // CS to low
    IRQ_DELAY;
    *left = spi_read();
    IRQ_DELAY;
    *right = spi_read();
    SPI_SS_HIGH();  // CS to high

    SPCR = 0;  //SPI OFF
}

///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
