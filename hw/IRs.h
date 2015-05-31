////////////////////////////////////////////////////////////////////////////////
// Jan Horacek
// jan.horacek@seznam.cz
// Diversity 2013
// 08.05.2013
////////////////////////////////////////////////////////////////////////////////
// IRs.h: HEADER FILE TO WORK WITH IRs
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

#ifndef IRS_H
#define IRS_H

#define IS_LINL_F           (LINL_F_PIN&(1<<LINL_F))
#define IS_LINR_F           (LINR_F_PIN&(1<<LINR_F))
#define IS_LINL_B           (LINL_B_PIN&(1<<LINL_B))
#define IS_LINR_B           (LINR_B_PIN&(1<<LINR_B))

#define IRID_FL             0           //front-left
#define IRID_FR             1           //front-right
#define IRID_BL             2           //front-left
#define IRID_BR             3           //front-right

#define LINLPWR_ON         LINLPWR_PORT&=~(1<<LINLPWR)
#define LINLPWR_OFF        LINLPWR_PORT|=(1<<LINLPWR)

#include "main.h"

class globalHW;

class IRs
{
    private:
        globalHW* globHW;

    public:
        IRs(globalHW* globHW);
        virtual ~IRs();

        bool get(byte id);

    protected:

};

#endif // IRS_H
