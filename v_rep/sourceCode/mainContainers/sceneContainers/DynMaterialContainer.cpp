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
#include "v_rep_internal.h"
#include "DynMaterialContainer.h"
#include "Tt.h"
#include "IloIlo.h"
#include "App.h"

CDynMaterialContainer::CDynMaterialContainer()
{
	CDynMaterialObject* def=new CDynMaterialObject(); // keep this object always in!!
	def->setObjectName("defaultMaterial");
	def->tagAsDefault();
	def->setEditableTag(false);
	def->setObjectID(SIM_IDSTART_DYNMATERIAL); // default object has always this ID!
	_allDynMaterialObjects.push_back(def);

	int idOffset=1;

	CDynMaterialObject* mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("highFrictionMaterial"); // box slightly slides on a 45.2deg inclined plane (plane and box have same mat.)
	mat->setOdeFriction(1.0f);
	mat->setBulletFriction(1.0f);
	std::vector<float> fp;
	mat->getVortexFloatParams(fp);
	fp[0]=1.0f;
	fp[1]=1.0f;
	mat->setVortexFloatParams(fp);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("lowFrictionMaterial"); // box slightly slides on a 10deg inclined plane (plane and box have same mat.)
	mat->setOdeFriction(0.41f);
	mat->setBulletFriction(0.41f);
	mat->getVortexFloatParams(fp);
	fp[0]=0.17f;
	fp[1]=0.17f;
	mat->setVortexFloatParams(fp);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("noFrictionMaterial"); // box slightly slides on a 10deg inclined plane (plane and box have same mat.)
	mat->setOdeFriction(0.0f);
	mat->setBulletFriction(0.0f);
	std::vector<int> ip;
	mat->getVortexIntParams(ip);
	ip[0]=6; // i.e. friction model: none
	ip[1]=6;
	mat->setVortexIntParams(ip);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("rest_stack_grasp_material"); // for things that have to be grasped, or have to stay on the floor or need to be stacked
	mat->setOdeFriction(1.0f);
	mat->setBulletFriction(1.0f);
	mat->getVortexIntParams(ip);
	mat->getVortexFloatParams(fp);
	ip[5]|=256; // i.e. auto-angular damping
	fp[0]=1.0f; // friction
	fp[1]=1.0f; // friction
	fp[22]=0.0f; // slip
	fp[23]=0.0f; // slip
	fp[31]=0.002f; // skin
	mat->setVortexIntParams(ip);
	mat->setVortexFloatParams(fp);
	mat->setEditableTag(false);
	mat->setDefaultRestStackGraspMaterial(true); // SPECIAL!!!! ONLY FOR THIS MATERIAL!!!!!!!!!!!!!!
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("footMaterial"); // for feet
	mat->setOdeFriction(1.0f);
	mat->setBulletFriction(1.0f);
	mat->getVortexIntParams(ip);
	mat->getVortexFloatParams(fp);
	fp[0]=1.0f; // friction
	fp[1]=1.0f; // friction
	fp[22]=0.0f; // slip
	fp[23]=0.0f; // slip
	fp[31]=0.002f; // skin
	mat->setVortexIntParams(ip);
	mat->setVortexFloatParams(fp);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("wheelMaterial"); // important: no auto-angular damping!
	mat->setOdeFriction(1.0f);
	mat->setBulletFriction(1.0f);
	mat->getVortexFloatParams(fp);
	fp[0]=1.0f; // friction
	fp[1]=1.0f; // friction
//	fp[22]=0.001f; // slip
//	fp[23]=0.001f; // slip
	fp[31]=0.0f; // skin
	mat->setVortexFloatParams(fp);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("gripperMaterial"); // for grippers, etc.
	mat->setOdeFriction(1.0f);
	mat->setBulletFriction(1.0f);
	mat->getVortexIntParams(ip);
	mat->getVortexFloatParams(fp);
	fp[0]=1.0f; // friction
	fp[1]=1.0f; // friction
	fp[22]=0.0f; // slip
	fp[23]=0.0f; // slip
	fp[31]=0.002f; // skin
	mat->setVortexIntParams(ip);
	mat->setVortexFloatParams(fp);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("floorMaterial"); // for floors
	mat->setOdeFriction(1.0f);
	mat->setBulletFriction(1.0f);
	mat->getVortexIntParams(ip);
	mat->getVortexFloatParams(fp);
	fp[0]=1.0f; // friction
	fp[1]=1.0f; // friction
	fp[22]=0.0f; // slip
	fp[23]=0.0f; // slip
	fp[31]=0.002f; // skin
	mat->setVortexIntParams(ip);
	mat->setVortexFloatParams(fp);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	/*
	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("cylindricalWheelMaterial"); // for wide cylindrical wheels
	mat->setOdeFriction(1.0f);
	mat->setBulletFriction(1.0f);
	mat->getVortexIntParams(ip);
	mat->getVortexFloatParams(fp);
	fp[0]=1.0f; // friction
	fp[1]=1.0f; // friction
	fp[22]=0.001f; // slip
	fp[23]=0.001f; // slip
	fp[31]=0.0f; // skin
	mat->setVortexIntParams(ip);
	mat->setVortexFloatParams(fp);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);
	*/

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("bulletMaterial_sticky_special"); // Bullet sticky contact
	mat->setBulletStickyContact(true);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("vortexMaterial_axisXWithoutFriction");
	mat->getVortexFloatParams(fp);
	mat->getVortexIntParams(ip);
	ip[5]=(ip[5]|32)-32; // i.e. 2nd axis uses same values as first axis
	ip[0]=6; // i.e. friction model: none
	fp[33]=1.0f; //x
	fp[34]=0.0f; //y
	fp[35]=0.0f; //z
	mat->setVortexIntParams(ip);
	mat->setVortexFloatParams(fp);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("vortexMaterial_axisYWithoutFriction");
	mat->getVortexFloatParams(fp);
	mat->getVortexIntParams(ip);
	ip[5]=(ip[5]|32)-32; // i.e. 2nd axis uses same values as first axis
	ip[0]=6; // i.e. friction model: none
	fp[33]=0.0f; //x
	fp[34]=1.0f; //y
	fp[35]=0.0f; //z
	mat->setVortexIntParams(ip);
	mat->setVortexFloatParams(fp);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("vortexMaterial_axisZWithoutFriction");
	mat->getVortexFloatParams(fp);
	mat->getVortexIntParams(ip);
	ip[5]=(ip[5]|32)-32; // i.e. 2nd axis uses same values as first axis
	ip[0]=6; // i.e. friction model: none
	fp[33]=0.0f; //x
	fp[34]=0.0f; //y
	fp[35]=1.0f; //z
	mat->setVortexIntParams(ip);
	mat->setVortexFloatParams(fp);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("vortexMaterial_pureShapesAsVxConvexMesh");
	mat->getVortexIntParams(ip);
	ip[5]|=1;
	mat->setVortexIntParams(ip);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("vortexMaterial_convexShapesAsVxTriangleMeshBVTree");
	mat->getVortexIntParams(ip);
	ip[5]|=2;
	mat->setVortexIntParams(ip);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);

	mat=new CDynMaterialObject(); // keep this object always in!!
	mat->setObjectName("vortexMaterial_randomShapesAsVxTriangleMeshUVGrid");
	mat->getVortexIntParams(ip);
	ip[5]|=4;
	mat->setVortexIntParams(ip);
	mat->setEditableTag(false);
	mat->setObjectID(SIM_IDSTART_DYNMATERIAL+idOffset++);
	_allDynMaterialObjects.push_back(mat);
}

CDynMaterialContainer::~CDynMaterialContainer()
{
	_removeAllObjects();
}

void CDynMaterialContainer::newSceneProcedure()
{
}

void CDynMaterialContainer::simulationAboutToStart()
{

}

void CDynMaterialContainer::simulationEnded()
{

}

void CDynMaterialContainer::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

CDynMaterialObject* CDynMaterialContainer::getDefaultObject()
{
	for (int i=0;i<int(_allDynMaterialObjects.size());i++)
	{
		if (_allDynMaterialObjects[i]->isDefaultObject())
			return(_allDynMaterialObjects[i]);
	}
	return(NULL); // should normally never happen!
}

CDynMaterialObject* CDynMaterialContainer::getRestStackGraspMaterial()
{
	for (int i=0;i<int(_allDynMaterialObjects.size());i++)
	{
		if (_allDynMaterialObjects[i]->isDefaultRestStackGraspMaterial())
			return(_allDynMaterialObjects[i]);
	}
	return(NULL); // should normally never happen!
}

CDynMaterialObject* CDynMaterialContainer::getObject(int objectID)
{
	for (int i=0;i<int(_allDynMaterialObjects.size());i++)
	{
		if (_allDynMaterialObjects[i]->getObjectID()==objectID)
			return(_allDynMaterialObjects[i]);
	}
	return(NULL);
}

CDynMaterialObject* CDynMaterialContainer::getObject(const char* objectName)
{
	for (int i=0;i<int(_allDynMaterialObjects.size());i++)
	{
		if (_allDynMaterialObjects[i]->getObjectName()==objectName)
			return(_allDynMaterialObjects[i]);
	}
	return(NULL);
}

CDynMaterialObject* CDynMaterialContainer::getObjectAtIndex(int index)
{
	if ( (index<0)||(index>=int(_allDynMaterialObjects.size())) )
		return(NULL);
	return(_allDynMaterialObjects[index]);
}

void CDynMaterialContainer::getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix)
{
	minSuffix=-1;
	maxSuffix=-1;
	for (int i=0;i<int(_allDynMaterialObjects.size());i++)
	{
		int s=tt::getNameSuffixNumber(_allDynMaterialObjects[i]->getObjectName().c_str(),true);
		if (i==0)
		{
			minSuffix=s;
			maxSuffix=s;
		}
		else
		{
			if (s<minSuffix)
				minSuffix=s;
			if (s>maxSuffix)
				maxSuffix=s;
		}
	}
}

bool CDynMaterialContainer::canSuffix1BeSetToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(_allDynMaterialObjects.size());i++)
	{
		int s1=tt::getNameSuffixNumber(_allDynMaterialObjects[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(_allDynMaterialObjects[i]->getObjectName().c_str(),true));
			for (int j=0;j<int(_allDynMaterialObjects.size());j++)
			{
				int s2=tt::getNameSuffixNumber(_allDynMaterialObjects[j]->getObjectName().c_str(),true);
				if (s2==suffix2)
				{
					std::string name2(tt::getNameWithoutSuffixNumber(_allDynMaterialObjects[j]->getObjectName().c_str(),true));
					if (name1==name2)
						return(false); // NO! We would have a name clash!
				}
			}
		}
	}
	return(true);
}

void CDynMaterialContainer::setSuffix1ToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(_allDynMaterialObjects.size());i++)
	{
		int s1=tt::getNameSuffixNumber(_allDynMaterialObjects[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(_allDynMaterialObjects[i]->getObjectName().c_str(),true));
			_allDynMaterialObjects[i]->setObjectName(tt::generateNewName_dash(name1,suffix2+1).c_str());
		}
	}
}

int CDynMaterialContainer::addObject(CDynMaterialObject* anObject,bool objectIsADuplicateThatShouldExistOnItsOwn)
{ // If object already exists (well, similar object), it is destroyed in here!
	if (!objectIsADuplicateThatShouldExistOnItsOwn)
	{
		CDynMaterialObject* theOldData=_getEquivalentDynMaterialObject(anObject);
		if (theOldData!=NULL)
		{ // we already have a similar object!!
			delete anObject;
			return(theOldData->getObjectID());
		}
	}

	int newID=SIM_IDSTART_DYNMATERIAL;
	while (getObject(newID)!=NULL)
		newID++;
	anObject->setObjectID(newID);
	std::string newName(anObject->getObjectName());
	while (getObject(newName.c_str())!=NULL)
		newName=tt::generateNewName_noDash(newName);
	anObject->setObjectName(newName.c_str());
	_allDynMaterialObjects.push_back(anObject);
	return(newID);
}

CDynMaterialObject* CDynMaterialContainer::_getEquivalentDynMaterialObject(CDynMaterialObject *theData)
{
	for (int i=0;i<int(_allDynMaterialObjects.size());i++)
	{
		if (_allDynMaterialObjects[i]->isSame(theData))
			return(_allDynMaterialObjects[i]);
	}
	return(NULL);
}

void CDynMaterialContainer::removeObject(int objectID)
{
	for (int i=0;i<int(_allDynMaterialObjects.size());i++)
	{
		if ((_allDynMaterialObjects[i]->getObjectID()==objectID)&&(!_allDynMaterialObjects[i]->isDefaultObject()))
		{ // the default object won't be erased here
			delete _allDynMaterialObjects[i];
			_allDynMaterialObjects.erase(_allDynMaterialObjects.begin()+i);
			App::ct->objCont->setFullDialogRefreshFlag();
			return;
		}
	}
}

int CDynMaterialContainer::getSameObjectID(CDynMaterialObject* anObject)
{
	for (int i=0;i<int(_allDynMaterialObjects.size());i++)
	{
		if (_allDynMaterialObjects[i]->isSame(anObject))
			return(_allDynMaterialObjects[i]->getObjectID());
	}	
	return(-1);
}

void CDynMaterialContainer::_removeAllObjects()
{ // the default object will also be erased!!
	for (int i=0;i<int(_allDynMaterialObjects.size());i++)
		delete _allDynMaterialObjects[i];
	_allDynMaterialObjects.clear();
}

void CDynMaterialContainer::storeDynMaterialObject(CSer& ar,CDynMaterialObject* it)
{
	ar.storeDataName(SER_DYNMATERIAL);
	ar.setCountingMode();
	it->serialize(ar);
	if (ar.setWritingMode())
		it->serialize(ar);
}

CDynMaterialObject* CDynMaterialContainer::loadDynMaterialObject(CSer& ar,std::string theName,bool &noHit)
{
	int byteNumber;
	if (theName.compare(SER_DYNMATERIAL)==0)
	{
		noHit=false;
		ar >> byteNumber; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CDynMaterialObject* myNewObject=new CDynMaterialObject();
		myNewObject->serialize(ar);
		return(myNewObject);
	}
	return(NULL);
}
