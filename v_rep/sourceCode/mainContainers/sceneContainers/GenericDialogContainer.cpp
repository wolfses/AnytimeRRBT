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
#include "GenericDialogContainer.h" 

CGenericDialogContainer::CGenericDialogContainer()
{
}


CGenericDialogContainer::~CGenericDialogContainer()
{
	removeAll(false);
}

void CGenericDialogContainer::newSceneProcedure()
{ // don't do anything here!!
//	removeAll(true);
}

void CGenericDialogContainer::simulationAboutToStart()
{
	removeAll(true);
}

void CGenericDialogContainer::simulationEnded()
{
	removeAll(true);
}

void CGenericDialogContainer::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void CGenericDialogContainer::removeAll(bool onlyDialogsCreatedInMainOrChilds)
{
	if (!onlyDialogsCreatedInMainOrChilds)
	{
		for (int i=0;i<int(_allDialogs.size());i++)
			delete _allDialogs[i];
		_allDialogs.clear();
	}
	else
	{
		for (int i=0;i<int(_allDialogs.size());i++)
		{
			if (_allDialogs[i]->getCreatedInMainOrChildScript())
			{
				removeDialogFromID(_allDialogs[i]->getGenericDialogID());
				i=-1; // ordering might have changed
			}
		}
	}
}

int CGenericDialogContainer::addDialog(CGenericDialog* theDialog)
{
	int theID=0;
	while (getDialogFromID(theID)!=NULL)
		theID++;
	theDialog->setGenericDialogID(theID);
	_allDialogs.push_back(theDialog);
	return(theID);
}

CGenericDialog* CGenericDialogContainer::getDialogFromID(int theID)
{
	for (int i=0;i<int(_allDialogs.size());i++)
	{
		if (_allDialogs[i]->getGenericDialogID()==theID)
			return(_allDialogs[i]);
	}
	return(NULL);	
}

void CGenericDialogContainer::buttonPressed(int blockID,int buttonID)
{
	for (int i=0;i<int(_allDialogs.size());i++)
		_allDialogs[i]->buttonPressed(blockID,buttonID);
}

void CGenericDialogContainer::textChanged(int blockID,int buttonID,const char* newTxt)
{
	for (int i=0;i<int(_allDialogs.size());i++)
		_allDialogs[i]->textChanged(blockID,buttonID,newTxt);
}

int CGenericDialogContainer::getDialogResult(int dlgHandle)
{
	CGenericDialog* it=getDialogFromID(dlgHandle);
	if (it==NULL)
		return(-1);
	return(it->getDialogResult());
}

bool CGenericDialogContainer::getDialogString(int dlgHandle,std::string& theString)
{
	CGenericDialog* it=getDialogFromID(dlgHandle);
	if (it==NULL)
		return(false);
	theString=it->getDialogString();
	return(true);
}

bool CGenericDialogContainer::removeDialogFromID(int theID)
{
	for (int i=0;i<int(_allDialogs.size());i++)
	{
		if (_allDialogs[i]->getGenericDialogID()==theID)
		{
			delete _allDialogs[i];
			_allDialogs.erase(_allDialogs.begin()+i);
			return(true);
		}
	}
	return(false);
}
