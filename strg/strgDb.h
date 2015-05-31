////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 08.05.2013
////////////////////////////////////////////////////////////////////////////////
// strgDb.h: database of all strategies
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

#ifndef STRGDBH_H_INCLUDED
#define STRGDBH_H_INCLUDED

#include "rbtStrg.h"

class myStrg;

// database of all strategies
// the class contains references to the all startegies
// when adding a strategy, it is necessary to call Add() to the database class
// main pholosophia: only 1 normal strategy is enabled, exec() of this strategy is called every db exec
//  + are enabled all critical strategies and their exec() is called

// critical strategy can be enabled only by itself event call SetCurrent(this)
// and also disabled only by itself (or can call NextStrategy())

class strgDb {
    private:
        static const int MAX = 32;
        static const int TIMEOUT = 1024;

        int cnt;                        //count of members in database
        myStrg* db[MAX-1];              //database
        int current;                    //current strategy
        int critical;                   //if a strategy wants a critical section, there is saved its index
        int time;                       //how many cycles long is enabled one strategy

        int GetIByName(char name);

        //when in critical section, only critical strategies are executed
        // because of priority of critical strategies (f. e. when turning, critical section is entered)
        int EnterCriticalSection(myStrg* sender);
        int LeaveCriticalSection(myStrg* sender);
        int NextStrategy(myStrg* sender, char name);
        int SetCurrent(myStrg* sender);                     //the stragy is set as current

        void controlTimeout();          //controls strategy timeout

        int Enable(int index);
        int Disable(int index,bool control = false);
    protected:

    public:
        strgDb();
        virtual ~strgDb();

        int Add(myStrg*);               //add the strategy to the database
        int Remove(myStrg*);            //remove the strategy from the db
        int Remove(char name);          //remove the strategy from the db
        int Enable(char name);          //enable the strategy
        int Disable(char name);         //diable the startegy
        void exec();                    //call execution of staregies
        void RandomEnable();            //enable random strategy ( = set random strategy)
        void InitRandomEnable();        //enable radnom init strategy
};

#endif // RBTSTRG_H_INCLUDED
