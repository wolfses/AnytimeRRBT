// This file is part of V-REP, the Virtual Robot Experimentation Platform.
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// V-REP is dual-licensed, under the terms of EITHER (at your option):
//   1. V-REP commercial license (contact us for details)
//   2. GNU GPL (see below)
// 
// GNU GPL license:
// -------------------------------------------------------------------
// V-REP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// V-REP IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with V-REP.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.1.2 on June 16th 2014

#include "vrepPrecompiledHeader.h"
#include "VDateTime.h"
#include <QDateTime>
#include <QElapsedTimer>

#ifndef WIN_VREP
#include <sys/time.h>
#endif

unsigned int VDateTime::getOSTimeInMs(int timeType)
{
	if (timeType==0)
	{
#ifdef WIN_VREP
	return(timeGetTime());
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return(now.tv_sec*1000+now.tv_usec/1000);
#endif
	}
	if (timeType==1)
	{ // todo
//		QElapsedTimer::SystemTime
	}
	return(0);
}

int VDateTime::getTimeInMs()
{
    static bool first=true;
    static QTime firstTime;
    if (first)
    {
		firstTime.start();
		first=false;
    }
    return(firstTime.elapsed());
}

int VDateTime::getTimeDiffInMs(int lastTime)
{
	return(getTimeDiffInMs(lastTime,getTimeInMs()));
}

int VDateTime::getTimeDiffInMs(int oldTime,int newTime)
{
	if (newTime<oldTime)
		return(newTime+86400000-oldTime);
	return(newTime-oldTime);
}

quint64 VDateTime::getSecondsSince1970()
{
    QDateTime now=QDateTime::currentDateTime();
    return(now.toTime_t());
}

void VDateTime::getYearMonthDayHourMinuteSecond(int* year,int* month,int* day,int* hour,int* minute,int* second)
{
    QDate now=QDate::currentDate();
    QTime nowTime=QTime::currentTime();
    if (year!=NULL)
	    year[0]=now.year();
    if (month!=NULL)
	    month[0]=now.month();
    if (day!=NULL)
	    day[0]=now.day();
    if (hour!=NULL)
	    hour[0]=nowTime.hour();
    if (minute!=NULL)
	    minute[0]=nowTime.minute();
    if (second!=NULL)
	    second[0]=nowTime.second();
}
