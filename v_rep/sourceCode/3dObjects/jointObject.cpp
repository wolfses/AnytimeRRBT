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
#include "jointObject.h"

#include "Tt.h"
#include "v_rep_internal.h"
#include "OGL.h"
#include "GV.h"
#include "LinMotionRoutines.h"
#include "v_repStrings.h"
#include "IloIlo.h"
#include "ttUtil.h"
#include "easyLock.h"
#include "App.h"


CJoint::CJoint()
{ // use only during serialization!!
	commonInit();
	// Following is not needed, but just in case!
	_jointType=sim_joint_revolute_subtype;
	_positionIsCyclic=true;
	_jointPositionRange=piValTimes2;
	_jointMinPosition=-piValue;
	_maxStepSize=10.0f*degToRad;
	_maxAcceleration=60.0f*degToRad;
	_measuredJointVelocity_velocityMeasurement=0.0f;
}

CJoint::CJoint(int jointType)
{
	_jointType=jointType;
	commonInit();
	if (jointType==sim_joint_revolute_subtype)
	{
		_jointMode=sim_jointmode_force;
		_positionIsCyclic=true;
		_jointPositionRange=piValTimes2;
		_jointMinPosition=-piValue;
		_maxStepSize=10.0f*degToRad;
		_maxAcceleration=60.0f*degToRad;
		_dynamicMotorMaximumForce=2.5f; // 0.25 m x 1kg x 9.81
		_dynamicMotorUpperLimitVelocity=360.0f*degToRad;
	}
	if (jointType==sim_joint_prismatic_subtype)
	{
		_jointMode=sim_jointmode_force;
		_positionIsCyclic=false;
		_jointPositionRange=1.0f;
		_jointMinPosition=-0.5f;
		_maxStepSize=0.1f;
		_maxAcceleration=0.1f;
		_dynamicMotorMaximumForce=50.0f; // 5kg x 9.81
		_dynamicMotorUpperLimitVelocity=10.0f;
	}
	if (jointType==sim_joint_spherical_subtype)
	{
		_jointMode=sim_jointmode_force;
		_positionIsCyclic=true;
		_jointPositionRange=piValue;
		_jointMinPosition=0.0f; // no meaning here
		_maxStepSize=10.0f*degToRad;
		_maxAcceleration=60.0f*degToRad;
		_dynamicMotorMaximumForce=0.0f; // no meaning here!
		_dynamicMotorUpperLimitVelocity=0.0f; // no meaning here!
	}
}

void CJoint::commonInit()
{ // set the joint type directly after that!
	_objectType=sim_object_joint_type;
	_screwPitch=0.0f;
	_sphericalTransformation.setIdentity();
	_jointPosition=0.0f;

	_jointPositionForMotionHandling=_jointPosition;
	_velocity=0.0f;
	_targetVelocity=0.0f;
	_initialValuesInitialized=false;

	_cumulatedForceOrTorque=0.0f;
	_cumulativeForceOrTorqueTmp=0.0f;

	_jointMode=sim_jointmode_ik;
	_dependencyJointID=-1;
	_dependencyJointCoeff=1.0f;
	_dependencyJointFact=0.0f;

	// Dynamic values:
	_dynamicMotorEnabled=false;
	_dynamicMotorTargetVelocity=0.0f;
	_dynamicMotorMaximumForce=1000.0f; // This value has to be adjusted according to the joint type
	_dynamicSecondPartIsValid=false;
	_dynamicSecondPartLocalTransform.setIdentity();

	_dynamicMotorControlLoopEnabled=false;
	_dynamicMotorCustomControl=false;
	_dynamicMotorPositionControl_P=0.1f;
	_dynamicMotorPositionControl_I=0.0f;
	_dynamicMotorPositionControl_D=0.0f;
	_dynamicMotorSpringControl_K=0.1f;
	_dynamicMotorSpringControl_C=0.0f;
	_dynamicMotorPositionControl_targetPosition=_jointPosition;
	_dynamicMotorPositionControl_torqueModulation=false;
	_dynamicMotorPositionControl_targetPosition=0.0f;
	_jointHasHybridFunctionality=false;

	_odeStopERP=0.6f;
	_odeStopCFM=0.00001f;
	_odeBounce=0.0f;
	_odeFudgeFactor=1.0f;
	_odeNormalCFM=0.00001f;

	_bulletStopERP=0.2f;
	_bulletStopCFM=0.0f;
	_bulletNormalCFM=0.0f;

	// Vortex parameters (neg. stiffness --> INF):
	// ----------------------------------------------------
	// next is index 0:
	_vortexFloatParams.push_back(0.0f); // lower limit damping
	_vortexFloatParams.push_back(0.0f); // upper limit damping
	_vortexFloatParams.push_back(-1.0f); // lower limit stiffness
	_vortexFloatParams.push_back(-1.0f); // upper limit stiffness
	_vortexFloatParams.push_back(0.0f); // lower limit restitution
	_vortexFloatParams.push_back(0.0f); // upper limit restitution
	_vortexFloatParams.push_back(-1.0f); // lower limit max force
	_vortexFloatParams.push_back(-1.0f); // upper limit max force
	_vortexFloatParams.push_back(0.001f); // motor constraint friction coeff
	_vortexFloatParams.push_back(10.0f); // motor constraint friction max force
	// next is index 10:
	_vortexFloatParams.push_back(0.0f); // motor constraint friction loss
	_vortexFloatParams.push_back(0.0f); // P0 loss
	_vortexFloatParams.push_back(-1.0f); // P0 stiffness
	_vortexFloatParams.push_back(0.0f); // P0 damping
	_vortexFloatParams.push_back(0.0f); // P0 friction coeff
	_vortexFloatParams.push_back(0.0f); // P0 friction max force
	_vortexFloatParams.push_back(0.0f); // P0 friction loss
	_vortexFloatParams.push_back(0.0f); // P1 loss
	_vortexFloatParams.push_back(-1.0f); // P1 stiffness
	_vortexFloatParams.push_back(0.0f); // P1 damping
	// next is index 20:
	_vortexFloatParams.push_back(0.0f); // P1 friction coeff
	_vortexFloatParams.push_back(0.0f); // P1 friction max force
	_vortexFloatParams.push_back(0.0f); // P1 friction loss
	_vortexFloatParams.push_back(0.0f); // P2 loss
	_vortexFloatParams.push_back(-1.0f); // P2 stiffness
	_vortexFloatParams.push_back(0.0f); // P2 damping
	_vortexFloatParams.push_back(0.0f); // P2 friction coeff
	_vortexFloatParams.push_back(0.0f); // P2 friction max force
	_vortexFloatParams.push_back(0.0f); // P2 friction loss
	_vortexFloatParams.push_back(0.0f); // A0 loss
	// next is index 30:
	_vortexFloatParams.push_back(-1.0f); // A0 stiffness
	_vortexFloatParams.push_back(0.0f); // A0 damping
	_vortexFloatParams.push_back(0.0f); // A0 friction coeff
	_vortexFloatParams.push_back(0.0f); // A0 friction max force
	_vortexFloatParams.push_back(0.0f); // A0 friction loss
	_vortexFloatParams.push_back(0.0f); // A1 loss
	_vortexFloatParams.push_back(-1.0f); // A1 stiffness
	_vortexFloatParams.push_back(0.0f); // A1 damping
	_vortexFloatParams.push_back(0.0f); // A1 friction coeff
	_vortexFloatParams.push_back(0.0f); // A1 friction max force
	// next is index 40:
	_vortexFloatParams.push_back(0.0f); // A1 friction loss
	_vortexFloatParams.push_back(0.0f); // A2 loss
	_vortexFloatParams.push_back(-1.0f); // A2 stiffness
	_vortexFloatParams.push_back(0.0f); // A2 damping
	_vortexFloatParams.push_back(0.0f); // A2 friction coeff
	_vortexFloatParams.push_back(0.0f); // A2 friction max force
	_vortexFloatParams.push_back(0.0f); // A2 friction loss
	// VORTEX_JOINT_FLOAT_PARAM_CNT_2=47

	// next is index 0:
	_vortexIntParams.push_back(2); // bit coded: 0set: motor friction enabled, 1set: proportional motor friction
	_vortexIntParams.push_back(0); // bit coded: 0set: relaxationP0 enabled, 1set: relaxationP1 enabled, etc.
	_vortexIntParams.push_back(0); // bit coded: 0set: P0 friction enabled, 1set: P1 friction enabled, etc.
	_vortexIntParams.push_back(1+2+4+8+16+32); // bit coded: 0set: P0 friction proportional, 1set: P1 friction proportional, etc.
	// VORTEX_JOINT_INT_PARAM_CNT_2=4
	// ----------------------------------------------------


	_objectName="default";
	_ikWeight=1.0f;

	_jointCallbackCallOrder=0;
	_explicitHandling=false;
	_unlimitedAcceleration=false;
	_invertTargetVelocityAtLimits=true;
	_diameter=0.02f;
	_length=0.15f;
	colorPart1.setDefaultValues();
	colorPart1.setColor(1.0f,0.2f,0.0f,AMBIENT_MODE);
	colorPart2.setDefaultValues();
	colorPart2.setColor(0.0f,0.0f,0.6f,AMBIENT_MODE);
	layer=JOINT_LAYER;
	_mainPropertyLocal=0;

	_objectName=IDSOGL_JOINT;
	if (_jointType==sim_joint_revolute_subtype)
		_objectName=IDSOGL_REVOLUTE_JOINT;
	if (_jointType==sim_joint_prismatic_subtype)
		_objectName=IDSOGL_PRISMATIC_JOINT;
	if (_jointType==sim_joint_spherical_subtype)
		_objectName=IDSOGL_SPHERICAL_JOINT;
}

CJoint::~CJoint()
{

}

void CJoint::setHybridFunctionality(bool h)
{
	if ( (_jointType==sim_joint_spherical_subtype)||(_jointMode==sim_jointmode_force) )
		return;
	_jointHasHybridFunctionality=h;
	if (h)
	{
		_dynamicMotorEnabled=true;
		_dynamicMotorControlLoopEnabled=true;
		if (_jointType==sim_joint_revolute_subtype)
		{
			_screwPitch=0.0f;
// removed on 18/11/2012: we apply now an offset on the physics engine side			_jointMinPosition=tt::getLimitedFloat(-piValue,piValue,_jointMinPosition);
			_jointPositionRange=tt::getLimitedFloat(0.0f,piValTimes2,_jointPositionRange);
			setPosition(getPosition()); // to make sure it is within range (will also get/set velocity)
		}
	}
}

void CJoint::setDynamicSecondPartIsValid(bool v)
{
	_dynamicSecondPartIsValid=v;
}

void CJoint::setDynamicSecondPartLocalTransform(const C7Vector& tr)
{
	_dynamicSecondPartLocalTransform=tr;
}

void CJoint::getDynamicJointErrors(float& linear,float& angular)
{
	linear=0.0f;
	angular=0.0f;
	if (!_dynamicSecondPartIsValid)
		return;
	if (_jointType==sim_joint_revolute_subtype)
	{
		linear=_dynamicSecondPartLocalTransform.X.getLength();
		angular=C3Vector::unitZVector.getAngle(_dynamicSecondPartLocalTransform.Q.getMatrix().axis[2]);
	}
	if (_jointType==sim_joint_prismatic_subtype)
	{
		float l=_dynamicSecondPartLocalTransform.X*C3Vector::unitZVector;
		linear=sqrt(_dynamicSecondPartLocalTransform.X*_dynamicSecondPartLocalTransform.X-l*l);
		C4Vector idQuat;
		idQuat.setIdentity();
		angular=_dynamicSecondPartLocalTransform.Q.getAngleBetweenQuaternions(idQuat);
	}
	if (_jointType==sim_joint_spherical_subtype)
		linear=_dynamicSecondPartLocalTransform.X.getLength();
}

void CJoint::getBulletParameters(float& bulletStopERP,float& bulletStopCFM,float& bulletNormalCFM)
{
	bulletStopERP=_bulletStopERP;
	bulletStopCFM=_bulletStopCFM;
	bulletNormalCFM=_bulletNormalCFM;
}

void CJoint::setBulletParameters(float bulletStopERP,float bulletStopCFM,float bulletNormalCFM)
{
	_bulletStopERP=bulletStopERP;
	_bulletStopCFM=bulletStopCFM;
	_bulletNormalCFM=bulletNormalCFM;
}

void CJoint::getOdeParameters(float& odeStopERP,float& odeStopCFM,float& odeBounce,float& odeFudgeFactor,float& odeNormalCFM)
{
	odeStopERP=_odeStopERP;
	odeStopCFM=_odeStopCFM;
	odeBounce=_odeBounce;
	odeFudgeFactor=_odeFudgeFactor;
	odeNormalCFM=_odeNormalCFM;
}

void CJoint::setOdeParameters(float odeStopERP,float odeStopCFM,float odeBounce,float odeFudgeFactor,float odeNormalCFM)
{
	_odeStopERP=odeStopERP;
	_odeStopCFM=odeStopCFM;
	_odeBounce=odeBounce;
	_odeFudgeFactor=odeFudgeFactor;
	_odeNormalCFM=odeNormalCFM;
}

void CJoint::getVortexFloatParams(std::vector<float>& p)
{
	p.assign(_vortexFloatParams.begin(),_vortexFloatParams.end());
}

void CJoint::setVortexFloatParams(const std::vector<float>& p)
{
	_vortexFloatParams.assign(p.begin(),p.end());
	// Forbid zero stiffness for relaxation axes:
	if (_vortexFloatParams[12]==0.0f)
		_vortexFloatParams[12]=1e-35;
	if (_vortexFloatParams[18]==0.0f)
		_vortexFloatParams[18]=1e-35;
	if (_vortexFloatParams[24]==0.0f)
		_vortexFloatParams[24]=1e-35;
	if (_vortexFloatParams[30]==0.0f)
		_vortexFloatParams[30]=1e-35;
	if (_vortexFloatParams[36]==0.0f)
		_vortexFloatParams[36]=1e-35;
	if (_vortexFloatParams[42]==0.0f)
		_vortexFloatParams[42]=1e-35;
}

void CJoint::getVortexIntParams(std::vector<int>& p)
{
	p.assign(_vortexIntParams.begin(),_vortexIntParams.end());
}

void CJoint::setVortexIntParams(const std::vector<int>& p)
{
	_vortexIntParams.assign(p.begin(),p.end());
}

void CJoint::copyEnginePropertiesTo(CJoint* target)
{
	// Bullet:
	target->_bulletStopERP=_bulletStopERP;
	target->_bulletStopCFM=_bulletStopCFM;
	target->_bulletNormalCFM=_bulletNormalCFM;

	// ODE:
	target->_odeStopERP=_odeStopERP;
	target->_odeStopCFM=_odeStopCFM;
	target->_odeBounce=_odeBounce;
	target->_odeFudgeFactor=_odeFudgeFactor;
	target->_odeNormalCFM=_odeNormalCFM;

	// Vortex:
	target->_vortexFloatParams.assign(_vortexFloatParams.begin(),_vortexFloatParams.end());
	target->_vortexIntParams.assign(_vortexIntParams.begin(),_vortexIntParams.end());
}

void CJoint::setEnableDynamicMotor(bool e)
{
	_dynamicMotorEnabled=e;
}

bool CJoint::getEnableDynamicMotor()
{
	return(_dynamicMotorEnabled);
}

void CJoint::setDynamicMotorTargetVelocity(float v)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	if (_jointType==sim_joint_revolute_subtype)
		v=tt::getLimitedFloat(-90000.0f*degToRad,+90000.0f*degToRad,v); // 250 rot/sec --> 0.25 rot/1ms
	if (_jointType==sim_joint_prismatic_subtype)
		v=tt::getLimitedFloat(-100.0f,+100.0f,v); // 100 m/sec --> 0.1 m/1ms
	_dynamicMotorTargetVelocity=v;
}

float CJoint::getDynamicMotorTargetVelocity()
{
	return(_dynamicMotorTargetVelocity);
}

void CJoint::setDynamicMotorUpperLimitVelocity(float v)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	v=tt::getLimitedFloat(0.01f,20.0f,v);
	_dynamicMotorUpperLimitVelocity=v;
}

float CJoint::getDynamicMotorUpperLimitVelocity()
{
	return(_dynamicMotorUpperLimitVelocity);
}

void CJoint::setDynamicMotorMaximumForce(float f)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	if (_jointType==sim_joint_revolute_subtype)
		f=tt::getLimitedFloat(0.0f,+10000000.0f,f);
	if (_jointType==sim_joint_prismatic_subtype)
		f=tt::getLimitedFloat(0.0f,+1000000.0f,f);
	_dynamicMotorMaximumForce=f;
}

float CJoint::getDynamicMotorMaximumForce()
{
	return(_dynamicMotorMaximumForce);
}

void CJoint::setEnableDynamicMotorControlLoop(bool p)
{
	_dynamicMotorControlLoopEnabled=p;
}

bool CJoint::getEnableDynamicMotorControlLoop()
{
	return(_dynamicMotorControlLoopEnabled);
}

void CJoint::setEnableDynamicMotorCustomControl(bool c,const char* scriptContent)
{
	_dynamicMotorCustomControl=c;

	// We remove a script that might be associated:
	CLuaScriptObject* script=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_callback(getID());
	if (script)
		App::ct->luaScriptContainer->removeScript(script->getScriptID());

	if (_dynamicMotorCustomControl)
	{ // we have to add a script
		CLuaScriptObject* script=new CLuaScriptObject(false);
		script->setScriptType(sim_scripttype_jointctrlcallback);
		if (scriptContent)
			script->setScriptText(scriptContent);
		App::ct->luaScriptContainer->insertScript(script);
		script->setObjectIDThatScriptIsAttachedTo_callback(getID());
	}
}

bool CJoint::getEnableDynamicMotorCustomControl()
{
	return(_dynamicMotorCustomControl);
}

void CJoint::setEnableTorqueModulation(bool p)
{
	_dynamicMotorPositionControl_torqueModulation=p;
}

bool CJoint::getEnableTorqueModulation()
{
	return(_dynamicMotorPositionControl_torqueModulation);
}

void CJoint::setDynamicMotorPositionControlParameters(float p_param,float i_param,float d_param)
{
	_dynamicMotorPositionControl_P=tt::getLimitedFloat(-1000.0f,1000.0f,p_param);
	_dynamicMotorPositionControl_I=tt::getLimitedFloat(-1000.0f,1000.0f,i_param);
	_dynamicMotorPositionControl_D=tt::getLimitedFloat(-1000.0f,1000.0f,d_param);
}

void CJoint::getDynamicMotorPositionControlParameters(float& p_param,float& i_param,float& d_param)
{
	p_param=_dynamicMotorPositionControl_P;
	i_param=_dynamicMotorPositionControl_I;
	d_param=_dynamicMotorPositionControl_D;
}

void CJoint::setDynamicMotorSpringControlParameters(float k_param,float c_param)
{
	float maxVal=+1000000.0f;
	if (_jointType==sim_joint_revolute_subtype)
		maxVal=+10000000.0f;

	_dynamicMotorSpringControl_K=tt::getLimitedFloat(-maxVal,maxVal,k_param);
	_dynamicMotorSpringControl_C=tt::getLimitedFloat(-maxVal,maxVal,c_param);
}

void CJoint::getDynamicMotorSpringControlParameters(float& k_param,float& c_param)
{
	k_param=_dynamicMotorSpringControl_K;
	c_param=_dynamicMotorSpringControl_C;
}

void CJoint::setDynamicMotorPositionControlTargetPosition(float pos)
{
	if (_jointType==sim_joint_revolute_subtype)
	{
		if (_positionIsCyclic)
			_dynamicMotorPositionControl_targetPosition=tt::getNormalizedAngle(pos);
		else
			_dynamicMotorPositionControl_targetPosition=pos;
	}
	if (_jointType==sim_joint_prismatic_subtype)
		_dynamicMotorPositionControl_targetPosition=pos;
}

float CJoint::getDynamicMotorPositionControlTargetPosition()
{
	return(_dynamicMotorPositionControl_targetPosition);
}

void CJoint::setDynamicMotorReflectedPosition_useOnlyFromDynamicPart(float rfp)
{
	{	// When the joint is in dynamic mode we disable the joint limits and allow a cyclic behaviour (revolute joints)
		// This is because dynamic joints can over or undershoot limits
		_jointPosition=rfp;
	}
	_rectifyDependentJoints();
}

void CJoint::setDependencyJointID(int depJointID)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	_dependencyJointID=depJointID;
	if (depJointID==-1)
	{ // We disable the dependency:
		_dependencyJointID=-1;
		App::ct->objCont->actualizeObjectInformation();
	}
	else
	{
		// We now check for an illegal loop:
		CJoint* it=App::ct->objCont->getJoint(depJointID);
		CJoint* iterat=it;
		while (iterat->getDependencyJointID()!=-1)
		{
			if (iterat->getJointMode()!=_jointMode)
				break; // We might have a loop, but it is interupted by another jointMode!! (e.g. IK dependency VS direct dependency)
			int joint=iterat->getDependencyJointID();
			if (joint==getID())
			{ // We have an illegal loop! We disable it:
				iterat->setDependencyJointID(-1);
				break;
			}
			iterat=App::ct->objCont->getJoint(joint);
		}
		App::ct->objCont->actualizeObjectInformation();
		setPosition(getPosition()); // To actualize dependencies
	}
}

void CJoint::setDependencyJointCoeff(float coeff)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	coeff=tt::getLimitedFloat(-10000.0f,10000.0f,coeff);
	_dependencyJointCoeff=coeff;
	setPosition(getPosition()); // To actualize dependencies
}

void CJoint::setDependencyJointFact(float fact)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	fact=tt::getLimitedFloat(-10000.0f,10000.0f,fact);
	_dependencyJointFact=fact;
	setPosition(getPosition()); // To actualize dependencies
}

void CJoint::setExplicitHandling(bool explicitHandl)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	_explicitHandling=explicitHandl;
}

bool CJoint::getExplicitHandling()
{
	return(_explicitHandling);
}

void CJoint::setUnlimitedAcceleration(bool unlimited)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	_unlimitedAcceleration=unlimited;
	setVelocity(getVelocity()); // To make sure velocity is within allowed range
}

bool CJoint::getUnlimitedAcceleration()
{
	return(_unlimitedAcceleration);
}

void CJoint::setInvertTargetVelocityAtLimits(bool invert)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	_invertTargetVelocityAtLimits=invert;
}

bool CJoint::getInvertTargetVelocityAtLimits()
{
	return(_invertTargetVelocityAtLimits);
}
void CJoint::setMaxAcceleration(float maxAccel)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	if (_jointType==sim_joint_prismatic_subtype)
		tt::limitValue(0.0001f,1000.0f,maxAccel);
	else
		tt::limitValue(0.001f*degToRad,36000.0f*degToRad,maxAccel);
	_maxAcceleration=maxAccel;
	setVelocity(getVelocity()); // To make sure velocity is within allowed range
}

float CJoint::getMaxAcceleration()
{
	return(_maxAcceleration);
}

void CJoint::setVelocity(float vel)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	if ( (vel!=0.0f)&&((_jointType==sim_joint_prismatic_subtype)||(!_positionIsCyclic))&&(!_unlimitedAcceleration) )
	{ // We check which is the max allowed:
		float m=float(CLinMotionRoutines::getMaxVelocityAtPosition(_jointPosition,_maxAcceleration,_jointMinPosition,_jointMinPosition+_jointPositionRange,0.0f,0.0f));
		tt::limitValue(-m,m,vel);		
	}
	_velocity=vel;
}

float CJoint::getVelocity()
{
	return(_velocity);
}

void CJoint::setTargetVelocity(float vel)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	if (_jointType==sim_joint_prismatic_subtype)
		tt::limitValue(-1000.0f,1000.0f,vel);
	else
		tt::limitValue(-36000.0f*degToRad,36000.0f*degToRad,vel);
	_targetVelocity=vel;
}

float CJoint::getTargetVelocity()
{
	return(_targetVelocity);
}

void CJoint::measureJointVelocity(float dt)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	if (_positionIsCyclic)
		_measuredJointVelocity_velocityMeasurement=tt::getAngleMinusAlpha(_jointPosition,_previousJointPosition_velocityMeasurement)/dt;
	else
		_measuredJointVelocity_velocityMeasurement=(_jointPosition-_previousJointPosition_velocityMeasurement)/dt;
	_previousJointPosition_velocityMeasurement=_jointPosition;
}

float CJoint::getMeasuredJointVelocity()
{
	return(_measuredJointVelocity_velocityMeasurement);
}

void CJoint::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	initializeInitialValuesMain(simulationIsRunning);
	_initialValuesInitialized=simulationIsRunning;
	_dynamicSecondPartIsValid=false; // do the same as for force sensors here?! (if the joint is copied while apart, paste it apart too!)
	_measuredJointVelocity_velocityMeasurement=0.0f;
	_previousJointPosition_velocityMeasurement=_jointPosition;
	if (simulationIsRunning)
	{
		_initialPosition=_jointPosition;
		_initialSphericalJointTransformation=_sphericalTransformation;
		_jointPositionForMotionHandling=_jointPosition;
		_velocity=0.0f;
		_initialVelocity=_velocity;
		_initialTargetVelocity=_targetVelocity;

		_initialDynamicMotorEnabled=_dynamicMotorEnabled;
		_initialDynamicMotorTargetVelocity=_dynamicMotorTargetVelocity;
		_initialDynamicMotorUpperLimitVelocity=_dynamicMotorUpperLimitVelocity;
		_initialDynamicMotorMaximumForce=_dynamicMotorMaximumForce;

		_initialDynamicMotorControlLoopEnabled=_dynamicMotorControlLoopEnabled;
		_initialDynamicMotorPositionControl_P=_dynamicMotorPositionControl_P;
		_initialDynamicMotorPositionControl_I=_dynamicMotorPositionControl_I;
		_initialDynamicMotorPositionControl_D=_dynamicMotorPositionControl_D;
		_initialDynamicMotorSpringControl_K=_dynamicMotorSpringControl_K;
		_initialDynamicMotorSpringControl_C=_dynamicMotorSpringControl_C;
		_initialTargetPosition=_dynamicMotorPositionControl_targetPosition;

		_initialJointMode=_jointMode;
		_initialHybridOperation=_jointHasHybridFunctionality;

		_initialExplicitHandling=_explicitHandling;

		_initialJointCallbackCallOrder=_jointCallbackCallOrder;

		_cumulatedForceOrTorque=0.0f;
		_cumulativeForceOrTorqueTmp=0.0f;
	}
}

void CJoint::simulationAboutToStart()
{
	initializeInitialValues(true);
}

void CJoint::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
	{
		_jointPosition=_initialPosition;
		_sphericalTransformation=_initialSphericalJointTransformation;

		_velocity=_initialVelocity;
		_targetVelocity=_initialTargetVelocity;

		_dynamicMotorEnabled=_initialDynamicMotorEnabled;
		_dynamicMotorTargetVelocity=_initialDynamicMotorTargetVelocity;
		_dynamicMotorUpperLimitVelocity=_initialDynamicMotorUpperLimitVelocity;
		_dynamicMotorMaximumForce=_initialDynamicMotorMaximumForce;

		_dynamicMotorControlLoopEnabled=_initialDynamicMotorControlLoopEnabled;
		_dynamicMotorPositionControl_P=_initialDynamicMotorPositionControl_P;
		_dynamicMotorPositionControl_I=_initialDynamicMotorPositionControl_I;
		_dynamicMotorPositionControl_D=_initialDynamicMotorPositionControl_D;
		_dynamicMotorSpringControl_K=_initialDynamicMotorSpringControl_K;
		_dynamicMotorSpringControl_C=_initialDynamicMotorSpringControl_C;
		_dynamicMotorPositionControl_targetPosition=_jointPosition;
		_dynamicMotorPositionControl_targetPosition=_initialTargetPosition;

		_jointMode=_initialJointMode;
		_jointHasHybridFunctionality=_initialHybridOperation;

		_explicitHandling=_initialExplicitHandling;

		_jointCallbackCallOrder=_initialJointCallbackCallOrder;
	}
	_dynamicSecondPartIsValid=false;
	_initialValuesInitialized=false;

	_cumulatedForceOrTorque=0.0f;
	_cumulativeForceOrTorqueTmp=0.0f;

	simulationEndedMain();
}

void CJoint::resetJoint()
{
	if ( (_jointMode!=sim_jointmode_motion)||(!App::ct->mainSettings->jointMotionHandlingEnabled) )
		return;
	if (_initialValuesInitialized)
	{
		setPosition(_initialPosition);
		setVelocity(_initialVelocity);
		setTargetVelocity(_initialTargetVelocity);
	}
}

void CJoint::handleJoint(float deltaTime)
{ // handling the motion here. Not elegant at all. In future, try using the Reflexxes RML library!
	if ( (_jointMode!=sim_jointmode_motion)||(!App::ct->mainSettings->jointMotionHandlingEnabled) )
		return;
	if (_unlimitedAcceleration)
	{
		_velocity=_targetVelocity;
		if (_velocity!=0.0f)
		{
			float newPos=_jointPositionForMotionHandling;
			if (!_positionIsCyclic)
			{
				if (_invertTargetVelocityAtLimits)
				{
					float cycleTime=2.0f*_jointPositionRange/_velocity;
					deltaTime=CMath::robustFmod(deltaTime,cycleTime);
					while (true)
					{
						_velocity=_targetVelocity;
						float absDist=_jointMinPosition+_jointPositionRange-newPos;
						if (_velocity<0.0f)
							absDist=newPos-_jointMinPosition;
						if (absDist>fabs(_velocity)*deltaTime)
						{
							newPos+=_velocity*deltaTime;
							break; // We reached the desired deltaTime
						}
						if (_velocity<0.0f)
							newPos-=absDist;
						else
							newPos+=absDist;
						deltaTime-=absDist/fabs(_velocity);
						_targetVelocity*=-1.0f; // We invert the target velocity
					}
				}
				else
				{
					newPos+=_velocity*deltaTime;
					float dv=newPos-(_jointMinPosition+_jointPositionRange);
					float dl=_jointMinPosition-newPos;
					if ( (dl>=0.0f)||(dv>=0.0f) )
						_velocity=0.0f;
				}
			}
			else
				newPos+=_velocity*deltaTime;
			setPosition(newPos);
			_jointPositionForMotionHandling=getPosition();
		}
	}
	else
	{ // Acceleration is not infinite!
		double newPos=double(_jointPositionForMotionHandling);
		float minV=-SIM_MAX_FLOAT;
		float maxV=+SIM_MAX_FLOAT;
		if (!_positionIsCyclic)
		{
			minV=_jointMinPosition;
			maxV=_jointMinPosition+_jointPositionRange;
			// Make sure we are within limits:
			float m=float(CLinMotionRoutines::getMaxVelocityAtPosition(newPos,_maxAcceleration,minV,maxV,0.0f,0.0f));
			tt::limitValue(-m,m,_velocity);
		}

		double velocityDouble=double(_velocity);
		double deltaTimeDouble=double(deltaTime);
		while (CLinMotionRoutines::getNextValues(newPos,velocityDouble,_targetVelocity,_maxAcceleration,minV,maxV,0.0f,0.0f,deltaTimeDouble))
		{
			if (_invertTargetVelocityAtLimits)
				_targetVelocity*=-1.0f;
			else
			{
				deltaTime=0.0f;
				deltaTimeDouble=0.0;
				break;
			}
		}
		_velocity=float(velocityDouble);

		setPosition(float(newPos));
		_jointPositionForMotionHandling=getPosition();
	}
}

std::string CJoint::getObjectTypeInfo()
{
	return(IDSOGL_JOINT);
}
std::string CJoint::getObjectTypeInfoExtended()
{
	std::string retVal(IDSOGL_JOINT);
	if (_jointType==sim_joint_revolute_subtype)
	{
		if (fabs(_screwPitch)<0.0000001f)
			retVal+=tt::decorateString(" (",IDSOGL_REVOLUTE,", p=");
		else
			retVal+=tt::decorateString(" (",IDSOGL_SCREW,", p=");
		retVal+=gv::getAngleStr(true,_jointPosition)+")";
	}
	if (_jointType==sim_joint_prismatic_subtype)
	{
		retVal+=tt::decorateString(" (",IDSOGL_PRISMATIC,", p=");
		retVal+=gv::getSizeStr(true,_jointPosition)+")";
	}
	if (_jointType==sim_joint_spherical_subtype)
	{
		retVal+=tt::decorateString(" (",IDSOGL_SPHERICAL,", a=");
		C3Vector euler(getSphericalTransformation().getEulerAngles());
		retVal+=gv::getAngleStr(true,euler(0))+", b="+gv::getAngleStr(true,euler(1))+", g="+gv::getAngleStr(true,euler(2))+")";
	}
	return(retVal);
}

bool CJoint::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	if (_jointType!=sim_joint_spherical_subtype)
	{
		maxV(0)=maxV(1)=_diameter/2.0f;
		maxV(2)=_length/2.0f;
		minV(0)=-maxV(0);
		minV(1)=-maxV(1);
		minV(2)=-maxV(2);
	}
	else
	{
		maxV(0)=maxV(1)=maxV(2)=_diameter;
		minV(0)=minV(1)=minV(2)=-maxV(0);
	}
	return(true);
}

bool CJoint::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(getFullBoundingBox(minV,maxV));
}

bool CJoint::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	return(false); // for now
}

void CJoint::display(CViewableBase* renderingObject,int displayAttrib)
{
	FUNCTION_INSIDE_DEBUG("CJoint::display");
	EASYLOCK(_objectMutex);
	// At the beginning of every 3DObject display routine:
	commonStart(displayAttrib);

	bool guiIsRendering=((displayAttrib&sim_displayattribute_forvisionsensor)==0);

	// Bounding box display:
	float sizeParam=getLength()/4.0f;
	if (sizeParam<getDiameter()) 
		sizeParam=_diameter;
	if (sizeParam>10.0f*getDiameter()) 
		sizeParam=10.0f*getDiameter();
	if (displayAttrib&sim_displayattribute_renderpass) 
		displayBoundingBox(displayAttrib,true,sizeParam);
	
	// Object display:
	if (getShouldObjectBeDisplayed(displayAttrib))
	{
		App::ct->objCont->enableAuxClippingPlanes(_objectID);
		if (displayAttrib&sim_displayattribute_dynamiccontentonly)
		{
			const float colFree[12]={0.0f,0.2f,1.0f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			const float colMot[12]={1.0f,0.0f,0.0f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			const float colPos[12]={0.6f,0.6f,0.0f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			if (getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering)==4)
				ogl::setColor(colFree,48,0);
			if (getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering)==8)
				ogl::setColor(colMot,48,0);
			if (getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering)==16)
				ogl::setColor(colPos,48,0);
		}
		else
			colorPart1.makeCurrentColor();

		displayJoint(displayAttrib,true,sizeParam);

		if (displayAttrib&sim_displayattribute_dynamiccontentonly)
		{
			const float colFree[12]={0.0f,0.2f,1.0f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			const float colMot[12]={1.0f,0.0f,0.0f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			const float colPos[12]={0.8f,0.8f,0.0f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			if (getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering)==4)
				ogl::setColor(colFree,48,0);
			if (getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering)==8)
				ogl::setColor(colMot,48,0);
			if (getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering)==16)
				ogl::setColor(colPos,48,0);
			const float colBase[12]={0.05f,0.05f,0.05f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			ogl::setColor(colBase,48,0);
		}
		else
			colorPart2.makeCurrentColor();
		displayJoint(displayAttrib,false,sizeParam);
		App::ct->objCont->disableAuxClippingPlanes();
	}

	// At the end of every 3DObject display routine:
	commonFinish();
}
	
void CJoint::displayJoint(int displayAttrib,bool partOne,float sizeParam)
{
	if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
	{
		if (_objectProperty&sim_objectproperty_selectmodelbaseinstead)
			glLoadName(getModelSelectionID());
		else
			glLoadName(getID());
	}
	else
		glLoadName(-1);

	glPushAttrib(GL_POLYGON_BIT);
	if ((displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass))
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	glPushMatrix();
	if (_jointType==sim_joint_spherical_subtype)
	{
		if (partOne)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			ogl::displaySphere(_diameter,16,8,true);
			glDisable(GL_CULL_FACE);
		}
		else
		{
			if (_dynamicSecondPartIsValid&&(!App::ct->simulation->isSimulationStopped()))
			{ // for dynamic mode
				glTranslatef(_dynamicSecondPartLocalTransform.X(0),_dynamicSecondPartLocalTransform.X(1),_dynamicSecondPartLocalTransform.X(2));
				C4Vector axis=_dynamicSecondPartLocalTransform.Q.getAngleAndAxisNoChecking();
				glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
			}
			else
			{
				C4Vector tr(_sphericalTransformation.getAngleAndAxis());
				glRotatef(tr(0)*radToDeg,tr(1),tr(2),tr(3));
			}

			ogl::displaySphere(_diameter/1.5f,16,8,true);
			if (displayAttrib&sim_displayattribute_selected) 
				drawReference(sizeParam);
		}
	}
	if (_jointType==sim_joint_revolute_subtype)
	{
		if (partOne)
			ogl::displayCylinder(_diameter,_length,8);
		else
		{
			if (_dynamicSecondPartIsValid&&(!App::ct->simulation->isSimulationStopped()))
			{ // for dynamic mode
				glTranslatef(_dynamicSecondPartLocalTransform.X(0),_dynamicSecondPartLocalTransform.X(1),_dynamicSecondPartLocalTransform.X(2));
				C4Vector axis=_dynamicSecondPartLocalTransform.Q.getAngleAndAxisNoChecking();
				glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
			}
			else
			{
				glRotatef(getPosition()*radToDeg,0.0f,0.0f,1.0f);
				glTranslatef(0.0f,0.0f,getPosition()*_screwPitch);
			}

			ogl::displayCylinder(_diameter/2.0f,_length*1.2f,8);
			if (displayAttrib&sim_displayattribute_selected)
				drawReference(sizeParam);
		}
	}
	if (_jointType==sim_joint_prismatic_subtype)
	{
		if (partOne)
			ogl::displayBox(_diameter,_diameter,_length,true);
		else
		{
			if (_dynamicSecondPartIsValid&&(!App::ct->simulation->isSimulationStopped()))
			{ // for dynamic mode
				glTranslatef(_dynamicSecondPartLocalTransform.X(0),_dynamicSecondPartLocalTransform.X(1),_dynamicSecondPartLocalTransform.X(2));
				C4Vector axis=_dynamicSecondPartLocalTransform.Q.getAngleAndAxisNoChecking();
				glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
			}
			else
				glTranslatef(0.0f,0.0f,_jointPosition);

			ogl::displayBox(_diameter/2.0f,_diameter/2.0f,_length*1.2f,true);
			if (displayAttrib&sim_displayattribute_selected)
				drawReference(sizeParam);
		}
	}
	glPopAttrib();
	glPopMatrix();
}

void CJoint::setScrewPitch(float pitch)
{
	if (_jointType==sim_joint_revolute_subtype)
	{
		if (_jointMode!=sim_jointmode_force)
		{ // no pitch when in torque/force mode
			pitch=tt::getLimitedFloat(-10.0f,10.0f,pitch);
			_screwPitch=pitch;
			if (_screwPitch!=0.0f)
				_jointHasHybridFunctionality=false;
		}
	}
}

void CJoint::setPositionIntervalMin(float min)
{
	if (_jointType==sim_joint_spherical_subtype)
		return;
	if (_jointType==sim_joint_revolute_subtype)
	{
		min=tt::getLimitedFloat(-100000.0f,100000.0f,min);
	}
	if (_jointType==sim_joint_prismatic_subtype)
		min=tt::getLimitedFloat(-1000.0f,1000.0f,min);
	_jointMinPosition=min;
	setPosition(getPosition()); // to make sure it is within range (will also get/set velocity)
}

void CJoint::setPositionIntervalRange(float range)
{
	if (_jointType==sim_joint_revolute_subtype)
	{

		// Commented following out on 23/3/2014 because:
		// limiting the range is problematic when doing motion planning for instance
		// imagine the angle has a range of 0, +- 360 degrees, which is often the case.
		// We solve the problem by simply having no dynamic limitations when the joint
		// range is larger than 360 degrees.
//		if ((_jointMode!=sim_jointmode_force)&&(!_jointHasHybridFunctionality))
		{
			range=tt::getLimitedFloat(0.001f*degToRad,10000000.0f*degToRad,range);
		}
//		else
//			range=tt::getLimitedFloat(0.0f,piValTimes2,range);

	}
	if (_jointType==sim_joint_prismatic_subtype)
		range=tt::getLimitedFloat(0.0f,1000.0f,range);
	if (_jointType==sim_joint_spherical_subtype)
	{
		if (_jointMode!=sim_jointmode_force)
			range=tt::getLimitedFloat(0.001f*degToRad,10000000.0f*degToRad,range);
		else
			range=piValue;
	}
	_jointPositionRange=range;
	setPosition(getPosition()); // to make sure it is within range (will also get/set velocity)
	setSphericalTransformation(getSphericalTransformation());
}

void CJoint::setLength(float l)
{
	tt::limitValue(0.001f,1000.0f,l);
	_length=l;
}

void CJoint::setDiameter(float d)
{
	tt::limitValue(0.0001f,100.0f,d);
	_diameter=d;
}

float CJoint::getLength() 
{ 
	return(_length); 
}
float CJoint::getDiameter() 
{ 
	return(_diameter); 
}

void CJoint::scaleObject(float scalingFactor)
{
	_diameter*=scalingFactor;
	_length*=scalingFactor;
	_screwPitch*=scalingFactor;
	if (_jointType==sim_joint_prismatic_subtype)
	{
		_jointPosition*=scalingFactor;
		_jointPositionForMotionHandling*=scalingFactor;
		_jointMinPosition*=scalingFactor;
		_jointPositionRange*=scalingFactor;
		_dependencyJointFact*=scalingFactor;
		_maxStepSize*=scalingFactor;
		_maxAcceleration*=scalingFactor;
		_velocity*=scalingFactor;
		_targetVelocity*=scalingFactor;
		_dynamicMotorPositionControl_targetPosition*=scalingFactor;

		// following 2 new since 7/5/2014:
		_dynamicMotorSpringControl_K*=scalingFactor*scalingFactor;
		_dynamicMotorSpringControl_C*=scalingFactor*scalingFactor;

		_dynamicMotorTargetVelocity*=scalingFactor;
		_dynamicMotorUpperLimitVelocity*=scalingFactor;
		if (_dynamicMotorPositionControl_torqueModulation) // this condition and next line added on 04/10/2013 (Alles Gute zu Geburtstag Mama :) )
			_dynamicMotorMaximumForce*=scalingFactor*scalingFactor; //*scalingFactor; removed one on 2010/02/17 b/c often working against gravity which doesn't change
		else
			_dynamicMotorMaximumForce*=scalingFactor*scalingFactor*scalingFactor; //*scalingFactor; removed one on 2010/02/17 b/c often working against gravity which doesn't change
		// Following removed on 04/10/2013. Why did we have this?!
		//_dynamicMotorPositionControl_targetPosition=_jointPosition;

		if (_initialValuesInitialized)
		{
			_initialPosition*=scalingFactor;
			_initialVelocity*=scalingFactor;
			_initialTargetVelocity*=scalingFactor;
			_initialTargetPosition*=scalingFactor;

			_initialDynamicMotorTargetVelocity*=scalingFactor;
			_initialDynamicMotorUpperLimitVelocity*=scalingFactor;
			if (_dynamicMotorPositionControl_torqueModulation) // this condition and next line added on 04/10/2013 (Alles Gute zu Geburtstag Mama :) )
				_initialDynamicMotorMaximumForce*=scalingFactor*scalingFactor;//*scalingFactor; removed one on 2010/02/17 b/c often working against gravity which doesn't change
			else
				_initialDynamicMotorMaximumForce*=scalingFactor*scalingFactor*scalingFactor;//*scalingFactor; removed one on 2010/02/17 b/c often working against gravity which doesn't change

			// following 2 new since 7/5/2014:
			_initialDynamicMotorSpringControl_K*=scalingFactor*scalingFactor;
			_initialDynamicMotorSpringControl_C*=scalingFactor*scalingFactor;
		}
	}

	if (_jointType==sim_joint_revolute_subtype)
	{
		_dynamicMotorMaximumForce*=scalingFactor*scalingFactor*scalingFactor*scalingFactor;//*scalingFactor; removed one on 2010/02/17 b/c often working against gravity which doesn't change

		// following 2 new since 7/5/2014:
		_dynamicMotorSpringControl_K*=scalingFactor*scalingFactor*scalingFactor*scalingFactor;
		_dynamicMotorSpringControl_C*=scalingFactor*scalingFactor*scalingFactor*scalingFactor;

		if (_initialValuesInitialized)
		{
			_initialDynamicMotorMaximumForce*=scalingFactor*scalingFactor*scalingFactor*scalingFactor;//*scalingFactor; removed one on 2010/02/17 b/c often working against gravity which doesn't change
			// following 2 new since 7/5/2014:
			_initialDynamicMotorSpringControl_K*=scalingFactor*scalingFactor*scalingFactor*scalingFactor;
			_initialDynamicMotorSpringControl_C*=scalingFactor*scalingFactor*scalingFactor*scalingFactor;
		}
	}

	scaleObjectMain(scalingFactor);

	// We have to reconstruct a part of the dynamics world:
	_dynamicsFullRefreshFlag=true;
}

void CJoint::addCumulativeForceOrTorque(float forceOrTorque,int countForAverage)
{ // New routine since 1/6/2011. The countForAverage mechanism is needed because we need to average all values in a simulation time step (but this is called every dynamic simulation time step!!)
	_cumulativeForceOrTorqueTmp+=forceOrTorque;
	if (countForAverage>0)
	{
		_cumulatedForceOrTorque=_cumulativeForceOrTorqueTmp/float(countForAverage);
		_cumulativeForceOrTorqueTmp=0.0f;
	}
}

bool CJoint::getDynamicForceOrTorque(float& forceOrTorque)
{
	if (!_dynamicSecondPartIsValid)
		return(false);
	forceOrTorque=_cumulatedForceOrTorque;
	return(true);
}

void CJoint::setJointCallbackCallOrder(int order)
{
	_jointCallbackCallOrder=order;
	tt::limitValue(-1,1,_jointCallbackCallOrder);
}

int CJoint::getJointCallbackCallOrder()
{
	return(_jointCallbackCallOrder);
}

void CJoint::handleDynJointControl(bool init,int loopCnt,int totalLoops,float currentPos,float effort,float dynStepSize,float errorV,float& velocity,float& forceTorque)
{
	bool spring=_dynamicMotorPositionControl_torqueModulation;
	bool rev=(_jointType==sim_joint_revolute_subtype);
	bool cycl=_positionIsCyclic;
	float lowL=_jointMinPosition;
	float highL=_jointMinPosition+_jointPositionRange;
	float targetPos=_dynamicMotorPositionControl_targetPosition;
	float targetVel=_dynamicMotorTargetVelocity;
	float maxForce=_dynamicMotorMaximumForce;
	float upperLimitVel=_dynamicMotorUpperLimitVelocity;

	// We check if a plugin wants to handle the joint controller:
	bool handleJointHere=true;
	int callbackCount=CIloIlo::allJointCtrlCallbacks.size();
	if (callbackCount!=0)
	{
		int intParams[3];
		float floatParams[10];
		float retParams[2];
		intParams[0]=0;
		if (init)
			intParams[0]|=1;
		if (rev)
			intParams[0]|=2;
		if (cycl)
			intParams[0]|=4;
		intParams[1]=loopCnt;
		intParams[2]=totalLoops;
		floatParams[0]=currentPos;
		floatParams[1]=targetPos;
		floatParams[2]=errorV;
		floatParams[3]=effort;
		floatParams[4]=dynStepSize;
		floatParams[5]=lowL;
		floatParams[6]=highL;
		floatParams[7]=targetVel;
		floatParams[8]=maxForce;
		floatParams[9]=upperLimitVel;
		for (int i=0;i<callbackCount;i++)
		{
			int res=((jointCtrlCallback)CIloIlo::allJointCtrlCallbacks[i])(getID(),App::ct->dynamicsContainer->getDynamicEngineType(),0,intParams,floatParams,retParams);
			if (res==0)
			{ // override... we don't want any control on this joint (free joint)
				forceTorque=0.0f;
				velocity=0.0f;
				handleJointHere=false;
				break;
			}
			if (res>0)
			{ // override... we use control values provided by the callback
				forceTorque=retParams[0];
				velocity=retParams[1];

				handleJointHere=false;
				break;
			}
		}
	}

	if (handleJointHere)
	{ // The plugins didn't want to handle that joint
		if (!_dynamicMotorCustomControl)
		{ // we have the built-in control (position PID or spring-damper KC)

			// Following 9 new since 7/5/2014:
			float P=_dynamicMotorPositionControl_P;
			float I=_dynamicMotorPositionControl_I;
			float D=_dynamicMotorPositionControl_D;
			if (spring)
			{
				P=_dynamicMotorSpringControl_K/maxForce;
				I=0.0f;
				D=_dynamicMotorSpringControl_C/maxForce;
			}

			if (init)
				_dynamicMotorPIDCumulativeErrorForIntegralParameter=0.0f;

			float e=errorV;

			// Proportional part:
			float ctrl=e*P;

			// Integral part:
			if (I!=0.0f) // so that if we turn the integral part on, we don't try to catch up all the past errors!
				_dynamicMotorPIDCumulativeErrorForIntegralParameter+=e*dynStepSize; // '*dynStepSize'  was forgotten and added on 7/5/2014. The I term is corrected during load operation.
			else
				_dynamicMotorPIDCumulativeErrorForIntegralParameter=0.0f; // added on 2009/11/29
			ctrl+=_dynamicMotorPIDCumulativeErrorForIntegralParameter*I;

			// Derivative part:
			if (!init) // this condition was forgotten. Added on 7/5/2014
				ctrl+=(e-_dynamicMotorPIDLastErrorForDerivativeParameter)*D/dynStepSize; // '/dynStepSize' was forgotten and added on 7/5/2014. The D term is corrected during load operation.
			_dynamicMotorPIDLastErrorForDerivativeParameter=e;

			if (spring)
			{ // "spring" mode, i.e. force modulation mode
				float vel=fabs(targetVel);
				if (ctrl<0.0f)
					vel=-vel;

				forceTorque=fabs(ctrl)*maxForce;

				// Following 2 lines new since 7/5/2014:
				if (forceTorque>maxForce)
					forceTorque=maxForce;

				velocity=vel;
			}
			else
			{ // regular position control (i.e. built-in PID)
				// We calculate the velocity needed to reach the position in one time step:
				float vel=ctrl/dynStepSize;
				float maxVel=upperLimitVel;
				if (vel>maxVel)
					vel=maxVel;
				if (vel<-maxVel)
					vel=-maxVel;

				forceTorque=maxForce;
				velocity=vel;
			}
		}
		else
		{ // we have a custom control here (Lua script)
			CLuaScriptObject* script=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo_callback(getID());
			if (script)
			{
				std::vector<bool> inDataBool;
				std::vector<int> inDataInt;
				std::vector<float> inDataFloat;
				std::vector<float> outData;
				inDataBool.push_back(init);
				inDataBool.push_back(rev);
				inDataBool.push_back(cycl);
				inDataInt.push_back(getID());
				inDataInt.push_back(loopCnt);
				inDataInt.push_back(totalLoops);
				inDataFloat.push_back(currentPos);
				inDataFloat.push_back(targetPos);
				inDataFloat.push_back(errorV);
				inDataFloat.push_back(effort);
				inDataFloat.push_back(dynStepSize);
				inDataFloat.push_back(lowL);
				inDataFloat.push_back(highL);
				inDataFloat.push_back(targetVel);
				inDataFloat.push_back(maxForce);
				inDataFloat.push_back(upperLimitVel);
				script->runJointCtrlCallback(inDataBool,inDataInt,inDataFloat,outData);
				if (outData.size()>=2)
				{
					forceTorque=outData[0];
					velocity=outData[1];
				}
			}
		}
	}
}

void CJoint::setPositionIsCyclic(bool isCyclic)
{
	if (!isCyclic)
		_positionIsCyclic=isCyclic;
	else
	{
		if (getJointType()==sim_joint_revolute_subtype)
		{
			_screwPitch=0.0f;
			_jointMinPosition=-piValue;
			_jointPositionRange=piValTimes2;
			_positionIsCyclic=isCyclic;
		}
	}
	setVelocity(getVelocity()); // To make sure velocity is within allowed range
}

C3DObject* CJoint::copyYourself()
{
	CJoint* newJoint=(CJoint*)copyYourselfMain();

	newJoint->_jointType=_jointType;
	newJoint->_jointMode=_jointMode;
	newJoint->_screwPitch=_screwPitch;
	newJoint->_sphericalTransformation=_sphericalTransformation;
	newJoint->_jointPosition=_jointPosition;
	newJoint->_jointPositionForMotionHandling=_jointPositionForMotionHandling;
	newJoint->_velocity=_velocity;
	newJoint->_targetVelocity=_targetVelocity;
	newJoint->_dependencyJointID=_dependencyJointID;
	newJoint->_dependencyJointCoeff=_dependencyJointCoeff;
	newJoint->_dependencyJointFact=_dependencyJointFact;
	newJoint->_ikWeight=_ikWeight;
	newJoint->_explicitHandling=_explicitHandling;
	newJoint->_unlimitedAcceleration=_unlimitedAcceleration;
	newJoint->_invertTargetVelocityAtLimits=_invertTargetVelocityAtLimits;
	newJoint->_diameter=_diameter;
	newJoint->_length=_length;
	newJoint->_positionIsCyclic=_positionIsCyclic;
	newJoint->_jointPositionRange=_jointPositionRange;
	newJoint->_jointMinPosition=_jointMinPosition;
	newJoint->_maxStepSize=_maxStepSize;
	newJoint->_maxAcceleration=_maxAcceleration;
	colorPart1.copyYourselfInto(&newJoint->colorPart1);
	colorPart2.copyYourselfInto(&newJoint->colorPart2);

	newJoint->_dynamicMotorEnabled=_dynamicMotorEnabled;
	newJoint->_dynamicMotorTargetVelocity=_dynamicMotorTargetVelocity;
	newJoint->_dynamicMotorUpperLimitVelocity=_dynamicMotorUpperLimitVelocity;
	newJoint->_dynamicMotorMaximumForce=_dynamicMotorMaximumForce;

	newJoint->_dynamicMotorControlLoopEnabled=_dynamicMotorControlLoopEnabled;
	newJoint->_dynamicMotorPositionControl_P=_dynamicMotorPositionControl_P;
	newJoint->_dynamicMotorPositionControl_I=_dynamicMotorPositionControl_I;
	newJoint->_dynamicMotorPositionControl_D=_dynamicMotorPositionControl_D;

	newJoint->_dynamicMotorSpringControl_K=_dynamicMotorSpringControl_K;
	newJoint->_dynamicMotorSpringControl_C=_dynamicMotorSpringControl_C;

	newJoint->_dynamicMotorPositionControl_targetPosition=_dynamicMotorPositionControl_targetPosition;
	newJoint->_dynamicMotorPositionControl_torqueModulation=_dynamicMotorPositionControl_torqueModulation;
	newJoint->_jointHasHybridFunctionality=_jointHasHybridFunctionality;
	newJoint->_dynamicMotorPositionControl_targetPosition=_dynamicMotorPositionControl_targetPosition;

	newJoint->_dynamicMotorCustomControl=_dynamicMotorCustomControl;

	newJoint->_odeStopERP=_odeStopERP;
	newJoint->_odeStopCFM=_odeStopCFM;
	newJoint->_odeBounce=_odeBounce;
	newJoint->_odeFudgeFactor=_odeFudgeFactor;
	newJoint->_odeNormalCFM=_odeNormalCFM;

	newJoint->_bulletStopERP=_bulletStopERP;
	newJoint->_bulletStopCFM=_bulletStopCFM;
	newJoint->_bulletNormalCFM=_bulletNormalCFM;

	newJoint->_vortexFloatParams.assign(_vortexFloatParams.begin(),_vortexFloatParams.end());
	newJoint->_vortexIntParams.assign(_vortexIntParams.begin(),_vortexIntParams.end());

	newJoint->_initialValuesInitialized=_initialValuesInitialized;
	newJoint->_initialExplicitHandling=_initialExplicitHandling;

	newJoint->_initialVelocity=_initialVelocity;
	newJoint->_initialPosition=_initialPosition;
	newJoint->_initialSphericalJointTransformation=_initialSphericalJointTransformation;
	newJoint->_initialTargetVelocity=_initialTargetVelocity;
	newJoint->_initialTargetPosition=_initialTargetPosition;
	newJoint->_initialDynamicMotorEnabled=_initialDynamicMotorEnabled;
	newJoint->_initialDynamicMotorTargetVelocity=_initialDynamicMotorTargetVelocity;
	newJoint->_initialDynamicMotorUpperLimitVelocity=_initialDynamicMotorUpperLimitVelocity;
	newJoint->_initialDynamicMotorMaximumForce=_initialDynamicMotorMaximumForce;
	newJoint->_dynamicSecondPartIsValid=_dynamicSecondPartIsValid;
	newJoint->_dynamicSecondPartLocalTransform=_dynamicSecondPartLocalTransform;
	newJoint->_initialDynamicMotorControlLoopEnabled=_initialDynamicMotorControlLoopEnabled;
	newJoint->_initialDynamicMotorPositionControl_P=_initialDynamicMotorPositionControl_P;
	newJoint->_initialDynamicMotorPositionControl_I=_initialDynamicMotorPositionControl_I;
	newJoint->_initialDynamicMotorPositionControl_D=_initialDynamicMotorPositionControl_D;
	newJoint->_initialDynamicMotorSpringControl_K=_initialDynamicMotorSpringControl_K;
	newJoint->_initialDynamicMotorSpringControl_C=_initialDynamicMotorSpringControl_C;
	newJoint->_initialJointMode=_initialJointMode;
	newJoint->_initialHybridOperation=_initialHybridOperation;

	newJoint->_jointCallbackCallOrder=_jointCallbackCallOrder;

	newJoint->_measuredJointVelocity_velocityMeasurement=_measuredJointVelocity_velocityMeasurement;
	newJoint->_previousJointPosition_velocityMeasurement=_previousJointPosition_velocityMeasurement;

	return(newJoint);
}

void CJoint::announceGroupWillBeErased(int groupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGroupWillBeErasedMain(groupID,copyBuffer);
}
void CJoint::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceCollisionWillBeErasedMain(collisionID,copyBuffer);
}
void CJoint::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceDistanceWillBeErasedMain(distanceID,copyBuffer);
}
void CJoint::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGcsObjectWillBeErasedMain(gcsObjectID,copyBuffer);
}

void CJoint::performIkLoadingMapping(std::vector<int>* map)
{
	performIkLoadingMappingMain(map);
}
void CJoint::performGroupLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performGroupLoadingMappingMain(map);
}
void CJoint::performCollisionLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performCollisionLoadingMappingMain(map);
}
void CJoint::performDistanceLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performDistanceLoadingMappingMain(map);
}

void CJoint::performGcsLoadingMapping(std::vector<int>* map)
{
	performGcsLoadingMappingMain(map);
}

void CJoint::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	performTextureObjectLoadingMappingMain(map);
}

void CJoint::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	performDynMaterialObjectLoadingMappingMain(map);
}

void CJoint::bufferMainDisplayStateVariables()
{
	bufferMainDisplayStateVariablesMain();
	_jointPosition_buffered=_jointPosition;
	_sphericalTransformation_buffered=_sphericalTransformation;
}

void CJoint::bufferedMainDisplayStateVariablesToDisplay()
{
	bufferedMainDisplayStateVariablesToDisplayMain();
	_jointPosition_forDisplay=_jointPosition_buffered;
	_sphericalTransformation_forDisplay=_sphericalTransformation_buffered;
}

bool CJoint::getHybridFunctionality()
{
	return(_jointHasHybridFunctionality);
}

float CJoint::getPosition_forDisplay(bool guiIsRendering)
{
	if (guiIsRendering)
		return(_jointPosition_forDisplay);
	else
		return(_jointPosition);
}

C4Vector CJoint::getSphericalTransformation_forDisplay(bool guiIsRendering) const
{
	if (guiIsRendering)
		return(_sphericalTransformation_forDisplay);
	else
		return(_sphericalTransformation);
}


void CJoint::serialize(CSer& ar)
{
	serializeMain(ar);
	if (ar.isStoring())
	{	// Storing. The order in which we are storing is very important!!!!

		ar.storeDataName("Jtt");
		ar << _jointType;
		ar.flush();

		ar.storeDataName("Jsp");
		ar << _screwPitch;
		ar.flush();

		ar.storeDataName("Jst");
		ar << _sphericalTransformation(0) << _sphericalTransformation(1);
		ar << _sphericalTransformation(2) << _sphericalTransformation(3);
		ar.flush();

		ar.storeDataName("Va9");
		BYTE dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,_positionIsCyclic);
		SIM_SET_CLEAR_BIT(dummy,1,_explicitHandling);
		SIM_SET_CLEAR_BIT(dummy,2,_unlimitedAcceleration);
		SIM_SET_CLEAR_BIT(dummy,3,_invertTargetVelocityAtLimits);
		SIM_SET_CLEAR_BIT(dummy,4,_dynamicMotorEnabled);
		SIM_SET_CLEAR_BIT(dummy,5,_dynamicMotorControlLoopEnabled);
		SIM_SET_CLEAR_BIT(dummy,6,_jointHasHybridFunctionality);
		SIM_SET_CLEAR_BIT(dummy,7,_dynamicMotorPositionControl_torqueModulation);
		ar << dummy;
		ar.flush();

		ar.storeDataName("Vaa");
		dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,_dynamicMotorCustomControl);
		ar << dummy;
		ar.flush();

		ar.storeDataName("Cl1");
		ar.setCountingMode();
		colorPart1.serialize(ar);
		if (ar.setWritingMode())
			colorPart1.serialize(ar);

		ar.storeDataName("Cl2");
		ar.setCountingMode();
		colorPart2.serialize(ar);
		if (ar.setWritingMode())
			colorPart2.serialize(ar);

		ar.storeDataName("Pmr");
		ar << _jointMinPosition << _jointPositionRange;
		ar.flush();

		ar.storeDataName("Prt");
		ar << _jointPosition;
		ar.flush();

		ar.storeDataName("Mss");
		ar << _maxStepSize;
		ar.flush();

		ar.storeDataName("Arg");
		ar << _length << _diameter;
		ar.flush();

		ar.storeDataName("Ikw");
		ar << _ikWeight;
		ar.flush();

		ar.storeDataName("Jmd");
		ar << _jointMode;
		ar.flush();

		ar.storeDataName("Jdt");
		ar << _dependencyJointID << _dependencyJointCoeff << _dependencyJointFact;
		ar.flush();

		ar.storeDataName("Jm2");
		ar << _maxAcceleration << _velocity << _targetVelocity;
		ar.flush();

		ar.storeDataName("Dmp");
		ar << _dynamicMotorTargetVelocity << _dynamicMotorMaximumForce;
		ar.flush();

		// Following for backward compatibility (7/5/2014):
		// Keep this before "Dp2"!
		ar.storeDataName("Dpc");
		ar << _dynamicMotorPositionControl_P << (_dynamicMotorPositionControl_I*0.005f) << (_dynamicMotorPositionControl_D/0.005f);
		ar.flush();

		ar.storeDataName("Dp2");
		ar << _dynamicMotorPositionControl_P << _dynamicMotorPositionControl_I << _dynamicMotorPositionControl_D;
		ar.flush();

		ar.storeDataName("Spp");
		ar << _dynamicMotorSpringControl_K << _dynamicMotorSpringControl_C;
		ar.flush();

		ar.storeDataName("Dtp");
		ar << _dynamicMotorPositionControl_targetPosition;
		ar.flush();

		ar.storeDataName("Od1");
		ar << _odeStopERP << _odeStopCFM << _odeBounce << _odeFudgeFactor << _odeNormalCFM;
		ar.flush();

		ar.storeDataName("Bt1");
		ar << _bulletStopERP << _bulletStopCFM << _bulletNormalCFM;
		ar.flush();

		ar.storeDataName("Vo2"); // vortex params:
		ar << int(_vortexFloatParams.size()) << int(_vortexIntParams.size());
		for (int i=0;i<int(_vortexFloatParams.size());i++)
			ar << _vortexFloatParams[i];
		for (int i=0;i<int(_vortexIntParams.size());i++)
			ar << _vortexIntParams[i];
		ar.flush();

		ar.storeDataName("Ulv");
		ar << _dynamicMotorUpperLimitVelocity;
		ar.flush();

		ar.storeDataName("Cco");
		ar << _jointCallbackCallOrder;
		ar.flush();

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		bool dynamicUpperVelocityLimitPresent=false; // for backward compatibility (2010/11/13)
		bool kAndCSpringParameterPresent=false; // for backward compatibility (7/5/2014)
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Prt")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _jointPosition;
				}
				if (theName.compare("Jsp")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _screwPitch;
				}
				if (theName.compare("Jst")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _sphericalTransformation(0) >> _sphericalTransformation(1);
					ar >> _sphericalTransformation(2) >> _sphericalTransformation(3);
					_sphericalTransformation.normalize();
				}
				if (theName.compare("Mss")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _maxStepSize;
				}
				if (theName.compare("Ikw")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _ikWeight;
				}
				if (theName.compare("Cl1")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					colorPart1.serialize(ar);
				}
				if (theName.compare("Cl2")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					colorPart2.serialize(ar);
				}
				if (theName.compare("Arg")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _length >> _diameter;
				}
				if (theName.compare("Pmr")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _jointMinPosition >> _jointPositionRange;
				}
				if (theName.compare("Va9")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					_positionIsCyclic=SIM_IS_BIT_SET(dummy,0);
					_explicitHandling=SIM_IS_BIT_SET(dummy,1);
					_unlimitedAcceleration=SIM_IS_BIT_SET(dummy,2);
					_invertTargetVelocityAtLimits=SIM_IS_BIT_SET(dummy,3);
					_dynamicMotorEnabled=SIM_IS_BIT_SET(dummy,4);
					_dynamicMotorControlLoopEnabled=SIM_IS_BIT_SET(dummy,5);
					_jointHasHybridFunctionality=SIM_IS_BIT_SET(dummy,6);
					_dynamicMotorPositionControl_torqueModulation=SIM_IS_BIT_SET(dummy,7);
				}	
				if (theName.compare("Vaa")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					_dynamicMotorCustomControl=SIM_IS_BIT_SET(dummy,0);
				}
				if (theName.compare("Jmd")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _jointMode;
				}
				if (theName.compare("Jdt")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _dependencyJointID >> _dependencyJointCoeff >> _dependencyJointFact;
				}
				if (theName.compare("Jtt")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _jointType;
				}
				if (theName.compare("Jm2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _maxAcceleration >> _velocity >> _targetVelocity;
				}
				if (theName.compare("Dmp")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynamicMotorTargetVelocity >> _dynamicMotorMaximumForce;
				}
				if (theName.compare("Dpc")==0)
				{ // keep for backward compatibility (7/5/2014)
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynamicMotorPositionControl_P >> _dynamicMotorPositionControl_I >> _dynamicMotorPositionControl_D;
					_dynamicMotorPositionControl_I/=0.005f;
					_dynamicMotorPositionControl_D*=0.005f;
				}
				if (theName.compare("Dp2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynamicMotorPositionControl_P >> _dynamicMotorPositionControl_I >> _dynamicMotorPositionControl_D;
				}

				if (theName.compare("Spp")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynamicMotorSpringControl_K >> _dynamicMotorSpringControl_C;
					kAndCSpringParameterPresent=true;
				}

				if (theName.compare("Dtp")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynamicMotorPositionControl_targetPosition;
				}
				if (theName.compare("Od1")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _odeStopERP >> _odeStopCFM >> _odeBounce >> _odeFudgeFactor >> _odeNormalCFM;
				}
				if (theName.compare("Bt1")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _bulletStopERP >> _bulletStopCFM >> _bulletNormalCFM;
				}
				if (theName.compare("Vo2")==0)
				{ // vortex params:
					noHit=false;
					ar >> byteQuantity;
					int cnt1,cnt2;
					ar >> cnt1 >> cnt2;
					float vf;
					int vi;
					for (int i=0;i<cnt1;i++)
					{ // new versions will always have same or more items in _vortexFloatParams already!
						ar >> vf;
						_vortexFloatParams[i]=vf;
					}
					for (int i=0;i<cnt2;i++)
					{ // new versions will always have same or more items in _vortexIntParams already!
						ar >> vi;
						_vortexIntParams[i]=vi;
					}
				}
				if (theName.compare("Ulv")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynamicMotorUpperLimitVelocity;
					dynamicUpperVelocityLimitPresent=true;
				}
				if (theName.compare("Cco")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _jointCallbackCallOrder;
				}

				if (noHit)
					ar.loadUnknownData();
			}
		}
		if (!dynamicUpperVelocityLimitPresent)
		{ // for backward compatibility (2010/11/13):
			if (_jointType==sim_joint_revolute_subtype)
				_dynamicMotorUpperLimitVelocity=360.0f*degToRad;
			if (_jointType==sim_joint_prismatic_subtype)
				_dynamicMotorUpperLimitVelocity=10.0f;
			if (_jointType==sim_joint_spherical_subtype)
				_dynamicMotorUpperLimitVelocity=0.0; // no meaning here
		}

		if (!kAndCSpringParameterPresent)
		{ // for backward compatibility (7/5/2014):
			if (_dynamicMotorEnabled&&_dynamicMotorControlLoopEnabled&&_dynamicMotorPositionControl_torqueModulation&&(!_dynamicMotorCustomControl))
			{ // we have a joint that behaves as a spring. We need to compute the corresponding K and C parameters, and adjust the max. force/torque (since that was not limited before):
				_dynamicMotorSpringControl_K=_dynamicMotorMaximumForce*_dynamicMotorPositionControl_P;
				_dynamicMotorSpringControl_C=_dynamicMotorMaximumForce*_dynamicMotorPositionControl_D;
				float maxTolerablePorDParam=1.0f;
				if (_jointType==sim_joint_revolute_subtype)
					maxTolerablePorDParam=1.0f/piValTimes2;
				float maxPorD=SIM_MAX(fabs(_dynamicMotorPositionControl_P),fabs(_dynamicMotorPositionControl_D));
				if (maxPorD>maxTolerablePorDParam)
				{ // we shift the limit up
					float corr=maxTolerablePorDParam/maxPorD;
					_dynamicMotorPositionControl_P*=corr;
					_dynamicMotorPositionControl_I*=corr;
					_dynamicMotorPositionControl_D*=corr;
					_dynamicMotorMaximumForce/=corr;
				}
			}
		}

		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			CTTUtil::scaleColorUp_(colorPart1.colors);
			CTTUtil::scaleColorUp_(colorPart2.colors);
		}
	}
}

void CJoint::serializeWExtIk(CExtIkSer& ar)
{
	serializeWExtIkMain(ar);

	ar.writeInt(_jointType);

	ar.writeFloat(_screwPitch);

	ar.writeFloat(_sphericalTransformation(0));
	ar.writeFloat(_sphericalTransformation(1));
	ar.writeFloat(_sphericalTransformation(2));
	ar.writeFloat(_sphericalTransformation(3));

	BYTE dummy=0;
	SIM_SET_CLEAR_BIT(dummy,0,_positionIsCyclic);
	ar.writeByte(dummy);

	ar.writeFloat(_jointMinPosition);
	ar.writeFloat(_jointPositionRange);

	ar.writeFloat(_jointPosition);

	ar.writeFloat(_maxStepSize);

	ar.writeFloat(_ikWeight);

	ar.writeInt(_jointMode);

	ar.writeInt(_dependencyJointID);
	ar.writeFloat(_dependencyJointCoeff);
	ar.writeFloat(_dependencyJointFact);
}



void CJoint::performObjectLoadingMapping(std::vector<int>* map)
{ // New_Object_ID=map[Old_Object_ID]
	performObjectLoadingMappingMain(map);
	_dependencyJointID=App::ct->objCont->getLoadingMapping(map,_dependencyJointID);
}

void CJoint::setJointMode(int theMode,bool correctDynMotorTargetPositions/*=true*/)
{
	if (theMode==sim_jointmode_passive)
	{
		_dependencyJointID=-1;
		App::ct->objCont->actualizeObjectInformation(); // to actualize dependencies
		_jointMode=theMode;
	}
	if (theMode==sim_jointmode_motion)
	{
		if (_jointType!=sim_joint_spherical_subtype)
		{
			_dependencyJointID=-1;
			App::ct->objCont->actualizeObjectInformation(); // to actualize dependencies
			_jointMode=theMode;
		}
	}
	if (theMode==sim_jointmode_ikdependent)
	{
		if (_jointType!=sim_joint_spherical_subtype)
		{
			App::ct->objCont->actualizeObjectInformation(); // to actualize dependencies
			_jointMode=theMode;
		}
	}
	if (theMode==sim_jointmode_dependent)
	{
		if (_jointType!=sim_joint_spherical_subtype)
		{
			App::ct->objCont->actualizeObjectInformation(); // to actualize dependencies
			_jointMode=theMode;
		}
	}
	if (theMode==sim_jointmode_force)
	{
		_jointHasHybridFunctionality=false;
		_dependencyJointID=-1;
		_screwPitch=0.0f;
		if (_jointType==sim_joint_revolute_subtype)
			_jointPositionRange=tt::getLimitedFloat(0.0f,piValTimes2,_jointPositionRange); // new since 18/11/2012 (was forgotten)
		if (_jointType==sim_joint_spherical_subtype)
			_jointPositionRange=piValue;

		App::ct->objCont->actualizeObjectInformation(); // to actualize dependencies
		_jointMode=theMode;
	}
	if (theMode==sim_jointmode_ik)
	{
		_dependencyJointID=-1;
		App::ct->objCont->actualizeObjectInformation(); // to actualize dependencies
		_jointMode=theMode;
	}
	setPosition(getPosition()); // not sure it is needed here, but better than not!
	if ((theMode==sim_jointmode_force)&&_dynamicMotorControlLoopEnabled&&correctDynMotorTargetPositions)
	{ // Make sure the target position is the same here (otherwise big jump)
		setDynamicMotorPositionControlTargetPosition(getPosition());
	}
}

void CJoint::_rectifyDependentJoints()
{	// We rectify now all joints linked to that one (rewritten on 2009-01-27):
	for (int i=0;i<int(directDependentJoints.size());i++)
	{
		if (directDependentJoints[i]->getJointMode()==sim_jointmode_dependent)
			directDependentJoints[i]->setPosition(0.0f,false); // value doesn't matter!
	}
}

int CJoint::getJointMode()
{
	return(_jointMode);
}

int CJoint::getDependencyJointID()
{
	return(_dependencyJointID);
}

float CJoint::getDependencyJointCoeff()
{
	return(_dependencyJointCoeff);
}

float CJoint::getDependencyJointFact()
{
	return(_dependencyJointFact);
}

int CJoint::getJointType()
{
	return(_jointType);
}

float CJoint::getScrewPitch() const
{
	return(_screwPitch);
}

void CJoint::setSphericalTransformation(const C4Vector& tr)
{
	C4Vector transf(tr);
	if (_jointPositionRange<piValue*0.99f)
	{
		C3X3Matrix theTr(transf);
		C3Vector zReset(0.0f,0.0f,1.0f);
		float angle=zReset.getAngle(theTr.axis[2]);
		if (angle>_jointPositionRange)
		{ // We have to limit the movement:
			C3Vector rotAxis((theTr.axis[2]^zReset).getNormalized());
			C4Vector rot(angle-_jointPositionRange,rotAxis);
			transf=rot*transf;
		}
	}
	_sphericalTransformation=transf;
}

C4Vector CJoint::getSphericalTransformation() const
{
	return(_sphericalTransformation);
}

void CJoint::setMaxStepSize(float stepS)
{
	if (_jointType==sim_joint_revolute_subtype)
		tt::limitValue(0.01f*degToRad,100000.0f,stepS); // high number for screws!
	if (_jointType==sim_joint_prismatic_subtype)
		tt::limitValue(0.0001f,1000.0f,stepS);
	if (_jointType==sim_joint_spherical_subtype)
		tt::limitValue(0.01f*degToRad,piValue,stepS);
	_maxStepSize=stepS;
}

float CJoint::getMaxStepSize()
{
	return(_maxStepSize);
}

float CJoint::getPosition(bool useTempValues) 
{  // useTempValues is false by default
	if (useTempValues)
		return(_jointPosition_tempForIK); 
	return(_jointPosition); 
}

float CJoint::getIKWeight()
{
	return(_ikWeight);
}

void CJoint::setIKWeight(float newWeight)
{
	newWeight=tt::getLimitedFloat(-1.0f,1.0f,newWeight);
	_ikWeight=newWeight;
}

float CJoint::getPosition_ratio()
{
	return((getPosition()-_jointMinPosition)/_jointPositionRange);
}

void CJoint::setPosition_ratio(float posRatio)
{
	setPosition(_jointMinPosition+posRatio*_jointPositionRange);
}

void CJoint::setPosition(float parameter,bool useTempValues)
{ // useTempValues is false by default
	if (_positionIsCyclic)
		parameter=tt::getNormalizedAngle(parameter);
	else
	{
		if (parameter>(getPositionIntervalMin()+getPositionIntervalRange()))
			parameter=getPositionIntervalMin()+getPositionIntervalRange();
		if (parameter<getPositionIntervalMin())
			parameter=getPositionIntervalMin();
	}
	if (useTempValues)	
		_jointPosition_tempForIK=parameter;
	else
		_jointPosition=parameter;

	if (_jointMode==sim_jointmode_dependent)
	{ // If this joint is constrained
		float linked=0.0f;
		if (_dependencyJointID!=-1)
		{
			CJoint* anAct=App::ct->objCont->getJoint(_dependencyJointID);
			if (anAct!=NULL)
				linked=_dependencyJointCoeff*anAct->getPosition(useTempValues);
		}
		if (useTempValues)	
			_jointPosition_tempForIK=linked+_dependencyJointFact;
		else
			_jointPosition=linked+_dependencyJointFact;
	}
	_rectifyDependentJoints();
	setVelocity(getVelocity()); // To make sure velocity is within allowed range
}

float CJoint::getPositionIntervalMin() 
{ 
	return(_jointMinPosition); 
}

float CJoint::getPositionIntervalRange() 
{ 
	return(_jointPositionRange); 
}

bool CJoint::getPositionIsCyclic()
{
	if (_jointType==sim_joint_prismatic_subtype)
		return(false);
	return(_positionIsCyclic);
}

void CJoint::initializeParametersForIK(float angularJointLimitationThreshold)
{
	if (_jointType!=sim_joint_spherical_subtype)
		_jointPosition_tempForIK=_jointPosition;
	else
	{
		// 1. Do we need to prepare the thing for the joint limitation?
		_sphericalTransformation_eulerLockTempForIK=0;
		C3X3Matrix m(_sphericalTransformation);
		float angle=C3Vector::unitZVector.getAngle(m.axis[2]);
		if ( (_jointPositionRange<179.9f*degToRad)&&(angle>1.0f*degToRad) )
		{
			if ((_jointPositionRange-angularJointLimitationThreshold)/2.0f<angle)
			{ // We have to activate the second type of spherical joint (with joint limitation (IK pass dependent))
				_sphericalTransformation_eulerLockTempForIK=2;
				C3Vector n(m.axis[2]);
				n(2)=0.0f;
				n.normalize();
				C3Vector y((C3Vector::unitZVector^n).getNormalized());
				float angle2=C3Vector::unitXVector.getAngle(y);
				C3Vector zz(C3Vector::unitXVector^y);
				if (zz(2)<0.0f)
					angle2=-angle2;
				_jointPosition_tempForIK=0.0f; // Not really needed!
				_sphericalTransformation_euler1TempForIK=angle2;
				_sphericalTransformation_euler2TempForIK=angle;
				float angle3=m.axis[0].getAngle(y);
				C3Vector nz(y^m.axis[0]);
				if (nz*m.axis[2]<0.0f)
					angle3=-angle3;
				_sphericalTransformation_euler3TempForIK=angle3;
			}
		}
		if (_sphericalTransformation_eulerLockTempForIK==0)
		{ // No joint limitations for the IK (in this IK pass)
			_jointPosition_tempForIK=0.0f; // Not really needed!
			_sphericalTransformation_euler1TempForIK=0.0f;
			_sphericalTransformation_euler2TempForIK=0.0f;
			_sphericalTransformation_euler3TempForIK=0.0f;
		}
	}
}

int CJoint::getDoFs()
{
	if (_jointType!=sim_joint_spherical_subtype)
		return(1);
	return(3);
}

int CJoint::getTempSphericalJointLimitations()
{
	return(_sphericalTransformation_eulerLockTempForIK);
}

void CJoint::getLocalTransformationExPart1(C7Vector& mTr,int index,bool useTempValues)
{ // Used for Jacobian calculation with spherical joints
	if (_sphericalTransformation_eulerLockTempForIK==0)
	{ // Spherical joint limitations are not activated in the IK algorithm (but if we come close to the limit, it might get activated in next pass!)
		if (index==0)
		{ 
			mTr.setIdentity();
			mTr.Q.setEulerAngles(0.0f,piValD2,0.0f);
			C7Vector tr2(getLocalTransformation());
			mTr=tr2*mTr;
		}
		if (index==1)
		{
			mTr.setIdentity();
			mTr.Q.setEulerAngles(-piValD2,0.0f,-piValD2);
		}
		if (index==2)
		{
			mTr.setIdentity();
			mTr.Q.setEulerAngles(piValD2,0.0f,0.0f);
		}
	}
	else
	{
		if (index==0)
		{
			mTr=getLocalTransformationPart1();
		}
		if (index==1)
		{
			mTr.setIdentity();
			mTr.Q.setEulerAngles(0.0f,piValD2,0.0f);
		}
		if (index==2)
		{
			mTr.setIdentity();
			mTr.Q.setEulerAngles(0.0f,-piValD2,0.0f);
		}
	}
}

float CJoint::getTempParameterEx(int index)
{
	if (index==0)
		return(_sphericalTransformation_euler1TempForIK);
	if (index==1)
		return(_sphericalTransformation_euler2TempForIK);
	if (index==2)
		return(_sphericalTransformation_euler3TempForIK);
	return(0.0f);
}

void CJoint::setTempParameterEx(float parameter,int index)
{
	if (index==0)
		_sphericalTransformation_euler1TempForIK=parameter;
	if (index==1)
		_sphericalTransformation_euler2TempForIK=parameter;
	if (index==2)
		_sphericalTransformation_euler3TempForIK=parameter;

	if (_sphericalTransformation_eulerLockTempForIK==0)
	{ // Spherical joint limitations are not activated in the IK algorithm (but if we come close to the limit, it might get activated in next pass!)
		C4Vector saved(_sphericalTransformation);
		applyTempParametersEx();
		C4Vector tr(saved.getInverse()*_sphericalTransformation);
		C3Vector euler(tr.getEulerAngles());
		_sphericalTransformation_euler1TempForIK=euler(0);
		_sphericalTransformation_euler2TempForIK=euler(1);
		_sphericalTransformation_euler3TempForIK=euler(2);
		_sphericalTransformation=saved;
	}
	else
	{ // Spherical joint limitations are activated in the IK algorithm
		C4Vector saved(_sphericalTransformation);
		applyTempParametersEx();

		C3X3Matrix m(_sphericalTransformation);

		float angle=C3Vector::unitZVector.getAngle(m.axis[2]);
		if (angle>0.01f*degToRad)
		{
			C3Vector n(m.axis[2]);
			n(2)=0.0f;
			n.normalize();
			C3Vector y((C3Vector::unitZVector^n).getNormalized());
			float angle2=C3Vector::unitXVector.getAngle(y);
			C3Vector zz(C3Vector::unitXVector^y);
			if (zz(2)<0.0f)
				angle2=-angle2;
			_sphericalTransformation_euler1TempForIK=angle2;
			_sphericalTransformation_euler2TempForIK=angle;
			float angle3=m.axis[0].getAngle(y);
			C3Vector nz(y^m.axis[0]);
			if (nz*m.axis[2]<0.0f)
				angle3=-angle3;
			_sphericalTransformation_euler3TempForIK=angle3;
		}
		else
		{ // This is a rare case and should never happen if the spherical joint limitation is not too small!
			float angle=C3Vector::unitXVector.getAngle(m.axis[0]);
			if ((C3Vector::unitXVector^m.axis[0])(2)<0.0f)
				angle=-angle;
			_sphericalTransformation_euler1TempForIK=angle;
			_sphericalTransformation_euler2TempForIK=0.0f;
			_sphericalTransformation_euler3TempForIK=0.0f;
		}
		_sphericalTransformation=saved;
	}
}

void CJoint::applyTempParametersEx()
{
	if (_jointType==sim_joint_spherical_subtype)
	{
		C7Vector tr1(getLocalTransformationPart1(true));
		C7Vector tr2(getLocalTransformation(true));
		setSphericalTransformation(tr1.Q.getInverse()*tr2.Q);
	}
}

bool CJoint::announceObjectWillBeErased(int objID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
	// Return value true means 'this' has to be erased too!
	announceObjectWillBeErasedMain(objID,copyBuffer);
	if (_dependencyJointID==objID)
	{
		_dependencyJointID=-1;
	}
	// We check if the joint is listed in the directDependentJoints:
	for (int i=0;i<int(directDependentJoints.size());i++)
	{
		if (directDependentJoints[i]->getID()==objID)
			directDependentJoints.erase(directDependentJoints.begin()+i);
	}
	return(false);
}

void CJoint::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceIkObjectWillBeErasedMain(ikGroupID,copyBuffer);
}


