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

#include "dynInterface.h"
#include "v_repConst.h"

#ifndef COMPILING_EXTERNAL_DYN_DLL
#include "pluginContainer.h"
#endif

#ifndef COMPILING_EXTERNAL_DYN_DLL
ptr_startSimulation CDynInterface::_startSimulation;
ptr_endSimulation CDynInterface::_endSimulation;
ptr_step CDynInterface::_step;
ptr_isInitialized CDynInterface::_isInitialized;
ptr_isDynamicContentAvailable CDynInterface::_isDynamicContentAvailable;
ptr_serializeDynamicContent CDynInterface::_serializeDynamicContent;
ptr_addParticleObject CDynInterface::_addParticleObject;
ptr_removeParticleObject CDynInterface::_removeParticleObject;
ptr_addParticleObjectItem CDynInterface::_addParticleObjectItem;
ptr_getParticleObjectOtherFloatsPerItem CDynInterface::_getParticleObjectOtherFloatsPerItem;
ptr_getContactPoints CDynInterface::_getContactPoints;
ptr_getParticles CDynInterface::_getParticles;
ptr_getParticleData CDynInterface::_getParticleData;
ptr_getContactForce CDynInterface::_getContactForce;
ptr_reportDynamicWorldConfiguration CDynInterface::_reportDynamicWorldConfiguration;
ptr_getDynamicStepDivider CDynInterface::_getDynamicStepDivider;
ptr_getEngineInfo CDynInterface::_getEngineInfo;


bool CDynInterface::_dynInterfaceInitialized=false;
bool CDynInterface::_dynSystemPluginsLoadPhaseOver=false;

void CDynInterface::setSystemPluginsLoadPhaseOver()
{
	_dynSystemPluginsLoadPhaseOver=true;
}

bool CDynInterface::initializeFunctionsIfNeeded()
{
	static bool alreadyTried=false;
	if (_dynInterfaceInitialized)
		return(true);
	if (alreadyTried&&_dynSystemPluginsLoadPhaseOver)
		return(false);
	alreadyTried=true;
	CPlugin* plugin=CPluginContainer::getPluginFromName("Dynamics");
	if (plugin==NULL)
		return(false); // plugin not there!
	// Get the version first:
	int auxVals[4]={0,0,0,0};
	int retVals[4];
	plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,retVals);
//	int version=retVals[0];
	// Now get all the function pointers:
	int v=1;
	auxVals[0]=v++;
	_startSimulation=(ptr_startSimulation)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_endSimulation=(ptr_endSimulation)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_step=(ptr_step)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_isInitialized=(ptr_isInitialized)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_isDynamicContentAvailable=(ptr_isDynamicContentAvailable)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_serializeDynamicContent=(ptr_serializeDynamicContent)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_addParticleObject=(ptr_addParticleObject)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_removeParticleObject=(ptr_removeParticleObject)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_addParticleObjectItem=(ptr_addParticleObjectItem)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getParticleObjectOtherFloatsPerItem=(ptr_getParticleObjectOtherFloatsPerItem)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getContactPoints=(ptr_getContactPoints)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getParticles=(ptr_getParticles)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getParticleData=(ptr_getParticleData)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getContactForce=(ptr_getContactForce)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_reportDynamicWorldConfiguration=(ptr_reportDynamicWorldConfiguration)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getDynamicStepDivider=(ptr_getDynamicStepDivider)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getEngineInfo=(ptr_getEngineInfo)plugin->sendEventCallbackMessage(sim_message_eventcallback_dynamicsplugin,auxVals,NULL,NULL);

	auxVals[0]=v++;
	_dynInterfaceInitialized=true;
	return(true);
}
#endif // COMPILING_EXTERNAL_DYN_DLL


void CDynInterface::startSimulation(int engine,const float floatParams[50],const int intParams[50],const char boolParams[50])
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		_dynamicEngineToUse=engine;

		int floatIndex=0;
		int intIndex=0;
		int boolIndex=0;

		_dynamicsBulletStepSize=floatParams[floatIndex++];
		_dynamicBulletCollisionMarginFactor=floatParams[floatIndex++];
		_dynamicsODEStepSize=floatParams[floatIndex++];
		_dynamicODEGlobalCFM=floatParams[floatIndex++];
		_dynamicODEGlobalERP=floatParams[floatIndex++];
		_positionScalingFactorDyn=floatParams[floatIndex++];
		_linearVelocityScalingFactorDyn=floatParams[floatIndex++];
		_massScalingFactorDyn=floatParams[floatIndex++];
		_masslessInertiaScalingFactorDyn=floatParams[floatIndex++];
		_forceScalingFactorDyn=floatParams[floatIndex++];
		_torqueScalingFactorDyn=floatParams[floatIndex++];
		_gravityScalingFactorDyn=floatParams[floatIndex++];
		_dynamicActivityRange=floatParams[floatIndex++];
		_dynamicsVortexStepSize=floatParams[floatIndex++];

		_dynamicBulletConstraintSolvingIterations=intParams[intIndex++];
		_dynamicODEConstraintSolvingIterations=intParams[intIndex++];
		_dynamicParticlesIdStart=intParams[intIndex++];
		_3dobjectIdStart=intParams[intIndex++];
		_3dObjectIdEnd=intParams[intIndex++];

		_dynamicODEUseQuickStep=boolParams[boolIndex++]!=0;

		if (CRigidBodyContainerDyn::isGivenEngineAvailable(_dynamicEngineToUse))
			_dynamicsContainer=new CRigidBodyContainerDyn(engine);
		return;
	}
#else
		return;
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	_startSimulation(engine,floatParams,intParams,boolParams);
#endif
}

void CDynInterface::endSimulation()
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (_dynamicsContainer!=NULL)
		{
			delete _dynamicsContainer;
			_dynamicsContainer=NULL;
		}
		return;
	}
#else
		return;
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	_endSimulation();
#endif
}

void CDynInterface::step(float timeStep,float simulationTime)
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (_dynamicsContainer!=NULL)
			_dynamicsContainer->handleDynamics(timeStep,simulationTime);
		return;
	}
#else
		return;
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	_step(timeStep,simulationTime);
#endif
}

char CDynInterface::isInitialized()
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	return(_dynamicsContainer!=NULL); // we use the internal functions
#else
		return(false);
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_isInitialized());
#endif
}

char CDynInterface::isDynamicContentAvailable()
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (_dynamicsContainer!=NULL)
			return(_dynamicsContainer->isDynamicContentAvailable());
		return(false);
	}
#else
		return(false);
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_isDynamicContentAvailable());
#endif
}

float* CDynInterface::getContactPoints(int* count)
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		float* retVal=NULL;
		count[0]=0;
		if (_dynamicsContainer!=NULL)
			retVal=_dynamicsContainer->getContactPoints(count);
		return(retVal);
	}
#else
		return(NULL);
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_getContactPoints(count));
#endif
}

void** CDynInterface::getParticles(int index,int* particlesCount,int* objectType,float** cols)
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (_dynamicsContainer==NULL)
		{
			particlesCount[0]=-1;
			return(NULL);
		}
		return(_dynamicsContainer->particleCont.getParticles(index,particlesCount,objectType,cols));
	}
#else
	{
		particlesCount[0]=-1;
		return(NULL);
	}
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_getParticles(index,particlesCount,objectType,cols));
#endif
}

char CDynInterface::getParticleData(const void* particle,float* pos,float* size,int* objectType,float** additionalColor)
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (particle==NULL)
			return(false);
		return(((CParticleDyn*)particle)->getRenderData(pos,size,objectType,additionalColor));
	}
#else
		return(false);
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_getParticleData(particle,pos,size,objectType,additionalColor));
#endif
}

void CDynInterface::serializeDynamicContent(const char* filenameAndPath,int bulletSerializationBuffer)
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (_dynamicsContainer!=NULL)
		{
			if (_dynamicEngineToUse==sim_physics_bullet)
				_dynamicsContainer->serializeBulletDynamicContent(filenameAndPath,bulletSerializationBuffer);
			if (_dynamicEngineToUse==sim_physics_ode)
				_dynamicsContainer->serializeOdeDynamicContent(filenameAndPath);
		}
		return;
	}
#else
		return;
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	_serializeDynamicContent(filenameAndPath,bulletSerializationBuffer);
#endif
}

int CDynInterface::addParticleObject(int objectType,float size,float massOverVolume,const void* params,float lifeTime,int maxItemCount,const float* ambient,const float* diffuse,const float* specular,const float* emission)
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (_dynamicsContainer!=NULL)
		{
			CParticleObject* it=new CParticleObject(objectType,size,massOverVolume,params,lifeTime,maxItemCount);
			for (int i=0;i<9;i++)
				it->color[i]=0.25f;
			for (int i=9;i<12;i++)
				it->color[i]=0.0f;
			for (int i=0;i<3;i++)
			{
				if (ambient!=NULL)
					it->color[0+i]=ambient[i];
				if (diffuse!=NULL)
					it->color[3+i]=diffuse[i];
				if (specular!=NULL)
					it->color[6+i]=specular[i];
				if (emission!=NULL)
					it->color[9+i]=emission[i];
			}
			return(_dynamicsContainer->particleCont.addObject(it));
		}
		return(-1); // error
	}
#else
		return(-1);
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_addParticleObject(objectType,size,massOverVolume,params,lifeTime,maxItemCount,ambient,diffuse,specular,emission));
#endif
}

char CDynInterface::removeParticleObject(int objectHandle)
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (_dynamicsContainer!=NULL)
		{
			if (objectHandle==sim_handle_all)
				_dynamicsContainer->particleCont.removeAllObjects();
			else
			{
				CParticleObject* it=_dynamicsContainer->particleCont.getObject(objectHandle,false);
				if (it==NULL)
					return(false); // error
				_dynamicsContainer->particleCont.removeObject(objectHandle);
			}
			return(true);
		}
		return(false); // error
	}
#else
		return(false);
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_removeParticleObject(objectHandle));
#endif
}

char CDynInterface::addParticleObjectItem(int objectHandle,const float* itemData,float simulationTime)
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (_dynamicsContainer!=NULL)
		{
			CParticleObject* it=_dynamicsContainer->particleCont.getObject(objectHandle,false);
			if (it==NULL)
				return(false); // error
			it->addParticle(simulationTime,itemData);
			return(true);
		}
		return(false); // error
	}
#else
		return(false);
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_addParticleObjectItem(objectHandle,itemData,simulationTime));
#endif
}

int CDynInterface::getParticleObjectOtherFloatsPerItem(int objectHandle)
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		int retVal=0;
		if (_dynamicsContainer!=NULL)
		{
			CParticleObject* it=_dynamicsContainer->particleCont.getObject(objectHandle,false);
			if (it!=NULL)
				retVal=it->getOtherFloatsPerItem();
			else if (objectHandle==-131183)
				retVal=61855195;
		}
		return(retVal);
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_getParticleObjectOtherFloatsPerItem(objectHandle));
#endif
}

char CDynInterface::getContactForce(int dynamicPass,int objectHandle,int index,int objectHandles[2],float contactInfo[6])
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (_dynamicsContainer!=NULL)
			return(_dynamicsContainer->getContactForce(dynamicPass,objectHandle,index,objectHandles,contactInfo));
		return(false);
	}
#else
		return(false);
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_getContactForce(dynamicPass,objectHandle,index,objectHandles,contactInfo));
#endif
}

void CDynInterface::reportDynamicWorldConfiguration(int totalPassesCount,char doNotApplyJointIntrinsicPositions,float simulationTime)
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (_dynamicsContainer!=NULL)
			_dynamicsContainer->reportDynamicWorldConfiguration(totalPassesCount,doNotApplyJointIntrinsicPositions!=0,simulationTime);
		return;
	}
#else
		return;
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	_reportDynamicWorldConfiguration(totalPassesCount,doNotApplyJointIntrinsicPositions,simulationTime);
#endif
}

int CDynInterface::getDynamicStepDivider()
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		if (_dynamicsContainer!=NULL)
			return(_dynamicsContainer->getDynamicsCalculationPasses());
		return(0);
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_getDynamicStepDivider());
#endif
}


int CDynInterface::getEngineInfo(int engine,int* data1,char* data2,char* data3)
{
#ifndef COMPILING_EXTERNAL_DYN_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_DYN_DLL
	{ // we use the internal functions
		return(CRigidBodyContainerDyn::getEngineInfo(engine,data1,data2,data3));
	}
#else
	{
		data3="";
		data1[0]=0;
		strcpy(data2,"0");
		return(0);
	}
#endif
#ifndef COMPILING_EXTERNAL_DYN_DLL
	// we call the plugin function:
	return(_getEngineInfo(engine,data1,data2,data3));
#endif
}


#ifdef COMPILING_EXTERNAL_DYN_DLL


int CDynInterface::getDynamicParticlesIdStart()
{
	return(_dynamicParticlesIdStart);
}

int CDynInterface::get3dObjectIdStart()
{
	return(_3dobjectIdStart);
}

int CDynInterface::get3dObjectIdEnd()
{
	return(_3dObjectIdEnd);
}

float CDynInterface::getDynamicActivityRange()
{
	return(_dynamicActivityRange);
}

int CDynInterface::getEngine()
{
	return(_dynamicEngineToUse);
}

float CDynInterface::getPositionScalingFactorDyn()
{
	return(_positionScalingFactorDyn);
}

float CDynInterface::getLinearVelocityScalingFactorDyn()
{
	return(_linearVelocityScalingFactorDyn);
}

float CDynInterface::getMassScalingFactorDyn()
{
	return(_massScalingFactorDyn);
}

float CDynInterface::getMasslessInertiaScalingFactorDyn()
{
	return(_masslessInertiaScalingFactorDyn);
}

float CDynInterface::getForceScalingFactorDyn()
{
	return(_forceScalingFactorDyn);
}

float CDynInterface::getTorqueScalingFactorDyn()
{
	return(_torqueScalingFactorDyn);
}

float CDynInterface::getGravityScalingFactorDyn()
{
	return(_gravityScalingFactorDyn);
}

float CDynInterface::getBulletDynamicsStepSize()
{
	return(_dynamicsBulletStepSize);
}

float CDynInterface::getBulletDynamicCollisionMarginFactor()
{
	return(_dynamicBulletCollisionMarginFactor);
}

int CDynInterface::getBulletDynamicConstraintSolvingIterations()
{
	return(_dynamicBulletConstraintSolvingIterations);
}

int CDynInterface::getODEDynamicConstraintSolvingIterations()
{
	return(_dynamicODEConstraintSolvingIterations);
}

float CDynInterface::getODEDynamicsStepSize()
{
	return(_dynamicsODEStepSize);
}

bool CDynInterface::getODEUseQuickStep()
{
	return(_dynamicODEUseQuickStep);
}

float CDynInterface::getODEDynamicGlobalCFM()
{
	return(_dynamicODEGlobalCFM);
}

float CDynInterface::getODEDynamicGlobalERP()
{
	return(_dynamicODEGlobalERP);
}

float CDynInterface::getVortexDynamicsStepSize()
{
	return(_dynamicsVortexStepSize);
}

float CDynInterface::getDynamicsInternalTimeStep()
{
	return(_dynamicsInternalTimeStep);
}

void CDynInterface::setDynamicsInternalTimeStep(float dt)
{
	_dynamicsInternalTimeStep=dt;
}

CRigidBodyContainerDyn* CDynInterface::_dynamicsContainer=NULL;

int CDynInterface::_dynamicEngineToUse;

float CDynInterface::_dynamicsBulletStepSize;
int CDynInterface::_dynamicBulletConstraintSolvingIterations;
float CDynInterface::_dynamicBulletCollisionMarginFactor;

float CDynInterface::_dynamicsODEStepSize;
int CDynInterface::_dynamicODEConstraintSolvingIterations;
bool CDynInterface::_dynamicODEUseQuickStep;
float CDynInterface::_dynamicODEGlobalCFM;
float CDynInterface::_dynamicODEGlobalERP;

float CDynInterface::_dynamicsVortexStepSize;

float CDynInterface::_positionScalingFactorDyn;
float CDynInterface::_linearVelocityScalingFactorDyn;
float CDynInterface::_massScalingFactorDyn;
float CDynInterface::_masslessInertiaScalingFactorDyn;
float CDynInterface::_forceScalingFactorDyn;
float CDynInterface::_torqueScalingFactorDyn;
float CDynInterface::_gravityScalingFactorDyn;

int CDynInterface::_dynamicParticlesIdStart;
int CDynInterface::_3dobjectIdStart;
int CDynInterface::_3dObjectIdEnd;
float CDynInterface::_dynamicActivityRange;

float CDynInterface::_dynamicsInternalTimeStep; // calculated

#endif






