// This file is part of the REMOTE API
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The REMOTE API is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The REMOTE API is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE REMOTE API IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the REMOTE API.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#ifndef __PORTING_H__
#define __PORTING_H__

#ifdef _WIN32
	#include <Windows.h>
	#include <process.h>
	#ifndef QT_COMPIL
		#pragma message("-----------------------> Adding library: Winmm.lib")
		#pragma comment(lib,"Winmm.lib")
		#pragma message("Adding library: Ws2_32.lib")
		#pragma comment(lib,"Ws2_32.lib")
	#endif
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

#define LINUX_NAMED_PIPES_DIR "/tmp/"
#define INVALID_SOCKET        -1
#define Sleep(x) (usleep(x*1000))

typedef int                 _SOCKET;
typedef struct timeval      _timeval;
typedef socklen_t           _socklen;

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned short      u_short;

#endif /* __linux || __APPLE__ */

DWORD getTimeInMs();
DWORD getTimeDiffInMs(DWORD lastTime);

#endif /* __PORTING_H__ */
