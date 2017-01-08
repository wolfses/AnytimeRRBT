// This file is part of the EXTERNAL IK for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The EXTERNAL IK is licensed under the terms of EITHER:
//   1. EXTERNAL IK commercial license (contact us for details)
//   2. EXTERNAL IK educational license (see below)
// 
// EXTERNAL IK educational license:
// -------------------------------------------------------------------
// The EXTERNAL IK educational license applies only to EDUCATIONAL
// ENTITIES composed by following people and institutions:
// 
// 1. Hobbyists, students, teachers and professors
// 2. Schools and universities
// 
// EDUCATIONAL ENTITIES do NOT include companies, research institutions,
// non-profit organisations, foundations, etc.
// 
// An EDUCATIONAL ENTITY may use, modify, compile and distribute the
// modified/unmodified EXTERNAL IK under following conditions:
//  
// 1. Distribution should be free of charge.
// 2. Distribution should be to EDUCATIONAL ENTITIES only.
// 3. Usage should be non-commercial.
// 4. Altered source versions must be plainly marked as such and distributed
//    along with any compiled code.
// 5. The origin of the EXTERNAL IK must not be misrepresented. you must
//    not claim that you wrote the original software.
// 
// THE EXTERNAL IK IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include "extIkSer.h"



CExtIkSer::CExtIkSer(unsigned char* data,int dataLength)
{
	_reading=true;
	_readBuffer=data;
	_readBufferLength=dataLength;
	_readLocation=0;
}

CExtIkSer::~CExtIkSer()
{
}



float CExtIkSer::readFloat()
{
	unsigned char tmp[4];
	tmp[0]=_readBuffer[_readLocation++];
	tmp[1]=_readBuffer[_readLocation++];
	tmp[2]=_readBuffer[_readLocation++];
	tmp[3]=_readBuffer[_readLocation++];
	return(((float*)(&tmp))[0]);
}

int CExtIkSer::readInt()
{
	unsigned char tmp[4];
	tmp[0]=_readBuffer[_readLocation++];
	tmp[1]=_readBuffer[_readLocation++];
	tmp[2]=_readBuffer[_readLocation++];
	tmp[3]=_readBuffer[_readLocation++];
	return(((int*)(&tmp))[0]);
}

unsigned int CExtIkSer::readUInt()
{
	unsigned char tmp[4];
	tmp[0]=_readBuffer[_readLocation++];
	tmp[1]=_readBuffer[_readLocation++];
	tmp[2]=_readBuffer[_readLocation++];
	tmp[3]=_readBuffer[_readLocation++];
	return(((unsigned int*)(&tmp))[0]);
}

unsigned short CExtIkSer::readWord()
{
	unsigned char tmp[2];
	tmp[0]=_readBuffer[_readLocation++];
	tmp[1]=_readBuffer[_readLocation++];
	return(((unsigned short*)(&tmp))[0]);
}

unsigned char CExtIkSer::readByte()
{
	return(_readBuffer[_readLocation++]);
}

char CExtIkSer::readChar()
{
	return((char)_readBuffer[_readLocation++]);
}

std::string CExtIkSer::readString()
{
	std::string retVal;
	while (_readBuffer[_readLocation]!=0)
		retVal+=(char)_readBuffer[_readLocation++];
	_readLocation++; // terminal zero
	return(retVal);
}
