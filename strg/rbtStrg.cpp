////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 08.05.2013
////////////////////////////////////////////////////////////////////////////////
// rbtStrg.h: executable file for myStrg virtual class
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

#include "rbtStrg.h"
#include "../hw/hardware.h"

////////////////////////////////////////////

myStrg::myStrg(rbtHW* hw)
{
    //ctor
    this->hw = hw;

    this->name     = '\0';
    this->type     = normal;
    this->enabled  = false;
    this->critical = false;

    //initialize events
    this->evECS.evFunc = 0;
    this->evLCS.evFunc = 0;
    this->evSD.evFunc = 0;
}

myStrg::~myStrg()
{
    //dtor
}

/////////////////////////////////////////////////////////////
//events registration

void myStrg::evRegECS(int (strgDb::*func)(myStrg*), strgDb* db)
{
    this->evECS.evFunc = func;
    this->evECS.parent = db;
}

void myStrg::evRegLCS(int (strgDb::*func)(myStrg*), strgDb* db)
{
    this->evLCS.evFunc = func;
    this->evLCS.parent = db;
}

void myStrg::evRegSD(int (strgDb::*func)(myStrg*, char), strgDb* db)
{
    this->evSD.evFunc = func;
    this->evSD.parent = db;
}

void myStrg::evRegSC(int (strgDb::*func) (myStrg*),strgDb* db)
{
    this->evSC.evFunc = func;
    this->evSC.parent = db;
}

///////////////////////////////////////////////////////////
//events call

int myStrg::callEvECS()
{
    if ((this->evECS.evFunc != 0) && (this->evECS.parent != 0))
        return ((this->evECS.parent)->*(this->evECS.evFunc))(this);
    return -1;
}

int myStrg::callEvLCS()
{
    if ((this->evLCS.evFunc != 0) && (this->evLCS.parent != 0))
        return ((this->evLCS.parent)->*(this->evLCS.evFunc))(this);
    return -1;
}

int myStrg::callEvSD(char str)
{
    if ((this->evSD.evFunc != 0) && (this->evSD.parent != 0))
        return ((this->evSD.parent)->*(this->evSD.evFunc))(this, str);
    return -1;
}

int myStrg::callEvSC()
{
    if ((this->evSC.evFunc != 0) && (this->evSC.parent != 0))
        return ((this->evSC.parent)->*(this->evSC.evFunc))(this);
    return -1;
}
