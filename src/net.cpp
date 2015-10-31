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


#include "net.h"
#include <cstring>
#include <iostream>

#ifdef SMOKESIGNAL_PLATFORM_UNIX
	#include <unistd.h>
#endif

using namespace net;


unsigned long packet::IP_BROADCAST = INADDR_BROADCAST;


packet::packet():
    src(0), dest(0), data(0), size(0)
{
}

packet::packet(unsigned int _size,const char* _data,const char* _dest):
	src(0), size(_size)
{
	if(_dest)
		dest = inet_addr(_dest);
	else
		dest = IP_BROADCAST;

	data = new char[size];
    if(_data)
	    memcpy(data, _data, size);
}

packet::packet(unsigned int _size,const char* _data,unsigned long _dest):
	src(0), size(_size)
{
	if(_dest)
		dest = _dest;
	else
		dest = IP_BROADCAST;

	data = new char[size];
    if(_data)
	    memcpy(data, _data, size);
}

packet::packet(std::string msg,const char* _dest):
	src(0)
{
	if(_dest)
		dest = inet_addr(_dest);
	else
		dest = IP_BROADCAST;

	if(!msg.empty())
    {
        data = new char[msg.size() + 1];
        strcpy(data, msg.c_str());
		size = sizeof(data);
    }else{
		msg.clear();
        size = 0;
    }
}

packet::packet(std::string msg,unsigned long _dest):
	src(0)
{
	if(_dest)
		dest = _dest;
	else
		dest = IP_BROADCAST;

	if(!msg.empty())
    {
        data = new char[msg.size() + 1];
		strcpy(data, msg.c_str());
		size = sizeof(data);
    }else{
		msg.clear();
        size = 0;
    }
}

packet::~packet()
{
	if(data)
		delete[]data;
}

/*
 * Return size of data inside packet
 */
unsigned int packet::getDataSize()
{
	return size;
}

/*
 * Return constant pointer to packet data
 */
const char* packet::getData()
{
	return data;
}

/*
 * Return source address from where packet originated
 */
unsigned long packet::getSrcAddr()
{
	return src;
}

/*
 * Return destination address where packet is headed to
 */
unsigned long packet::getDestAddr()
{
	return dest;
}



/*
 * Creates a socket and opens a port
 * Arguments: portNumber - port number to bind socket to
 * Return: true if the socket at portNumber was successfully opened
 */
bool socket::open(int _pn)
{
	// open socket

	// windows' socket setup
#ifdef SMOKESIGNAL_PLATFORM_WINDOWS

	WORD wVersionRequested = MAKEWORD(1,1); // version 1.1
	WSADATA wsaData; // windows data
	WSAStartup(wVersionRequested, &wsaData);

#endif

    broadcasting = 0;

	addr.sin_family = AF_INET; // always AF_INET
    addr.sin_port = htons(_pn); // convert from host to net byte order
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// use Address Family Internet and datagrams
    if((id = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return false;

	unsigned long arg = 0; // 0 for blocking io
#ifdef SMOKESIGNAL_PLATFORM_WINDOWS
	ioctlsocket(id, FIONBIO, &arg);
#else
	ioctl(id, FIONBIO, &arg);
#endif	

    if( bind(id, (sockaddr*)&addr, sizeof(addr)) < 0)
        return false;

	return true;
}

/*
 * Send data to a single destination
 * Arguments: packet - packet to send
 * Return: true if successful, or if packet destination address is 0,
 *			broadcast it
 */
bool socket::sendPacket(const packet& pkt)
{
    if(pkt.dest != packet::IP_BROADCAST)
    {
        if(broadcasting)
	        setsockopt(id, SOL_SOCKET, SO_BROADCAST, 
			(char*)&(broadcasting = 0), sizeof(int));

    }else if(!broadcasting)
        setsockopt(id, SOL_SOCKET, SO_BROADCAST, (char*)&(broadcasting = 1), 
		sizeof(int));

    addr.sin_addr.s_addr = pkt.dest;

	if(sendto(id, pkt.data, pkt.size, 0, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
#ifdef SMOKESIGNAL_PLATFORM_WINDOWS
		std::clog << "ERROR: " << WSAGetLastError() << std::endl;
#else
		std::clog << "ERROR: " << std::endl;
#endif
		return false;
	}
	return true;
}

/*
 * Check if a packet has been received
 * Return: true if socket has received a packet or packets
 */
bool socket::hasPacket()
{
	unsigned long arg = 0;
#ifdef SMOKESIGNAL_PLATFORM_WINDOWS
	ioctlsocket(id, FIONREAD, &arg);
#else
	ioctl(id, FIONREAD, &arg);
#endif
	return arg;
}

/*
 * Retrieves data in buffer in order of received
 * Arguments: packet - packet to fill in with data
 * Return: length of data; Does not return until data is received
 */
bool socket::getPacket(packet& pkt)
{
    sockaddr_in sender;
    socklen_t len = sizeof(sender);
    int re = recvfrom(id, pkt.data, pkt.size, 0, (sockaddr*)&sender, &len);
	if(re <= 0)
	{
		//std::clog << "ERROR: " << WSAGetLastError() << std::endl;
		return false;
	}
    pkt.src = sender.sin_addr.s_addr;
    return re > 0;
}

/*
 * Returns the network IP address of the host this socket is open on
 */
unsigned long socket::getIP()
{
    sockaddr_in sender;
    socklen_t len = sizeof(sender);
	getpeername(id, (sockaddr*)&sender, &len); 
	return sender.sin_addr.s_addr;
}

/*
 * Closes the socket
 */
void socket::close()
{
	// close socket
#ifdef SMOKESIGNAL_PLATFORM_WINDOWS
	closesocket(id);
	WSACleanup();
#else
	::shutdown(id, 2);
#endif
}
