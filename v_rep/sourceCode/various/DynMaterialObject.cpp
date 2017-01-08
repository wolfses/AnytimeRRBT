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
#include "DynMaterialObject.h"
#include "IloIlo.h"
#include "App.h"
#include "Tt.h"
#include <boost/format.hpp>

int CDynMaterialObject::_nextUniqueID=0;

CDynMaterialObject::CDynMaterialObject()
{
	_objectID=SIM_IDSTART_DYNMATERIAL;
	_objectName="usr_";
	_defaultObject=false; // there can be only one default obect
	_defaultRestStackGraspMaterial=false;
	_editableTag=true;
	_usedTag=false;

	_uniqueID=_nextUniqueID++;

	_bulletStickyContact=false;
	_bulletRestitution=0.0f;
	_bulletFriction=0.71f; // (box on 26.8deg inclined plane slightly slides (same mat. for box and plane)
	_bulletLinearDamping=0.0f;
	_bulletAngularDamping=0.0f;
	_bulletNonDefaultCollisionMarginFactor=0.1f;
	_bulletNonDefaultCollisionMargin=false;
	_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape=0.002f;
	_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=true;
	_bulletAutoShrinkConvexMesh=false;

	_odeMaxContacts=8;
	_odeFriction=0.71f; // (box on 26.8deg inclined plane slightly slides (same mat. for box and plane)
	_odeSoftERP=0.2f;
	_odeSoftCFM=0.0f;
	_odeLinearDamping=0.0f;
	_odeAngularDamping=0.0f;

	// Vortex parameters:
	// ----------------------------------------------------
	_vortexFloatParams.push_back(0.5f); // friction coeff., linear axis, primary (box on 26.8deg inclined plane slightly slides (same mat. for box and plane)
	_vortexFloatParams.push_back(0.5f); // friction coeff., linear axis, secondary (box on 26.8deg inclined plane slightly slides (same mat. for box and plane)
	_vortexFloatParams.push_back(0.0f); // friction coeff., angular axis, primary
	_vortexFloatParams.push_back(0.0f); // friction coeff., angular axis, secondary
	_vortexFloatParams.push_back(0.0f); // friction coeff., angular axis, normal

	_vortexFloatParams.push_back(1.1f); // static friction scale, linear axis, primary (was 1.0f)
	_vortexFloatParams.push_back(1.1f); // static friction scale, linear axis, secondary (was 1.0f)
	_vortexFloatParams.push_back(1.0f); // static friction scale, angular axis, primary
	_vortexFloatParams.push_back(1.0f); // static friction scale, angular axis, secondary
	_vortexFloatParams.push_back(1.0f); // static friction scale, angular axis, normal

	_vortexFloatParams.push_back(0.00000001f); // compliance (was 0.000005f)
	_vortexFloatParams.push_back(10000000.0f); // damping (was 0.0f)
	_vortexFloatParams.push_back(0.0f); // restitution
	_vortexFloatParams.push_back(0.5f); // restitution threshold
	_vortexFloatParams.push_back(0.0f); // adhesive force
	_vortexFloatParams.push_back(0.0f); // linear velocity damping
	_vortexFloatParams.push_back(0.0f); // angular velocity damping

	_vortexFloatParams.push_back(0.0f); // slide, linear axis, primary
	_vortexFloatParams.push_back(0.0f); // slide, linear axis, secondary
	_vortexFloatParams.push_back(0.0f); // slide, angular axis, primary
	_vortexFloatParams.push_back(0.0f); // slide, angular axis, secondary
	_vortexFloatParams.push_back(0.0f); // slide, angular axis, normal

	_vortexFloatParams.push_back(0.0f); // slip, linear axis, primary (was 0.0001f)
	_vortexFloatParams.push_back(0.0f); // slip, linear axis, secondary (was 0.0001f)
	_vortexFloatParams.push_back(0.0f); // slip, angular axis, primary
	_vortexFloatParams.push_back(0.0f); // slip, angular axis, secondary
	_vortexFloatParams.push_back(0.0f); // slip, angular axis, normal

	_vortexFloatParams.push_back(0.14f); // autosleep linear speed threshold
	_vortexFloatParams.push_back(0.045f); // autosleep linear accel threshold
	_vortexFloatParams.push_back(0.03f); // autosleep angular speed threshold
	_vortexFloatParams.push_back(0.045f); // autosleep angular accel threshold

	_vortexFloatParams.push_back(0.0f); // skin thickness (was 0.002 before)
	_vortexFloatParams.push_back(0.01f); // auto angular damping tension ratio
	_vortexFloatParams.push_back(0.0f);	// primary axis vector (x component)
	_vortexFloatParams.push_back(0.0f);	// primary axis vector (y component)
	_vortexFloatParams.push_back(1.0f);	// primary axis vector (z component) --> by default, primary axis aligned with Z axis (if possible)
	// VORTEX_SHAPE_FLOAT_PARAM_CNT_2=36

	_vortexIntParams.push_back(4); // friction model, linear axis, primary (0=box, 1=scaledBox, 2=proportionalLow, 3=proportionalHigh, 4=scaledBoxFast, 5=neutral, 6=none)
	_vortexIntParams.push_back(4); // friction model, linear axis, secondary
	_vortexIntParams.push_back(6); // friction model, angular axis, primary (none)
	_vortexIntParams.push_back(6); // friction model, angular axis, secondary (none)
	_vortexIntParams.push_back(6); // friction model, angular axis, normal (none)

	_vortexIntParams.push_back(8+0+32+64+128); // bit coded: 0set=treat pure shapes as convex shapes, 1set=treat convex shapes as random shapes, 2set=threat random shapes as terrain, 3set=fast moving, 4set=auto-slip,
												// 5set=sec. lin. axis uses same values as prim. lin. axis, 6set=sec. ang. axis uses same values as prim. ang. axis, 7set=norm. ang. axis uses same values as prim. ang. axis (bits 5-7 are not taken into account on the plugin side: they just serve as setting memory)
												// 8set=auto angular damping is on
	_vortexIntParams.push_back(10); // autosleep step live threshold
	_vortexIntParams.push_back(_uniqueID); // the material unique ID. The ID is redefined in each session
	// VORTEX_SHAPE_INT_PARAM_CNT_2=8
	// ----------------------------------------------------
}

CDynMaterialObject::~CDynMaterialObject()
{
}

void CDynMaterialObject::tagAsDefault()
{
	_defaultObject=true;
}

bool CDynMaterialObject::isDefaultObject()
{
	return(_defaultObject);
}

bool CDynMaterialObject::isDefaultRestStackGraspMaterial()
{
	return(_defaultRestStackGraspMaterial);
}

void CDynMaterialObject::setDefaultRestStackGraspMaterial(bool d)
{
	_defaultRestStackGraspMaterial=d;
}


void CDynMaterialObject::setUsedTag(bool used)
{
	_usedTag=used;
}

bool CDynMaterialObject::getUsedTag()
{
	return(_usedTag);
}

void CDynMaterialObject::setEditableTag(bool editable)
{
	_editableTag=editable;
}

bool CDynMaterialObject::getEditableTag()
{
	return(_editableTag);
}

void CDynMaterialObject::setObjectID(int newID)
{
	_objectID=newID;
}

int CDynMaterialObject::getObjectID()
{
	return(_objectID);
}

void CDynMaterialObject::setObjectName(const char* newName)
{
	_objectName=newName;
}

std::string CDynMaterialObject::getObjectName()
{
	return(_objectName);
}


// Engine setters/getters:
//************************
void CDynMaterialObject::setBulletRestitution(float r)
{
	_bulletRestitution=tt::getLimitedFloat(0.0f,10.0f,r);
}

float CDynMaterialObject::getBulletRestitution()
{
	return(_bulletRestitution);
}

void CDynMaterialObject::setBulletFriction(float f)
{
	if (f<0.0f)
		f=0.0f;
	_bulletFriction=f;
}

float CDynMaterialObject::getBulletFriction()
{
	return(_bulletFriction);
}

void CDynMaterialObject::setBulletStickyContact(bool sticky)
{
	_bulletStickyContact=sticky;
}

bool CDynMaterialObject::getBulletStickyContact()
{
	return(_bulletStickyContact);
}

void CDynMaterialObject::setBulletLinearDamping(float d)
{
	if (d<0.0f)
		d=0.0f;
	_bulletLinearDamping=d;
}

float CDynMaterialObject::getBulletLinearDamping()
{
	return(_bulletLinearDamping);
}

void CDynMaterialObject::setBulletAngularDamping(float d)
{
	if (d<0.0f)
		d=0.0f;
	_bulletAngularDamping=d;
}

float CDynMaterialObject::getBulletAngularDamping()
{
	return(_bulletAngularDamping);
}

void CDynMaterialObject::setBulletNonDefaultCollisionMargin(bool for_convex_and_not_pure,bool b)
{
	if (for_convex_and_not_pure)
		_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=b;
	else
		_bulletNonDefaultCollisionMargin=b;
}

bool CDynMaterialObject::getBulletNonDefaultCollisionMargin(bool for_convex_and_not_pure)
{
	if (for_convex_and_not_pure)
		return(_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape);
	else
		return(_bulletNonDefaultCollisionMargin);
}

void CDynMaterialObject::setBulletNonDefaultCollisionMarginFactor(bool for_convex_and_not_pure,float m)
{
	m=tt::getLimitedFloat(0.0001f,1000.0f,m);
	if (for_convex_and_not_pure)
		_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape=m;
	else
		_bulletNonDefaultCollisionMarginFactor=m;
}

float CDynMaterialObject::getBulletNonDefaultCollisionMarginFactor(bool for_convex_and_not_pure)
{
	if (for_convex_and_not_pure)
		return(_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape);
	else
		return(_bulletNonDefaultCollisionMarginFactor);
}

void CDynMaterialObject::setBulletAutoShrinkConvexMesh(bool s)
{
	_bulletAutoShrinkConvexMesh=s;
}

bool CDynMaterialObject::getBulletAutoShrinkConvexMesh()
{
	return(_bulletAutoShrinkConvexMesh);
}

void CDynMaterialObject::setOdeMaxContacts(int m)
{
	_odeMaxContacts=tt::getLimitedInt(-64,64,m);
}

int CDynMaterialObject::getOdeMaxContacts()
{
	return(_odeMaxContacts);
}

void CDynMaterialObject::setOdeFriction(float f)
{
	if (f<0.0f)
		f=0.0f;
	_odeFriction=f;
}

float CDynMaterialObject::getOdeFriction()
{
	return(_odeFriction);
}

void CDynMaterialObject::setOdeSoftERP(float e)
{
	_odeSoftERP=tt::getLimitedFloat(0.0f,1.0f,e);
}

float CDynMaterialObject::getOdeSoftERP()
{
	return(_odeSoftERP);
}

void CDynMaterialObject::setOdeSoftCFM(float e)
{
	_odeSoftCFM=tt::getLimitedFloat(0.0f,1.0f,e);
}

float CDynMaterialObject::getOdeSoftCFM()
{
	return(_odeSoftCFM);
}

void CDynMaterialObject::setOdeLinearDamping(float d)
{
	if (d<0.0f)
		d=0.0f;
	_odeLinearDamping=d;
}

float CDynMaterialObject::getOdeLinearDamping()
{
	return(_odeLinearDamping);
}

void CDynMaterialObject::setOdeAngularDamping(float d)
{
	if (d<0.0f)
		d=0.0f;
	_odeAngularDamping=d;
}

float CDynMaterialObject::getOdeAngularDamping()
{
	return(_odeAngularDamping);
}

void CDynMaterialObject::getVortexFloatParams(std::vector<float>& p)
{
	p.assign(_vortexFloatParams.begin(),_vortexFloatParams.end());
}

void CDynMaterialObject::setVortexFloatParams(const std::vector<float>& p)
{
	int l=int(p.size());
	if (l>int(_vortexFloatParams.size()))
		l=int(_vortexFloatParams.size()); // should normally never happen!
	for (int i=0;i<l;i++)
		_vortexFloatParams[i]=p[i];

	_vortexFloatParams[31]=tt::getLimitedFloat(0.0f,10.0f,_vortexFloatParams[31]); // skin thickness
	_vortexFloatParams[32]=tt::getLimitedFloat(0.0f,1000.0f,_vortexFloatParams[32]); // auto angular damping tension ratio
	// make sure the vector for the primary axis is normalized:
	C3Vector v(&_vortexFloatParams[0]+33);
	if (v.getLength()<0.01f)
		v=C3Vector::unitZVector;
	v.normalize();
	_vortexFloatParams[33]=v(0);
	_vortexFloatParams[34]=v(1);
	_vortexFloatParams[35]=v(2);
}

void CDynMaterialObject::getVortexIntParams(std::vector<int>& p)
{
	_vortexIntParams[7]=_uniqueID;
	p.assign(_vortexIntParams.begin(),_vortexIntParams.end());
}

void CDynMaterialObject::setVortexIntParams(const std::vector<int>& p)
{
	int l=int(p.size());
	if (l>int(_vortexIntParams.size()))
		l=int(_vortexIntParams.size()); // should normally never happen!
	for (int i=0;i<l;i++)
		_vortexIntParams[i]=p[i];

	_vortexIntParams[6]=tt::getLimitedInt(1,10000,_vortexIntParams[6]); // auto-sleep steps
	_vortexIntParams[7]=_uniqueID;
}
//************************


bool CDynMaterialObject::isSame(CDynMaterialObject* theNewObj)
{
	if (theNewObj->_vortexFloatParams.size()!=_vortexFloatParams.size())
		return(false); // should never happen
	if (theNewObj->_vortexIntParams.size()!=_vortexIntParams.size())
		return(false); // should never happen
	for (int i=0;i<int(_vortexFloatParams.size());i++)
	{
		if (theNewObj->_vortexFloatParams[i]!=_vortexFloatParams[i])
			return(false);
	}
	for (int i=0;i<int(_vortexIntParams.size());i++)
	{
		if ((i!=7)&&(theNewObj->_vortexIntParams[i]!=_vortexIntParams[i])) // at index 7 we have the unique ID that we want to ignore
			return(false);
	}
	if (theNewObj->_bulletStickyContact!=_bulletStickyContact)
		return(false);
	if (theNewObj->_bulletRestitution!=_bulletRestitution)
		return(false);
	if (theNewObj->_bulletFriction!=_bulletFriction)
		return(false);
	if (theNewObj->_bulletLinearDamping!=_bulletLinearDamping)
		return(false);
	if (theNewObj->_bulletAngularDamping!=_bulletAngularDamping)
		return(false);
	if (theNewObj->_bulletNonDefaultCollisionMarginFactor!=_bulletNonDefaultCollisionMarginFactor)
		return(false);
	if (theNewObj->_bulletNonDefaultCollisionMargin!=_bulletNonDefaultCollisionMargin)
		return(false);
	if (theNewObj->_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape!=_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape)
		return(false);
	if (theNewObj->_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape!=_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape)
		return(false);
	if (theNewObj->_bulletAutoShrinkConvexMesh!=_bulletAutoShrinkConvexMesh)
		return(false);
	if (theNewObj->_odeMaxContacts!=_odeMaxContacts)
		return(false);
	if (theNewObj->_odeFriction!=_odeFriction)
		return(false);
	if (theNewObj->_odeSoftERP!=_odeSoftERP)
		return(false);
	if (theNewObj->_odeSoftCFM!=_odeSoftCFM)
		return(false);
	if (theNewObj->_odeLinearDamping!=_odeLinearDamping)
		return(false);
	if (theNewObj->_odeAngularDamping!=_odeAngularDamping)
		return(false);
	if (theNewObj->_objectName.compare(_objectName)==0)
		return(true);

	// And finally: if the new object has a name such as "usr_XXX", where XXX is any positive integer, then
	// we don't compare the names (i.e. such a material will be merged with another material, even if the names are not same.
	// This is because when the material container was implemented (around 12/9/2013), a lot of materials had to be given automatic names. So to
	// avoid polluting the name space, those "automatic" names won't matter.
	std::string n(theNewObj->_objectName);
	if (n.size()<4)
		return(false);
	if (n.compare(0,4,"usr_")!=0)
		return(false);
	for (int i=4;i<int(n.length());i++)
	{
		if ((n[i]<'0')||(n[i]>'9'))
			return(false);
	}
	return(true);
}

CDynMaterialObject* CDynMaterialObject::copyYourself()
{
	CDynMaterialObject* newObj=new CDynMaterialObject();
	newObj->_objectID=_objectID;
	newObj->_objectName=_objectName;

	newObj->_bulletStickyContact=_bulletStickyContact;
	newObj->_bulletRestitution=_bulletRestitution;
	newObj->_bulletFriction=_bulletFriction;
	newObj->_bulletLinearDamping=_bulletLinearDamping;
	newObj->_bulletAngularDamping=_bulletAngularDamping;
	newObj->_bulletNonDefaultCollisionMarginFactor=_bulletNonDefaultCollisionMarginFactor;
	newObj->_bulletNonDefaultCollisionMargin=_bulletNonDefaultCollisionMargin;

	newObj->_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape=_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape;
	newObj->_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape;

	newObj->_bulletAutoShrinkConvexMesh=_bulletAutoShrinkConvexMesh;

	newObj->_odeMaxContacts=_odeMaxContacts;
	newObj->_odeFriction=_odeFriction;
	newObj->_odeSoftERP=_odeSoftERP;
	newObj->_odeSoftCFM=_odeSoftCFM;
	newObj->_odeLinearDamping=_odeLinearDamping;
	newObj->_odeAngularDamping=_odeAngularDamping;

	newObj->_vortexFloatParams.assign(_vortexFloatParams.begin(),_vortexFloatParams.end());
	newObj->_vortexIntParams.assign(_vortexIntParams.begin(),_vortexIntParams.end());
	newObj->_vortexIntParams[7]=newObj->_uniqueID;

	return(newObj);
}

void CDynMaterialObject::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Ipa");
		ar << _objectID;
		ar.flush();

		ar.storeDataName("Gon");
		ar << _objectName;
		ar.flush();

		ar.storeDataName("Bul");
		ar << _bulletRestitution << _bulletFriction << _bulletLinearDamping << _bulletAngularDamping << _bulletNonDefaultCollisionMarginFactor << _bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape;
		ar.flush();

		ar.storeDataName("Ode");
		ar << _odeMaxContacts << _odeFriction << _odeSoftERP << _odeSoftCFM << _odeLinearDamping << _odeAngularDamping;
		ar.flush();

		ar.storeDataName("Vo4"); // vortex params:
		ar << int(_vortexFloatParams.size()) << int(_vortexIntParams.size());
		for (int i=0;i<int(_vortexFloatParams.size());i++)
			ar << _vortexFloatParams[i];
		for (int i=0;i<int(_vortexIntParams.size());i++)
			ar << _vortexIntParams[i];
		ar.flush();

		ar.storeDataName("Var");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_bulletNonDefaultCollisionMargin);
		SIM_SET_CLEAR_BIT(nothing,1,_bulletStickyContact);
		SIM_SET_CLEAR_BIT(nothing,2,_bulletAutoShrinkConvexMesh);
		SIM_SET_CLEAR_BIT(nothing,3,_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape);
		ar << nothing;
		ar.flush();

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		bool vortexDataLoaded=false;
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Ipa")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectID;
				}
				if (theName.compare("Gon")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectName;
				}
				if (theName.compare("Bul")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _bulletRestitution >> _bulletFriction >> _bulletLinearDamping >> _bulletAngularDamping >> _bulletNonDefaultCollisionMarginFactor >> _bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape;
				}
				if (theName.compare("Ode")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _odeMaxContacts >> _odeFriction >> _odeSoftERP >> _odeSoftCFM >> _odeLinearDamping >> _odeAngularDamping;
				}
				/*
				if (theName.compare("Vor")==0)
				{ // Keep for backward compatibility (8/10/2013)
					noHit=false;
					ar >> byteQuantity;
					int cnt1,cnt2;
					ar >> cnt1 >> cnt2;
					float vf;
					int vi;
					for (int i=0;i<cnt1;i++)
					{ // new versions will always have same or more items in _vortexFloatParams already!
						ar >> vf;
						_vortexFloatParams[i]=vf;
					}
					for (int i=0;i<cnt2;i++)
					{ // new versions will always have same or more items in _vortexIntParams already!
						ar >> vi;
						_vortexIntParams[i]=vi;
					}
				}
				if (theName.compare("Vo2")==0)
				{ // Keep for backward compatibility (11/10/2013)
					noHit=false;
					ar >> byteQuantity;
					int cnt1,cnt2;
					ar >> cnt1 >> cnt2;
					float vf;
					int vi;
					for (int i=0;i<cnt1;i++)
					{ // new versions will always have same or more items in _vortexFloatParams already!
						ar >> vf;
						_vortexFloatParams[i]=vf;
					}
					for (int i=0;i<cnt2;i++)
					{ // new versions will always have same or more items in _vortexIntParams already!
						ar >> vi;
						_vortexIntParams[i]=vi;
					}
				}
				if (theName.compare("Vo3")==0)
				{ // vortex params:
					noHit=false;
					ar >> byteQuantity;
					int cnt1,cnt2;
					ar >> cnt1 >> cnt2;
					float vf;
					int vi;
					for (int i=0;i<cnt1;i++)
					{ // new versions will always have same or more items in _vortexFloatParams already!
						ar >> vf;
						_vortexFloatParams[i]=vf;
					}
					for (int i=0;i<cnt2;i++)
					{ // new versions will always have same or more items in _vortexIntParams already!
						ar >> vi;
						_vortexIntParams[i]=vi;
					}
				}
				*/
				if (theName.compare("Vo4")==0)
				{ // vortex params:
					noHit=false;
					ar >> byteQuantity;
					int cnt1,cnt2;
					ar >> cnt1 >> cnt2;
					float vf;
					int vi;
					for (int i=0;i<cnt1;i++)
					{ // new versions will always have same or more items in _vortexFloatParams already!
						ar >> vf;
						_vortexFloatParams[i]=vf;
					}
					for (int i=0;i<cnt2;i++)
					{ // new versions will always have same or more items in _vortexIntParams already!
						ar >> vi;
						_vortexIntParams[i]=vi;
					}
					vortexDataLoaded=true;
				}
				if (theName=="Var")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_bulletNonDefaultCollisionMargin=SIM_IS_BIT_SET(nothing,0);
					_bulletStickyContact=SIM_IS_BIT_SET(nothing,1);
					_bulletAutoShrinkConvexMesh=SIM_IS_BIT_SET(nothing,2);
					_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=SIM_IS_BIT_SET(nothing,3);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		if (!vortexDataLoaded)
		{ // keep for backward compatibility (16/10/2013)
			// Try to guess the friction we would need for Vortex:
			float averageFriction=0.0f;
			if (_bulletFriction>1.0f)
				averageFriction+=1.0f;
			else
				averageFriction+=_bulletFriction;
			if (_odeFriction>1.0f)
				averageFriction+=1.0f;
			else
				averageFriction+=_odeFriction;
			averageFriction*=0.5f;
			if (averageFriction<0.01f)
			{
				_vortexIntParams[0]=6;
				_vortexIntParams[1]=6;
			}
			else
			{
				_vortexFloatParams[0]=averageFriction;
				_vortexFloatParams[1]=averageFriction;
			}
		}
	}
}
