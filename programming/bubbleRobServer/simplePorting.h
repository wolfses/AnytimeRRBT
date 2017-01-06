// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#ifndef __SIMPLEPORTING_H__
#define __SIMPLEPORTING_H__

#ifdef _WIN32
	#include <winsock2.h>
	#pragma message("-----------------------> Adding library: Winmm.lib") 
	#pragma comment(lib,"Winmm.lib")
	#pragma message("-----------------------> Adding library: Ws2_32.lib")
	#pragma comment(lib,"Ws2_32.lib")
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

#endif /* __linux || __APPLE__ */

#include <string>

#ifdef _WIN32
typedef SOCKET              _SOCKET;
typedef timeval             _timeval;
typedef int                 _socklen;
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
#define INVALID_SOCKET        -1
#define Sleep(x) (usleep(x*1000))

typedef int                 _SOCKET;
typedef struct timeval      _timeval;
typedef socklen_t           _socklen;

typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned short      u_short;
#endif /* __linux || __APPLE__ */

DWORD getTimeInMs();
DWORD getTimeDiffInMs(DWORD lastTime);

#endif /* __SIMPLEPORTING_H__ */
