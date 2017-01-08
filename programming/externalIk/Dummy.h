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

#pragma once

#include "extIk.h"
#include "3DObject.h"
#include "3Vector.h"
#include "7Vector.h"
#include <vector>
#include <string>

class CDummy : public C3DObject  
{
public:

	CDummy();
	virtual ~CDummy();

	// Following functions are inherited from 3DObject

public:
	bool announceObjectWillBeErased(int objID,bool copyBuffer);
	void announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);

	void performObjectLoadingMapping(std::vector<int>* map);

	void setLinkedDummyID(int id,bool setDirectly);
	void setLinkType(int lt,bool setDirectly);
	int getLinkedDummyID();
	int getLinkType();

	C7Vector getTempLocalTransformation();
	void setTempLocalTransformation(const C7Vector& tr);

protected:
	C7Vector _localTransformation_temp; // used for IK (needed when the dummy is freely sliding on a path object!)

	// Variables which need to be serialized & copied 
	int _linkedDummyID;
	int _linkType;

public:
	void serializeRExtIk(CExtIkSer& ar);
};
