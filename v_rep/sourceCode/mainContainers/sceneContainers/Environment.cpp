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
#include "Environment.h"
#include "OGL.h"
#include "Tt.h"
#include "Global.h"
#include "IloIlo.h"

#include "FileMenu.h"
#include "ObjectEdition.h"
#include "VDateTime.h"
#include "ttUtil.h"
#include "App.h"

int CEnvironment::_nextSceneUniqueID=0;

CEnvironment::CEnvironment()
{
	setUpDefaultValues();
}

CEnvironment::~CEnvironment()
{
}

void CEnvironment::setSceneCanBeDiscardedWhenNewSceneOpened(bool canBeDiscarded)
{
	_sceneCanBeDiscardedWhenNewSceneOpened=canBeDiscarded;
}

bool CEnvironment::getSceneCanBeDiscardedWhenNewSceneOpened()
{
	return(_sceneCanBeDiscardedWhenNewSceneOpened);
}

void CEnvironment::setUpDefaultValues()
{
	fogEnabled=false;
	fogDensity=0.5f;
	fogStart=0.0f;
	fogEnd=5.0f;
	fogType=0; // 0=linear, 1=exp, 2=exp2

	_sceneCanBeDiscardedWhenNewSceneOpened=false;
	autoSaveLastSaveTimeInSecondsSince1970=VDateTime::getSecondsSince1970();

	backGroundColor[0]=0.72f;
	backGroundColor[1]=0.81f;
	backGroundColor[2]=0.88f;
	backGroundColorDown[0]=0.05f;
	backGroundColorDown[1]=0.05f;
	backGroundColorDown[2]=0.1f;

	fogBackgroundColor[0]=0.0f;
	fogBackgroundColor[1]=0.0f;
	fogBackgroundColor[2]=0.0f;

	ambientLightColor[0]=0.2f;
	ambientLightColor[1]=0.2f;
	ambientLightColor[2]=0.2f;

	wirelessEmissionVolumeColor.setColorsAllBlack();
	wirelessEmissionVolumeColor.setColor(1.0f,1.0f,0.0f,EMISSION_MODE);
	wirelessEmissionVolumeColor.translucid=true;
	wirelessEmissionVolumeColor.transparencyFactor=0.2f;
	wirelessTransmissionColor.setColorsAllBlack();
	wirelessTransmissionColor.setColor(1.0f,1.0f,0.0f,EMISSION_MODE);

	sceneHistory.clear();
	_acknowledgement="";
	_visualizeWirelessEmitters=false;
	_visualizeWirelessReceivers=false;
	_requestFinalSave=false;
	_sceneIsLocked=false;
	_shapeTexturesEnabled=true;
	_showingInertias=false;
	_2DElementTexturesEnabled=true;
	_calculationMaxTriangleSize=0.3f; // from 0.8 to 0.3 on 2010/07/07
	_calculationMinRelTriangleSize=0.02f;
	_saveExistingCalculationStructures=false;
	_useCustomContactHandlingViaScript=false;
	_sceneUniqueID=_nextSceneUniqueID++;
}

int CEnvironment::getSceneUniqueID()
{
	return(_sceneUniqueID);
}

void CEnvironment::setSaveExistingCalculationStructures(bool s)
{
	_saveExistingCalculationStructures=s;
}

bool CEnvironment::getSaveExistingCalculationStructures()
{
	return(_saveExistingCalculationStructures);
}

void CEnvironment::setSaveExistingCalculationStructuresTemp(bool s)
{
	_saveExistingCalculationStructuresTemp=s;
}

bool CEnvironment::getSaveExistingCalculationStructuresTemp()
{
	return(_saveExistingCalculationStructuresTemp);
}

void CEnvironment::setCalculationMaxTriangleSize(float s)
{
	_calculationMaxTriangleSize=tt::getLimitedFloat(0.01f,100.0f,s);
}

float CEnvironment::getCalculationMaxTriangleSize()
{
	return(_calculationMaxTriangleSize);
}

void CEnvironment::setCalculationMinRelTriangleSize(float s)
{
	_calculationMinRelTriangleSize=tt::getLimitedFloat(0.001f,1.0f,s);
}

float CEnvironment::getCalculationMinRelTriangleSize()
{
	return(_calculationMinRelTriangleSize);
}

void CEnvironment::setShapeTexturesEnabled(bool e)
{
	_shapeTexturesEnabled=e;
}
bool CEnvironment::getShapeTexturesEnabled()
{
	return(_shapeTexturesEnabled);
}

void CEnvironment::setShowingInertias(bool e)
{
	_showingInertias=e;
}
bool CEnvironment::getShowingInertias()
{
	return(_showingInertias);
}



void CEnvironment::set2DElementTexturesEnabled(bool e)
{
	_2DElementTexturesEnabled=e;
}
bool CEnvironment::get2DElementTexturesEnabled()
{
	return(_2DElementTexturesEnabled);
}

void CEnvironment::setRequestFinalSave(bool finalSaveActivated)
{
	_requestFinalSave=finalSaveActivated;
}

bool CEnvironment::getRequestFinalSave()
{
	return(_requestFinalSave);
}

void CEnvironment::setSceneLocked()
{
	_sceneIsLocked=true;
	_requestFinalSave=false;
	App::ct->objCont->resetDialogRefreshFlags();
}

bool CEnvironment::getSceneLocked()
{
	return(_sceneIsLocked);
}

void CEnvironment::newSceneProcedure()
{
	setUpDefaultValues();
}

void CEnvironment::simulationAboutToStart()
{

}

void CEnvironment::simulationEnded()
{
//	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
//	{
//	}

}

void CEnvironment::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void CEnvironment::setVisualizeWirelessEmitters(bool v)
{
	_visualizeWirelessEmitters=v;
}

bool CEnvironment::getVisualizeWirelessEmitters()
{
	return(_visualizeWirelessEmitters);
}

void CEnvironment::setVisualizeWirelessReceivers(bool v)
{
	_visualizeWirelessReceivers=v;
}

bool CEnvironment::getVisualizeWirelessReceivers()
{
	return(_visualizeWirelessReceivers);
}

void CEnvironment::setEnableCustomContactHandlingViaScript(bool c,const char* scriptContent)
{
	_useCustomContactHandlingViaScript=c;

	// We remove a script that might be associated:
	CLuaScriptObject* script=App::ct->luaScriptContainer->getCustomContactHandlingScript_callback();
	if (script)
		App::ct->luaScriptContainer->removeScript(script->getScriptID());

	if (_useCustomContactHandlingViaScript)
	{ // we have to add a script
		CLuaScriptObject* script=new CLuaScriptObject(false);
		script->setScriptType(sim_scripttype_contactcallback);
		if (scriptContent)
			script->setScriptText(scriptContent);
		App::ct->luaScriptContainer->insertScript(script);
	}
}

bool CEnvironment::getEnableCustomContactHandlingViaScript()
{
	return(_useCustomContactHandlingViaScript);
}

void CEnvironment::setAcknowledgement(const std::string& a)
{
	_acknowledgement=a;
	if (_acknowledgement.length()>3000)
		_acknowledgement.erase(_acknowledgement.begin()+2999,_acknowledgement.end());
}

std::string CEnvironment::getAcknowledgement()
{
	return(_acknowledgement);
}

void CEnvironment::setBackgroundColor(int viewSize[2])
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,viewSize[0],0,viewSize[1],-100,100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	ogl::setColorsAllBlack();
	glDisable(GL_DEPTH_TEST);
	glColorMaterial(GL_FRONT_AND_BACK,GL_EMISSION);
	glEnable(GL_COLOR_MATERIAL);
	glBegin(GL_QUADS);
	if (fogEnabled)
		glColor3fv(fogBackgroundColor);
	else
		glColor3fv(backGroundColorDown);
	glVertex3i(0,0,0);
	glVertex3i(viewSize[0],0,0);
	if (fogEnabled)
		glColor3fv(fogBackgroundColor);
	else
		glColor3fv(backGroundColor);
	glVertex3i(viewSize[0],viewSize[1],0);
	glVertex3i(0,viewSize[1],0);
	glEnd();
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void CEnvironment::activateAmbientLight(bool a)
{
	if (a)
	{
		GLfloat ambient[]={ambientLightColor[0],ambientLightColor[1],ambientLightColor[2],1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
	}
	else
	{
		GLfloat ambient[]={0.0f,0.0f,0.0f,1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
	}
}
	
void CEnvironment::serialize(CSer& ar,bool saveAndLoadSceneHistory)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Fdn");
		ar << fogEnd;
		ar.flush();

		ar.storeDataName("Fd2");
		ar << fogType << fogStart << fogDensity;
		ar.flush();

		ar.storeDataName("Clc");
		ar << backGroundColor[0] << backGroundColor[1] << backGroundColor[2];
		ar.flush();

		ar.storeDataName("Cld");
		ar << backGroundColorDown[0] << backGroundColorDown[1] << backGroundColorDown[2];
		ar.flush();

		ar.storeDataName("Fbg");
		ar << fogBackgroundColor[0] << fogBackgroundColor[1] << fogBackgroundColor[2];
		ar.flush();

		ar.storeDataName("Alc");
		ar << ambientLightColor[0] << ambientLightColor[1] << ambientLightColor[2];
		ar.flush();
		
		if (saveAndLoadSceneHistory)
		{
			DWORD vli=123456;
			DWORD lsnb=0;
			if (sceneHistory.size()!=0)
			{ // Not newly created scene
				if ( (sceneHistory[sceneHistory.size()-5]==vli)&&(sceneHistory[sceneHistory.size()-4]==lsnb) )
				{
					sceneHistory.pop_back();
					sceneHistory.pop_back();
					sceneHistory.pop_back();
					sceneHistory.pop_back();
					sceneHistory.pop_back();
				}
			}
			sceneHistory.push_back(vli);
			sceneHistory.push_back(lsnb);
			int year,month,day;
			VDateTime::getYearMonthDayHourMinuteSecond(&year,&month,&day,NULL,NULL,NULL);
			sceneHistory.push_back(WORD(year));
			sceneHistory.push_back(WORD(month));
			sceneHistory.push_back(WORD(day));

			ar.storeDataName("Sh2");
			ar << int(sceneHistory.size());
			for (int i=0;i<int(sceneHistory.size());i++)
				ar << sceneHistory[i];
			ar.flush();
		}


		ar.storeDataName("Var");
		BYTE dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,!_2DElementTexturesEnabled);
		SIM_SET_CLEAR_BIT(dummy,1,_saveExistingCalculationStructures); // only needed for undo/redo
		SIM_SET_CLEAR_BIT(dummy,2,_visualizeWirelessEmitters);
		SIM_SET_CLEAR_BIT(dummy,3,!_visualizeWirelessReceivers);
		SIM_SET_CLEAR_BIT(dummy,4,fogEnabled);
		SIM_SET_CLEAR_BIT(dummy,5,_sceneIsLocked);
		SIM_SET_CLEAR_BIT(dummy,6,_requestFinalSave); // needed so that undo/redo works on that item too!
		SIM_SET_CLEAR_BIT(dummy,7,!_shapeTexturesEnabled);
		ar << dummy;
		ar.flush();

		ar.storeDataName("Va2");
		dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,_showingInertias);
		SIM_SET_CLEAR_BIT(dummy,1,_useCustomContactHandlingViaScript);		
		ar << dummy;
		ar.flush();

		ar.storeDataName("Ack");
		ar << _acknowledgement;
		ar.flush();

		ar.storeDataName("Mt2");
		ar << _calculationMaxTriangleSize;
		ar.flush();

		ar.storeDataName("Mrs");
		ar << _calculationMinRelTriangleSize;
		ar.flush();

		ar.storeDataName("Evc");
		ar.setCountingMode();
		wirelessEmissionVolumeColor.serialize(ar);
		if (ar.setWritingMode())
			wirelessEmissionVolumeColor.serialize(ar);

		ar.storeDataName("Wtc");
		ar.setCountingMode();
		wirelessTransmissionColor.serialize(ar);
		if (ar.setWritingMode())
			wirelessTransmissionColor.serialize(ar);

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
				if (theName.compare("Fdn")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> fogEnd;
				}
				if (theName.compare("Fd2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> fogType >> fogStart >> fogDensity;
				}
				if (theName.compare("Var")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					_2DElementTexturesEnabled=!SIM_IS_BIT_SET(dummy,0);
					_saveExistingCalculationStructures=SIM_IS_BIT_SET(dummy,1); // only needed for undo/redo 
					_visualizeWirelessEmitters=SIM_IS_BIT_SET(dummy,2);
					_visualizeWirelessReceivers=!SIM_IS_BIT_SET(dummy,3);
					fogEnabled=SIM_IS_BIT_SET(dummy,4);
					_sceneIsLocked=SIM_IS_BIT_SET(dummy,5);
					_requestFinalSave=SIM_IS_BIT_SET(dummy,6); // needed so that undo/redo works on that item too!
					_shapeTexturesEnabled=!SIM_IS_BIT_SET(dummy,7);
				}
				if (theName.compare("Va2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					_showingInertias=SIM_IS_BIT_SET(dummy,0);
					_useCustomContactHandlingViaScript=SIM_IS_BIT_SET(dummy,1);
				}
				if (theName.compare("Clc")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> backGroundColor[0] >> backGroundColor[1] >> backGroundColor[2];
					backGroundColorDown[0]=backGroundColor[0];
					backGroundColorDown[1]=backGroundColor[1];
					backGroundColorDown[2]=backGroundColor[2];
				}
				if (theName.compare("Cld")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> backGroundColorDown[0] >> backGroundColorDown[1] >> backGroundColorDown[2];
				}
				if (theName.compare("Fbg")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> fogBackgroundColor[0] >> fogBackgroundColor[1] >> fogBackgroundColor[2];
				}
				if (theName.compare("Alc")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> ambientLightColor[0] >> ambientLightColor[1] >> ambientLightColor[2];
				}
				if (saveAndLoadSceneHistory)
				{
					if (theName.compare("Sh2")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						int entries;
						ar >> entries;
						sceneHistory.clear();
						DWORD dummy;
						for (int i=0;i<entries;i++)
						{
							ar >> dummy;
							sceneHistory.push_back(dummy);
						}
					}
				}
				if (theName.compare("Ack")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _acknowledgement;
				}
				if (theName.compare("Mts")==0)
				{ // keep for backward compatibility (2010/07/07)
					noHit=false;
					ar >> byteQuantity;
					ar >> _calculationMaxTriangleSize;
					_calculationMaxTriangleSize=0.3f;
				}
				if (theName.compare("Mt2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _calculationMaxTriangleSize;
				}
				if (theName.compare("Mrs")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _calculationMinRelTriangleSize;
				}
				if (theName.compare("Evc")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					wirelessEmissionVolumeColor.serialize(ar);
				}
				if (theName.compare("Wtc")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					wirelessTransmissionColor.serialize(ar);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}

		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			float avg=(ambientLightColor[0]+ambientLightColor[1]+ambientLightColor[2])/3.0f;
			if (avg>0.21f)
				CTTUtil::scaleLightDown_(ambientLightColor);
		}
	}
}
