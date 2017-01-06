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
#include "LuaCustomFunctionContainer.h"

CLuaCustomFunctionContainer::CLuaCustomFunctionContainer()
{
}

CLuaCustomFunctionContainer::~CLuaCustomFunctionContainer()
{
	removeAllCustomFunctions();
	removeAllCustomVariables();
}

void CLuaCustomFunctionContainer::removeAllCustomFunctions()
{
	for (int i=0;i<int(allCustomFunctions.size());i++)
		delete allCustomFunctions[i];
	allCustomFunctions.clear();
}

bool CLuaCustomFunctionContainer::removeCustomFunctionFromName(const char* functionName)
{
	for (int i=0;i<int(allCustomFunctions.size());i++)
	{
		if (std::string(functionName)==allCustomFunctions[i]->getFunctionName())
		{ // we have to remove this one
			delete allCustomFunctions[i];
			allCustomFunctions.erase(allCustomFunctions.begin()+i);
			return(true);
		}
	}
	return(false);
}

bool CLuaCustomFunctionContainer::insertCustomFunction(CLuaCustomFunction* function)
{
	if (doesCustomFunctionAlreadyExist(function))
		return(false);
	if (function->getFunctionName().length()==0)
		return(false);
	int newID=0;
	while (getCustomFunctionFromID(newID)!=NULL)
		newID++;
	function->setFunctionID(newID);
	allCustomFunctions.push_back(function);
	return(true);
}

bool CLuaCustomFunctionContainer::doesCustomFunctionAlreadyExist(CLuaCustomFunction* function)
{
	for (int i=0;i<int(allCustomFunctions.size());i++)
	{
		if (function->getFunctionName()==allCustomFunctions[i]->getFunctionName())
			return(true);
	}
	return(false);
}

CLuaCustomFunction* CLuaCustomFunctionContainer::getCustomFunctionFromID(int functionID)
{
	for (int i=0;i<int(allCustomFunctions.size());i++)
	{
		if (functionID==allCustomFunctions[i]->getFunctionID())
			return(allCustomFunctions[i]);
	}
	return(NULL);
}

void CLuaCustomFunctionContainer::removeAllCustomVariables()
{
	allCustomVariableNames.clear();
	allCustomVariableValues.clear();
}

bool CLuaCustomFunctionContainer::removeCustomVariable(const char* variableName)
{
	for (int i=0;i<int(allCustomVariableNames.size());i++)
	{
		if (std::string(variableName)==allCustomVariableNames[i])
		{ // we have to remove this one
			allCustomVariableNames.erase(allCustomVariableNames.begin()+i);
			allCustomVariableValues.erase(allCustomVariableValues.begin()+i);
			return(true);
		}
	}
	return(false);
}

bool CLuaCustomFunctionContainer::insertCustomVariable(const char* variableName,const char* variableValue)
{
	std::string v(variableName);
	if (v.length()==0)
		return(false);
	if (doesCustomVariableAlreadyExist(variableName))
		return(false);
	allCustomVariableNames.push_back(variableName);
	allCustomVariableValues.push_back(variableValue);
	return(true);
}

bool CLuaCustomFunctionContainer::doesCustomVariableAlreadyExist(const char* variableName)
{
	for (int i=0;i<int(allCustomVariableNames.size());i++)
	{
		if (allCustomVariableNames[i]==std::string(variableName))
			return(true);
	}
	return(false);
}
