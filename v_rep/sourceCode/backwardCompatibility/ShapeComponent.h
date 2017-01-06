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

// This is needed only to be backward compatible with serialization version 15 (Serialization version 16 since Nov. 2012)

#pragma once

#include "VisualParam.h"
#include "TextureProperty.h"
#include "geomWrap.h"

class CGeomObject_OLD;

class CShapeComponent_OLD  
{
public:
	CShapeComponent_OLD();
	virtual ~CShapeComponent_OLD();

	bool getInsideAndOutsideFacesSameColor();
	CTextureProperty* getTextureProperty();
	void setTextureProperty(CTextureProperty* tp);
	bool getVisibleEdges();
	int getEdgeWidth();
	bool getCulling();
	bool getWireframe();
	float getGouraudShadingAngle();
	std::string getName();
	float getMass();
	bool getBulletStickyContact();
	float getBulletRestitution();
	float getBulletFriction();
	float getBulletLinearDamping();
	float getBulletAngularDamping();
	bool getBulletNonDefaultCollisionMargin();
	float getBulletNonDefaultCollisionMarginFactor();
	int getOdeMaxContacts();
	float getOdeFriction();
	float getOdeSoftERP();
	float getOdeSoftCFM();
	float getOdeLinearDamping();
	float getOdeAngularDamping();
	bool isGrouping();

	float* getHeightfieldData(int& xCount,int& yCount,float& minHeight,float& maxHeight);
	int getPurePrimitiveType();
	void getPurePrimitiveSizes(C3Vector& s);
	C7Vector getLocalInertiaFrame();
	C3Vector getPrincipalMomentsOfInertia();

	void buildEquivalentGeometricsAndInsert(bool firstItem,CGeomObject_OLD* geomObj,CGeomWrap* geomWrap);
	void serialize(CSer& ar);

	CVisualParam color;
	CVisualParam insideColor;
	CVisualParam edgeColor;
	int vertStart;
	int vertRange;
	int triStart;
	int triRange;
	int renderingOrder;

	std::vector<CShapeComponent_OLD*> childrenList;

	std::vector<float> _heightfieldHeights;
	int _heightfieldXCount;
	int _heightfieldYCount;

	C7Vector _purePrimitiveConfigurationRelativeToMultishapeFrame;

private:
	bool _visibleEdges;
	bool _culling;
	bool _insideAndOutsideFacesSameColor;
	bool _wireframe;
	int _edgeWidth;
	float _gouraudShadingAngle;
	std::string _name;

	int _purePrimitive;
	float _purePrimitiveXSizeOrDiameter;
	float _purePrimitiveYSize;
	float _purePrimitiveZSizeOrHeight;
	float _mass;

	bool _bulletStickyContact;
	float _bulletRestitution;
	float _bulletFriction;
	float _bulletLinearDamping;
	float _bulletAngularDamping;
	float _bulletNonDefaultCollisionMarginFactor;
	bool _bulletNonDefaultCollisionMargin;

	int _odeMaxContacts;
	float _odeFriction;
	float _odeSoftERP;
	float _odeSoftCFM;
	float _odeLinearDamping;
	float _odeAngularDamping;

	C7Vector _localInertiaFrame;
	C3Vector _principalMomentsOfInertia;
	CTextureProperty* _textureProperty;
	BYTE _level;
};
