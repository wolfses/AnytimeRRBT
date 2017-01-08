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

// This file requires serious refactoring!!!

#include "vrepPrecompiledHeader.h"
#include "v_rep_internal.h"
#include "IloIlo.h"
#include "Tt.h"
#include "OGL.h"
#include "Graph.h"
#include "VDateTime.h"
#include "v_repStrings.h"
#include "ThreadPool.h"
#include "App.h"


void CIloIlo::selectItemOfCombobox(QComboBox* theBox,int itemData)
{
	theBox->setCurrentIndex(_getIndexOfComboboxItemWithData(theBox,itemData));
}
int CIloIlo::_getIndexOfComboboxItemWithData(QComboBox* theBox,int itemData)
{
	for (int i=0;i<theBox->count();i++)
	{
		if (theBox->itemData(i).toInt()==itemData)
			return(i);
	}
	return(-1);
}


void CIloIlo::setModelAcknowledgementOLD_20100714(const std::string& ack)
{
	_modelAcknowledgementOLD_20100714=ack;
	if (_modelAcknowledgementOLD_20100714.length()>1000)
		_modelAcknowledgementOLD_20100714.erase(_modelAcknowledgementOLD_20100714.begin()+999,_modelAcknowledgementOLD_20100714.end());

}

std::string CIloIlo::getModelAcknowledgementOLD_20100714()
{
	return(_modelAcknowledgementOLD_20100714);
}

void CIloIlo::quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment()
{
	cNameSuffixAdjustmentTemporarilyDisabled=true;
}

void CIloIlo::enableCNameSuffixAdjustment()
{
	cNameSuffixAdjustmentTemporarilyDisabled=false;
}

std::string CIloIlo::getCNameSuffixAdjustedName(const char* name)
{
	std::string retVal(name);
	if ((!cNameSuffixAdjustmentTemporarilyDisabled)&&(retVal.find('#')==std::string::npos))
	{ // Lua script calls never enter here
		if (cNameSuffixNumber>-1)
		{
			retVal=tt::getNameWithoutSuffixNumber(name,true);
			retVal=tt::generateNewName_dash(retVal,cNameSuffixNumber+1);
		}
	}

	if (retVal.length()!=0)
	{
		if (retVal[retVal.length()-1]=='#')
			retVal.erase(retVal.end()-1);
	}
	return(retVal);
}

void CIloIlo::setCNameSuffixNumber(int number)
{
	cNameSuffixNumber=number;	
}

int CIloIlo::getCNameSuffixNumber()
{
	return(cNameSuffixNumber);	
}

bool CIloIlo::outputSimDllErrors=false;


CLuaCustomFunctionContainer CIloIlo::luaCustomFunctionContainer;

int CIloIlo::flyModeCameraHandle=-1;
int CIloIlo::flyModeCameraHandleInstance=-1;
int CIloIlo::nextfreeServerPortToUse=20000;

std::vector<contactCallback> CIloIlo::allContactCallbacks;
std::vector<jointCtrlCallback> CIloIlo::allJointCtrlCallbacks;


bool CIloIlo::frustumCulling=true;
bool CIloIlo::debugSensorCutting=false;
bool CIloIlo::collisionCachingOff=false;
bool CIloIlo::distanceCachingOff=false;
bool CIloIlo::simulationShowEnabled=false;
int CIloIlo::simulationShowTimeIntervalInMiliSec=15000;

int CIloIlo::maximumOpenGlLights=8;


bool CIloIlo::outputSceneOrModelLoadMessagesWithApiCall=false;
bool CIloIlo::fullModelCopyFromApi=true;

bool CIloIlo::wirelessForceShow_emission=false;
bool CIloIlo::wirelessForceShow_reception=false;

volatile int CIloIlo::apiLocked=0;
DWORD CIloIlo::idOfLockingThread=-1;

std::string CIloIlo::_modelAcknowledgementOLD_20100714="";

bool CIloIlo::cNameSuffixAdjustmentTemporarilyDisabled=false;
int CIloIlo::cNameSuffixNumber=-1;

int CIloIlo::proxSensorClickSelectDown=0; // reset at simulation start!
int CIloIlo::proxSensorClickSelectUp=0; // reset at simulation start!
