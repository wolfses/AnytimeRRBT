// This file is part of the EXTERNAL IK for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The EXTERNAL IK is licensed under the terms of EITHER:
//   1. EXTERNAL IK commercial license (contact us for details)
//   2. EXTERNAL IK educational license (see below)
// 
// EXTERNAL IK educational license:
// -------------------------------------------------------------------
// The EXTERNAL IK educational license applies only to EDUCATIONAL
// ENTITIES composed by following people and institutions:
// 
// 1. Hobbyists, students, teachers and professors
// 2. Schools and universities
// 
// EDUCATIONAL ENTITIES do NOT include companies, research institutions,
// non-profit organisations, foundations, etc.
// 
// An EDUCATIONAL ENTITY may use, modify, compile and distribute the
// modified/unmodified EXTERNAL IK under following conditions:
//  
// 1. Distribution should be free of charge.
// 2. Distribution should be to EDUCATIONAL ENTITIES only.
// 3. Usage should be non-commercial.
// 4. Altered source versions must be plainly marked as such and distributed
//    along with any compiled code.
// 5. The origin of the EXTERNAL IK must not be misrepresented. you must
//    not claim that you wrote the original software.
// 
// THE EXTERNAL IK IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

// This file needs serious refactoring!!

#pragma once

#include "extIk.h"
#include "3DObject.h"
#include "jointObject.h"
#include "ikGroup.h"
#include "MainCont.h"

class CObjCont : public CMainCont 
{
public:
	CObjCont();
	virtual ~CObjCont();


public:
	void newSceneProcedure();
	void removeAllObjects();
	void actualizeObjectInformation();

	int getLoadingMapping(std::vector<int>* map,int oldVal);

	int getObjectIdentifier(const std::string& objectName);
	C3DObject* getObject(int identifier);
	CDummy* getDummy(int identifier);
	CJoint* getJoint(int identifier);
	C3DObject* getObject(const std::string& name);

	bool makeObjectChildOf(C3DObject* childObject,C3DObject* parentObject);
	void setAbsoluteConfiguration(int identifier,const C7Vector& v,bool keepChildrenInPlace);

	int getHighestObjectID();
	bool eraseObject(C3DObject* it);

	void announceObjectWillBeErased(int objectID);
	void announceIkGroupWillBeErased(int ikGroupID);

	std::vector<int> objectsWhichNeedToBeErased;
	void enableObjectActualization(bool e);

	std::vector<C3DObject*> _objectIndex;

	bool _objectActualizationEnabled;

	int _nextObjectHandle;

	std::vector<int> orphanList;
	std::vector<int> objectList;
	std::vector<int> jointList;
	std::vector<int> dummyList;

private:


public:
	void importIkContent(CExtIkSer& ar);
	bool addObjectToSceneExtIk(C3DObject* newObject);
};

