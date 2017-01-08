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
#include "VFile.h"
#include "App.h"
#include "v_repStrings.h"

WORD VFile::CREATE				=1;
WORD VFile::WRITE				=2;
WORD VFile::SHARE_EXCLUSIVE		=4;
WORD VFile::READ				=8;
WORD VFile::SHARE_DENY_NONE		=16;

VFile::VFile(const std::string& filename,WORD flags,bool dontThrow)
{
	_theFile=new QFile(QString::fromLocal8Bit(filename.c_str()));
	QFlags<QIODevice::OpenModeFlag> openFlags=0;
	if (flags&CREATE)
		openFlags|=QIODevice::Truncate;
	if (flags&WRITE)
		openFlags|=QIODevice::WriteOnly;
	if (flags&READ)
		openFlags|=QIODevice::ReadOnly;

	// Create the path directories if needed (added on 13/6/2012 because of a bug report of Matthias Füller):
	if (flags&(CREATE+WRITE))
	{
		QFileInfo pathInfo(QString::fromLocal8Bit(filename.c_str()));
		QDir dir("");
		dir.mkpath(pathInfo.path());
	}

	std::exception dummyException;
	if (!_theFile->open(openFlags))
	{
		delete _theFile;
		_theFile=NULL;
		if (!dontThrow)
			throw dummyException;
	}
}

VFile::VFile(const std::string& filename)
{ // opens a Qt resource file
	_theFile=new QFile(QString::fromLocal8Bit(filename.c_str()));
	std::exception dummyException;
	if (!_theFile->exists())
	{
	   throw dummyException;
	}
}

VFile::~VFile()
{
	delete _theFile;
}

void VFile::reportAndHandleFileExceptionError(VFILE_EXCEPTION_TYPE e)
{
	// stl file exceptions:
	App::uiThread->messageBox_critical(NULL,strTranslate(IDSN_FILE_EXCEPTION_ERROR),e.what(),VMessageBox::OKELI);
	// And #include <exception>
}

void VFile::eraseFile(const std::string& filenameAndPath)
{
	try
	{
		QFile::remove(filenameAndPath.c_str());
	}
	catch(VFILE_EXCEPTION_TYPE e)
	{
		VFile::reportAndHandleFileExceptionError(e);
	}
}


bool VFile::doesFileExist(const std::string& filenameAndPath)
{
	return(_doesFileOrFolderExist(filenameAndPath,false));
}

bool VFile::doesFolderExist(const std::string& foldernameAndPath)
{
	return(_doesFileOrFolderExist(foldernameAndPath,true));
}

bool VFile::_doesFileOrFolderExist(const std::string& filenameOrFoldernameAndPath,bool checkForFolder)
{
	QString dat(QString::fromLocal8Bit(filenameOrFoldernameAndPath.c_str()));
	if (checkForFolder)
	{
		QDir dir(dat);
		return(dir.exists());		
	}
	else
	{
		QFile file(dat);
		return(file.exists());		
	}
}

