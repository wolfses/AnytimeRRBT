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

#include "MainCont.h"
#include <string>
#include <vector>

class CSignalContainer : public CMainCont 
{
public:
	CSignalContainer();
	virtual ~CSignalContainer();
	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);
	void newSceneProcedure();

	void removeAllSignals(bool onlyThoseFromEmbeddedScripts);

	void setIntegerSignal(const char* signalName,int value,bool fromEmbeddedScript);
	bool getIntegerSignal(const char* signalName,int& value);
	bool getIntegerSignalNameAtIndex(int index,std::string& signalName);
	int clearIntegerSignal(const char* signalName);
	int clearAllIntegerSignals(bool onlyThoseFromEmbeddedScripts);

	void setFloatSignal(const char* signalName,float value,bool fromEmbeddedScript);
	bool getFloatSignal(const char* signalName,float& value);
	bool getFloatSignalNameAtIndex(int index,std::string& signalName);
	int clearFloatSignal(const char* signalName);
	int clearAllFloatSignals(bool onlyThoseFromEmbeddedScripts);


	void setStringSignal(const char* signalName,const std::string& value,bool fromEmbeddedScript);
	bool getStringSignal(const char* signalName,std::string& value);
	bool getStringSignalNameAtIndex(int index,std::string& signalName);
	int clearStringSignal(const char* signalName);
	int clearAllStringSignals(bool onlyThoseFromEmbeddedScripts);

protected:
	int _getIntegerSignalIndex(const char* signalName);
	int _getFloatSignalIndex(const char* signalName);
	int _getStringSignalIndex(const char* signalName);

	std::vector<std::string> _intSignalNames;
	std::vector<int> _intSignalValues;
	std::vector<bool> _intSignalEmbScriptCreated;

	std::vector<std::string> _floatSignalNames;
	std::vector<float> _floatSignalValues;
	std::vector<bool> _floatSignalEmbScriptCreated;

	std::vector<std::string> _stringSignalNames;
	std::vector<std::string> _stringSignalValues;
	std::vector<bool> _stringSignalEmbScriptCreated;
};
