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

#include "Ct.h"


ct::ct()
{
}

ct::~ct()
{
}



int ct::createNewInstance(bool triggerCallbackAndHandleDialogs)
{
	if (globalInstanceCreationOrSwitchingLocked)
		return(-1);



	ikGroupList.push_back(NULL);
	objContList.push_back(NULL);

	currentInstanceIndex=int(objContList.size())-1;


	ikGroups=NULL;
	objCont=NULL;

	ikGroups=new CRegisterediks();
	objCont=new CObjCont();


	ikGroupList[currentInstanceIndex]=ikGroups;
	objContList[currentInstanceIndex]=objCont;


	return(currentInstanceIndex);
}

int ct::getCurrentInstanceIndex()
{
	return (currentInstanceIndex);
}

int ct::destroyCurrentInstance()
{
	if (currentInstanceIndex==-1)
		return(-1);


	// Important to empty objects first (since ct::objCont->announce....willBeErase
	// might be called for already destroyed objects!)

	ikGroups->removeAllIkGroups();


	objCont->removeAllObjects();




	delete objCont;
	objCont=NULL;


	delete ikGroups;
	ikGroups=NULL;



	objContList.erase(objContList.begin()+currentInstanceIndex);
	ikGroupList.erase(ikGroupList.begin()+currentInstanceIndex);

	if (objContList.size()!=0)
	{
		currentInstanceIndex=-1; // We set this so that next instruction succeeds (otherwise we might already be in instance 0 and the function will not process). Added on 5/3/2012
		makeInstanceCurrentFromIndex(int(objContList.size())-1,true);
	}
	else
		currentInstanceIndex=-1;
	return(currentInstanceIndex);	
}

int ct::getInstanceNumber()
{
	return(int(objContList.size()));
}

void ct::createSingletonsAndOneInstance(bool enableMultipleInst)
{

	createNewInstance(true);


	globalInstanceCreationOrSwitchingLocked=!enableMultipleInst;
}

void ct::destroySingletonsAndAllInstances()
{

	while (objContList.size()!=0)
	{
		makeInstanceCurrentFromIndex(int(objContList.size())-1,true);
		destroyCurrentInstance();
	}

}

void ct::emptyScene(bool notCalledFromUndoFunction)
{

	// Following is saved:

	ikGroups->removeAllIkGroups();


	objCont->newSceneProcedure();

}

bool ct::makeInstanceCurrentFromIndex(int instanceIndex,bool triggerCallbackAndHandleDialogs)
{ 
	if ( (instanceIndex<0)||(instanceIndex>=int(objContList.size())))
		return(false);

	if (currentInstanceIndex==instanceIndex)
		return(true);
	if (isInstanceSwitchingLocked())
		return(false);


	currentInstanceIndex=instanceIndex;


	ikGroups=ikGroupList[currentInstanceIndex];
	objCont=objContList[currentInstanceIndex];


	return(true);
}

bool ct::isInstanceSwitchingLocked()
{
	return(globalInstanceCreationOrSwitchingLocked);
}

bool ct::globalInstanceCreationOrSwitchingLocked=false;
int ct::currentInstanceIndex=-1;

CRegisterediks* ct::ikGroups=NULL;
CObjCont* ct::objCont=NULL;

std::vector<CRegisterediks*> ct::ikGroupList;
std::vector<CObjCont*> ct::objContList;

