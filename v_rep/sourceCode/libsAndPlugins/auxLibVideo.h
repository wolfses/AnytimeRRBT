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

#pragma once

#include <QLibrary>
#define LIBRARY QLibrary*
#define FARPROC_BLA void*

typedef char (__cdecl *pVideo_recorderGetEncoderString)(int index,char* encoderName_200Chars);
typedef char (__cdecl *pVideo_recorderInitialize)(int resX,int resY,const char* fileAndPath,int frameRate,int encoderIndex);
typedef char (__cdecl *pVideo_recorderAddFrame)(const unsigned char* buffer);
typedef char (__cdecl *pVideo_recorderEnd)(void);

// FULLY STATIC CLASS
class CAuxLibVideo  
{
public:
	static bool loadLibrary(const char* pathAndFilename);
	static void unloadLibrary();
	static bool getAuxLibProcAddresses();
	static FARPROC_BLA _getProcAddress(const char* funcName);

	static LIBRARY lib;

	static pVideo_recorderGetEncoderString video_recorderGetEncoderString;
	static pVideo_recorderInitialize video_recorderInitialize;
	static pVideo_recorderAddFrame video_recorderAddFrame;
	static pVideo_recorderEnd video_recorderEnd;
};
