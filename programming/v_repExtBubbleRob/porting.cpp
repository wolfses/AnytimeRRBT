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

#include "porting.h"

#ifdef _WIN32
DWORD getTimeInMs(void)
{
	return(timeGetTime()&0x03ffffff);
}
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
DWORD getTimeInMs(void)
{
	struct timeval tv;
	DWORD result=0;
	if (gettimeofday(&tv,NULL)==0)
		result=(tv.tv_sec*1000+tv.tv_usec/1000)&0x03ffffff;
	return(result);
}
#endif /* __linux || __APPLE__ */

DWORD getTimeDiffInMs(DWORD lastTime)
{
	DWORD currentTime=getTimeInMs();
	if (currentTime<lastTime)
		return(currentTime+0x03ffffff-lastTime);
	return(currentTime-lastTime);
}
