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


#ifndef __NET_H__
#define __NET_H__


#include "platform.h"


#ifdef SMOKESIGNAL_PLATFORM_WINDOWS
	#include <WS2tcpip.h>
	#include <windows.h>
#else
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <sys/ioctl.h>
#endif

#include <string>


namespace net
{

class socket;

/*
 * Encapsulates a packet. Technically the real packet is assembled at a lower
 * layer, but this name seems as befitting as it is conceptual
 */
class packet
{
	friend class socket;
	protected:
		static unsigned long IP_BROADCAST;
		unsigned long src;
		unsigned long dest;
        unsigned short port;
		unsigned int size;
		char* data;

        packet();

	public:
		/*
		 * If packet destination address is 0, it will be broadcasted
		 */
		packet(unsigned int _size,const char* _data = 0,const char* _dest = 0);
		packet(unsigned int _size,const char* _data = 0,
			unsigned long _dest = 0);
		packet(std::string msg,const char* _dest = 0);
		packet(std::string msg,unsigned long _dest = 0);
		virtual ~packet();

		/*
		 * Return size of data inside packet
		 */
		unsigned int getDataSize();

		/*
		 * Return constant pointer to packet data
		 */
		const char* getData();

		/*
		 * Return source address from where packet originated
		 */
		unsigned long getSrcAddr();

		/*
		 * Return destination address where packet is headed to
		 */
		unsigned long getDestAddr();
};


/*
 * Encapsulates a single socket with a bound port number using UDP
 */
class socket
{
	protected:
		int id; // this socket's ID
		sockaddr_in addr; // source internet address
        int broadcasting;

	public:

		/*
		 * Creates a socket and opens a port
		 * Arguments: portNumber - port number to bind socket to
		 * Return: true if the socket at portNumber was successfully opened
		 */
		bool open(int portNumber);

		/*
		 * Send data to a single destination
		 * Arguments: packet - packet to send
		 * Return: true if successful, or if packet destination address is 0,
		 *			broadcast it
		 */
		bool sendPacket(const packet&);

		/*
		 * Check if a packet has been received
		 * Return: true if socket has received a packet or packets
		 */
		bool hasPacket();

		/*
		 * Retrieves data in buffer in order of received
		 * Arguments: packet - packet to fill in with data
		 * Return: length of data; Does not return until data is received
		 */
		bool getPacket(packet&);

		/*
		 * Returns the network IP address of the host this socket is open on
		 */
		unsigned long getIP();

		/*
		 * Closes the socket
		 */
		void close();
};

}

#endif
