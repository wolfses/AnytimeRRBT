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
#include "GraphDataComb.h"
#include "Global.h"
#include "Graph.h"
#include "Tt.h"
#include "OGL.h"
#include "App.h"
#include "IloIlo.h"

CGraphDataComb::CGraphDataComb()
{
	name="Curve";
	visible=true;
	visibleOnTopOfEverything=false;
	threeDCurveWidth=1.0f;
	data[0]=-1;
	data[1]=-1;
	data[2]=-1;
	ambientColor[0]=ambientColor[1]=1.0f;
	ambientColor[2]=0.0f;
	emissiveColor[0]=emissiveColor[1]=emissiveColor[2]=0.0f;
	linkPoints=true;
	label=true;
	_curveRelativeToWorld=false;
	_lifeID=-1;
}

CGraphDataComb::~CGraphDataComb()
{
	CLifeControl::announceDeath(_lifeID);
}

int CGraphDataComb::getLifeID()
{
	if (_lifeID==-1)
		_lifeID=CLifeControl::getLifeID();
	return(_lifeID);
}

void CGraphDataComb::setLinkPoints(bool l)
{
	linkPoints=l;
}
void CGraphDataComb::setLabel(bool l)
{
	label=l;
}
bool CGraphDataComb::getLabel() 
{
	return(label); 
}

bool CGraphDataComb::getLinkPoints() 
{
	return(linkPoints); 
}
int CGraphDataComb::getIdentifier() 
{
	return(identifier); 
}
bool CGraphDataComb::getVisible() 
{
	return(visible); 
}
void CGraphDataComb::setVisible(bool v)
{
	visible=v;
}

void CGraphDataComb::setCurveRelativeToWorld(bool relToWorld)
{
	_curveRelativeToWorld=relToWorld;
}

bool CGraphDataComb::getCurveRelativeToWorld()
{
	return(_curveRelativeToWorld);
}

void CGraphDataComb::setVisibleOnTopOfEverything(bool v)
{
	visibleOnTopOfEverything=v;
}

bool CGraphDataComb::getVisibleOnTopOfEverything()
{
	return(visibleOnTopOfEverything);
}

void CGraphDataComb::set3DCurveWidth(float width)
{
	tt::limitValue(1.0f,6.0f,width);
	threeDCurveWidth=width;
}

float CGraphDataComb::get3DCurveWidth()
{
	return(threeDCurveWidth);
}

void CGraphDataComb::setIdentifier(int newIdentifier)
{
	identifier=newIdentifier;
}
void CGraphDataComb::setName(std::string theName)
{
	name=theName;
}
std::string CGraphDataComb::getName() 
{ 
	return(name); 
}
CGraphDataComb* CGraphDataComb::copyYourself()
{	// Everything is copied, even the identifier and name
	CGraphDataComb* newObj=new CGraphDataComb();
	newObj->setName(getName());
	newObj->setIdentifier(getIdentifier());
	newObj->setVisible(getVisible());
	newObj->setVisibleOnTopOfEverything(getVisibleOnTopOfEverything());
	newObj->set3DCurveWidth(get3DCurveWidth());
	newObj->setLinkPoints(getLinkPoints());
	newObj->setLabel(getLabel());
	newObj->setCurveRelativeToWorld(getCurveRelativeToWorld());
	for (int i=0;i<3;i++)
	{
		newObj->data[i]=data[i];
		newObj->ambientColor[i]=ambientColor[i];
		newObj->emissiveColor[i]=emissiveColor[i];
	}
	return(newObj);
}

void CGraphDataComb::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Nme");
		ar << name;
		ar.flush();

		ar.storeDataName("Oid");
		ar << identifier;
		ar.flush();

		ar.storeDataName("Col");
		ar << ambientColor[0] << ambientColor[1] << ambientColor[2];
		ar << emissiveColor[0] << emissiveColor[1] << emissiveColor[2];
		ar.flush();

		ar.storeDataName("Did");
		ar << data[0] << data[1] << data[2];
		ar.flush();

		ar.storeDataName("3dw");
		ar << threeDCurveWidth;
		ar.flush();


		ar.storeDataName("Par");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,visible);
		SIM_SET_CLEAR_BIT(nothing,1,linkPoints);
		SIM_SET_CLEAR_BIT(nothing,2,label);
		SIM_SET_CLEAR_BIT(nothing,3,visibleOnTopOfEverything);
		SIM_SET_CLEAR_BIT(nothing,4,_curveRelativeToWorld);
		ar << nothing;
		ar.flush();

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
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
					ar >> name;
				}
				if (theName.compare("Oid")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> identifier;
				}
				if (theName.compare("Col")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> ambientColor[0] >> ambientColor[1] >> ambientColor[2];
					ar >> emissiveColor[0] >> emissiveColor[1] >> emissiveColor[2];
				}
				if (theName.compare("Did")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> data[0] >> data[1] >> data[2];
				}
				if (theName=="3dw")
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> threeDCurveWidth;
				}
				if (theName=="Par")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					visible=SIM_IS_BIT_SET(nothing,0);
					linkPoints=SIM_IS_BIT_SET(nothing,1);
					label=SIM_IS_BIT_SET(nothing,2);
					visibleOnTopOfEverything=SIM_IS_BIT_SET(nothing,3);
					_curveRelativeToWorld=SIM_IS_BIT_SET(nothing,4);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
