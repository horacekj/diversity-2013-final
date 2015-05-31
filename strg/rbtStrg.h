////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 08.05.2013
////////////////////////////////////////////////////////////////////////////////
// rbtStrg.h: header file for myStrg virtual class
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

#ifndef RBTSTRG_H_INCLUDED
#define RBTSTRG_H_INCLUDED

#include "strgDb.h"
#include "../hw/hardware.h"

// header file for myStrg virtual class
// all strategies all born from the virtual class myStrg

// at least one strategy must be init

//because of cross references
class myStrg;
class strgDb;

//strategy type
typedef enum {
    normal,         //cannost be enabled in init
    init,           //can be enabled always
    init_only       //can be enabled only in init
} TmyStrgType;

//events:
typedef struct {
    int (strgDb::*evFunc)(myStrg* sender);                 //function pointer
    strgDb* parent;                                        //where to call event (parent class)
} TmyPlainEvent;

typedef struct {
    int (strgDb::*evFunc)(myStrg* sender, char str);      //function pointer
    strgDb* parent;                                        //where to call event (parent class)
} TmyStringEvent;

//class myStrg
class myStrg {
    private:
    protected:
        TmyStrgType type;       //strategy type
        char name;              //startegy name
        bool enabled;           //is startegy enabled ?
        bool critical;          //critical strategy is always called
        rbtHW* hw;              //pointer to the all robot hardware

        //event pointers
        TmyPlainEvent evECS;    //enter critical section
        TmyPlainEvent evLCS;    //leave critical section
        TmyStringEvent evSD;    //self - destruct
        TmyPlainEvent evSC;     //set current

        //when we want to call event, we call those methods (from myStrrg) and they call parent objects` event
        int callEvECS();        //Enter Critical Section
        int callEvLCS();        //Leave Critical Section
        int callEvSD(char);    //Self-destruct ( = Next Strategy)
        int callEvSC();         //Set Current; sets the strategy as current in database; used for example in Follow strategy
    public:
        myStrg(rbtHW*);                                 //ctor
        virtual ~myStrg();                              //dtor

        virtual void Start() {this->enabled = true;}    //start (enable) the strategy, is called when strategy is started or when we gget back from critical section and the strategy is current
        virtual void Stop() {if (!this->critical) this->enabled = false;}    //stop (disable) the strategy, is called whens trategy is stopping
        virtual int exec() = 0;                         //startegy execution, is called in main loop (when strategy is enabled)
        virtual bool canDisable() = 0;                  //we ask the strategy, when we want to disable it by normal process (by hard disable there is no ask) and it can answer yes/no

        char getName() {return this->name;}
        bool getCritical() {return this->critical;}
        bool getEnabled() {return this->enabled;}
        TmyStrgType getType() {return this->type;}

        //event registration, called to register event (callback) functions
        void evRegECS(int (strgDb::*func) (myStrg*),strgDb*);
        void evRegLCS(int (strgDb::*func) (myStrg*),strgDb*);
        void evRegSD(int (strgDb::*func) (myStrg*, char),strgDb*);
        void evRegSC(int (strgDb::*func) (myStrg*),strgDb*);
};

#endif // RBTSTRG_H_INCLUDED
