////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 08.05.2013
////////////////////////////////////////////////////////////////////////////////
// strgDb.cpp: database of all strategies
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

#include "strgDb.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

//ctor
strgDb::strgDb()
{
    this->cnt = 0;
    this->current = -1;
    this->critical = -1;
    this->time = 0;
}

//dtor
strgDb::~strgDb()
{
    this->cnt = 0;
    for (int i = 0;i<this->cnt;i++) delete this->db[i];
}

//get index in db by name
int strgDb::GetIByName(char name)
{
    for (int i = 0;i<this->cnt;i++)
        if (this->db[i]->getName() == name) return i;
    return -1;
}

//add the strategy to the database
int strgDb::Add(myStrg* strg)
{
    if (this->cnt >= this->MAX) return 1;

    this->db[this->cnt] = strg;

    //register events
    strg->evRegECS(&strgDb::EnterCriticalSection,this);
    strg->evRegLCS(&strgDb::LeaveCriticalSection,this);
    strg->evRegSD(&strgDb::NextStrategy,this);
    strg->evRegSC(&strgDb::SetCurrent,this);

    //if strategy is critical, is enaled
    if (this->db[this->cnt]->getCritical()) this->db[this->cnt]->Start();

    this->cnt++;
    return 0;
}

//remove the strategy from the db
int strgDb::Remove(myStrg* strg)
{
    int i, index;

    //get index
    index = -1;
    for (i = 0;i<this->cnt;i++)
        if (this->db[i] == strg)
        {
            index = i;
            break;
        }

    if (index < 0) return 1;

    for (i = index;i<(this->cnt-1);i++)
        this->db[i] = this->db[i+1];

    this->cnt--;
    return 0;
}

//remove the strategy from the db
int strgDb::Remove(char name)
{
    int index = this->GetIByName(name);
    if (index < 0) return 1;

    if (this->db[index]->getEnabled()) this->db[index]->Stop();

    this->Remove(this->db[index]);
    return 0;
}

//enable the strategy
int strgDb::Enable(char name)
{
    return this->Enable(this->GetIByName(name));
}

//diable the startegy
int strgDb::Disable(char name)
{
    return this->Disable(this->GetIByName(name), true);
}

//call execution of staregies
void strgDb::exec()
{
    this->controlTimeout();

    //when critical section, only the critcal strategy is executed, !! not even other critical strategies !!
    if (this->critical > -1)
    {
        this->db[this->critical]->exec();
        return;
    }

    //critical strategies are execute always
    for (int i = 0;i<this->cnt;i++)
        if ((this->db[i]->getCritical()) && (this->db[i]->getEnabled()))
            this->db[i]->exec();

    //normal strategy is execute only, when its time to execute
    //strategy enabled is not controlled: current strategy must always have resources to execute
    if ((this->current > -1) && (!this->db[this->current]->getCritical())) this->db[this->current]->exec();
}

int strgDb::EnterCriticalSection(myStrg* sender)
{
    if (this->critical != -1) return 1;       //if there is a critical strategy, noone can enter critical section
    for (int i = 0;i<this->cnt;i++)
        if (this->db[i] == sender)
        {
            this->critical = i;
            return 0;
        }

    return 2;
}

int strgDb::LeaveCriticalSection(myStrg* sender)
{
    //everyone can call LeaveCriticalSection(), not necessary to control a strategy
    this->critical = -1;

    //we call start of the actual strategy
    if (this->current > -1) this->db[this->current]->Start();
    return 0;
}

int strgDb::NextStrategy(myStrg* sender, char name)
{
    int index = -1;
    for (int i = 0;i<this->cnt;i++)
        if (this->db[i] == sender)
        {
            index = i;
            break;
        }
    if (index < 0) return 1;        //not found
    this->Disable(index,false);     //critical strategy cannot be disabled, only can enable other strategy

    //no name = random strategy
    if (name == '\0')
    {
        this->RandomEnable();
    } else {
        index = this->GetIByName(name);
        if (index < 0) this->RandomEnable(); else this->Enable(index);       //if not found, random strategy
    }
    this->time = 0;
    return 0;
}//NextStrategy

int strgDb::SetCurrent(myStrg* sender)
{
    int index = -1;
    for (int i = 0;i<this->cnt;i++)
        if (this->db[i] == sender)
        {
            index = i;
            break;
        }
    if (index < 0) return 1;
    if (this->Disable(this->current, true) > 0) return 2;

    this->current = index;

    return 0;
}

void strgDb::RandomEnable()
{
    int index;
    if (this->current > -1)
        if (this->db[this->current]->getCritical()) return;
    this->Disable(this->current, true);

    //the not critical strategy must exist, otherwise, we have got a big problem!
    do {
        srand(TCNT0 + rand());
        index = rand()%(this->cnt);
    } while ((this->db[index]->getCritical()) || (this->db[index]->getType() == init_only));  //we do not want to enable critical strategy, because they are already enabled

    this->Enable(index);
}

void strgDb::InitRandomEnable()
{
    int index;
    if (this->current > -1) return;
    this->Disable(this->current, false);

    //the init strategy must exist, otherwise, we have got a big problem!
    do {
        srand(TCNT0 + rand());
        index = rand()%(this->cnt);
        //index = 0;
    } while ((this->db[index]->getCritical()) || (this->db[index]->getType() == normal));  //we do not want to enable critical strategy, because they are already enabled

    this->Enable(index);
}

void strgDb::controlTimeout()
{
    if (this->current < 0) return;

    this->time++;

    if (this->time > this->TIMEOUT)
    {
        //timeout overflowed
        this->time = 0;
        this->RandomEnable();
    }//timeout overflow
}//strgDb::controlTimeout

//enable a strategy
int strgDb::Enable(int index)
{
    if (this->critical > -1) return 1;
    if (this->current > -1)  return 2;
    if (index < 0)           return 3;

    this->db[index]->Start();
    if (!this->db[index]->getCritical())
    {
        this->current = index;
        this->time = 0;
    }
    return 0;
}

//disable a strategy
int strgDb::Disable(int index,bool control)
{
    if (index < 0) return 1;
//    if (this->db[index]->getCritical()) return 1;
    if ((control) && (!this->db[index]->canDisable())) return 3;

    this->db[index]->Stop();
    this->current = -1;
    return 0;
}
