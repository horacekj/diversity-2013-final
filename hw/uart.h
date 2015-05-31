////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 16.05.2013
////////////////////////////////////////////////////////////////////////////////
//uart.h: HEADER FILE TO THE UART CPP FILE
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

#ifndef UART_H
#define UART_H

#include "main.h"

#define UART_TX_BUFFER_SIZE 64
#define UART_RX_BUFFER_SIZE 64

//data protocol:
// 0. byte = message lenght (all bytes, even 0.)
// 1. byte = command byte
// 2.-(n-2) byte = data
// n-1 byte = checksum

//protocol robot to PC:
//header byte:  other bytes:
// 1 = send motor speed
//              [2] = motor left hi
//              [3] = motor left lo
//              [4] = motor right hi
//              [5] = motor right lo

// 2 = send motor speed and values from motor sensors
//              [2] = motor left hi
//              [3] = motor left lo
//              [4] = motor right hi
//              [5] = motor right lo
//              [6] = motor sensor left hi
//              [7] = motor sensor left lo
//              [8] = motor sensor right hi
//              [9] = motor sensor right lo

// 3 = send motor ad values
//              [2] = motor sensor left hi
//              [3] = motor sensor left lo
//              [4] = motor sensor right hi
//              [5] = motor sensor right lo


//protocol PC to robot:

//header byte:  other bytes:
// 1 = motor stop

// 2 = set motor speed
//              [2] = motor left hi
//              [3] = motor left lo
//              [4] = motor right hi
//              [5] = motor right lo

// 3 = get motor ad values

// 4 = set motor fd speed
//              [2] = motor speed

class uart
{
    private:
        byte RX_buf[UART_RX_BUFFER_SIZE];                   //RX buffer
        byte RX_lenght;                                     //actual RX buffer lenght

        byte TX_buf[UART_TX_BUFFER_SIZE];                   //TX buffer
        byte TX_lenght;                                     //actual TX buffer lenght
        byte TX_pos;                                        //position of sending byte in TX buffer

        void hw_transmit_byte(byte data);                   //transmit one byte
        void hw_send_data(byte lenght, byte data[]);        //send array via uart
        byte calc_checksum(byte lenght, byte data[]);       //calculate checksum (checksum is returned, not added to the array)

        bool control_checksum(byte lenght, byte data[]);    //control of checksum (byte data[lenght-1]) is a checksum (this byte is controlled)
        void parse_data(byte lenght, byte data[]);          //paste read data

        void init(void);                                    //init hardware (uart communication)

        //events:
        int (*evOnStop)(uart* sender);                                      //motor stop event function pointer
        int (*evOnSetSp)(uart* sender, signed int left, signed int right);  //motor set speed event function pointer
        int (*evOnGetMotAD)(uart* sender);  //
        int (*evOnSetMotFD)(uart* sender, signed int fd);  //

        //when we want to call event, we call those methods (from this class) and they call event function
        int callEvOnStop();                                                 //call motor stop event
        int callEvOnSetSp(signed int left, signed int right);               //call motor set spedd event
        int callEvOnGetMotAD();
        int callEvOnSetMotFD(signed int fd);

    public:
        int send_data(byte lenght, byte data[]);            //send array

        uart();                                             //ctor
        virtual ~uart();                                    //dtor

        void SIG_UART_DATA(void);                           //data send interrupt
        void USART_RXC_vect(void);                          //data receive interrupt

        void send_motor_info(unsigned int left, unsigned int right);        //send info about motor speed (called from motor class)
        void send_motor_ad_info(unsigned int left, unsigned int right, unsigned int sensl, unsigned int sensr);     //send info about motor speed and motor ad values
        void send_motor_only_ad_info(unsigned int sensl, unsigned int sensr);

        //event registration, called to register event (callback) functions
        void evRegOnStop(int (*func)(uart* sender));                        //motor stop event registration
        void evRegOnSetSp(int (*func)(uart* sender, signed int left, signed int right));         //motor set speed event registration
        void evRegOnGetMotAD(int (*func)(uart* sender));
        void evRegOnSetMotFD(int (*func)(uart* sender, signed int fd));

    protected:
};

#endif // SONAR_H
