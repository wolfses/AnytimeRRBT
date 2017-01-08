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

#include "Ser.h"
#include <vector>

class CDynMaterialObject
{
public:
	CDynMaterialObject();
	virtual ~CDynMaterialObject();

	void setObjectID(int newID);
	int getObjectID();
	bool isSame(CDynMaterialObject* theNewObj);
	void setObjectName(const char* newName);
	void tagAsDefault();
	bool isDefaultObject();
	bool isDefaultRestStackGraspMaterial();
	void setDefaultRestStackGraspMaterial(bool d);

	void setUsedTag(bool used);
	bool getUsedTag();
	void setEditableTag(bool editable);
	bool getEditableTag();

	std::string getObjectName();
	CDynMaterialObject* copyYourself();
	void serialize(CSer& ar);

	// Engine properties
	// ---------------------
	void setBulletStickyContact(bool sticky);
	bool getBulletStickyContact();
	void setBulletRestitution(float r);
	float getBulletRestitution();
	void setBulletFriction(float f);
	float getBulletFriction();
	void setBulletLinearDamping(float d);
	float getBulletLinearDamping();
	void setBulletAngularDamping(float d);
	float getBulletAngularDamping();
	void setBulletNonDefaultCollisionMargin(bool for_convex_param,bool b);
	bool getBulletNonDefaultCollisionMargin(bool for_convex_param);
	void setBulletNonDefaultCollisionMarginFactor(bool for_convex_param,float m);
	float getBulletNonDefaultCollisionMarginFactor(bool for_convex_param);
	void setBulletAutoShrinkConvexMesh(bool s);
	bool getBulletAutoShrinkConvexMesh();

	void setOdeMaxContacts(int m);
	int getOdeMaxContacts();
	void setOdeFriction(float f);
	float getOdeFriction();
	void setOdeSoftERP(float e);
	float getOdeSoftERP();
	void setOdeSoftCFM(float e);
	float getOdeSoftCFM();
	void setOdeLinearDamping(float d);
	float getOdeLinearDamping();
	void setOdeAngularDamping(float d);
	float getOdeAngularDamping();

	void getVortexFloatParams(std::vector<float>& p);
	void setVortexFloatParams(const std::vector<float>& p);
	void getVortexIntParams(std::vector<int>& p);
	void setVortexIntParams(const std::vector<int>& p);
	// ---------------------

	
protected:
	static int _nextUniqueID;

	// following 3 are not copied nor serialized:
	bool _usedTag;
	bool _editableTag;
	bool _defaultObject;
	bool _defaultRestStackGraspMaterial;

	int _objectID;
	std::string _objectName;
	int _uniqueID;


	// Engine properties
	// ---------------------
	bool _bulletStickyContact;
	float _bulletRestitution;
	float _bulletFriction;
	float _bulletLinearDamping;
	float _bulletAngularDamping;
	float _bulletNonDefaultCollisionMarginFactor;
	bool _bulletNonDefaultCollisionMargin;
	float _bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape;
	bool _bulletNonDefaultCollisionMargin_forConvexAndNonPureShape;
	bool _bulletAutoShrinkConvexMesh;

	int _odeMaxContacts;
	float _odeFriction;
	float _odeSoftERP;
	float _odeSoftCFM;
	float _odeLinearDamping;
	float _odeAngularDamping;

	std::vector<float> _vortexFloatParams;
	std::vector<int> _vortexIntParams;
	// ---------------------

};
