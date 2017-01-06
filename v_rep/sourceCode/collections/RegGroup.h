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

#include "RegGroupEl.h"
#include "vrepPrecompiledHeader.h"
#include <vector>

class CRegGroup  
{
public:
	CRegGroup(std::string grName);
	virtual ~CRegGroup();

	void initializeInitialValues(bool simulationIsRunning);
	void simulationAboutToStart();
	void simulationEnded();

	bool actualizeGroup(bool full=true);
	bool isObjectInGroup(int objID);
	int getGroupObjectCount();
	CRegGroupEl* getSubGroup(int subGroupID);
	bool addSubGroup(CRegGroupEl* subGr);
	bool removeSubGroup(int subGroupID);
	void unmarkAll();
	void mark(int objID);
	void unmark(int objID);
	bool areAllMarked();
	void serialize(CSer& ar);
	void performObjectLoadingMapping(std::vector<int>* map);
	bool announceObjectWillBeErased(int objID,bool copyBuffer);
	int getGroupID();
	void setGroupID(int newID);
	std::string getGroupName();
	void setGroupName(std::string newName);
	bool getOverridesObjectMainProperties();
	void setOverridesObjectMainProperties(bool o);
	CRegGroup* copyYourself();

	// Variables which need to be serialized
	std::vector<CRegGroupEl*> subGroupList;
	// Other
	std::vector<int> groupObjects;

private:
	// Variables which need to be serialized
	std::string groupName;
	int groupID;
	bool _overridesObjectMainProperties;
	// Other
	std::vector<int> markedObjects;
};
