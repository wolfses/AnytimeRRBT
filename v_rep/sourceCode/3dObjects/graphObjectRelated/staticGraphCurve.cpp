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
#include "staticGraphCurve.h"
#include "IloIlo.h"

CStaticGraphCurve::CStaticGraphCurve()
{
}

CStaticGraphCurve::CStaticGraphCurve(int type,std::vector<float>* xVals,std::vector<float>* yVals,std::vector<float>* zVals)
{	
	_curveType=type;
	_linkPoints=true;
	_label=true;
	color[0]=1.0f;
	color[1]=color[2]=0.0f;
	_name="StaticCurve";
	_curveWidth=1.0f;
	_relativeToWorld=false;
	for (int i=0;i<int(xVals->size());i++)
	{
		values.push_back((*xVals)[i]);
		values.push_back((*yVals)[i]);
		if (zVals!=NULL)
			values.push_back((*zVals)[i]);
	}
}

CStaticGraphCurve::~CStaticGraphCurve()
{
}

void CStaticGraphCurve::setLabel(bool l)
{
	_label=l;
}

bool CStaticGraphCurve::getLabel()
{ 
	return(_label);
}

bool CStaticGraphCurve::getLinkPoints()
{
	return(_linkPoints);
}

int CStaticGraphCurve::getCurveType()
{
	return(_curveType);
}

void CStaticGraphCurve::setCurveWidth(float w)
{
	_curveWidth=w;
}

float CStaticGraphCurve::getCurveWidth()
{
	return(_curveWidth);
}

void CStaticGraphCurve::setRelativeToWorld(bool r)
{
	_relativeToWorld=r;
}

bool CStaticGraphCurve::getRelativeToWorld()
{
	return(_relativeToWorld);
}

void CStaticGraphCurve::setLinkPoints(bool l)
{
	_linkPoints=l;
}

void CStaticGraphCurve::setName(std::string theName)
{
	_name=theName;
}

std::string CStaticGraphCurve::getName() 
{ 
	return(_name); 
}

CStaticGraphCurve* CStaticGraphCurve::copyYourself()
{	// We copy everything
	CStaticGraphCurve* newObj=new CStaticGraphCurve();
	newObj->_curveType=_curveType;
	newObj->color[0]=color[0];
	newObj->color[1]=color[1];
	newObj->color[2]=color[2];
	newObj->_linkPoints=_linkPoints;
	newObj->_label=_label;
	newObj->_name=_name;
	newObj->_curveWidth=_curveWidth;
	newObj->_relativeToWorld=_relativeToWorld;
	for (int i=0;i<int(values.size());i++)
		newObj->values.push_back(values[i]);
	return(newObj);
}

void CStaticGraphCurve::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Ilo");
		ar << _curveType << color[0] << color[1] << color[2] << _name << _curveWidth;
		ar.flush();

		ar.storeDataName("Par");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_linkPoints);
		SIM_SET_CLEAR_BIT(nothing,1,_label);
		SIM_SET_CLEAR_BIT(nothing,2,_relativeToWorld);
		ar << nothing;
		ar.flush();

		ar.storeDataName("Val");
		ar << int(values.size());
		for (int i=0;i<int(values.size());i++)
			ar << values[i];
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
				if (theName.compare("Ilo")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _curveType >> color[0] >> color[1] >> color[2] >> _name >> _curveWidth;
				}
				if (theName=="Par")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_linkPoints=SIM_IS_BIT_SET(nothing,0);
					_label=SIM_IS_BIT_SET(nothing,1);
					_relativeToWorld=SIM_IS_BIT_SET(nothing,2);
				}
				if (theName=="Val")
				{
					noHit=false;
					ar >> byteQuantity;
					int count;
					ar >> count;
					values.clear();
					for (int i=0;i<count;i++)
					{
						float dummy;
						ar >> dummy;
						values.push_back(dummy);
					}
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
