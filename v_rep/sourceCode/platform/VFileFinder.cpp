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
#include "VFileFinder.h"
#include <QDateTime>

VFileFinder::VFileFinder()
{
}

VFileFinder::~VFileFinder()
{
}

int VFileFinder::searchFilesWithExtension(const std::string& pathWithoutTerminalSlash,const std::string& extension)
{
	return(_searchFilesOrFolders(pathWithoutTerminalSlash,extension,0));
}

int VFileFinder::searchFolders(const std::string& pathWithoutTerminalSlash)
{
	return(_searchFilesOrFolders(pathWithoutTerminalSlash,"",1));
}

int VFileFinder::searchFilesOrFolders(const std::string& pathWithoutTerminalSlash)
{
	return(_searchFilesOrFolders(pathWithoutTerminalSlash,"",2));
}


int VFileFinder::_searchFilesOrFolders(const std::string& pathWithoutTerminalSlash,const std::string& extension,int mode)
{ // mode=0 --> file, mode=1 --> folder, mode=2 --> file and folder
	_searchResult.clear();
	QDir dir(QString::fromLocal8Bit(pathWithoutTerminalSlash.c_str()));

	if (mode==0)
	{ // file
		dir.setFilter(QDir::Files|QDir::Hidden);//|QDir::NoSymLinks); 11/4/2013
		dir.setSorting(QDir::Name|QDir::IgnoreCase);
		if (extension.length()!=0)
		{
			QStringList filters;
			std::string tmp("*.");
			tmp+=extension;
			filters << tmp.c_str();
			dir.setNameFilters(filters);
		}
	}
	if (mode==1)
	{ // folder
		dir.setFilter(QDir::Dirs|QDir::Hidden|QDir::NoSymLinks);
		dir.setSorting(QDir::Name|QDir::IgnoreCase);
	}
	if (mode==2)
	{ // folder + files
		dir.setFilter(QDir::Files|QDir::Dirs|QDir::Hidden); //|QDir::NoSymLinks); // 11/4/2013
		dir.setSorting(QDir::Name|QDir::IgnoreCase);
	}
    QFileInfoList list=dir.entryInfoList();
    for (int i=0;i<list.size();++i)
	{
        QFileInfo fileInfo=list.at(i);
		SFileOrFolder f;
		f.isFile=!fileInfo.isDir();
		f.name=fileInfo.fileName().toLocal8Bit().data();
		f.path=fileInfo.filePath().toLocal8Bit().data();
		QDateTime lastWriteTime(fileInfo.lastModified());
		f.lastWriteTime=lastWriteTime.toTime_t();
		_searchResult.push_back(f);
    }
	return(int(_searchResult.size()));
}

SFileOrFolder* VFileFinder::getFoundItem(int index)
{
	if ( (index<0)||(index>=int(_searchResult.size())) )
		return(NULL);
	return(&_searchResult[index]);
}

