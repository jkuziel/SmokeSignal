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
#include <cstring>
#include <string>
#include <map>

using namespace std;
using namespace net;


extern char userName[32];


// command function pointer type
typedef void(*cmdptr)(int argc,const char** argv);

// command map
typedef map<const string, cmdptr> cmd_map_type;
typedef cmd_map_type::iterator cmd_map_itr;

cmd_map_type cmd_map;



void cmd_help(int argc,const char** argv)
{
	cout << "Commands:" << endl;
	cout << "  /help : shows help" << endl;
	cout << "  /name <name> : sets your user name" << endl;
	cout << "  /sendto <username> : specify user id you're chatting with or none for all" << endl;
	cout << "  /key <userid> <key> : use encription key for specific user" << endl;
	cout << "  /hi : broadcast to all available users" << endl;
}

void cmd_name(int argc,const char** argv)
{
	if(argc != 2)
	{
		cout << "name takes one argument" << endl;
		return;
	}

	strncpy(userName, argv[1], 32);
}

void cmd_sendto(int argc,const char** argv)
{
	if(argc > 2)
	{
		cout << "sendto takes zero or one argument" << endl;
		return;
	}

	
}

void cmd_hi(int argc,const char** argv)
{
	if(argc != 1)
	{
		cout << "hi takes no arguments" << endl;
		return;
	}

	bcstHello();
}

void initCmd()
{
	cmd_map["help"] = cmd_help;
	cmd_map["name"] = cmd_name;
    cmd_map["hi"] = cmd_hi;
}

void exeCmd(char* str)
{
	int argc = 0;
	char* argv[8];

	char* k = strtok(str, " ");
	while(k != 0 && argc < 8)
	{
		argv[argc] = k;
		argc++;
		k = strtok(0, " ");
	}

    cmd_map_itr i;
    if(k)
	    i = cmd_map.find(argv[0]);
    else
        i = cmd_map.find(str);
	if(i == cmd_map.end())
	{
		cout << "Invalid command." << endl;
		return;
	}

	i->second(argc, (const char**)argv);
}
