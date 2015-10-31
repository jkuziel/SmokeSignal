////////////////////////////////////////////////////////////////////////////////
//
// SmokeSignal - LocalChat standard compliant application
//
// Copyright (C) 2010-2013 Joseph Kuziel and Micca Osmar
//
// This file is part of SmokeSignal.
//
// SmokeSignal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SmokeSignal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SmokeSignal.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////


#include "cmd.h"
#include "net.h"
#include "proc.h"
#include <iostream>
#include <string.h>

using namespace std;
using namespace net;


extern char userName[32];

#ifdef _CONSOLE

int main(int argc,char* argv[])
{
	cout << "SmokeSignal v0.1" << endl;
	cout << "----------------" << endl;
	cout << "  Type \"/help\" for list of commands.\n" << endl;

	if(!initProc())
		return 0;

	initCmd();

	char in_str[141];

	while(1)
	{
		cout << userName << "> ";
		cin.getline(in_str, 140);

		if(in_str[0] == '/')
        {
            if(!strcmp(in_str + 1, "exit"))
                break;
			exeCmd((in_str+1));
            continue;
        }

        sendMsg(in_str);
	}

	exitProc();

	return 0;
}


#endif // _CONSOLE
