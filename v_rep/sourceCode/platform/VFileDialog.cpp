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
#include "VFileDialog.h"
#include "App.h"
#include <QFileDialog>

std::string VFileDialog::getOpenFileName_(QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10)
{
	return(_getOpenOrSaveFileName(true,parent,option,title,startPath,initFilename,allowAnyFile,extensionName,extension1,extension2,extension3,extension4,extension5,extension6,extension7,extension8,extension9,extension10));
}

std::string VFileDialog::getSaveFileName(QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10)
{
	return(_getOpenOrSaveFileName(false,parent,option,title,startPath,initFilename,allowAnyFile,extensionName,extension1,extension2,extension3,extension4,extension5,extension6,extension7,extension8,extension9,extension10));
}

std::string VFileDialog::_getOpenOrSaveFileName(bool open,QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10)
{
	FUNCTION_DEBUG;
	QString retString;
	QString pathAndOptionalFilename(QString::fromLocal8Bit(startPath.c_str()));
	if (initFilename.length()!=0)
		pathAndOptionalFilename=QString::fromLocal8Bit(initFilename.c_str());
	std::string filter=extensionName+" (*.";
	filter+=extension1;
	if (extension2.length()!=0)
		filter+=" *."+extension2;
	if (extension3.length()!=0)
		filter+=" *."+extension3;
	if (extension4.length()!=0)
		filter+=" *."+extension4;
	if (extension5.length()!=0)
		filter+=" *."+extension5;
	if (extension6.length()!=0)
		filter+=" *."+extension6;
	if (extension7.length()!=0)
		filter+=" *."+extension7;
	if (extension8.length()!=0)
		filter+=" *."+extension8;
	if (extension9.length()!=0)
		filter+=" *."+extension9;
	if (extension10.length()!=0)
		filter+=" *."+extension10;
	filter+=")";
	if (allowAnyFile)
		filter+=";;all (*.*)";

	bool native=true;
#ifndef WIN_VREP // native dialogs have a bug on MacOS/Linux versions: the initial directory is not set. Because of that, we don't use native dialogs
	native=false;
#endif
	if (App::userSettings->fileDialogs!=-1)
	{
		if (App::userSettings->fileDialogs==0)
			native=true;
		if (App::userSettings->fileDialogs==1)
			native=false;
	}

	if (App::userSettings->applicationTweak3==1)
		CSimAndUiThreadSync::setProcessAllMessages_tweak3(true);

#ifndef WIN_VREP
	// On Linux (and sometimes on Mac too), we have several problems if the message queue is not fully processed,
	// but only partially processed (e.g. App::qtApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers)).
	// A quick/dirty fix is tweak1 below. The full fix is to process all messages while
	// waiting for a lock, which is done now by default.
	QFileDialog dlg(parent,title.c_str(),pathAndOptionalFilename,filter.c_str());
	dlg.setNameFilter(filter.c_str());
	if (open)
	{
		dlg.setFileMode(QFileDialog::ExistingFile);
		dlg.setAcceptMode(QFileDialog::AcceptOpen);
	}
	else
	{
		dlg.setFileMode(QFileDialog::AnyFile);
		dlg.setAcceptMode(QFileDialog::AcceptSave);
	}
	if (!native)
		dlg.setOptions(QFileDialog::DontUseNativeDialog);

	if (App::userSettings->applicationTweak1==1)
		dlg.show();
	if (dlg.exec())
	{
		QStringList qfiles=dlg.selectedFiles();
		if (qfiles.size()==1)
			retString=qfiles.at(0);
	}
#else
	if (!native)
	{
		if (open)
			retString=QFileDialog::getOpenFileName(parent,title.c_str(),pathAndOptionalFilename,filter.c_str(),0,QFileDialog::DontUseNativeDialog);
		else
			retString=QFileDialog::getSaveFileName(parent,title.c_str(),pathAndOptionalFilename,filter.c_str(),0,QFileDialog::DontUseNativeDialog);
	}
	else
	{
		if (open)
			retString=QFileDialog::getOpenFileName(parent,title.c_str(),pathAndOptionalFilename,filter.c_str());
		else
			retString=QFileDialog::getSaveFileName(parent,title.c_str(),pathAndOptionalFilename,filter.c_str());
	}
#endif

	if (App::userSettings->applicationTweak3==1)
		CSimAndUiThreadSync::setProcessAllMessages_tweak3(false);

	return(retString.toLocal8Bit().data());
}


void VFileDialog::getOpenFileNames(std::vector<std::string>& files,QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10)
{
	FUNCTION_DEBUG;
	files.clear();
	QString pathAndOptionalFilename(QString::fromLocal8Bit(startPath.c_str()));
	if (initFilename.length()!=0)
		pathAndOptionalFilename=QString::fromLocal8Bit(initFilename.c_str());
	std::string filter=extensionName+" (*.";
	filter+=extension1;
	if (extension2.length()!=0)
		filter+=" *."+extension2;
	if (extension3.length()!=0)
		filter+=" *."+extension3;
	if (extension4.length()!=0)
		filter+=" *."+extension4;
	if (extension5.length()!=0)
		filter+=" *."+extension5;
	if (extension6.length()!=0)
		filter+=" *."+extension6;
	if (extension7.length()!=0)
		filter+=" *."+extension7;
	if (extension8.length()!=0)
		filter+=" *."+extension8;
	if (extension9.length()!=0)
		filter+=" *."+extension9;
	if (extension10.length()!=0)
		filter+=" *."+extension10;
	filter+=")";
	if (allowAnyFile)
		filter+=";;all (*.*)";

	bool native=true;
#ifndef WIN_VREP // native dialogs have a bug on MacOS/Linux versions: the initial directory is not set. Because of that, we don't use native dialogs
	native=false;
#endif
	if (App::userSettings->fileDialogs!=-1)
	{
		if (App::userSettings->fileDialogs==0)
			native=true;
		if (App::userSettings->fileDialogs==1)
			native=false;
	}

	QStringList qfiles;

	if (App::userSettings->applicationTweak3==1)
		CSimAndUiThreadSync::setProcessAllMessages_tweak3(true);

#ifndef WIN_VREP
	// On Linux (and sometimes on Mac too), the static functions sometimes just display a blank dialog. For that reason,
	// we use the non-static version on those platforms! The "show()/hide()" below might be important, otherwise we have the same
	// problem as with the static version of the functions!
	QFileDialog dlg(parent,title.c_str(),pathAndOptionalFilename,filter.c_str());
	dlg.setNameFilter(filter.c_str());
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	if (!native)
		dlg.setOptions(QFileDialog::DontUseNativeDialog);
	if (App::userSettings->applicationTweak1==1)
		dlg.show();
	if (dlg.exec())
		qfiles=dlg.selectedFiles();
#else
	if (!native)
		qfiles=QFileDialog::getOpenFileNames(parent,title.c_str(),pathAndOptionalFilename,filter.c_str(),0,QFileDialog::DontUseNativeDialog);
	else
		qfiles=QFileDialog::getOpenFileNames(parent,title.c_str(),pathAndOptionalFilename,filter.c_str());
#endif

	if (App::userSettings->applicationTweak3==1)
		CSimAndUiThreadSync::setProcessAllMessages_tweak3(false);


	for (int i=0;i<int(qfiles.size());i++)
		files.push_back(qfiles.at(i).toLocal8Bit().data());
}
