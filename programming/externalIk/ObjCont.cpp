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

#include "Tt.h"
#include "ObjCont.h"
#include "Ct.h"
#include "Global.h"
#include "v_repConst.h"


CObjCont::CObjCont()
{
	_objectActualizationEnabled=true;
	_nextObjectHandle=SIM_IDSTART_3DOBJECT;

	newSceneProcedure();
}

CObjCont::~CObjCont()
{
	removeAllObjects();
}



int CObjCont::getLoadingMapping(std::vector<int>* map,int oldVal)
{

	// using a slow mapping (little of RAM)
	for (int i=0;i<int(map->size())/2;i++)
	{
		if (oldVal==map->at(2*i+0))
			return(map->at(2*i+1));
	}
	return(-1);
}

void CObjCont::newSceneProcedure()
{
	removeAllObjects();
}

void CObjCont::removeAllObjects()
{

	while (objectList.size()!=0)
	{
		C3DObject* it=getObject(objectList[0]);
		if (it!=NULL) 
			eraseObject(it);
	}
	// The above loop destroys all 3DObjects, and normally automatically
	// all resources linked to them. So we don't have to destroy the resources

	objectList.clear();

	_objectIndex.clear();


	jointList.clear();
	dummyList.clear();
	orphanList.clear();
}

bool CObjCont::makeObjectChildOf(C3DObject* childObject,C3DObject* parentObject)
{	// This will trigger an actualization (important! orphanList needs also an update and other things too)
	// Careful: this routine should be able to be used for objects in the object
	// container, but also for objects in the copy buffer!!!! So don't make
	// use of any 'getObject(id)' or similar function!!!!! <-- this is a very old comment. Is it still true??
	if (childObject==NULL) 
		return(false);
	// Check if the child has already his desired parent (so that we don't have to call the actualization (heavy and will also refresh all dialogs) (added on 2009/12/15)
	if (childObject->getParent()==parentObject)
		return(true);
	if (parentObject==NULL)
	{
		C7Vector oldAbsoluteTransf(childObject->getCumulativeTransformationPart1());
		childObject->setParent(NULL);
		childObject->setLocalTransformation(oldAbsoluteTransf);
		actualizeObjectInformation();
		return(true);
	}
	// Illegal loop checking:
	if (parentObject->isObjectParentedWith(childObject)) 
		return(false);
	C7Vector oldAbsoluteTransf(childObject->getCumulativeTransformationPart1());
	C7Vector parentInverse(parentObject->getCumulativeTransformation().getInverse());
	childObject->setLocalTransformation(parentInverse*oldAbsoluteTransf);
	childObject->setParent(parentObject);

	

	actualizeObjectInformation();
	return(true);
}

void CObjCont::setAbsoluteConfiguration(int identifier,const C7Vector& v,bool keepChildrenInPlace)
{
	C3DObject* it=getObject(identifier);
	if (it==NULL)
		return;
	C7Vector childPreTr(it->getLocalTransformation());
	C7Vector parentInverse(it->getParentCumulativeTransformation().getInverse());
	it->setLocalTransformation(parentInverse*v);
	if (keepChildrenInPlace)
	{
		childPreTr=it->getLocalTransformation().getInverse()*childPreTr;
		for (int i=0;i<int(it->childList.size());i++)
			it->childList[i]->setLocalTransformation(childPreTr*it->childList[i]->getLocalTransformationPart1());
	}
}


void CObjCont::enableObjectActualization(bool e)
{
	_objectActualizationEnabled=e;
}

void CObjCont::actualizeObjectInformation()
{

	if (_objectActualizationEnabled) 
	{

		// Actualize each object's child list
		// Following rewritten on 2009/03/15 to make it faster:
		for (int i=0;i<int(objectList.size());i++)
		{
			C3DObject* it=_objectIndex[objectList[i]];
			it->childList.clear();
		}
		for (int i=0;i<int(objectList.size());i++)
		{
			C3DObject* it=_objectIndex[objectList[i]];
			C3DObject* parent=it->getParent();
			if (parent!=NULL)
				parent->childList.push_back(it);
		}

		// Actualize the light, graph, sensor & joint list
		jointList.clear();
		dummyList.clear();
		orphanList.clear();

		for (int i=0;i<int(objectList.size());i++)
		{
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_joint_type)
				jointList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_dummy_type)
				dummyList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getParent()==NULL)
				orphanList.push_back(objectList[i]);
		}
		// We actualize the direct linked joint list of each joint: (2009-01-27)
		for (int i=0;i<int(jointList.size());i++)
		{
			CJoint* it=getJoint(jointList[i]);
			it->directDependentJoints.clear();
			for (int j=0;j<int(jointList.size());j++)
			{
				CJoint* anAct=getJoint(ct::objCont->jointList[j]);
				if (anAct!=it)
				{
					if (anAct->getJointMode()==sim_jointmode_dependent) 
					{
						if (anAct->getDependencyJointID()==it->getID())
							it->directDependentJoints.push_back(anAct);
					}
				}
			}
		}

	}


}


int CObjCont::getObjectIdentifier(const std::string& objectName)
{
	for (int i=0;i<int(objectList.size());i++)
	{
		if (_objectIndex[objectList[i]]->getName().compare(objectName)==0)
			return(objectList[i]);
	}
	return(-1);
}

int CObjCont::getHighestObjectID()
{
	int highest=-1;
	for (int i=0;i<int(objectList.size());i++)
	{
		if (objectList[i]>highest)
			highest=objectList[i];
	}
	return(highest);
}

C3DObject* CObjCont::getObject(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	return(_objectIndex[identifier]);
}

CDummy* CObjCont::getDummy(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_dummy_type)
		return(NULL);
	return((CDummy*)_objectIndex[identifier]);
}

CJoint* CObjCont::getJoint(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_joint_type)
		return(NULL);
	return((CJoint*)_objectIndex[identifier]);
}

C3DObject* CObjCont::getObject(const std::string& name)
{
	return(getObject(getObjectIdentifier(name)));
}

bool CObjCont::eraseObject(C3DObject* it)
{

	if (it==NULL) 
		return(false);

	// We announce the object will be erased:
	announceObjectWillBeErased(it->getID()); // this may trigger other "eraseObject" calls (not really, since we don't have versatiles anymore!)
	// We remove the object from the object list
	int i;
	for (i=0;i<int(objectList.size());i++)
	{
		if (objectList[i]==it->getID()) 
			break;
	}
	objectList.erase(objectList.begin()+i);
	// Now remove the object from the index
	_objectIndex[it->getID()]=NULL;
//	int idSaved=it->getID();
	delete it;
	actualizeObjectInformation();

	return(true);
}

//------------------ Object destruction announcement -------------------------
void CObjCont::announceObjectWillBeErased(int objectID)
{
	for (int i=0;i<int(objectList.size());i++)
	{
		C3DObject* it=getObject(objectList[i]);
		if (it->getID()!=objectID)
		{
			if (it->announceObjectWillBeErased(objectID,false))
			{ // We should never enter here since one obj destruction cannot trigger another obj destruction (anymore, no more versatiles!) 
				eraseObject(it);
				i=-1; // ordering might have changed!
			}
		}
	}

	// First objects that won't trigger any more destructions:

	// Now objects that could trigger other destructions:

	ct::ikGroups->announceObjectWillBeErased(objectID); // can trigger pathPlanning object destruction
}

void CObjCont::announceIkGroupWillBeErased(int ikGroupID)
{
	for (int i=0;i<int(objectList.size());i++)
		getObject(objectList[i])->announceIkObjectWillBeErased(ikGroupID,false); // this never triggers 3DObject destruction!
	ct::ikGroups->announceIkGroupWillBeErased(ikGroupID); // This will never trigger an Ik group destruction
}



void CObjCont::importIkContent(CExtIkSer& ar)
{
	removeAllObjects();
	ct::ikGroups->removeAllIkGroups(); // just in case

	int versionNumber=ar.readInt(); // this is the ext IK serialization version. Not forward nor backward compatible!

	int objCnt=ar.readInt();

	std::vector<int> objectMapping;
	for (int i=0;i<objCnt;i++)
	{
		int objType=ar.readInt();

		C3DObject* it;

		if (objType==sim_object_joint_type)
		{
			CJoint* joint=new CJoint();
			joint->serializeRExtIk(ar);
			it=joint;
		}
		else
		{
			CDummy* dum=new CDummy();
			dum->serializeRExtIk(ar);
			it=dum;
		}
		objectMapping.push_back(it->getID()); // Old ID
		addObjectToSceneExtIk(it);
		objectMapping.push_back(it->getID()); // New ID
	}

	// We do it the slow way (in order not to use too much RAM)
	// prepareFastLoadingMapping(objectMapping);

	for (int i=0;i<int(objectList.size());i++)
	{
		C3DObject* it=getObject(objectList[i]);
		it->performObjectLoadingMapping(&objectMapping);
	}

	int ikGroupCnt=ar.readInt();

	for (int i=0;i<ikGroupCnt;i++)
	{
		CikGroup* it=new CikGroup();
		it->serializeRExtIk(ar);
		ct::ikGroups->addIkGroupExtIk(it);
	}

	for (int i=0;i<int(ct::ikGroups->ikGroups.size());i++)
	{
		CikGroup* it=ct::ikGroups->ikGroups[i];
		it->performObjectLoadingMapping(&objectMapping);
	}
}

bool CObjCont::addObjectToSceneExtIk(C3DObject* newObject)
{
	// Give the object a new identifier
	int id=-1;
	for (int i=0;i<int(_objectIndex.size());i++)
	{
		if (_objectIndex[i]==NULL)
		{
			_objectIndex[i]=newObject;
			id=i;
			break;
		}
	}
	if (id==-1)
	{
		id=int(_objectIndex.size());
		_objectIndex.push_back(newObject);
	}

	// set the new handle to the object:
	newObject->setID(id);
	objectList.push_back(id);

	// Actualize the object information
	actualizeObjectInformation();
	return(true);
}
