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
#include "LuaScriptParameters.h"
#include "IloIlo.h"
#include "App.h"

CLuaScriptParameters::CLuaScriptParameters()
{
	_initialValuesInitialized=false;
}

CLuaScriptParameters::~CLuaScriptParameters()
{
}

void CLuaScriptParameters::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	clearInitialParameters();
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		for (int i=0;i<int(parameterNames.size());i++)
		{
			_initialParameterNames.push_back(parameterNames[i]);
			_initialParameterUnits.push_back(parameterUnits[i]);
			_initialParameterValues.push_back(parameterValues[i]);
			_initialParameterProperties.push_back(parameterProperties[i]);
		}
	}
}

void CLuaScriptParameters::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
	{
		std::vector<std::string> currentParameterNames(parameterNames);
		std::vector<std::string> currentParameterUnits(parameterUnits);
		std::vector<std::string> currentParameterValues(parameterValues);
		std::vector<int> currentParameterProperties(parameterProperties);
		parameterNames.clear();
		parameterUnits.clear();
		parameterValues.clear();
		parameterProperties.clear();
		for (int i=0;i<int(_initialParameterNames.size());i++)
		{
			if (_initialParameterProperties[i]&2)
			{ // parameter is persistent
				parameterNames.push_back(currentParameterNames[i]);
				parameterUnits.push_back(currentParameterUnits[i]);
				parameterValues.push_back(currentParameterValues[i]);
				parameterProperties.push_back(currentParameterProperties[i]);
			}
			else
			{
				parameterNames.push_back(_initialParameterNames[i]);
				parameterUnits.push_back(_initialParameterUnits[i]);
				parameterValues.push_back(_initialParameterValues[i]);
				parameterProperties.push_back(_initialParameterProperties[i]);
			}
		}
	}
	_initialValuesInitialized=false;
}

void CLuaScriptParameters::clearInitialParameters()
{
	_initialValuesInitialized=false;
	_initialParameterNames.clear();
	_initialParameterUnits.clear();
	_initialParameterValues.clear();
	_initialParameterProperties.clear();
}

bool CLuaScriptParameters::setParameterValue(const char* paramName,const char* paramValue,int paramValueLength)
{
	int ind=getParameterIndex(paramName);
	if (ind>=0)
	{
		parameterValues[ind].assign(paramValue,paramValueLength);
		for (int i=0;i<paramValueLength;i++)
		{
			if (paramValue[i]==0)
			{
				parameterProperties[ind]|=4; // Indicates that string contains embedded 0's
				break;
			}
		}
		return(true);
	}
	return(false);
}

bool CLuaScriptParameters::getParameterValue(const char* paramName,std::string& paramValue)
{
	int ind=getParameterIndex(paramName);
	if (ind>=0)
	{
		paramValue=parameterValues[ind];
		return(true);
	}
	return(false);
}

void CLuaScriptParameters::addParameterValue(const char* paramName,const char* unitText,const char* paramValue,int paramValueLength)
{
	int ind=getParameterIndex(paramName);
	if (ind<0)
	{ // parameter not yet present!
		parameterNames.push_back(paramName);	
		parameterUnits.push_back("");	
		parameterValues.push_back("");
		parameterProperties.push_back(0);
		ind=parameterNames.size()-1;
	}
	parameterUnits[ind]=unitText;
	parameterValues[ind].assign(paramValue,paramValueLength);
	for (int i=0;i<paramValueLength;i++)
	{
		if (paramValue[i]==0)
		{
			parameterProperties[ind]|=4; // Indicates that string contains embedded 0's
			break;
		}
	}
}

bool CLuaScriptParameters::removeParameterValue(int index)
{
	if ( (index<0)||(index>=int(parameterNames.size())) )
		return(false);
	parameterNames.erase(parameterNames.begin()+index);
	parameterUnits.erase(parameterUnits.begin()+index);
	parameterValues.erase(parameterValues.begin()+index);
	parameterProperties.erase(parameterProperties.begin()+index);
	return(true);
}

bool CLuaScriptParameters::removeParameterValue(const char* paramName)
{
	int ind=getParameterIndex(paramName);
	if (ind<0)
		return(false);
	return(removeParameterValue(ind));
}

int CLuaScriptParameters::getParameterIndex(const char* paramName)
{
	for (int i=0;i<int(parameterNames.size());i++)
	{
		if (parameterNames[i]==paramName)
			return(i);
	}
	return(-1);
}

CLuaScriptParameters* CLuaScriptParameters::copyYourself()
{
	// First the regular stuff:
	CLuaScriptParameters* p=new CLuaScriptParameters();
	for (int i=0;i<int(parameterNames.size());i++)
	{
		p->parameterNames.push_back(parameterNames[i]);
		p->parameterUnits.push_back(parameterUnits[i]);
		p->parameterValues.push_back(parameterValues[i]);
		p->parameterProperties.push_back(parameterProperties[i]);
	}

	//// Now we also have to copy the temp values (values created/modified/deleted during simulation)
	//for (int i=0;i<int(_initialParameterNames.size());i++)
	//{
	//	p->_initialParameterNames.push_back(_initialParameterNames[i]);
	//	p->_initialParameterUnits.push_back(_initialParameterUnits[i]);
	//	p->_initialParameterValues.push_back(_initialParameterValues[i]);
	//	p->_initialParameterProperties.push_back(_initialParameterProperties[i]);
	//}
	//p->_initialValuesInitialized=_initialValuesInitialized;
	return(p);	
}

void CLuaScriptParameters::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Par");
		ar << int(parameterNames.size());
		for (int i=0;i<int(parameterNames.size());i++)
		{
			ar << parameterNames[i];
			ar << parameterUnits[i];
			ar << parameterValues[i];
			ar << parameterProperties[i];
		}
		ar.flush();

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Par")==0)
				{
					noHit=false;
					int paramCount;
					ar >> byteQuantity;
					ar >> paramCount;
					for (int i=0;i<paramCount;i++)
					{
						std::string tmp;
						ar >> tmp;
						parameterNames.push_back(tmp);
						ar >> tmp;
						parameterUnits.push_back(tmp);
						ar >> tmp;
						parameterValues.push_back(tmp);
						int dummy;
						ar >> dummy;
						parameterProperties.push_back(dummy);
					}
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
