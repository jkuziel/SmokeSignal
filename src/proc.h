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


#ifndef __PROC_H__
#define __PROC_H__


// CONSTANTS //////////////////////////////////////////////////////////////////

#define BCST_MSG_SIZE	36	// 4 + 32 bytes
#define BACK_MSG_SIZE	36	// 4 + 32 bytes
#define MESG_MSG_SIZE	182	// 4 + 32 + 5 + 1 + 140 bytes
#define MACK_MSG_SIZE	41	// 4 + 32 + 5

// message types
#define BCST_TYPE_STR	"BCST"
#define BACK_TYPE_STR	"BACK"
#define MESG_TYPE_STR	"MESG"
#define MACK_TYPE_STR	"MACK"


#define USER_STRLEN	32
#define MSG_STRLEN	140


// STRUCTURES /////////////////////////////////////////////////////////////////

typedef struct
{
	char userName[USER_STRLEN + 1];

	unsigned long ipAddr; // user's ip address

	unsigned int seqNum; // sequence number

	char encrypt; // 1 = encrypt messages

	char msg[MESG_MSG_SIZE]; // last message sent

	unsigned int sendAttempts; // 0 is ok to send

	char encryptKey[MSG_STRLEN]; // encryption key

} user_t;

typedef void(*msgCallbackType)(const char*);


// FUNCTIONS //////////////////////////////////////////////////////////////////

/*
 * Initialize variables, setup sockets, and start listening threads
 * Return: true is successful, false if failed
 */
bool initProc();

/*
 * Close sockets and stop threads
 */
void exitProc();

/*
 * Broadcast a BCST type message
 */
void bcstHello();

/*
 * Drop user from the ip map
 * Arguments: username - the user's name string
 */
void dropUser(const char* username);

/*
 * Select user to send messages to
 * Arguments: username - user name to send to (based on ip map)
 */
void sendToUser(const char* username);

/*
 * Send message to currently selected user
 * Arguments: msg - message to be sent
 */
void sendMsg(const char* msg);

/*
 * Specifies a callback function when a new user is entered on the ip map
 * Arguments: callback function of prototype msgCallbackType
 */
void setNewUserCallback(msgCallbackType);

/*
 * Specifies a callback function when a user is removed from the ip map
 * Arguments: callback function of prototype msgCallbackType
 */
void setRemoveUserCallback(msgCallbackType);

/*
 * Return: user name string(33 chars max) of application
 */
const char* getUserName();

/*
 * Get the pointer to the user structure based on its user name field
 * Arguments: username - the user name to look up
 * Returns: user_t structure pointer, or 0 if user name was not found
 */
user_t* getUser(const char* username);

/*
 * Set if the sequence number should be increased every message send
 * Arguments: true if increment, false if do not increment
 */
void setIncSeqNum(bool);

/*
 * Sets the message ACK timeout value
 * Arguments: TOValue, in milliseconds
 */
void setTOVal(unsigned int);

/*
 * Get the message ACK timeout value
 * Return: TOValue
 */
unsigned int getTOVal();



#endif // __PROC_H__
