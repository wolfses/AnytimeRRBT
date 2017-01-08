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
#include "VisualParam.h"
#include "OGL.h"
#include "App.h"
#include "VDateTime.h"

CVisualParam::CVisualParam()
{
	setDefaultValues();
}

CVisualParam::~CVisualParam()
{

}

void CVisualParam::setFlash(bool flashIsOn)
{
	flash=flashIsOn;
}

bool CVisualParam::getFlash()
{
	return(flash);
}

void CVisualParam::setDefaultValues()
{
	setColorsAllBlack();
	setDiffuseAndSpecularToDefault();
	translucid=false;
	flash=false;
	useSimulationTime=false;
	flashFrequency=2.0f;
	flashRatio=0.5f;
	flashPhase=0.0f;
	transparencyFactor=0.5f;
	shininess=48;
	colorName="";
}

void CVisualParam::setColorsAllBlack()
{
	for (int i=0;i<12;i++)
		colors[i]=0.0f;
}
void CVisualParam::setDiffuseAndSpecularToDefault()
{
	for (int i=3;i<6;i++)
		colors[i]=0.25f;
	for (int i=6;i<9;i++)
		colors[i]=0.25f;
}

void CVisualParam::setColor(const float theColor[3],BYTE colorMode)
{
	int offset=0;
	if (colorMode==AMBIENT_MODE)
		offset=0;
	else if (colorMode==DIFFUSE_MODE)
		offset=3;
	else if (colorMode==SPECULAR_MODE)
		offset=6;
	else if (colorMode==EMISSION_MODE)
		offset=9;
	for (int i=0;i<3;i++)
		colors[offset+i]=theColor[i];
}
void CVisualParam::setColor(float r,float g,float b,BYTE colorMode)
{
	float col[3]={r,g,b};
	setColor(col,colorMode);
}

void CVisualParam::makeCurrentColor()
{
	makeCurrentColor(false);
}

void CVisualParam::makeCurrentColor(bool forceNonTransparent)
{
	if (flash)
	{
		float black[3]={0.0f,0.0f,0.0f};
		float t=0.0f;
		if (useSimulationTime&&(!App::ct->simulation->isSimulationStopped()))
			t=App::ct->simulation->getSimulationTime();
		if (!useSimulationTime)
			t=float(VDateTime::getTimeInMs())/1000.0f;
		t+=flashPhase/flashFrequency;
		t=CMath::robustFmod(t,1.0f/flashFrequency)*flashFrequency;
		if (t>(1.0f-flashRatio))
		{ // Flash is on
			if (true)//smoothFlash)
			{
				t=t-1.0f+flashRatio;
				t/=flashRatio;
				t=sin(t*piValue);
				float l=0.0f;
				float col0[12]={colors[0],colors[1],colors[2],colors[3],colors[4],colors[5],colors[6],colors[7],colors[8],0.0f,0.0f,0.0f};
				float col1[12]={colors[0]*l,colors[1]*l,colors[2]*l,colors[3]*l,colors[4]*l,colors[5]*l,colors[6]*l,colors[7]*l,colors[8]*l,colors[9],colors[10],colors[11]};
				for (int i=0;i<12;i++)
					col0[i]=col0[i]*(1.0f-t)+col1[i]*t;
				ogl::setColor(col0,shininess,transparencyFactor);
			}
//			else
//				ogl::setColor(black,black,black,colors+9,shininess,transparencyFactor);
		}
		else
			ogl::setColor(colors,colors+3,colors+6,black,shininess,transparencyFactor); // flash is off
	}
	else
		ogl::setColor(colors,shininess,transparencyFactor);
	if (translucid&&(!forceNonTransparent))
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	else
		ogl::setBlending(false);
}

void CVisualParam::copyYourselfInto(CVisualParam* it)
{ // all is true by default
	for (int i=0;i<12;i++)
		it->colors[i]=colors[i];
	it->flash=flash;
	it->useSimulationTime=useSimulationTime;
	it->flashFrequency=flashFrequency;
	it->flashRatio=flashRatio;
	it->flashPhase=flashPhase;
	it->shininess=shininess;
	it->transparencyFactor=transparencyFactor;
	it->translucid=translucid;
	it->colorName=colorName;
}

void CVisualParam::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing

		ar.storeDataName("Cls");
		for (int i=0;i<12;i++)
			ar << colors[i];
		ar.flush();

		ar.storeDataName("Sh2");
		ar << shininess;
		ar.flush();

		ar.storeDataName("Trf");
		ar << transparencyFactor;
		ar.flush();

		ar.storeDataName("Var");
		BYTE dummy=0;
//		FREE
//		FREE
		SIM_SET_CLEAR_BIT(dummy,2,!translucid);
//		FREE
		SIM_SET_CLEAR_BIT(dummy,4,flash);
		SIM_SET_CLEAR_BIT(dummy,5,useSimulationTime);
		ar << dummy;
		ar.flush();

		ar.storeDataName("Fi3");
		ar << flashFrequency << flashRatio << flashPhase;
		ar.flush();

		ar.storeDataName("Cnm");
		ar << colorName;
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
				if (theName.compare("Cls")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					for (int i=0;i<12;i++)
						ar >> colors[i];
				}
				if (theName.compare("Shi")==0)
				{ // for backward compatibility (around January-March 2012?)
					noHit=false;
					ar >> byteQuantity;
					ar >> shininess;
					if (shininess<48)
						shininess=48;
				}
				if (theName.compare("Sh2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> shininess;
				}
				if (theName.compare("Trf")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> transparencyFactor;
				}
				if (theName.compare("Var")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					translucid=!SIM_IS_BIT_SET(dummy,2);
					flash=SIM_IS_BIT_SET(dummy,4);
					useSimulationTime=SIM_IS_BIT_SET(dummy,5);
				}
				if (theName.compare("Fi3")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> flashFrequency >> flashRatio >> flashPhase;
				}
				if (theName.compare("Cnm")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> colorName;
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
