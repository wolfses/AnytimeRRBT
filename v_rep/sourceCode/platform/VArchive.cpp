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
#include "VArchive.h"

WORD VArchive::LOAD=0;
WORD VArchive::STORE=1;

VArchive::VArchive(VFile* file,WORD flag)
{
	_theFile=file;
	_loading=((flag&1)==0);
	_theArchive=new QDataStream(file->getFile());
	// Following 2 important to be compatible with the files written with first V-REP versions:
	_theArchive->setFloatingPointPrecision(QDataStream::SinglePrecision);
	_theArchive->setByteOrder(QDataStream::LittleEndian);
}

VArchive::~VArchive()
{
	delete _theArchive;
}

void VArchive::writeString(const std::string& str)
{
	for (int i=0;i<int(str.length());i++)
		(*this) << str[i];
}

void VArchive::writeLine(const std::string& line)
{
	writeString(line);
	(*this) << char(13);
	(*this) << char(10);
}

bool VArchive::readLine(DWORD& actualPosition,std::string& line,bool keepTabs)
{
	DWORD archiveLength=DWORD(_theFile->getLength());
	BYTE oneByte;
	bool returnCarrier=false;
	line="";
	while (actualPosition<archiveLength)
	{
		(*this) >> oneByte;
		actualPosition++;
		if (oneByte==(BYTE)13)
			returnCarrier=true;
		if (oneByte==(BYTE)10)
			return(true);
		if (oneByte!=(BYTE)13)
		{
			if ( keepTabs||(oneByte!=(BYTE)9) )
				line.insert(line.end(),(char)oneByte);
		}
	}
	return(line.length()!=0); 
}

