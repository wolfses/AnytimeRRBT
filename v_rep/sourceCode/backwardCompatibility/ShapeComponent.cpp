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

#include "vrepPrecompiledHeader.h"
#include "ShapeComponent.h"
#include "Tt.h"
#include "GeomObject.h"
#include "v_rep_internal.h"
#include "geometric.h"
#include "IloIlo.h"
#include "ttUtil.h"
#include "App.h"

CShapeComponent_OLD::CShapeComponent_OLD()
{
	color.setDefaultValues();
	edgeColor.setColorsAllBlack();
	insideColor.setDefaultValues();

	_mass=1.0f;

	_bulletStickyContact=false;
	_bulletRestitution=0.0f;
	_bulletFriction=1.0f;
	_bulletLinearDamping=0.0f;
	_bulletAngularDamping=0.0f;

	_bulletNonDefaultCollisionMarginFactor=0.1f;
	_bulletNonDefaultCollisionMargin=false;

	_odeMaxContacts=8;
	_odeFriction=1.0f;
	_odeSoftERP=0.2f;
	_odeSoftCFM=0.0f;
	_odeLinearDamping=0.0f;
	_odeAngularDamping=0.0f;

	_localInertiaFrame.setIdentity();
	_principalMomentsOfInertia(0)=0.001f;
	_principalMomentsOfInertia(1)=0.001f;
	_principalMomentsOfInertia(2)=0.001f;

	_purePrimitive=sim_pure_primitive_none;
	_purePrimitiveXSizeOrDiameter=0.0f;
	_purePrimitiveYSize=0.0f;
	_purePrimitiveZSizeOrHeight=0.0f;

	_heightfieldXCount=0;
	_heightfieldYCount=0;

	_purePrimitiveConfigurationRelativeToMultishapeFrame.setIdentity();
	_textureProperty=NULL;

	_edgeWidth=1;
	_name="sub__0";
	_level=0;
	vertStart=0;
	vertRange=0;
	triStart=0;
	triRange=0;
	_visibleEdges=false;
	_culling=false;
	_wireframe=false;
	_insideAndOutsideFacesSameColor=true;
	_gouraudShadingAngle=0.5f*degToRad;
	renderingOrder=0;
}

CShapeComponent_OLD::~CShapeComponent_OLD()
{
	for (int i=0;i<int(childrenList.size());i++)
		delete childrenList[i];
	delete _textureProperty;
}

void CShapeComponent_OLD::buildEquivalentGeometricsAndInsert(bool firstItem,CGeomObject_OLD* geomObj,CGeomWrap* geomWrap)
{ // Routine needed for the transition to the new shape-Geom architecture (November 2012). This is only needed when loading files with serialization version 15 or lower
	// Before we had: CShape-CGeomObject_OLD-CShapeComponent_OLD
	// Now we have: CShape-CGeomProxy-CGeomWrap/CGeometric

	// This routine only used for multishape CShapeComponents!!! (i.e. they don't have texture or heightfield infos)

	CGeometric* geom=new CGeometric();

	color.copyYourselfInto(&geom->color);
	insideColor.copyYourselfInto(&geom->insideColor);
	edgeColor.copyYourselfInto(&geom->edgeColor);
	geom->setVisibleEdges(_visibleEdges);
	geom->setCulling(_culling);
	geom->setInsideAndOutsideFacesSameColor(_insideAndOutsideFacesSameColor);
	geom->setWireframe(_wireframe);
	geom->setEdgeWidth(_edgeWidth);
	geom->setGouraudShadingAngle(_gouraudShadingAngle);
	geom->setEdgeThresholdAngle(_gouraudShadingAngle);
	geom->setName(_name);
	geom->setPurePrimitiveType(_purePrimitive,_purePrimitiveXSizeOrDiameter,_purePrimitiveYSize,_purePrimitiveZSizeOrHeight);

	bool isPure=(_purePrimitive!=sim_pure_primitive_none);
	if (isPure)
		geom->setVerticeLocalFrame(_purePrimitiveConfigurationRelativeToMultishapeFrame);

	// cut out the correct vertices, indices, normals and visible edges:
	std::vector<float> vert;
	std::vector<int> ind;
	std::vector<float> norm;
	std::vector<unsigned char> ed;
	for (int i=vertStart;i<vertStart+vertRange;i++)
	{
		C3Vector v(&geomObj->visibleVertices[3*i]);
		if (isPure)
			v*=_purePrimitiveConfigurationRelativeToMultishapeFrame.getInverse();
		vert.push_back(v(0));
		vert.push_back(v(1));
		vert.push_back(v(2));
	}
	for (int i=triStart;i<triStart+triRange;i++)
	{
		ind.push_back(geomObj->visibleIndices[3*i+0]-vertStart);
		ind.push_back(geomObj->visibleIndices[3*i+1]-vertStart);
		ind.push_back(geomObj->visibleIndices[3*i+2]-vertStart);
	}
	for (int i=9*triStart;i<(triStart+triRange)*9;i++)
		norm.push_back(geomObj->visibleNormals[i]);

	ed.resize((ind.size()/8)+1,0);
	for (int i=3*triStart;i<(triStart+triRange)*3;i++)
	{
		int j=i-3*triStart;
		if ( ( (geomObj->visibleEdges[i>>3]&(1<<(i&7)))!=0) )
			ed[j>>3]|=(1<<(j&7));
	}
	//printf("Vert, ind, norm: %i, %i, %i\n",vert.size(),ind.size(),norm.size()); //XXAA
	geom->setMeshDataDirect(vert,ind,norm,ed);

	geom->setMass(_mass);

	geom->__bulletStickyContact=_bulletStickyContact;
	geom->__bulletRestitution=_bulletRestitution;
	geom->__bulletFriction=_bulletFriction;
	geom->__bulletLinearDamping=_bulletLinearDamping;
	geom->__bulletAngularDamping=_bulletAngularDamping;
	geom->__bulletNonDefaultCollisionMargin=_bulletNonDefaultCollisionMargin;
	geom->__bulletNonDefaultCollisionMarginFactor=_bulletNonDefaultCollisionMarginFactor;

	geom->__odeMaxContacts=_odeMaxContacts;
	geom->__odeFriction=_odeFriction;
	geom->__odeSoftCFM=_odeSoftCFM;
	geom->__odeSoftERP=_odeSoftERP;
	geom->__odeLinearDamping=_odeLinearDamping;
	geom->__odeAngularDamping=_odeAngularDamping;

	geom->setPrincipalMomentsOfInertia(_principalMomentsOfInertia);
	geom->setLocalInertiaFrame(_purePrimitiveConfigurationRelativeToMultishapeFrame*_localInertiaFrame); 
	geom->setTransformationsSinceGrouping(_purePrimitiveConfigurationRelativeToMultishapeFrame);

	geomWrap->childList.push_back(geom);

	// Now the children:
	for (int i=0;i<int(childrenList.size());i++)
		childrenList[i]->buildEquivalentGeometricsAndInsert(false,geomObj,geomWrap);
}

int CShapeComponent_OLD::getPurePrimitiveType()
{
	return(_purePrimitive);
}

C7Vector CShapeComponent_OLD::getLocalInertiaFrame()
{
	return (_localInertiaFrame);
}

C3Vector CShapeComponent_OLD::getPrincipalMomentsOfInertia()
{
	return (_principalMomentsOfInertia);
}

bool CShapeComponent_OLD::getInsideAndOutsideFacesSameColor()
{
	return(_insideAndOutsideFacesSameColor);
}

CTextureProperty* CShapeComponent_OLD::getTextureProperty()
{
	return(_textureProperty);
}

void CShapeComponent_OLD::setTextureProperty(CTextureProperty* tp)
{
	_textureProperty=tp;
}

float* CShapeComponent_OLD::getHeightfieldData(int& xCount,int& yCount,float& minHeight,float& maxHeight)
{
	if ( (_purePrimitive!=sim_pure_primitive_heightfield)||(_heightfieldHeights.size()==0) )
		return(NULL);
	xCount=_heightfieldXCount;
	yCount=_heightfieldYCount;
	minHeight=+99999999.0f;
	maxHeight=-99999999.0f;
	for (int i=0;i<int(_heightfieldHeights.size());i++)
	{
		if (_heightfieldHeights[i]<minHeight)
			minHeight=_heightfieldHeights[i];
		if (_heightfieldHeights[i]>maxHeight)
			maxHeight=_heightfieldHeights[i];
	}
	return(&_heightfieldHeights[0]);
}

void CShapeComponent_OLD::getPurePrimitiveSizes(C3Vector& s)
{
	s(0)=_purePrimitiveXSizeOrDiameter;
	s(1)=_purePrimitiveYSize;
	s(2)=_purePrimitiveZSizeOrHeight;
}

int CShapeComponent_OLD::getEdgeWidth()
{
	return(_edgeWidth);
}

float CShapeComponent_OLD::getGouraudShadingAngle()
{
	return(_gouraudShadingAngle);
}
bool CShapeComponent_OLD::getVisibleEdges()
{
	return(_visibleEdges);
}

bool CShapeComponent_OLD::getCulling()
{
	return(_culling);
}
bool CShapeComponent_OLD::getWireframe()
{
	return(_wireframe);
}

std::string CShapeComponent_OLD::getName()
{
	return(_name);
}


float CShapeComponent_OLD::getMass()
{
	return(_mass);
}

float CShapeComponent_OLD::getBulletRestitution()
{
	return(_bulletRestitution);
}

float CShapeComponent_OLD::getBulletFriction()
{
	return(_bulletFriction);
}

bool CShapeComponent_OLD::getBulletStickyContact()
{
	return(_bulletStickyContact);
}

float CShapeComponent_OLD::getBulletLinearDamping()
{
	return(_bulletLinearDamping);
}

float CShapeComponent_OLD::getBulletAngularDamping()
{
	return(_bulletAngularDamping);
}

bool CShapeComponent_OLD::getBulletNonDefaultCollisionMargin()
{
	return(_bulletNonDefaultCollisionMargin);
}

float CShapeComponent_OLD::getBulletNonDefaultCollisionMarginFactor()
{
	return(_bulletNonDefaultCollisionMarginFactor);
}

int CShapeComponent_OLD::getOdeMaxContacts()
{
	return(_odeMaxContacts);
}

float CShapeComponent_OLD::getOdeFriction()
{
	return(_odeFriction);
}

float CShapeComponent_OLD::getOdeSoftERP()
{
	return(_odeSoftERP);
}

float CShapeComponent_OLD::getOdeSoftCFM()
{
	return(_odeSoftCFM);
}

float CShapeComponent_OLD::getOdeLinearDamping()
{
	return(_odeLinearDamping);
}

bool CShapeComponent_OLD::isGrouping()
{
	return(childrenList.size()!=0);
}

float CShapeComponent_OLD::getOdeAngularDamping()
{
	return(_odeAngularDamping);
}


void CShapeComponent_OLD::serialize(CSer& ar)
{
	if (!ar.isStoring())
	{		// Loading
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Nme")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _name;
				}
				if (theName.compare("Cl0")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					color.serialize(ar);
				}
				if (theName.compare("Cl2")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					insideColor.serialize(ar);
				}
				if (theName.compare("Ecl")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					edgeColor.serialize(ar);
					edgeColor.colors[3]=0.0f;
					edgeColor.colors[4]=0.0f;
					edgeColor.colors[5]=0.0f;
					edgeColor.colors[6]=0.0f;
					edgeColor.colors[7]=0.0f;
					edgeColor.colors[8]=0.0f;
				}
				if (theName.compare("Vrt")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> vertStart >> vertRange;
				}
				if (theName.compare("Tri")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> triStart >> triRange;
				}
				if (theName.compare("Lvl")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _level;
				}
				if (theName.compare("Rdo")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> renderingOrder;
				}
				if (theName.compare("Mas")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _mass;
				}
				if (theName.compare("Dyp")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _bulletRestitution >> _bulletFriction >> _bulletLinearDamping >> _bulletAngularDamping;
				}
				if (theName.compare("Ppr")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _purePrimitive >> _purePrimitiveXSizeOrDiameter >> _purePrimitiveYSize >> _purePrimitiveZSizeOrHeight;
				}
				if (theName.compare("Ine")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _localInertiaFrame(0) >> _localInertiaFrame(1) >> _localInertiaFrame(2) >> _localInertiaFrame(3);
					ar >> _localInertiaFrame(4) >> _localInertiaFrame(5) >> _localInertiaFrame(6);
					ar >> _principalMomentsOfInertia(0) >> _principalMomentsOfInertia(1) >> _principalMomentsOfInertia(2);
				}
				if (theName.compare("Ppf")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _purePrimitiveConfigurationRelativeToMultishapeFrame(0) >> _purePrimitiveConfigurationRelativeToMultishapeFrame(1) >> _purePrimitiveConfigurationRelativeToMultishapeFrame(2) >> _purePrimitiveConfigurationRelativeToMultishapeFrame(3);
					ar >> _purePrimitiveConfigurationRelativeToMultishapeFrame(4) >> _purePrimitiveConfigurationRelativeToMultishapeFrame(5) >> _purePrimitiveConfigurationRelativeToMultishapeFrame(6);
				}
				if (theName.compare("Gsa")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _gouraudShadingAngle;
				}
				if (theName.compare("Ewt")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _edgeWidth;
				}
				if (theName=="Ved")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_visibleEdges=SIM_IS_BIT_SET(nothing,0);
					_culling=SIM_IS_BIT_SET(nothing,1);
					_wireframe=SIM_IS_BIT_SET(nothing,2);
					_bulletNonDefaultCollisionMargin=SIM_IS_BIT_SET(nothing,3);
					_bulletStickyContact=SIM_IS_BIT_SET(nothing,4);
					_insideAndOutsideFacesSameColor=!SIM_IS_BIT_SET(nothing,5);
				}
				if (theName.compare("Shc")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					CShapeComponent_OLD* it=new CShapeComponent_OLD();
					it->serialize(ar);
					childrenList.push_back(it);
				}
				if (theName.compare("Toj")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					_textureProperty=new CTextureProperty();
					_textureProperty->serialize(ar);
				}
				if (theName.compare("Od1")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _odeMaxContacts >> _odeFriction >> _odeSoftERP >> _odeSoftCFM >> _odeLinearDamping >> _odeAngularDamping;
				}
				if (theName.compare("Bcm")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _bulletNonDefaultCollisionMarginFactor;
				}
				if (theName.compare("Hfd")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _heightfieldXCount >> _heightfieldYCount;
					for (int i=0;i<_heightfieldXCount*_heightfieldYCount;i++)
					{
						float dummy;
						ar >> dummy;
						_heightfieldHeights.push_back(dummy);
					}
					if (_heightfieldHeights.size()!=0)
						_purePrimitive=sim_pure_primitive_heightfield;
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			CTTUtil::scaleColorUp_(color.colors);
			CTTUtil::scaleColorUp_(insideColor.colors);
			CTTUtil::scaleColorUp_(edgeColor.colors);
		}
	}
}
