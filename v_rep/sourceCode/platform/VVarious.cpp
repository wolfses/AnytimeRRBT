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
#include "VVarious.h"
#include "auxLib.h"
#include "Tt.h"
#include "VFile.h"
#include "VArchive.h"
#include "ttUtil.h"
#include "VDateTime.h"

#ifdef WIN_VREP
	#include <Shellapi.h>
#endif


#include <QCoreApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QLibrary>

#ifdef LIN_VREP
    #include <stdio.h>
    #include <sys/ioctl.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <net/if.h>
#endif

bool VVarious::openUrl(const std::string& url)
{
	std::string f(url);
	f="file:///"+f;
	return(QDesktopServices::openUrl(QUrl(QString::fromLocal8Bit(f.c_str()),QUrl::TolerantMode)));
}

bool VVarious::openTextFile(const std::string& file)
{
	std::string f(file);
	f="file:///"+f;
	return(QDesktopServices::openUrl(QUrl(QString::fromLocal8Bit(f.c_str()),QUrl::TolerantMode)));
}

bool VVarious::executeExternalApplication(const std::string& file,const std::string& arguments,const std::string& switchToDirectory,int showFlag)
{
#ifdef WIN_VREP
	int sh=SW_SHOWDEFAULT;
	if (showFlag==VVARIOUS_SHOWMAXIMIZED)
		sh=SW_SHOWMAXIMIZED;
	if (showFlag==VVARIOUS_HIDE)
		sh=SW_HIDE;
	if (switchToDirectory.length()!=0)
		SetCurrentDirectoryA(switchToDirectory.c_str()); // needed because otherwise the shellExecute command might switch directories!
	return (int(ShellExecuteA(NULL,"open",file.c_str(),arguments.c_str(),NULL,sh))>32);
#else
	// Check here: http://stackoverflow.com/questions/859517/osx-equivalent-of-shellexecute
	QString cmd;
	if (file.find('/')==std::string::npos)
		cmd="./";
	cmd+=QString::fromLocal8Bit(file.c_str());
	QStringList strList;
	std::string args(arguments);
	std::string word;
	while (tt::extractWord(args,word))
		strList << QString(word.c_str());
	return(QProcess::startDetached(cmd,strList,QString::fromLocal8Bit(switchToDirectory.c_str()),NULL));
#endif
}


bool VVarious::copyTextToClipboard(const std::string& text)
{
	QClipboard *clipboard=QApplication::clipboard();
	clipboard->setText(text.c_str());
	return(true);
}

std::string VVarious::getModulePathAndFile()
{
	return(QCoreApplication::applicationFilePath().toLocal8Bit().data());
}

void VVarious::removePathFinalSlashOrBackslash(std::string& pathWithOrWithoutFinalThing)
{
	int l=pathWithOrWithoutFinalThing.length();
	if (l>0)
	{
		if ((pathWithOrWithoutFinalThing[l-1]=='/')||(pathWithOrWithoutFinalThing[l-1]=='\\'))
		{ // this might be a / or \\ part of a double char. Check for that:
			bool removeIt=true;
			if (l>1)
			{
				if ((unsigned char)(pathWithOrWithoutFinalThing[l-2])>0x7F)
					removeIt=false;
			}

			if (removeIt)
				pathWithOrWithoutFinalThing.erase(pathWithOrWithoutFinalThing.end()-1);
		}
	}
}

std::string VVarious::splitPath_path(const std::string& fullPathAndName)
{	// returns the absolute path, without a final / or backslash
	std::string retVal;
	QFileInfo pathInfo(QString::fromLocal8Bit(fullPathAndName.c_str()));
	retVal=pathInfo.path().toLocal8Bit().data();
	int l=retVal.length();
	if (l!=0)
	{
		if ((retVal[l-1]=='/')||(retVal[l-1]=='\\'))
		{ // this might be a / or \\ part of a double char. Check for that:
			bool removeIt=true;
			if (l>1)
			{
				if ((unsigned char)(retVal[l-2])>0x7F)
					removeIt=false;
			}

			if (removeIt)
				retVal.erase(retVal.end()-1);
		}
	}
	return(retVal);
}

std::string VVarious::splitPath_fileBaseAndExtension(const std::string& fullPathAndName)
{ // returns the filename including extension
	std::string retVal;
	QFileInfo pathInfo(QString::fromLocal8Bit(fullPathAndName.c_str()));
	retVal=pathInfo.fileName().toLocal8Bit().data();
	return(retVal);
}


std::string VVarious::splitPath_fileBase(const std::string& fullPathAndName)
{	// returns the base of a filename, without path or extension
	std::string retVal;
	QFileInfo pathInfo(QString::fromLocal8Bit(fullPathAndName.c_str()));
	retVal=pathInfo.completeBaseName().toLocal8Bit().data();
	return(retVal);
}

std::string VVarious::splitPath_fileExtension(const std::string& fullPathAndName)
{	// returns the filename extension (without '.')
	std::string retVal;
	QFileInfo pathInfo(QString::fromLocal8Bit(fullPathAndName.c_str()));
	retVal=pathInfo.suffix().toLocal8Bit().data();
	return(retVal);
}

bool VVarious::isAbsolutePath(const std::string& pathAndOptionalFilename)
{
	QFileInfo pathInfo(QString::fromLocal8Bit(pathAndOptionalFilename.c_str()));
	return(pathInfo.isAbsolute());
}

