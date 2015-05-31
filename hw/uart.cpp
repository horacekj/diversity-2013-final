////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 16.05.2013
////////////////////////////////////////////////////////////////////////////////
//uart.cpp: UART CPP FILE
// this file cares about uart bus = serial communication (with PC)
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

#include "uart.h"
#include "main.h"
#include "hw_io_cfg.h"

//transmitted data format;
// 0. byte = lenght (with this byte and with checksum)
// 1.-(n-2). byte = data
// n-1 byte = checksum

///////////////////////////////////////////////////////////

uart* pUart;       //bacause of access from the interrupts

//ctor
uart::uart()
{
    this->TX_lenght = 0;
    this->TX_pos    = 0;

    this->RX_lenght = 0;

    this->evOnStop  = 0;
    this->evOnSetSp = 0;

    pUart = this;

    //init hw
    this->init();
}

//dtor
uart::~uart()
{

}

///////////////////////////////////////////////////////////

//UART initialization
void uart::init()
{
   UBRRL=16;			//speed 57600@16MHz
	//UBRRL=8;			//speed 57600@8MHz
   UCSRB=0x98;//10011000
   UCSRC=0x86;
}

///////////////////////////////////////////////////////////

//send one byte:
void uart::hw_transmit_byte(byte data)
{
    //Wait for free buffer space
   	while (this->TX_lenght == UART_TX_BUFFER_SIZE);

    //if buffer is empty, write from the [0] element (= from the beginnning)
    if (this->TX_lenght == 0) this->TX_pos = 0;

    //save the data into the buffer
   	this->TX_buf[this->TX_lenght] = data;
   	this->TX_lenght++;

   	UCSRB|=(1<<UDRIE);				//enable send data interrupt (SIG_UART_DATA())
}

///////////////////////////////////////////////////////////

//private send data:
void uart::hw_send_data(byte lenght, byte data[])
{
    for (int i = 0;i<lenght;i++)
        this->hw_transmit_byte(data[i]);
}

///////////////////////////////////////////////////////////

//calculate checksum
//this function will return checksum
byte uart::calc_checksum(byte lenght, byte data[])
{
    //sum is calculated as a xor
    byte sum = 0xFF;
    for (int i = 0;i<lenght;i++)
        sum = sum xor data[i];

    return sum;
}

///////////////////////////////////////////////////////////

//public send data:
//this function gets only data, lenghts and checksum is calculated in the function
int uart::send_data(byte lenght, byte data[])
{
    if (lenght >= (UART_TX_BUFFER_SIZE-1)) return 1;

    //new array, because 0. byte of the packet is lenght of the packet
    byte tmp[lenght+2];
    tmp[0] = (lenght+2);        //+2: lenght byte, checksum byte
    for (int i = 0;i<lenght;i++) tmp[i+1] = data[i];
    lenght++;

    //checksum
    tmp[lenght] = this->calc_checksum(lenght, tmp);
    lenght++;

    //send data
    this->hw_send_data(lenght, tmp);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//Interrupt of the empty transmission (is invoked periodically)
//If the data was sent, it is necessary the switch on / off
////////////////////////////////////////////////////////////////////////////////
SIGNAL(SIG_UART_DATA)
{
    //call class method
    pUart->SIG_UART_DATA();
}

////////////////////////////////////////////////////////////////////////////////
//Class method of the empty transmission ( = transission finished)
////////////////////////////////////////////////////////////////////////////////
void uart::SIG_UART_DATA(void)
{
	if (this->TX_lenght == this->TX_pos) //check that all the data has already been sent
	{
	    UCSRB&=~(1<<UDRIE);			//disable interrupts UDRE
	    this->TX_lenght = 0;
	    this->TX_pos    = 0;
	    return;
	}

   	UDR = this->TX_buf[this->TX_pos];
   	this->TX_pos++;
}

///////////////////////////////////////////////////////////

// one received character method
void uart::USART_RXC_vect(void)
{
    //control buffer overflow
    if (this->RX_lenght >= UART_RX_BUFFER_SIZE)
    {
        //in this very special case, we erase all data
        //because system must have integrity
        this->RX_lenght = 0;
    }

    //add data to the buffer
    this->RX_buf[this->RX_lenght] = UDR;
    this->RX_lenght++;

    //control whole message
    if (this->RX_lenght >= this->RX_buf[0])
    {
        //got new message
        if (!this->control_checksum(this->RX_buf[0], this->RX_buf))
        {
            this->RX_lenght = 0; //clear all data
            return;
        }

        this->parse_data(this->RX_lenght-2, this->RX_buf+1);      //without lenght and checksum
        this->RX_lenght = 0;
    }
}

///////////////////////////////////////////////////////////
// The received character interrupt
///////////////////////////////////////////////////////////
ISR(USART_RXC_vect)
{
    pUart->USART_RXC_vect();
}

///////////////////////////////////////////////////////////

//parsing of read data
void uart::parse_data(byte lenght, byte data[])
{
    switch (data[0])
    {
        case 1: {
                    //motor stop
                    this->callEvOnStop();
                    break;
        }

        case 2: {
                    //motor set speed
                    //get left and right motor speed
                    signed int left  = (data[1]<<8)+data[2];
                    signed int right = (data[3]<<8)+data[4];
                    this->callEvOnSetSp(left, right);
                    break;
        }

        case 3: {
                    //motor get ad values
                    this->callEvOnGetMotAD();
                    break;
        }

        case 4: {
                    //motor FS
                    signed int fd  = (data[1]<<8)+data[2];
                    this->callEvOnSetMotFD(fd);
                    break;
        }

        default:
            break;
    }//switch
}//uart::parse_data

///////////////////////////////////////////////////////////

//control checksum of received message
//return: true  = checksum ok
//return: false = checksum error
//! data in input array are WITH checksum
bool uart::control_checksum(byte lenght, byte data[])
{
    //calculate checksum
    byte check = this->calc_checksum(lenght-1, data);

    //control calculated checksum and last byte value (should be checksum)
    if (check == data[lenght-1]) return true; else return false;
}

///////////////////////////////////////////////////////////

//send motor speed data
void uart::send_motor_info(unsigned int left, unsigned int right)
{
    byte ar[] = {1, hi(left), lo(left), hi(right), lo(right)};
    this->send_data(5, ar);
}

///////////////////////////////////////////////////////////

//send motor speed data + ad speed sensors data
void uart::send_motor_ad_info(unsigned int left, unsigned int right, unsigned int sensl, unsigned int sensr)
{
    byte ar[] = {2, hi(left), lo(left), hi(right), lo(right), hi(sensl), lo(sensl), hi(sensr), lo(sensr)};
    this->send_data(9, ar);
}

void uart::send_motor_only_ad_info(unsigned int sensl, unsigned int sensr)
{
    byte ar[] = {3, hi(sensl), lo(sensl), hi(sensr), lo(sensr)};
    this->send_data(5, ar);
}

///////////////////////////////////////////////////////////
//eventss calling:

int uart::callEvOnStop()
{
    if (this->evOnStop != 0) return (*(this->evOnStop))(this);
    return -1;
}

int uart::callEvOnSetSp(signed int left, signed int right)
{
    if (this->evOnStop != 0) return (*(this->evOnSetSp))(this, left, right);
    return -1;
}

int uart::callEvOnGetMotAD()
{
    if (this->evOnGetMotAD != 0) return (*(this->evOnGetMotAD))(this);
    return -1;
}

int uart::callEvOnSetMotFD(signed int fd)
{
    if (this->evOnSetMotFD != 0) return (*(this->evOnSetMotFD))(this, fd);
    return -1;
}

///////////////////////////////////////////////////////////
//events registration:

void uart::evRegOnStop(int (*func)(uart* sender))
{
    this->evOnStop = func;
}

void uart::evRegOnSetSp(int (*func)(uart* sender, signed int left, signed int right))
{
    this->evOnSetSp = func;
}

void uart::evRegOnGetMotAD(int (*func)(uart* sender))
{
    this->evOnGetMotAD = func;
}

void uart::evRegOnSetMotFD(int (*func)(uart* sender, signed int fd))
{
    this->evOnSetMotFD = func;
}

///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
