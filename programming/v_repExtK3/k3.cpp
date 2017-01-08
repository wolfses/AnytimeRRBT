// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

//**************************************************
// This class represents a K3 inside this
// plugin. Each K3 model in V-REP
// should have its corresponding CK3 object
// in this plugin. If a K3 model
// in V-REP is removed (i.e. erased), then its
// corresponding CK3 object should also be
// removed. Same when a new K3 model is added
//**************************************************

#include "stdafx.h"
#include "k3.h"
#include "v_repLib.h"
#include "access.h"
#include <math.h>
#include <sstream>
#include <iomanip>

#define NO_DETECTION_VALUE_DISTANCE 100.0f
#define NO_DETECTION_VALUE_INTENSITY 0.0f
#define ENCODER_IMPULSES_PER_TURN 2764

CK3::CK3()
{
	_id=-1;
	_associatedObjectID=-1;
	_associatedObjectUniqueID=-1;
	_initialized=false;

	// Following are default values:
	_maxVelocity=6.283f;
	_maxAcceleration=25.0f;
	_maxArmAcceleration=0.5f;
	_modelVersion=0;

	_targetVelocities[0]=0.0f;
	_targetVelocities[1]=0.0f;
	_currentVelocities[0]=0.0f;
	_currentVelocities[1]=0.0f;

	_scaling=1.0f;

}

CK3::~CK3()
{
}

void CK3::setID(int newID)
{
	_id=newID;
}

int CK3::getID()
{
	return(_id);
}

void CK3::setAssociatedObject(int objID,int objUniqueID)
{
	_associatedObjectID=objID;
	_associatedObjectUniqueID=objUniqueID;
}

int CK3::getAssociatedObject()
{
	return(_associatedObjectID);
}

int CK3::getAssociatedObjectUniqueId()
{
	return(_associatedObjectUniqueID);
}

void CK3::synchronizeSceneObject()
{ // We update CK3's associated scene object custom data:
	putK3TagToSceneObject(_associatedObjectID,_modelVersion,_maxVelocity,_maxAcceleration,_maxArmAcceleration);
}

void CK3::synchronize()
{ // We update CK3's data from its associated scene object custom data:
	// 1. Get all the developer data attached to the associated scene object (this is custom data added by the developer):
	int buffSize=simGetObjectCustomDataLength(_associatedObjectID,DEVELOPER_DATA_HEADER);
	char* datBuff=new char[buffSize];
	simGetObjectCustomData(_associatedObjectID,DEVELOPER_DATA_HEADER,datBuff);
	std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
	delete[] datBuff;
	// 2. From that retrieved data, try to extract sub-data with the K3_MAXVELOCITY,K3_MODELVERSION, etc. tags:
	std::vector<unsigned char> k3Data;
	if (CAccess::extractSerializationData(developerCustomData,K3_MAXVELOCITY,k3Data))
		_maxVelocity=CAccess::pop_float(k3Data);
	if (CAccess::extractSerializationData(developerCustomData,K3_MODELVERSION,k3Data))
		_modelVersion=CAccess::pop_int(k3Data);

	if (CAccess::extractSerializationData(developerCustomData,K3_MAXACCELERATION,k3Data))
		_maxAcceleration=CAccess::pop_float(k3Data);
	if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_ARMMAXACCELERATION,k3Data))
		_maxArmAcceleration=CAccess::pop_float(k3Data);
}

void CK3::setMaxVelocity(float maxVel)
{
	_maxVelocity=maxVel;
	if (_maxVelocity>2.0f*3.1415f)
		_maxVelocity=2.0f*3.1415f; // limit it to max 80 RPM
	if (_maxVelocity<3.1415f/8.0f)
		_maxVelocity=3.1415f/8.0f; // limit it to min 5 RPM
	synchronizeSceneObject(); // make sure the associated scene object has the same values stored as this
}

float CK3::getMaxVelocity()
{
	return(_maxVelocity);
}

void CK3::setModelVersion(int version)
{
	_modelVersion=version;
	if (_modelVersion<0)
		_modelVersion=0;
	synchronizeSceneObject(); // make sure the associated scene object has the same values stored as this
}

int CK3::getModelVersion()
{
	return(_modelVersion);
}

float CK3::getMaxAcceleration()
{
	return(_maxAcceleration);
}

void CK3::setMaxAcceleration(float maxAccel)
{
	_maxAcceleration=maxAccel;
	if (_maxAcceleration<0.1f)
		_maxAcceleration=0.1f;
	synchronizeSceneObject(); // make sure the associated scene object has the same values stored as this
}

float CK3::getMaxArmAcceleration()
{
	return(_maxArmAcceleration);
}

void CK3::setMaxArmAcceleration(float maxAccel)
{
	_maxArmAcceleration=maxAccel;
	if (_maxArmAcceleration<0.1f)
		_maxArmAcceleration=0.1f;
	synchronizeSceneObject(); // make sure the associated scene object has the same values stored as this
}

void CK3::putK3TagToSceneObject(int objectHandle,int modelVersion,float maxVelocity,float maxAccel,float maxArmAccel)
{	// This creates/updates a K3_BASE tag
	// 1. Get all the developer data attached to the associated scene object (this is custom data added by the developer):
	int buffSize=simGetObjectCustomDataLength(objectHandle,DEVELOPER_DATA_HEADER);
	char* datBuff=new char[buffSize];
	simGetObjectCustomData(objectHandle,DEVELOPER_DATA_HEADER,datBuff);
	std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
	delete[] datBuff;
	// 2. From that retrieved data, extract sub-data with the K3_BASE,K3_MAXVELOCITY, etc tags, update them, and add them back to the retrieved data:
	std::vector<unsigned char> k3Data;

	CAccess::extractSerializationData(developerCustomData,K3_BASE,k3Data);
	k3Data.clear(); // we discard the old value (if present)
	CAccess::push_float(k3Data,0); // we replace it with the new value
	CAccess::insertSerializationData(developerCustomData,K3_BASE,k3Data);

	CAccess::extractSerializationData(developerCustomData,K3_MAXVELOCITY,k3Data);
	k3Data.clear();
	CAccess::push_float(k3Data,maxVelocity);
	CAccess::insertSerializationData(developerCustomData,K3_MAXVELOCITY,k3Data);

	CAccess::extractSerializationData(developerCustomData,K3_MODELVERSION,k3Data);
	k3Data.clear();
	CAccess::push_int(k3Data,modelVersion);
	CAccess::insertSerializationData(developerCustomData,K3_MODELVERSION,k3Data);

	CAccess::extractSerializationData(developerCustomData,K3_MAXACCELERATION,k3Data);
	k3Data.clear();
	CAccess::push_float(k3Data,maxAccel);
	CAccess::insertSerializationData(developerCustomData,K3_MAXACCELERATION,k3Data);

	CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_ARMMAXACCELERATION,k3Data);
	k3Data.clear();
	CAccess::push_float(k3Data,maxArmAccel);
	CAccess::insertSerializationData(developerCustomData,K3_GRIPPER_ARMMAXACCELERATION,k3Data);

	// 3. We add/update the scene object with the updated custom data:
	simAddObjectCustomData(objectHandle,DEVELOPER_DATA_HEADER,(simChar*)&developerCustomData[0],int(developerCustomData.size()));
}

void CK3::startOfSimulation()
{
	_initialize();
}

bool CK3::endOfSimulation()
{
	_initialized=false;
	return(false); // We don't want this object automatically destroyed at the end of simulation
}

void CK3::handleSimulation()
{ // called when the main script calls: simHandleModule
	_initialize();
	float dt=simGetSimulationTimeStep();
	for (int i=0;i<2;i++)
	{
		if (_targetVelocities[i]>_currentVelocities[i])
		{
			_currentVelocities[i]=_currentVelocities[i]+_maxAcceleration*dt;
			if (_currentVelocities[i]>_targetVelocities[i])
				_currentVelocities[i]=_targetVelocities[i];
		}
		else
		{
			_currentVelocities[i]=_currentVelocities[i]-_maxAcceleration*dt;
			if (_currentVelocities[i]<_targetVelocities[i])
				_currentVelocities[i]=_targetVelocities[i];
		}
		simSetJointTargetVelocity(_k3MotorHandles[i],_currentVelocities[i]);
		float jp;
		simGetJointPosition(_k3MotorHandles[i],&jp);
		float dp=jp-_previousMotorAngles[i];
		if (fabs(dp)>piValue)
			dp-=(2.0f*piValue*fabs(dp)/dp);
//		_cumulativeMotorAngles[i]+=dp/(2.0f*piValue);
		_cumulativeMotorAngles[i]+=dp; // corrected on 5/3/2012 thanks to Felix Fischer
		_previousMotorAngles[i]=jp;
	}


	float adp=_targetArmPosition-_currentArmPosition;
	if (adp!=0.0f)
	{
		if (adp*_currentArmVelocity>=0.0f)
		{
			float decelToZeroTime=(fabs(_currentArmVelocity)+_maxArmAcceleration*dt*1.0f)/_maxArmAcceleration;
			float distToZero=0.5f*_maxArmAcceleration*decelToZeroTime*decelToZeroTime;
			float dir=1.0f;
			if (_currentArmVelocity!=0.0f)
				dir=_currentArmVelocity/fabs(_currentArmVelocity);
			else
				dir=adp/fabs(adp);
			if (fabs(adp)>distToZero)
				_currentArmVelocity+=dir*_maxArmAcceleration*dt;
			else
				_currentArmVelocity-=dir*_maxArmAcceleration*dt;
		}
		else
			_currentArmVelocity*=(1-_maxArmAcceleration*dt/fabs(_currentArmVelocity));
	}
	else
	{
		if (_currentArmVelocity!=0.0f)
		{
			float dv=-_currentArmVelocity*_maxArmAcceleration*dt/fabs(_currentArmVelocity);
			if ((_currentArmVelocity+dv)*_currentArmVelocity<0.0f)
				_currentArmVelocity=0.0f;
			else
				_currentArmVelocity+=dv;
		}
	}

	_currentArmPosition+=_currentArmVelocity*dt;

	simSetJointTargetPosition(_k3GripperArmMotorHandles[0],_currentArmPosition);
	simSetJointTargetPosition(_k3GripperArmMotorHandles[1],-_currentArmPosition);
	simSetJointTargetPosition(_k3GripperArmMotorHandles[2],_currentArmPosition);
	simSetJointTargetPosition(_k3GripperArmMotorHandles[3],_currentArmPosition);
	simSetJointTargetPosition(_k3GripperArmMotorHandles[4],_currentArmPosition);
	simSetJointTargetPosition(_k3GripperArmMotorHandles[5],_currentArmPosition);

	float jp;
	simGetJointPosition(_k3GripperFingerMotorHandles[0],&jp);
	_currentGripperGap_unscaled=(jp/_scaling)+0.04f; // Don't forget scaling!!
	float dp=_targetGripperGap_unscaled-_currentGripperGap_unscaled;
	float velToRegulate=0.0f;
	if (fabs(dp)<0.005f)
	{
		if (dp!=0.0f)
			velToRegulate=(fabs(dp)/0.005f)*0.045f*dp/fabs(dp);
	}
	else
		velToRegulate=0.045f*dp/fabs(dp);
	simSetJointTargetVelocity(_k3GripperFingerMotorHandles[0],velToRegulate*_scaling); // Don't forget scaling!!
	simSetJointTargetPosition(_k3GripperFingerMotorHandles[1],-jp*0.5f/_scaling); // Don't forget scaling!!
	simSetJointTargetPosition(_k3GripperFingerMotorHandles[2],-jp*0.5f/_scaling); // Don't forget scaling!!

	_updateStateVisualization();
}

void  CK3::checkScale()
{
	_scaling=simGetObjectSizeFactor(_associatedObjectID);
	if (_scaling<=0.0f)
		_scaling=1.0f; // error, the _associatedObjectID doesn't exist!
}

void CK3::_initialize()
{
	if (_initialized)
		return;
	checkScale();
	// We need to find the handle of K3's motors and sensors according to data tags attached to objects.
	// Since there might be several K3s in the scene, we should only explore this K3's tree hierarchy
	// to find those tags:
	for (int i=0;i<2;i++)
		_k3MotorHandles[i]=-1;
	for (int i=0;i<2;i++)
		_k3ColorSensorHandles[i]=-1;
	for (int i=0;i<9;i++)
		_k3IrSensorHandles[i]=-1;
	for (int i=0;i<5;i++)
		_k3UsSensorHandles[i]=-1;
	for (int i=0;i<6;i++)
		_k3GripperArmMotorHandles[i]=-1;
	for (int i=0;i<3;i++)
		_k3GripperFingerMotorHandles[i]=-1;
	for (int i=0;i<2;i++)
		_k3GripperDistanceSensorHandles[i]=-1;
	for (int i=0;i<2;i++)
		_k3GripperColorSensorHandles[i]=-1;

	std::vector<int> toExplore;
	toExplore.push_back(_associatedObjectID); // We start exploration with the base of the K3-tree
	while (toExplore.size()!=0)
	{
		int objHandle=toExplore[toExplore.size()-1];
		toExplore.pop_back();
		// 1. Add this object's children to the list to explore:
		int index=0;
		int childHandle=simGetObjectChild(objHandle,index++);
		while (childHandle!=-1)
		{
			toExplore.push_back(childHandle);
			childHandle=simGetObjectChild(objHandle,index++);
		}
		// 2. Now check if this object has one of the tags we are looking for:
		// a. Get all the developer data attached to this scene object (this is custom data added by the developer):
		int buffSize=simGetObjectCustomDataLength(objHandle,DEVELOPER_DATA_HEADER);
		if (buffSize!=0)
		{ // Yes there is some custom data written by us (the developer with the DEVELOPER_DATA_HEADER header)
			char* datBuff=new char[buffSize];
			simGetObjectCustomData(objHandle,DEVELOPER_DATA_HEADER,datBuff);
			std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
			delete[] datBuff;
			// b. From that retrieved data, try to extract sub-data with the searched tags:
			std::vector<unsigned char> k3TagData;
			if (CAccess::extractSerializationData(developerCustomData,K3_LEFTMOTOR,k3TagData))
				_k3MotorHandles[0]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_RIGHTMOTOR,k3TagData))
				_k3MotorHandles[1]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_COLORSENSORLEFT,k3TagData))
				_k3ColorSensorHandles[0]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_COLORSENSORRIGHT,k3TagData))
				_k3ColorSensorHandles[1]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_IRSENSOR1,k3TagData))
				_k3IrSensorHandles[0]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_IRSENSOR2,k3TagData))
				_k3IrSensorHandles[1]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_IRSENSOR3,k3TagData))
				_k3IrSensorHandles[2]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_IRSENSOR4,k3TagData))
				_k3IrSensorHandles[3]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_IRSENSOR5,k3TagData))
				_k3IrSensorHandles[4]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_IRSENSOR6,k3TagData))
				_k3IrSensorHandles[5]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_IRSENSOR7,k3TagData))
				_k3IrSensorHandles[6]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_IRSENSOR8,k3TagData))
				_k3IrSensorHandles[7]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_IRSENSOR9,k3TagData))
				_k3IrSensorHandles[8]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_USSENSOR1,k3TagData))
				_k3UsSensorHandles[0]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_USSENSOR2,k3TagData))
				_k3UsSensorHandles[1]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_USSENSOR3,k3TagData))
				_k3UsSensorHandles[2]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_USSENSOR4,k3TagData))
				_k3UsSensorHandles[3]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_USSENSOR5,k3TagData))
				_k3UsSensorHandles[4]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_ARMMOTOR1,k3TagData))
				_k3GripperArmMotorHandles[0]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_ARMMOTOR2,k3TagData))
				_k3GripperArmMotorHandles[1]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_ARMMOTOR3,k3TagData))
				_k3GripperArmMotorHandles[2]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_ARMMOTOR4,k3TagData))
				_k3GripperArmMotorHandles[3]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_ARMMOTOR5,k3TagData))
				_k3GripperArmMotorHandles[4]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_ARMMOTOR6,k3TagData))
				_k3GripperArmMotorHandles[5]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_GRIPPERMOTOR1,k3TagData))
				_k3GripperFingerMotorHandles[0]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_GRIPPERMOTOR2,k3TagData))
				_k3GripperFingerMotorHandles[1]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_GRIPPERMOTOR3,k3TagData))
				_k3GripperFingerMotorHandles[2]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_LEFTDISTSENSOR,k3TagData))
				_k3GripperDistanceSensorHandles[0]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_RIGHTDISTSENSOR,k3TagData))
				_k3GripperDistanceSensorHandles[1]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_LEFTCOLSENSOR,k3TagData))
				_k3GripperColorSensorHandles[0]=objHandle;
			if (CAccess::extractSerializationData(developerCustomData,K3_GRIPPER_RIGHTCOLSENSOR,k3TagData))
				_k3GripperColorSensorHandles[1]=objHandle;
		}
	}
	// Now get the display handle (Custom UI):
	char* baseName=simGetObjectName(_associatedObjectID);
	int suffixNumber=simGetNameSuffix(baseName);
	simReleaseBuffer(baseName);
	int globalSuffixSave=simGetNameSuffix(NULL);
	simSetNameSuffix(suffixNumber);
	_k3DisplayHandle=simGetUIHandle("K3_stateVisualization");
	simSetNameSuffix(globalSuffixSave); // reset to previous value
	
	_targetVelocities[0]=0.0f;
	_targetVelocities[1]=0.0f;
	_currentVelocities[0]=0.0f;
	_currentVelocities[1]=0.0f;
	_cumulativeMotorAngles[0]=0.0f;
	_cumulativeMotorAngles[1]=0.0f;
	_previousMotorAngles[0]=0.0f;
	_previousMotorAngles[1]=0.0f;
	_currentArmPosition=0.0f;
	_targetArmPosition=0.0f;
	_currentArmVelocity=0.0f;
	_targetGripperGap_unscaled=0.055f;

	_initialized=true;
}

void CK3::_updateStateVisualization()
{
	_initialize();
	// IR sensors on the base:
	float ptAndDist[4];
	std::string s;
	for (int i=0;i<9;i++)
	{
		if (((simGetExplicitHandling(_k3IrSensorHandles[i])&1)==0)&&(simReadProximitySensor(_k3IrSensorHandles[i],ptAndDist,NULL,NULL)>0))
		{
			simSetUIButtonLabel(_k3DisplayHandle,110+i,"&&Box",NULL);
			std::stringstream out;
			out << int(ptAndDist[3]*1000.0f);
			s=out.str();
			simSetUIButtonLabel(_k3DisplayHandle,210+i,s.c_str(),NULL);
		}
		else
		{
			simSetUIButtonLabel(_k3DisplayHandle,110+i,"",NULL);
			simSetUIButtonLabel(_k3DisplayHandle,210+i,"",NULL);
		}
	}
	
	// UI title:
	char* objName=simGetObjectName(_associatedObjectID);
	if (objName!=NULL)
	{
		std::string nm(objName);
		simReleaseBuffer(objName);
		nm+=" state visualization";
		simSetUIButtonLabel(_k3DisplayHandle,0,nm.c_str(),NULL);
	}

	// US sensors on the base:
	for (int i=0;i<5;i++)
	{
		if (((simGetExplicitHandling(_k3UsSensorHandles[i])&1)==0)&&(simReadProximitySensor(_k3UsSensorHandles[i],ptAndDist,NULL,NULL)>0))
		{
			simSetUIButtonLabel(_k3DisplayHandle,100+i,"&&Box",NULL);
			std::stringstream out;
			out << int(ptAndDist[3]*1000.0f);
			s=out.str();
			simSetUIButtonLabel(_k3DisplayHandle,200+i,s.c_str(),NULL);
		}
		else
		{
			simSetUIButtonLabel(_k3DisplayHandle,100+i,"",NULL);
			simSetUIButtonLabel(_k3DisplayHandle,200+i,"",NULL);
		}
	}
	// Color sensors on base:
	for (int i=0;i<2;i++)
	{
		float* auxValues=NULL;
		int* auxValuesCount=NULL;
		float col[3]={0.0f,0.0f,0.0f};
		if (simReadVisionSensor(_k3ColorSensorHandles[i],&auxValues,&auxValuesCount)>=0)
		{
			if ((auxValuesCount[0]>0)||(auxValuesCount[1]>=15))
			{
				col[0]=auxValues[11];
				col[1]=auxValues[12];
				col[2]=auxValues[13];
			}
			simReleaseBuffer((char*)auxValues);
			simReleaseBuffer((char*)auxValuesCount);
		}
		simSetUIButtonColor(_k3DisplayHandle,300+i,col,NULL,NULL);
	}

	// Base motor velocities and encoders:
	for (int i=0;i<2;i++)
	{
		std::stringstream out1,out2;
		out1 << std::fixed << std::setprecision(1) << _currentVelocities[i]*180.0f/piValue;
		s=out1.str();
		simSetUIButtonLabel(_k3DisplayHandle,320+i,s.c_str(),NULL);

		out2 << int(float(ENCODER_IMPULSES_PER_TURN)*_cumulativeMotorAngles[i]/(2.0f*piValue));
		s=out2.str();
		simSetUIButtonLabel(_k3DisplayHandle,310+i,s.c_str(),NULL);
	}

	// Arm position:
	std::stringstream out;
	out << int((1.0f-(_currentArmPosition*180.0f/(195.0f*piValue)))*600.0f+300.0f);
	s=out.str();
	simSetUIButtonLabel(_k3DisplayHandle,120,s.c_str(),NULL);
	simSetUISlider(_k3DisplayHandle,121,int(1000.0f*_currentArmPosition*180.0f/(195.0f*piValue)));

	// Finger motors:
	out.str("");
	out << int(_currentGripperGap_unscaled*1000.0f);
	s=out.str();
	simSetUIButtonLabel(_k3DisplayHandle,130,s.c_str(),NULL);
	simSetUISlider(_k3DisplayHandle,131,int(0.5f+(1.0f-_currentGripperGap_unscaled/0.055f)*1000.0f));
	simSetUISlider(_k3DisplayHandle,132,int(0.5f+(_currentGripperGap_unscaled/0.055f)*1000.0f));


	// Gripper proximity sensors:
	for (int i=0;i<2;i++)
	{
		if (((simGetExplicitHandling(_k3GripperDistanceSensorHandles[i])&1)==0)&&(simReadProximitySensor(_k3GripperDistanceSensorHandles[i],ptAndDist,NULL,NULL)>0))
		{
			out.str("");
			out << int(ptAndDist[3]*1000.0f);
			s=out.str();
			simSetUIButtonLabel(_k3DisplayHandle,133+i,s.c_str(),NULL);
		}
		else
			simSetUIButtonLabel(_k3DisplayHandle,133+i,"",NULL);
	}
	// Gripper color sensors:
	for (int i=0;i<2;i++)
	{
		float* auxValues=NULL;
		int* auxValuesCount=NULL;
		float col[3]={0.0f,0.0f,0.0f};
		if (simReadVisionSensor(_k3GripperColorSensorHandles[i],&auxValues,&auxValuesCount)>=0)
		{
			if ((auxValuesCount[0]>0)||(auxValuesCount[1]>=15))
			{
				col[0]=auxValues[11];
				col[1]=auxValues[12];
				col[2]=auxValues[13];
			}
			simReleaseBuffer((char*)auxValues);
			simReleaseBuffer((char*)auxValuesCount);
		}
		simSetUIButtonColor(_k3DisplayHandle,135+i,col,NULL,NULL);
	}
	
}

int CK3::getEncoder(int index)
{
	_initialize();
	if ( (index<0)||(index>=2) )
		return(0); // error
	return(int(float(ENCODER_IMPULSES_PER_TURN)*_cumulativeMotorAngles[index]/(2.0f*piValue)));
}

void CK3::setEncoders(int leftSide,int rightSide)
{
	_initialize();
	_cumulativeMotorAngles[0]=float(leftSide)*(2.0f*piValue)/float(ENCODER_IMPULSES_PER_TURN);
	_cumulativeMotorAngles[1]=float(rightSide)*(2.0f*piValue)/float(ENCODER_IMPULSES_PER_TURN);
}

float CK3::getInfrared(int index)
{
	_initialize();
	if ( (index<0)||(index>=9) )
		return(-1.0f); // error
	float ptAndDist[4];
	if (((simGetExplicitHandling(_k3IrSensorHandles[index])&1)==0)&&(simReadProximitySensor(_k3IrSensorHandles[index],ptAndDist,NULL,NULL)>0))
		return(ptAndDist[3]);
	else
		return(NO_DETECTION_VALUE_DISTANCE);
}

float CK3::getUltrasonic(int index)
{
	_initialize();
	if ( (index<0)||(index>=5) )
		return(-1.0f); // error
	float ptAndDist[4];
	if (((simGetExplicitHandling(_k3UsSensorHandles[index])&1)==0)&&(simReadProximitySensor(_k3UsSensorHandles[index],ptAndDist,NULL,NULL)>0))
		return(ptAndDist[3]);
	else
		return(NO_DETECTION_VALUE_DISTANCE);
}

float CK3::getLineSensor(int index)
{
	_initialize();
	if ( (index<0)||(index>=2) )
		return(-1.0f); // error
	float* auxValues=NULL;
	int* auxValuesCount=NULL;
	float retVal=NO_DETECTION_VALUE_INTENSITY;
	if (simReadVisionSensor(_k3ColorSensorHandles[index],&auxValues,&auxValuesCount)>=0)
	{
		if ((auxValuesCount[0]>0)||(auxValuesCount[1]>=15))
			retVal=auxValues[10];
		simReleaseBuffer((char*)auxValues);
		simReleaseBuffer((char*)auxValuesCount);
	}
	return(retVal);
}

void CK3::setVelocity(float leftWheel,float rightWheel)
{
	_initialize();
	_targetVelocities[0]=leftWheel;
	if (_targetVelocities[0]>_maxVelocity)
		_targetVelocities[0]=_maxVelocity;
	if (_targetVelocities[0]<-_maxVelocity)
		_targetVelocities[0]=-_maxVelocity;
	_targetVelocities[1]=rightWheel;
	if (_targetVelocities[1]>_maxVelocity)
		_targetVelocities[1]=_maxVelocity;
	if (_targetVelocities[1]<-_maxVelocity)
		_targetVelocities[1]=-_maxVelocity;
}

void CK3::setArmPosition(int position)
{
	_initialize();
	if (position>900)
		position=900;
	if (position<300)
		position=300;
	_targetArmPosition=(1.0f-float(position-300)/600.0f)*195.0f*piValue/180.0f;
}

void CK3::setGripperGap(int gap)
{
	_initialize();
	if (gap>170)
		gap=170;
	if (gap<0)
		gap=0;
	_targetGripperGap_unscaled=0.055f*float(gap)/170.0f;
}

float CK3::getFingerProxSensor(int index)
{
	_initialize();
	if ( (index<0)||(index>=2) )
		return(-1.0f); // error
	float ptAndDist[4];
	if (((simGetExplicitHandling(_k3GripperDistanceSensorHandles[index])&1)==0)&&(simReadProximitySensor(_k3GripperDistanceSensorHandles[index],ptAndDist,NULL,NULL)>0))
		return(ptAndDist[3]);
	else
		return(NO_DETECTION_VALUE_DISTANCE);
}