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
#include "auxLibVideo.h"

LIBRARY CAuxLibVideo::lib=NULL;

pVideo_recorderGetEncoderString CAuxLibVideo::video_recorderGetEncoderString=0;
pVideo_recorderInitialize CAuxLibVideo::video_recorderInitialize=0;
pVideo_recorderAddFrame CAuxLibVideo::video_recorderAddFrame=0;
pVideo_recorderEnd CAuxLibVideo::video_recorderEnd=0;

bool CAuxLibVideo::loadLibrary(const char* pathAndFilename)
{
	lib=new QLibrary(pathAndFilename);
	if (!lib->load())
	{
		delete lib;
		lib=NULL;
	}
	return (lib!=NULL);
}

void CAuxLibVideo::unloadLibrary()
{
	if (lib!=0)
	{
		lib->unload();
		delete lib;
	}
	lib=NULL;
}

FARPROC_BLA CAuxLibVideo::_getProcAddress(const char* funcName)
{
	return (void*)lib->resolve(funcName);
}

bool CAuxLibVideo::getAuxLibProcAddresses()
{
	video_recorderGetEncoderString=(pVideo_recorderGetEncoderString)(_getProcAddress("getAvailableEncoderName"));
	video_recorderInitialize=(pVideo_recorderInitialize)(_getProcAddress("recorderInitialize"));
	video_recorderAddFrame=(pVideo_recorderAddFrame)(_getProcAddress("recorderAddFrame"));
	video_recorderEnd=(pVideo_recorderEnd)(_getProcAddress("recorderEnd"));

	if (video_recorderGetEncoderString==0) return false;
	if (video_recorderInitialize==0) return false;
	if (video_recorderAddFrame==0) return false;
	if (video_recorderEnd==0) return false;
	return true;
}
