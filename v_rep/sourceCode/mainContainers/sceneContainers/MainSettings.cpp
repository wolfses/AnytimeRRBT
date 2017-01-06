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
#include "MainSettings.h"
#include "Global.h"
#include "OGL.h"
#include "Tt.h"
#include "App.h"
#include "IloIlo.h"

CMainSettings::CMainSettings()
{
	setUpDefaultValues();
}

CMainSettings::~CMainSettings()
{

}

void CMainSettings::newSceneProcedure()
{
	setUpDefaultValues();
}

void CMainSettings::simulationAboutToStart()
{

}

void CMainSettings::simulationEnded()
{

}

void CMainSettings::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void CMainSettings::setUpDefaultValues()
{
	scenePathAndName="";
	activeLayers=0x00ff;

	infoWindowColorStyle=0; // default
	statusBoxOpenState=false;
	infoWindowOpenState=true;
	
	collisionColor.setDefaultValues();
	collisionColor.setColor(0.0f,0.0f,0.0f,AMBIENT_MODE);
	collisionColor.setColor(1.0f,0.0f,0.0f,EMISSION_MODE);

	proximitySensorsEnabled=true;
	visionSensorsEnabled=true;
	ikCalculationEnabled=true;
	gcsCalculationEnabled=true;
	collisionDetectionEnabled=true;
	distanceCalculationEnabled=true;
	jointMotionHandlingEnabled=true;
	pathMotionHandlingEnabled=true;
	millsEnabled=true;
	mirrorsDisabled=false;
	clippingPlanesDisabled=false;

	forBackwardCompatibility_03_01_2012_stillUsingStepSizeDividers=false;
}

void CMainSettings::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing

		ar.storeDataName("Va5");
		BYTE dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,proximitySensorsEnabled);
		SIM_SET_CLEAR_BIT(dummy,1,statusBoxOpenState);
		ar << dummy;
		ar.flush();

		ar.storeDataName("Va2");
		dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,!infoWindowOpenState);
		SIM_SET_CLEAR_BIT(dummy,1,mirrorsDisabled);
		SIM_SET_CLEAR_BIT(dummy,2,clippingPlanesDisabled);
		SIM_SET_CLEAR_BIT(dummy,3,ikCalculationEnabled);
		ar << dummy;
		ar.flush();

		ar.storeDataName("Va3");
		dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,gcsCalculationEnabled);
		SIM_SET_CLEAR_BIT(dummy,1,collisionDetectionEnabled);
		SIM_SET_CLEAR_BIT(dummy,2,distanceCalculationEnabled);
		SIM_SET_CLEAR_BIT(dummy,3,!jointMotionHandlingEnabled);
		SIM_SET_CLEAR_BIT(dummy,4,!pathMotionHandlingEnabled);
		SIM_SET_CLEAR_BIT(dummy,5,!visionSensorsEnabled);
		SIM_SET_CLEAR_BIT(dummy,6,!millsEnabled);
// 27/11/2012		SIM_SET_CLEAR_BIT(dummy,7,!_dynamicsEnabled);
		ar << dummy;
		ar.flush();

		ar.storeDataName("Crc");
		ar.setCountingMode();
		collisionColor.serialize(ar);
		if (ar.setWritingMode())
			collisionColor.serialize(ar);

		ar.storeDataName("Al2");
		ar << activeLayers;
		ar.flush();

		ar.storeDataName("Iwc");
		ar << infoWindowColorStyle;
		ar.flush();

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		forBackwardCompatibility_03_01_2012_stillUsingStepSizeDividers=false;
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Va1")==0)
				{ // Keep for backward compatibility (3/1/2012)
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					
					bool bulletUseDefault=!SIM_IS_BIT_SET(dummy,0);
					bool _displayContactPoints=SIM_IS_BIT_SET(dummy,2);
					App::ct->dynamicsContainer->setDisplayContactPoints(_displayContactPoints);
					bool odeUseDefault=!SIM_IS_BIT_SET(dummy,3);
					proximitySensorsEnabled=SIM_IS_BIT_SET(dummy,5);
					bool _dynamicODEUseQuickStep=!SIM_IS_BIT_SET(dummy,6);
					App::ct->dynamicsContainer->setODEUseQuickStep(_dynamicODEUseQuickStep,true);
					statusBoxOpenState=SIM_IS_BIT_SET(dummy,7);

					int _dynamicDefaultTypeCalculationParameters=1; // precise.
					if ((!bulletUseDefault)||(!odeUseDefault))
						_dynamicDefaultTypeCalculationParameters=4; // custom
					App::ct->dynamicsContainer->setUseDynamicDefaultCalculationParameters(_dynamicDefaultTypeCalculationParameters);
					
					forBackwardCompatibility_03_01_2012_stillUsingStepSizeDividers=true;
				}

				if (theName.compare("Ss2")==0)
				{ // Keep for backward compatibility (27/11/2012)
					noHit=false;
					ar >> byteQuantity;
					float _dynamicsBULLETStepSize,_dynamicsODEStepSize;
					ar >> _dynamicsBULLETStepSize >> _dynamicsODEStepSize;
					App::ct->dynamicsContainer->setDynamicsStepSize(_dynamicsBULLETStepSize,true);
					App::ct->dynamicsContainer->setODEDynamicsStepSize(_dynamicsODEStepSize,true);
				}

				if (theName.compare("Ddi")==0)
				{ // Keep for backward compatibility (27/11/2012)
					noHit=false;
					ar >> byteQuantity;
					int _dynamicDefaultTypeCalculationParameters;
					ar >> _dynamicDefaultTypeCalculationParameters;
					App::ct->dynamicsContainer->setUseDynamicDefaultCalculationParameters(_dynamicDefaultTypeCalculationParameters);

				}

				if (theName.compare("Va4")==0)
				{ // for backward compatibility (27/11/2012)
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					bool _displayContactPoints=SIM_IS_BIT_SET(dummy,2);
					App::ct->dynamicsContainer->setDisplayContactPoints(_displayContactPoints);
					proximitySensorsEnabled=SIM_IS_BIT_SET(dummy,5);
					bool _dynamicODEUseQuickStep=!SIM_IS_BIT_SET(dummy,6);
					App::ct->dynamicsContainer->setODEUseQuickStep(_dynamicODEUseQuickStep,true);
					statusBoxOpenState=SIM_IS_BIT_SET(dummy,7);
				}
				if (theName.compare("Va5")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					proximitySensorsEnabled=SIM_IS_BIT_SET(dummy,0);
					statusBoxOpenState=SIM_IS_BIT_SET(dummy,1);
				}

				if (theName.compare("Va2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					infoWindowOpenState=!SIM_IS_BIT_SET(dummy,0);
					mirrorsDisabled=SIM_IS_BIT_SET(dummy,1);
					clippingPlanesDisabled=SIM_IS_BIT_SET(dummy,2);
					ikCalculationEnabled=SIM_IS_BIT_SET(dummy,3);
				}

				if (theName.compare("Va3")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					gcsCalculationEnabled=SIM_IS_BIT_SET(dummy,0);
					collisionDetectionEnabled=SIM_IS_BIT_SET(dummy,1);
					distanceCalculationEnabled=SIM_IS_BIT_SET(dummy,2);
					jointMotionHandlingEnabled=!SIM_IS_BIT_SET(dummy,3);
					pathMotionHandlingEnabled=!SIM_IS_BIT_SET(dummy,4);
					visionSensorsEnabled=!SIM_IS_BIT_SET(dummy,5);
					millsEnabled=!SIM_IS_BIT_SET(dummy,6);
					bool _dynamicsEnabled=!SIM_IS_BIT_SET(dummy,7);
					App::ct->dynamicsContainer->setDynamicsEnabled(_dynamicsEnabled);
				}
				if (theName.compare("Crc")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					collisionColor.serialize(ar);
				}
				if (theName.compare("Al2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> activeLayers;
				}
				if (theName.compare("Iwc")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> infoWindowColorStyle;
				}
				if (theName.compare("Dsd")==0)
				{ // Keep for backward compatibility (27/11/2012)
					noHit=false;
					ar >> byteQuantity;
					ar >> dynamicsBULLETStepSizeDivider_forBackwardCompatibility_03_01_2012;
				}
				if (theName.compare("Dis")==0)
				{ // Keep for backward compatibility (27/11/2012)
					noHit=false;
					ar >> byteQuantity;
					float _dynamicBULLETInternalScalingFactor;
					ar >> _dynamicBULLETInternalScalingFactor;
					App::ct->dynamicsContainer->setDynamicInternalScalingFactor(_dynamicBULLETInternalScalingFactor,true);
				}
				if (theName.compare("Dcs")==0)
				{ // Keep for backward compatibility (27/11/2012)
					noHit=false;
					ar >> byteQuantity;
					int _dynamicBULLETConstraintSolvingIterations;
					ar >> _dynamicBULLETConstraintSolvingIterations;
					App::ct->dynamicsContainer->setDynamicConstraintSolvingIterations(_dynamicBULLETConstraintSolvingIterations,true);
				}
				if (theName.compare("Gvy")==0)
				{ // Keep for backward compatibility (27/11/2012)
					noHit=false;
					ar >> byteQuantity;
					C3Vector _gravity;
					ar >> _gravity(0) >> _gravity(1) >> _gravity(2);
					App::ct->dynamicsContainer->setGravity(_gravity);
				}
				if (theName.compare("Od1")==0)
				{ // for backward compatibility (3/1/2012)
					noHit=false;
					ar >> byteQuantity;
					float _dynamicODEInternalScalingFactor,_dynamicODEGlobalERP,_dynamicODEGlobalCFM;
					int _dynamicODEConstraintSolvingIterations;
					ar >> dynamicsODEStepSizeDivider_forBackwardCompatibility_03_01_2012 >> _dynamicODEInternalScalingFactor >> _dynamicODEConstraintSolvingIterations >> _dynamicODEGlobalERP >> _dynamicODEGlobalCFM;
					App::ct->dynamicsContainer->setODEDynamicInternalScalingFactor(_dynamicODEInternalScalingFactor,true);
					App::ct->dynamicsContainer->setODEDynamicConstraintSolvingIterations(_dynamicODEConstraintSolvingIterations,true);
					App::ct->dynamicsContainer->setODEDynamicGlobalERP(_dynamicODEGlobalERP,true);
					App::ct->dynamicsContainer->setODEDynamicGlobalCFM(_dynamicODEGlobalCFM,true);
				}
				if (theName.compare("Od2")==0)
				{ // Keep for backward compatibility (27/11/2012)
					noHit=false;
					ar >> byteQuantity;
					float _dynamicODEInternalScalingFactor,_dynamicODEGlobalERP,_dynamicODEGlobalCFM;
					int _dynamicODEConstraintSolvingIterations;
					ar >> _dynamicODEInternalScalingFactor >> _dynamicODEConstraintSolvingIterations >> _dynamicODEGlobalERP >> _dynamicODEGlobalCFM;
					App::ct->dynamicsContainer->setODEDynamicInternalScalingFactor(_dynamicODEInternalScalingFactor,true);
					App::ct->dynamicsContainer->setODEDynamicConstraintSolvingIterations(_dynamicODEConstraintSolvingIterations,true);
					App::ct->dynamicsContainer->setODEDynamicGlobalERP(_dynamicODEGlobalERP,true);
					App::ct->dynamicsContainer->setODEDynamicGlobalCFM(_dynamicODEGlobalCFM,true);
				}
				if (theName.compare("Deu")==0)
				{ // Keep for backward compatibility (27/11/2012)
					noHit=false;
					ar >> byteQuantity;
					int _dynamicEngineToUse;
					ar >> _dynamicEngineToUse;
					App::ct->dynamicsContainer->setDynamicEngineType(_dynamicEngineToUse);
				}
				if (theName.compare("Bcm")==0)
				{ // Keep for backward compatibility (27/11/2012)
					noHit=false;
					ar >> byteQuantity;
					float _dynamicBULLETCollisionMarginFactor;
					ar >> _dynamicBULLETCollisionMarginFactor;
					App::ct->dynamicsContainer->setBulletDynamicCollisionMarginFactor(_dynamicBULLETCollisionMarginFactor,true);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}

