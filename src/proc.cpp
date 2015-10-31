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


// INCLUDES ///////////////////////////////////////////////////////////////////

#include "proc.h"
#include "net.h"
#include "platform.h"

#include <iostream>
#include <fstream>
#include <map>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <mutex>

#ifdef SMOKESIGNAL_PLATFORM_UNIX
	#include <unistd.h>
#endif


// GLOBAL VARIABLES ///////////////////////////////////////////////////////////

char	userName[33];	// this client's user name / outgoing name field

net::socket bcstSock;	// broadcast socket
net::socket msgSock;	// message socket

bool bcstListening;	// broadcast listen is on/off
bool msgListening;	// message listen is on/off

msgCallbackType newUserCallback;
msgCallbackType removeUserCallback;


// thread stuff
std::thread	bcstThrd; // broadcast listener thread
std::thread	msgThrd;  // incoming message thread

// only allow single access to ipMap
std::mutex	userMutex;

// only allow single access to cout
std::mutex	msgMutex;

// association of ip addresses to users online
std::map<unsigned long, user_t> ipMap;

// current ip address to send messages to
std::map<unsigned long, user_t>::iterator sendToIP = ipMap.end();

// options
// increment sequence number
bool optIncSeqNum;
// ACK timeout value
unsigned int optTOVal;
// max send attempts before print failure
unsigned int optMaxSendAttempts;

/*
 * Broadcast thread. Listens on port 8888 for BCST and BACK type messages
 * Arguments: none
 * Returns: 0
 */
void listenForBcst()
{
	while(bcstListening)
	{
		net::packet pkt((unsigned int)BCST_MSG_SIZE, 0, (unsigned long)0);
		if(!bcstSock.getPacket(pkt))
			continue;

		const char* data = pkt.getData();
		if(!data)
			continue;

		unsigned long ipAddr = pkt.getSrcAddr();

		// is this broadcast from this host?
		if(strncmp(data + 4, userName, 32) == 0)
			continue;

		// if message type bcst, send back message
		if(strncmp(BCST_TYPE_STR, data, 4) == 0)
		{
			// wait to send acknowledgement
#ifdef SMOKESIGNAL_PLATFORM_WINDOWS
			Sleep(rand() % 1000);
#else
			usleep((rand() % 1000) * 1000);
#endif

			char msg[BACK_MSG_SIZE];
			memcpy(msg, BACK_TYPE_STR, 4);
			memcpy(msg+4, userName, 32);

			bcstSock.sendPacket(net::packet(BACK_MSG_SIZE, msg, ipAddr));
		}

		userMutex.lock();
		// is user already in our map?
		std::map<unsigned long,user_t>::iterator ip;
		if((ip = ipMap.find(ipAddr)) != ipMap.end())
		{
			// ip's match, but see if userName matches what we have on record
			if(strncmp(ip->second.userName, data + 4, 32) == 0)
			{
				userMutex.unlock();
				continue;
			}
			// user names do not match, so update record
			if(removeUserCallback) removeUserCallback(ip->second.userName);
			memcpy(ip->second.userName, data + 4, 32);
			if(newUserCallback) newUserCallback(ip->second.userName);

			msgMutex.lock();
			in_addr adr;
#ifdef SMOKESIGNAL_PLATFORM_WINDOWS
			adr.S_un.S_addr = ipAddr;
#else
			adr.s_addr = ipAddr;
#endif
			std::cout << ip->second.userName << " is online ("
				<< inet_ntoa(adr) << ")" << std::endl;
			msgMutex.unlock();

			userMutex.unlock();
			continue;
		}
		userMutex.unlock();

		// parse message and fill in user struct
		user_t newusr;
		memcpy(newusr.userName, data + 4, 32);
		newusr.userName[32] = 0;

		newusr.ipAddr = ipAddr;
		newusr.encrypt = '0';
		newusr.seqNum = 0;
		newusr.sendAttempts = 0;
		memset(newusr.encryptKey, 0, MSG_STRLEN);

		userMutex.lock();
		ipMap.insert(std::pair<unsigned long,user_t>(ipAddr, newusr));
		userMutex.unlock();

		msgMutex.lock();
		in_addr adr;
#ifdef SMOKESIGNAL_PLATFORM_WINDOWS
		adr.S_un.S_addr = ipAddr;
#else
		adr.s_addr = ipAddr;
#endif
		std::cout << newusr.userName << " is online ("
			<< inet_ntoa(adr) << ")" << std::endl;
		msgMutex.unlock();

		if(newUserCallback) newUserCallback(newusr.userName);
	}
}

/*
 * Message thread; Listen on port 9999 for type MESG and MACK messages
 * Arguments: none
 * Returns: 0
 */
void listenForMsg()
{
	while(msgListening)
	{
		net::packet pkt(MESG_MSG_SIZE, 0, (unsigned long)0);
		if(!msgSock.getPacket(pkt))
			continue;

		const char* data = pkt.getData();
		if(!data)
			continue;

		// find user
		userMutex.lock();
		std::map<unsigned long, user_t>::iterator ip =
			ipMap.find(pkt.getSrcAddr());

		if(ip == ipMap.end())
		{
			userMutex.unlock();
			continue;
		}

		// user mutex is locked from now on, NO CONTINUES, BREAKS, OR RETURNS!

		// message recieved
		if(strncmp(MESG_TYPE_STR, data, 4) == 0)
		{
			// extract message text
			char msg[MSG_STRLEN+1];
			memcpy(msg, data + 42, 140);
			msg[140] = 0;

			char seqNum[6];
			memcpy(seqNum, data + 36, 5);
			seqNum[5] = 0;

			int tempSeqNum = atoi(seqNum);

			// if sequence numbers do not match, that means new message!
			if(tempSeqNum != ip->second.seqNum)
			{
				// is message encrypted?
				if(data[41] == '1')
				{
					const char* key = ip->second.encryptKey;
					int keyLen = strlen(key);

					if(keyLen < 1)
					{
						msgMutex.lock();
						std::cout << "ERROR: encrypted message received," <<
							"but no decryption key for " << ip->second.userName
							<< std::endl;
						msgMutex.unlock();
					}else{
						// decrypt message
						for(int i = 0; i < MSG_STRLEN; i++)
							msg[i] = msg[i] ^ key[i % keyLen];

						msgMutex.lock();
						std::cout << ip->second.userName << " >> " << msg <<
							std::endl;
						msgMutex.unlock();
					}
				}else{
					msgMutex.lock();
					std::cout << ip->second.userName << " >> " << msg <<
						std::endl;
					msgMutex.unlock();
				}
				// update sequence number
				ip->second.seqNum = tempSeqNum;
			}

			// send ack
			char ackmsg[MACK_MSG_SIZE];
			memcpy(ackmsg, MACK_TYPE_STR, 4);
			memcpy(ackmsg+4, userName, 32);
			memcpy(ackmsg+36, data + 36, 5);
			msgSock.sendPacket(net::packet(MACK_MSG_SIZE, ackmsg,
				pkt.getSrcAddr()));

		// message acknowledged
		}else if(strncmp(MACK_TYPE_STR, data, 4) == 0)
		{
			char seqNum[6];
			memcpy(seqNum, data + 36, 5);
			seqNum[5] = 0;

			int tempSeqNum = atoi(seqNum);

			ip->second.sendAttempts = 0;
		}

		userMutex.unlock();
	}
}

/*
 * Message send thread; Resends messages three times over a period of
 *		TOValue*3 msecs, if no messages are received
 * Arguments: pointer to user_t structure, i.e. the user to send message to
 * Returns: 0
 */
void unmackedSend(user_t* usr)
{
	while(usr && usr->sendAttempts > 0
		&& usr->sendAttempts <= optMaxSendAttempts)
	{
		userMutex.lock();
		if(usr && sendToIP != ipMap.end())
		{
			msgSock.sendPacket(net::packet(MESG_MSG_SIZE,
				sendToIP->second.msg, sendToIP->second.ipAddr));
			usr->sendAttempts++;
		}
		userMutex.unlock();
#ifdef SMOKESIGNAL_PLATFORM_WINDOWS
		Sleep(optTOVal);
#else
		usleep(optTOVal * 1000);
#endif
	}

	if(usr && usr->sendAttempts > optMaxSendAttempts)
	{
		msgMutex.lock();
		std::cout << "ERROR: message to " << usr->userName << " failed." <<
			std::endl;
		msgMutex.unlock();
		usr->sendAttempts = 0;
	}
}

/*
 * Randomly generate a user name if a valid one cannot be found
 */
void genUserName()
{
	// generate random user name
	srand((unsigned int)time(0));
	for(int i = 0; i < USER_STRLEN; i++)
		userName[i] = 'a' + rand() % 26;
}

/*
 * Initialize variables, setup sockets, and start listening threads
 * Return: true is successful, false if failed
 */
bool initProc()
{
	// fill userName with 0s
	memset(userName, 0, 32);

	// set callbacks to null
	newUserCallback = 0;
	removeUserCallback = 0;

	// set options
	optIncSeqNum = true;
	optTOVal = 10;
	optMaxSendAttempts = 3;

	// get userName from config file
	std::ifstream file;
	file.open("config.cfg", std::ios::in);
	if(file.fail())
		genUserName();
	else{
		// user options
		char line[256];
		char* t = 0;
		while(file.getline(line, 256))
		{
			if(t = strtok(line, "="))
			{
				if(strcmp(t, "username") == 0)
				{
					if(!(t = strtok(0, "=")))
					{
						genUserName();
					}else{
						for(size_t i = 0; i < strlen(t); i++)
						{
							if(t[i] < 'a' || t[i] > 'z')
							{
								if(i == 0)
									genUserName();
								break;
							}else
								userName[i] = t[i];
						}
					}
				}
			}
		}
	}
	file.close();
	// end file

	if(!bcstSock.open(8888) || !msgSock.open(9999))
		return false;

	bcstListening = true;
	msgListening = true;

	bcstThrd = std::thread(listenForBcst);
	msgThrd = std::thread(listenForMsg);

	return true;
}

/*
 * Close sockets and stop threads
 */
void exitProc()
{
	bcstListening = false;
	msgListening = false;

	msgSock.close();
	bcstSock.close();

    msgThrd.join();
    bcstThrd.join();
}

/*
 * Broadcast a BCST type message
 */
void bcstHello()
{
	char msg[BCST_MSG_SIZE];
	memcpy(msg, BCST_TYPE_STR, 4);
	memcpy(msg+4, userName, 32);
	if(!bcstSock.sendPacket(net::packet(BCST_MSG_SIZE, msg, (unsigned long)0)))
		std::clog << "broadcast failed" << std::endl;
}

/*
 * Drop user from the ip map
 * Arguments: username - the user's name string
 */
void dropUser(const char* username)
{
	std::map<unsigned long,user_t>::iterator i;
	userMutex.lock();
	for(i = ipMap.begin(); i != ipMap.end(); i++)
		if(strcmp(i->second.userName, username) == 0)
			break;
	if(i != ipMap.end())
	{
		if(sendToIP == i)
			sendToIP = ipMap.end();
		ipMap.erase(i);
	}else
		std::clog << "invalid username" << std::endl;
	userMutex.unlock();
}

/*
 * Select user to send messages to
 * Arguments: username - user name to send to (based on ip map)
 */
void sendToUser(const char* username)
{
	userMutex.lock();
	for(std::map<unsigned long,user_t>::iterator i = ipMap.begin();
		i != ipMap.end(); i++)
	{
		if(strcmp(i->second.userName, username) == 0)
		{
			sendToIP = i;
			std::clog << i->second.userName << " selected" << std::endl;
			userMutex.unlock();
			return;
		}
	}
	userMutex.unlock();
	std::clog << "invalid username" << std::endl;
}

/*
 * Send message to currently selected user
 * Arguments: msg - message to be sent
 */
void sendMsg(const char* rawtxt)
{
	if(sendToIP == ipMap.end())
		return;

	if(sendToIP->second.sendAttempts != 0)
		return;

	// increment to keep seqNums unique
	if(optIncSeqNum)
		sendToIP->second.seqNum++;

	char seqNum[6];
	sprintf(seqNum, "%5d", sendToIP->second.seqNum);

	char* msg = sendToIP->second.msg;
	memcpy(msg, MESG_TYPE_STR, 4);
	memcpy(msg + 4, userName, 32);
	memcpy(msg + 36, seqNum, 5);
	memcpy(msg + 41, &sendToIP->second.encrypt, 1);
	const char* key = sendToIP->second.encryptKey;
	int keyLen = strlen(key);

	char txt[MSG_STRLEN];
	strncpy(txt, rawtxt, MSG_STRLEN);

	if(sendToIP->second.encrypt == '1')
	{
		if(keyLen < 1)
		{
			msgMutex.lock();
			std::cout << "ERROR: no encryption key for " <<
				sendToIP->second.userName << std::endl;
			msgMutex.unlock();
			return;
		}

		for(int i = 0; i < MSG_STRLEN; i++)
			msg[42 + i] = key[i % keyLen] ^ txt[i];
	}else
		strncpy(msg + 42, txt, 140);

	// don't want to print to the console twice
#ifndef _CONSOLE
	msgMutex.lock();
	std::cout << sendToIP->second.userName << " << " << txt << std::endl;
	msgMutex.unlock();
#endif

	sendToIP->second.sendAttempts = 1;

    std::thread(unmackedSend, &sendToIP->second).detach();
}

/*
 * Specifies a callback function when a new user is entered on the ip map
 * Arguments: callback function of prototype msgCallbackType
 */
void setNewUserCallback(msgCallbackType t)
{
	newUserCallback = t;
}

/*
 * Specifies a callback function when a user is removed from the ip map
 * Arguments: callback function of prototype msgCallbackType
 */
void setRemoveUserCallback(msgCallbackType t)
{
	removeUserCallback = t;
}

/*
 * Return: user name string(33 chars max) of application
 */
const char* getUserName()
{
	return userName;
}

/*
 * Get the pointer to the user structure based on its user name field
 * Arguments: username - the user name to look up
 * Returns: user_t structure pointer, or 0 if user name was not found
 */
user_t* getUser(const char* username)
{
	userMutex.lock();
	for(std::map<unsigned long,user_t>::iterator i = ipMap.begin();
		i != ipMap.end(); i++)
	{
		if(strcmp(i->second.userName, username) == 0)
		{
			userMutex.unlock();
			return &i->second;
		}
	}
	userMutex.unlock();
	return 0;
}

/*
 * Set if the sequence number should be increased every message send
 * Arguments: true if increment, false if do not increment
 */
void setIncSeqNum(bool t)
{
	optIncSeqNum = t;
}

/*
 * Sets the message ACK timeout value
 * Arguments: TOValue, in milliseconds
 */
void setTOVal(unsigned int i)
{
	optTOVal = i;
}

/*
 * Get the message ACK timeout value
 * Return: TOValue
 */
unsigned int getTOVal()
{
	return optTOVal;
}
