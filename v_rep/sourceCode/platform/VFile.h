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

#include <string>

#include <exception>
#define VFILE_EXCEPTION_TYPE std::exception&
#include <QFile>
#include <QDir>

class VFile  
{
public:
	VFile(const std::string& filename,WORD flags,bool dontThrow=false);
	VFile(const std::string& filename); // opens a Qt resource files
	virtual ~VFile();

	static void reportAndHandleFileExceptionError(VFILE_EXCEPTION_TYPE e);
	static bool createFolder(const std::string& pathAndName)
	{
		QDir qdir("");
		return(qdir.mkdir(QString::fromLocal8Bit(pathAndName.c_str())));
	}

	inline quint64 getLength()
	{
		return(_theFile->size());
	}

	inline void setLength(quint64 l)
	{
		_theFile->resize(l);
	}

	inline void close()
	{
		_theFile->close();
	}

	inline QFile* getFile()
	{
		return(_theFile);
	}

private:
	static bool _doesFileOrFolderExist(const std::string& filenameOrFoldernameAndPath,bool checkForFolder);

	QFile* _theFile;

public:
	static bool doesFileExist(const std::string& filenameAndPath);
	static bool doesFolderExist(const std::string& foldernameAndPath); // no final slash!
	static void eraseFile(const std::string& filenameAndPath);

	static WORD CREATE;
	static WORD WRITE;
	static WORD SHARE_EXCLUSIVE;
	static WORD READ;
	static WORD SHARE_DENY_NONE;
};
