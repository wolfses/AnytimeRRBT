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

#ifndef __PORTING_H__
#define __PORTING_H__

#ifdef _WIN32
#ifndef QT_COMPIL
#include <afxwin.h>         // MFC core and standard components
#include <Mmsystem.h>
#pragma message("-----------------------> Adding library: Winmm.lib")
#pragma comment(lib,"Winmm.lib")
#else
#include <Windows.h>
#include <process.h>
typedef unsigned long       DWORD;
#endif
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)

#include <sys/time.h>
#include <string.h>

#endif /* __linux || __APPLE__ */

#include <string>

#ifdef _WIN32

#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)

#define Sleep(x) (usleep(x*1000))
#define _stricmp(x,y) strcasecmp(x,y)
typedef unsigned long       DWORD;

#endif /* __linux || __APPLE__ */

DWORD getTimeInMs();
DWORD getTimeDiffInMs(DWORD lastTime);

#endif /* __PORTING_H__ */
