// This file is part of the CHAI3D PLUGIN for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// A big thanks to Julien Tharin, François Conti, Sebastien Granges, 
// Patrick Helmer, Force Dimension, Lucian Cucu and Thomas Estier
// for their precious help.
// 
// The CHAI3D PLUGIN is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The ROS PLUGIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE ROS PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the ROS PLUGIN.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

//==============================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2003-2014, CHAI3D.
    (www.chai3d.org)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of CHAI3D nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE. 

    \author    <http://www.chai3d.org>
    \author    Francois Conti
    \version   3.0.0 $Rev: 1292 $
*/
//==============================================================================


#include "v_repExtCHAI3D.h"
#include "v_repLib.h"
#include <iostream>
#include <time.h>
#include "chai3d.h"

/* #define VREP_IS_NAN(x) false
#define VREP_IS_FINITE(x) true
#include "../v_repMath/7Vector.h"
*/

#ifndef MACOSX
#include "GL/glut.h"
#else
#include "GLUT/glut.h"
#endif

using namespace chai3d;
using namespace std;

#ifdef _WIN32
	#include <shlwapi.h>
	#pragma comment(lib, "Shlwapi.lib")
	#define MUTEX_HANDLE HANDLE
	#define MUTEX_HANDLE_X MUTEX_HANDLE
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
	#include <unistd.h>
	#include <string.h>
	#define _stricmp(x,y) strcasecmp(x,y)
	#define MUTEX_HANDLE pthread_mutex_t
	#define MUTEX_HANDLE_X MUTEX_HANDLE*
	#define Sleep(x) (usleep(x*1000))
#endif

#define PLUGIN_VERSION 1
#define MAX_HAPTIC 1

enum tForce {eCST_FORCE,eCENT_SPRING};

struct sHaptic
{
	cToolCursor *tool;
	cGenericHapticDevicePtr hapticDevice;
	cHapticDeviceInfo *DeviceInfo;
	double radius;
	bool used;
	int currentButtons;
	int eventButtons;
};

struct sObstacle
{
	cMesh *mesh;
	int id;
};

struct sForce
{
	tForce type;
	int id;
	cVector3d cstforce;
	cVector3d springOriginPosition;
	double springConstantK;
	double springConstantC;
	double springMaxForce;
};

std::vector<std::vector<float>> meshes_vertices;
std::vector<std::vector<int>> meshes_indices;
int _argc=1;
char* _argv[1]={" "};
cWorld* world;
bool deviceRunning=false;
bool deviceThreadRunning=false;
cFrequencyCounter frequencyCounter;
cHapticDeviceHandler* handler=NULL;
sHaptic tools[MAX_HAPTIC];
cThread* hapticsThread;
float position[3];
float interactionForce[3];
int currentButtons=0;
int eventButtons=0;
vector<sObstacle> obstacles; // vector of obstacles
int current_buttons=0,event_buttons=0;

vector<sForce> forces; // vector of forces
MUTEX_HANDLE _mutex;
int nextObjOrForceId=0;
double wsScaleFactor=1.0;

// cmd: 0=remove,1=sphere,2=cuboid,3=constant force,4=spring force
std::vector<int> commandsAndIdAndOpt_4_tmp;
std::vector<double> commandData_10_tmp;

std::vector<int> commandsAndIdAndOpt_4;
std::vector<double> commandData_10;

void _simpleLock(MUTEX_HANDLE_X mutex)
{
#ifdef _WIN32
	while (WaitForSingleObject(mutex,INFINITE)!=WAIT_OBJECT_0)
		Sleep(1);
#elif defined (__linux) || defined (__APPLE__)
	while (pthread_mutex_lock(mutex)==-1)
		Sleep(1);
#endif
}

void _simpleUnlock(MUTEX_HANDLE_X mutex)
{
#ifdef _WIN32
	ReleaseMutex(mutex);
#elif defined (__linux) || defined (__APPLE__)
	pthread_mutex_unlock(mutex);
#endif
}

void simpleLock()
{
#ifdef _WIN32
	_simpleLock(_mutex);
#elif defined (__linux) || defined (__APPLE__)
	_simpleLock(&_mutex);
#endif
}

void simpleUnlock()
{
#ifdef _WIN32
	_simpleUnlock(_mutex);
#elif defined (__linux) || defined (__APPLE__)
	_simpleUnlock(&_mutex);
#endif
}

int getTimeInMs()
{
  static cPrecisionClock clock;
  return((int(clock.getCPUTimeSeconds()*1e3))&0x03ffffff);
}

int getTimeDiffInMs(int lastTime)
{
	int currentTime=getTimeInMs();
	if (currentTime<lastTime)
		return(currentTime+0x03ffffff-lastTime);
	return(currentTime-lastTime);
}

std::string connectToDevice(int zeroBasedDeviceIndex, double toolRadius) {
	if (zeroBasedDeviceIndex>MAX_HAPTIC-1)
		return (std::string("ERROR: device index too high!"));
	if (zeroBasedDeviceIndex<0)
		return (std::string("ERROR: device index too low!"));
	if (tools[zeroBasedDeviceIndex].used)
		return (std::string("ERROR: device already used!"));

	if (handler==NULL)
		handler=new cHapticDeviceHandler();

    // get access to the first available haptic device
     if (!handler->getDevice(tools[zeroBasedDeviceIndex].hapticDevice, zeroBasedDeviceIndex))
		 return (std::string("ERROR: cannot get acccess to device number ").append(to_string(zeroBasedDeviceIndex).append(" !")));
	 	
    // open a connection to haptic device
	if(!tools[zeroBasedDeviceIndex].hapticDevice->open())
		return (std::string("ERROR: cannot connect to device number ").append(to_string(zeroBasedDeviceIndex).append(" !")));

    tools[zeroBasedDeviceIndex].hapticDevice->calibrate();
    tools[zeroBasedDeviceIndex].DeviceInfo = new cHapticDeviceInfo(tools[zeroBasedDeviceIndex].hapticDevice->getSpecifications());
    tools[zeroBasedDeviceIndex].hapticDevice->setEnableGripperUserSwitch(true);
    tools[zeroBasedDeviceIndex].tool = new cToolCursor(world);
    world->addChild(tools[zeroBasedDeviceIndex].tool );
    tools[zeroBasedDeviceIndex].tool ->setHapticDevice(tools[zeroBasedDeviceIndex].hapticDevice);
    tools[zeroBasedDeviceIndex].tool ->setWorkspaceRadius(1.0);

	tools[zeroBasedDeviceIndex].tool->setWorkspaceScaleFactor(wsScaleFactor*2.5); // why 2.5??


    tools[zeroBasedDeviceIndex].tool->setRadius(toolRadius);
	tools[zeroBasedDeviceIndex].radius=toolRadius;
    tools[zeroBasedDeviceIndex].tool->setShowContactPoints(true, false);

    // enable if objects in the scene are going to rotate or translate
    // or possibly collide against the tool. If the environment
    // is entirely static, you can set this parameter to "false"
    tools[zeroBasedDeviceIndex].tool->enableDynamicObjects(true);

    // start the haptic tool
    if (!(tools[zeroBasedDeviceIndex].tool->start()))
		return (std::string("ERROR: could not initiate communication with the tool!"));

	tools[zeroBasedDeviceIndex].used=true;
	return (std::string(""));
}

bool disconnectFromDevice(int zeroBasedDeviceIndex)
{
	if (zeroBasedDeviceIndex>MAX_HAPTIC-1)
		return false;
	if (zeroBasedDeviceIndex<0)
		return false;
	if (!(tools[zeroBasedDeviceIndex].used))
		return false;

	tools[zeroBasedDeviceIndex].tool->stop();
    world->removeChild(tools[zeroBasedDeviceIndex].tool );
    delete tools[zeroBasedDeviceIndex].tool;
	delete tools[zeroBasedDeviceIndex].DeviceInfo;
	tools[zeroBasedDeviceIndex].hapticDevice->close();
    // No! common to all devices! delete handler;
	tools[zeroBasedDeviceIndex].used=false;
	return true;
}

bool deinit()
{
	if (deviceRunning)
	{
		deviceRunning=false;
		while (deviceThreadRunning)
			cSleepMs(100);
		delete hapticsThread;
		for (int i=0;i<MAX_HAPTIC;i++)
		{
			if (tools[i].used)
				disconnectFromDevice(i);
		}
		delete world;
		return(true);
	}
	return(false);
}

void flush()
{
	simpleLock();

	// Append the commands:
	commandsAndIdAndOpt_4.insert(commandsAndIdAndOpt_4.end(),commandsAndIdAndOpt_4_tmp.begin(),commandsAndIdAndOpt_4_tmp.end());
	commandData_10.insert(commandData_10.end(),commandData_10_tmp.begin(),commandData_10_tmp.end());
	commandsAndIdAndOpt_4_tmp.clear();
	commandData_10_tmp.clear();

	// Now make sure there are no duplicate commands when setting an object's pose (simply keep the last such command):
	for (int i=int(commandsAndIdAndOpt_4.size()/4)-1;i>=0;i--)
	{
		int cmd=commandsAndIdAndOpt_4[4*i+0];
		int shapeId=commandsAndIdAndOpt_4[4*i+1];
		if (cmd==6)
		{
			for (int j=i-1;j>=0;j--)
			{
				int cmd2=commandsAndIdAndOpt_4[4*j+0];
				int shapeId2=commandsAndIdAndOpt_4[4*j+1];
				if ((cmd2==cmd)&&(shapeId==shapeId2))
					commandsAndIdAndOpt_4[4*j+0]=99999; // instead of removing this entry, we simply overwrite it with an unknown command ID
			}
		}
	}

	simpleUnlock();
}

int addConstantForce(double forceVectorInNewton[3])
{
	commandsAndIdAndOpt_4_tmp.push_back(3);
	commandsAndIdAndOpt_4_tmp.push_back(nextObjOrForceId);
	commandsAndIdAndOpt_4_tmp.push_back(0);
	commandsAndIdAndOpt_4_tmp.push_back(0);
	commandData_10_tmp.push_back(forceVectorInNewton[0]);
	commandData_10_tmp.push_back(forceVectorInNewton[1]);
	commandData_10_tmp.push_back(forceVectorInNewton[2]);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	return(nextObjOrForceId++);
}

void addConstantForce_(int id,double forceVectorInNewton[3])
{
	sForce force;
	force.type=eCST_FORCE;
	force.id=id;
	force.cstforce=cVector3d(forceVectorInNewton[0],forceVectorInNewton[1],forceVectorInNewton[2]);
	forces.push_back(force);
}

int addCentralSpring(double springOriginPosition[3],double springConstantK,double springConstantC,double springMaxForce)
{
	commandsAndIdAndOpt_4_tmp.push_back(4);
	commandsAndIdAndOpt_4_tmp.push_back(nextObjOrForceId);
	commandsAndIdAndOpt_4_tmp.push_back(0);
	commandsAndIdAndOpt_4_tmp.push_back(0);
	commandData_10_tmp.push_back(springOriginPosition[0]);
	commandData_10_tmp.push_back(springOriginPosition[1]);
	commandData_10_tmp.push_back(springOriginPosition[2]);
	commandData_10_tmp.push_back(springConstantK);
	commandData_10_tmp.push_back(springConstantC);
	commandData_10_tmp.push_back(springMaxForce);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	return(nextObjOrForceId++);
}

void addCentralSpring_(int id,double springOriginPosition[3],double springConstantK,double springConstantC,double springMaxForce)
{
	sForce force;
	force.type=eCENT_SPRING;
	force.id=id;
	force.springOriginPosition =cVector3d(springOriginPosition[0],springOriginPosition[1],springOriginPosition[2]);
	force.springConstantK=springConstantK;
	force.springConstantC=springConstantC;
	force.springMaxForce=springMaxForce;
	forces.push_back(force);
}


void removeObject(int objectHandle)
{
	commandsAndIdAndOpt_4_tmp.push_back(0);
	commandsAndIdAndOpt_4_tmp.push_back(objectHandle);
	commandsAndIdAndOpt_4_tmp.push_back(0);
	commandsAndIdAndOpt_4_tmp.push_back(0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
}

void removeForce_(int id)
{
	for (int i=0;i<int(forces.size());i++)
	{
		if (forces[i].id==id)
		{
			forces.erase(forces.begin()+i);
			break;
		}
	}
}

void addObstacle(int id,cMesh *mesh)
{
	sObstacle obstacle;
	obstacle.mesh=mesh;
	obstacle.id=id;
	obstacles.push_back(obstacle);
}

int addMesh(int meshDataIndex,double position[3],double orientation[3])
{
	commandsAndIdAndOpt_4_tmp.push_back(5);
	commandsAndIdAndOpt_4_tmp.push_back(nextObjOrForceId);
	commandsAndIdAndOpt_4_tmp.push_back(meshDataIndex);
	commandsAndIdAndOpt_4_tmp.push_back(0);
	commandData_10_tmp.push_back(position[0]);
	commandData_10_tmp.push_back(position[1]);
	commandData_10_tmp.push_back(position[2]);
	commandData_10_tmp.push_back(orientation[0]);
	commandData_10_tmp.push_back(orientation[1]);
	commandData_10_tmp.push_back(orientation[2]);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	return(nextObjOrForceId++);
}

void addMesh_(int id,int meshDataIndex,double position[3],double orientation[3])
{
	if ( (meshDataIndex>=0)&&(meshDataIndex<int(meshes_vertices.size())) )
	{
		// read the scale factor between the physical workspace of the haptic
	    // device and the virtual workspace defined for the tool
	    double maxStiffness= tools[0].DeviceInfo->m_maxLinearStiffness / tools[0].tool->getWorkspaceScaleFactor();
		double maxForce=tools[0].DeviceInfo->m_maxLinearForce;
		double maxDamping=tools[0].DeviceInfo->m_maxLinearDamping / tools[0].tool->getWorkspaceScaleFactor();
		cMesh *mesh = new cMesh();

		for (int i=0;i<int(meshes_indices[meshDataIndex].size())/3;i++)
		{
			int ind[3]={meshes_indices[meshDataIndex][3*i+0],meshes_indices[meshDataIndex][3*i+1],meshes_indices[meshDataIndex][3*i+2]};
			cVector3d v1=cVector3d(meshes_vertices[meshDataIndex][3*ind[0]+0],meshes_vertices[meshDataIndex][3*ind[0]+1],meshes_vertices[meshDataIndex][3*ind[0]+2]);
			cVector3d v2=cVector3d(meshes_vertices[meshDataIndex][3*ind[1]+0],meshes_vertices[meshDataIndex][3*ind[1]+1],meshes_vertices[meshDataIndex][3*ind[1]+2]);
			cVector3d v3=cVector3d(meshes_vertices[meshDataIndex][3*ind[2]+0],meshes_vertices[meshDataIndex][3*ind[2]+1],meshes_vertices[meshDataIndex][3*ind[2]+2]);
			mesh->newTriangle(v1,v2,v3);
		}

		world->addChild(mesh);

		cMatrix3d rot=cMatrix3d(orientation[0],orientation[1],orientation[2],C_EULER_ORDER_ZYX,false);
		cVector3d transl=cVector3d(position[0],position[1],position[2]);
		mesh->setLocalPos(transl);
		mesh->setLocalRot(rot);

		mesh->computeAllNormals();
		mesh->m_material->setStiffness(1.0 * maxStiffness);
		mesh->m_material->setDynamicFriction(0.0);
		mesh->m_material->setStaticFriction(0.0);
		mesh->computeBoundaryBox(true);
		mesh->createAABBCollisionDetector(1.0*tools[0].radius);
		mesh->m_material->setHapticTriangleSides(true,true);

		addObstacle(id,mesh);
	}
}

void setShapePose(int shapeId,double position[3],double orientation[3],int interpolationTimeInMs)
{
	commandsAndIdAndOpt_4_tmp.push_back(6);
	commandsAndIdAndOpt_4_tmp.push_back(shapeId);
	if (interpolationTimeInMs<0)
		interpolationTimeInMs=0;
	commandsAndIdAndOpt_4_tmp.push_back(interpolationTimeInMs);
	commandsAndIdAndOpt_4_tmp.push_back(0);
	commandData_10_tmp.push_back(position[0]);
	commandData_10_tmp.push_back(position[1]);
	commandData_10_tmp.push_back(position[2]);
	commandData_10_tmp.push_back(orientation[0]);
	commandData_10_tmp.push_back(orientation[1]);
	commandData_10_tmp.push_back(orientation[2]);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
}

void setShapePose_(int id,const double position[3],const double orientation[3])
{
	vector<sObstacle>::iterator it;
	for (it=obstacles.begin();it!=obstacles.end();it++)
	{
		if ((*it).id==id)
		{
			((*it).mesh)->setLocalPos(position[0],position[1],position[2]);
			((*it).mesh)->setLocalRot(cMatrix3d(orientation[0],orientation[1],orientation[2],C_EULER_ORDER_ZYX,false));
			break;
		}
	}
}

bool getShapePose_(int id,float* matrix)
{
	vector<sObstacle>::iterator it;
	for (it=obstacles.begin();it!=obstacles.end();it++)
	{
		if ((*it).id==id)
		{
			cVector3d pos=((*it).mesh)->getLocalPos();
			cMatrix3d matr=((*it).mesh)->getLocalRot();
			matrix[3]=float(pos.x());
			matrix[7]=float(pos.y());
			matrix[11]=float(pos.z());

			matrix[0]=float(matr(2,2));
			matrix[4]=float(matr(2,1));
			matrix[8]=float(matr(2,0));

			matrix[1]=float(matr(1,2));
			matrix[5]=float(matr(1,1));
			matrix[9]=float(matr(1,0));

			matrix[2]=float(matr(0,2));
			matrix[6]=float(matr(0,1));
			matrix[10]=float(matr(0,0));
			return(true);
		}
	}
	return(false);
}


void setShapeEffect(int shapeId,int effectType,double effectParam[3])
{
	commandsAndIdAndOpt_4_tmp.push_back(9);
	commandsAndIdAndOpt_4_tmp.push_back(shapeId);
	commandsAndIdAndOpt_4_tmp.push_back(effectType);
	commandsAndIdAndOpt_4_tmp.push_back(0);
	commandData_10_tmp.push_back(effectParam[0]);
	commandData_10_tmp.push_back(effectParam[1]);
	commandData_10_tmp.push_back(effectParam[2]);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
	commandData_10_tmp.push_back(0.0);
}

void setShapeEffect_(int id,int effectType,double effectParam[3])
{
    double maxStiffness= tools[0].DeviceInfo->m_maxLinearStiffness / tools[0].tool->getWorkspaceScaleFactor();
	double maxForce=tools[0].DeviceInfo->m_maxLinearForce;
	double maxDamping=tools[0].DeviceInfo->m_maxLinearDamping / tools[0].tool->getWorkspaceScaleFactor();

	vector<sObstacle>::iterator it;
	for (it=obstacles.begin();it!=obstacles.end();it++)
	{
		if ((*it).id==id)
		{
			if (effectType==0)
			{ // remove all effects. Stiffness keeps unchanged
				((*it).mesh)->deleteEffectStickSlip();
				((*it).mesh)->deleteEffectViscosity();
				((*it).mesh)->deleteEffectMagnetic();
				((*it).mesh)->deleteEffectVibration();
				((*it).mesh)->deleteEffectSurface();
			}
			if (effectType==1)
			{ // adjust stiffness
				((*it).mesh)->m_material->setStiffness(effectParam[0]*maxStiffness);
			}
			if (effectType==2)
			{ // add vibration effect
				((*it).mesh)->m_material->setVibrationAmplitude(effectParam[0]*maxForce);
				((*it).mesh)->m_material->setVibrationFrequency(effectParam[1]);
				((*it).mesh)->createEffectVibration();
				((*it).mesh)->createEffectSurface();
			}
			if (effectType==3)
			{ // add magnetic effect
				((*it).mesh)->m_material->setMagnetMaxForce(effectParam[0]*maxForce);
				((*it).mesh)->m_material->setMagnetMaxDistance(effectParam[1]);
				((*it).mesh)->createEffectMagnetic();
				((*it).mesh)->createEffectSurface();
			}
			if (effectType==4)
			{ // add viscosityc effect
				((*it).mesh)->m_material->setViscosity(effectParam[0]*maxDamping);
				((*it).mesh)->createEffectViscosity();
			}
			if (effectType==5)
			{ // add stick slip effect
			    ((*it).mesh)->m_material->setStickSlipForceMax(effectParam[1]*maxForce); 
			    ((*it).mesh)->m_material->setStickSlipStiffness(effectParam[1]*maxStiffness);
				((*it).mesh)->createEffectStickSlip();
			}
			if (effectType==6)
			{ // adjust friction
				((*it).mesh)->m_material->setStaticFriction(effectParam[0]);
				((*it).mesh)->m_material->setDynamicFriction(effectParam[1]);
			}
			break;
		}
	}
}

bool removeMeshObject_(int id)
{
	int found=0;
	vector<sObstacle>::iterator it;
	for (it=obstacles.begin();it!=obstacles.end();it++)
	{
		if ((*it).id==id)
		{
			found=1;
			break;
		}
	}
	if (!found)
		return false;
	if ((world->removeChild((*it).mesh)))
	{
		delete (*it).mesh;
		obstacles.erase(it);
		return true;
	}
	return false;	
}

void removeAllObjects()
{
	vector<sObstacle>::iterator it;
	for (it=obstacles.begin();it!=obstacles.end();it++)
		world->removeChild((*it).mesh);
	obstacles.clear();
	forces.clear();
}

int getButtons(int zeroBasedDeviceIndex,int *currentButtons,int *eventsButtons)
{
	if (zeroBasedDeviceIndex>MAX_HAPTIC-1)
		return -1;
	if (zeroBasedDeviceIndex<0)
		return -2;
	if (!(tools[zeroBasedDeviceIndex].used))
		return -3;
	if ((currentButtons== NULL) || (eventsButtons== NULL))
		return -4;

	*currentButtons=tools[zeroBasedDeviceIndex].currentButtons;
	*eventsButtons=tools[zeroBasedDeviceIndex].eventButtons;
	return 0;
}


bool removeMeshObject__(cMesh *mesh)
{
	int found=0;
	vector<sObstacle>::iterator it;
	for (it=obstacles.begin();it!=obstacles.end();it++)
	{
		if ((*it).mesh == mesh)
		{
			found=1;
			break;
		}
	}

	if (!found)
		return false;
	if (!(world->removeChild((*it).mesh)))
		return false;	
	obstacles.erase(it);
	return true;
}

void updateHaptics(void)
{
	chai3d::cVector3d toolpos,oldtoolpos;

    deviceRunning  = true;
    deviceThreadRunning = true;

	int lastTimeHere=getTimeInMs();
	while(deviceRunning)
    {
		int timeDiff=getTimeDiffInMs(lastTimeHere);
		lastTimeHere=getTimeInMs();
		std::vector<int> commandsAndIdAndOpt_4_internalPost;
		std::vector<double> commandData_10_internalPost;

		int i=0; // Device 0 (for now, one single device is supported) 
		if (tools[i].used) 
		{
			simpleLock();
			for (int j=0;j<int(commandsAndIdAndOpt_4.size())/4;j++)
			{
				int cmd=commandsAndIdAndOpt_4[4*j+0];
				int id=commandsAndIdAndOpt_4[4*j+1];
				int opt=commandsAndIdAndOpt_4[4*j+2];
				int opt2=commandsAndIdAndOpt_4[4*j+3];
				int pOff=10*j;
				if (cmd==0)
				{
					removeForce_(id);
					removeMeshObject_(id);
				}
				if (cmd==3)
				{
					addConstantForce_(id,&commandData_10[pOff]);
				}
				if (cmd==4)
				{
					addCentralSpring_(id,&commandData_10[pOff],commandData_10[pOff+3],commandData_10[pOff+4],commandData_10[pOff+5]);
				}
				if (cmd==5)
				{
					addMesh_(id,opt,&commandData_10[pOff],&commandData_10[pOff+3]);
				}
				if (cmd==6)
				{
					if (timeDiff<opt)
					{ // we need to set an interpolated position/orientation, and repost the updated command:
						float t=float(timeDiff)/float(opt);
						float currentMatr[12];
						if (getShapePose_(id,currentMatr))
						{
							float goalMatr[12];
							float pos[3]={float(commandData_10[pOff+0]),float(commandData_10[pOff+1]),float(commandData_10[pOff+2])};
							float euler[3]={float(commandData_10[pOff+3]),float(commandData_10[pOff+4]),float(commandData_10[pOff+5])};
							simBuildMatrix(pos,euler,goalMatr);
							float axis[3];
							float angle;
							simGetRotationAxis(currentMatr,goalMatr,axis,&angle);
							float interpolatedPos[3]={currentMatr[3]*(1.0f-t)+goalMatr[3]*t,currentMatr[7]*(1.0f-t)+goalMatr[7]*t,currentMatr[11]*(1.0f-t)+goalMatr[11]*t};
							float interpolatedMatrix[12];
							simRotateAroundAxis(currentMatr,axis,interpolatedPos,angle*t,interpolatedMatrix); // interpolatedPos is not important here
							float interpolatedEuler[3];
							simGetEulerAnglesFromMatrix(interpolatedMatrix,interpolatedEuler);
							double ipos[3]={interpolatedPos[0],interpolatedPos[1],interpolatedPos[2]};
							double ieul[3]={interpolatedEuler[0],interpolatedEuler[1],interpolatedEuler[2]};
							setShapePose_(id,ipos,ieul); 

							commandsAndIdAndOpt_4_internalPost.push_back(cmd);
							commandsAndIdAndOpt_4_internalPost.push_back(id);
							commandsAndIdAndOpt_4_internalPost.push_back(opt-timeDiff); // the time left
							commandsAndIdAndOpt_4_internalPost.push_back(0);
							for (int k=0;k<10;k++)
								commandData_10_internalPost.push_back(commandData_10[pOff+k]);
						}
					}
					else
						setShapePose_(id,&commandData_10[pOff+0],&commandData_10[pOff+3]); // set instantaneously to the target position
				}
				if (cmd==9)
				{
					setShapeEffect_(id,opt,&commandData_10[pOff]);
				}
			}
			commandsAndIdAndOpt_4.assign(commandsAndIdAndOpt_4_internalPost.begin(),commandsAndIdAndOpt_4_internalPost.end());
			commandData_10.assign(commandData_10_internalPost.begin(),commandData_10_internalPost.end());
			simpleUnlock();

			toolpos=tools[i].tool->getDeviceLocalPos();
			position[0]=float(toolpos.x()/wsScaleFactor);
			position[1]=float(toolpos.y()/wsScaleFactor);
			position[2]=float(toolpos.z()/wsScaleFactor);

			cVector3d v=tools[i].tool->m_lastComputedGlobalForce;
			interactionForce[0]=(float)v.x();
			interactionForce[1]=(float)v.y();
			interactionForce[2]=(float)v.z();
				
			int tempButtons= tools[i].tool->getUserSwitch(0)?1:0;
			tempButtons+=tools[i].tool->getUserSwitch(1)?2:0;
			tempButtons+=tools[i].tool->getUserSwitch(2)?4:0;
			tempButtons+=tools[i].tool->getUserSwitch(3)?8:0;
			if (tempButtons != tools[i].currentButtons)
			{
				tools[i].eventButtons=tools[i].currentButtons;
				tools[i].currentButtons=tempButtons;
				eventButtons=tools[i].eventButtons;
				currentButtons=tools[i].currentButtons;
			}

			// Set forces
			cVector3d force(0.0,0.0,0.0);
			cVector3d position;
			position=tools[i].tool->getDeviceGlobalPos();
	
			// read linear velocity 
			cVector3d linearVelocity;
			linearVelocity=tools[i].tool->getDeviceGlobalLinVel();

			vector<sForce>::iterator it;
			for (it=forces.begin();it!=forces.end();it++)
			{
				if ((*it).type==eCST_FORCE)
					force.add((*it).cstforce);
				else if ((*it).type==eCENT_SPRING)
				{
					// compute linear force
					cVector3d csforce = (*it).springConstantK * ((*it).springOriginPosition - position);
					// add linear damping force
					csforce.add( -(*it).springConstantC * linearVelocity); 
					double len=csforce.length();
					// limit string force
					if (len > (*it).springMaxForce)
					{
						if (len>0)
							force.add(csforce*(*it).springMaxForce/len);
					}
					else
						force.add(csforce);
				} 			
			}

			// apply reaction force to haptic device 
			tools[i].tool->m_lastComputedGlobalForce.add(force);

			// send forces to haptic device
			tools[i].tool->applyForces(); 

			world->computeGlobalPositions(true);

			// update position and orientation of tool
			tools[i].tool->updatePose();
			// compute interaction forces
			tools[i].tool->computeInteractionForces();
		}
    }
	removeAllObjects();
	meshes_vertices.clear();
	meshes_indices.clear();
	deviceThreadRunning = false;
}

bool init(float radius)
{
    world = new cWorld();

	string err;
	err= connectToDevice(0,radius); // create device 0
	if (err != "")
	{
		delete world;
		return(false);
	}

    hapticsThread = new cThread();
    hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);
	deviceRunning=true;
	return(true);
}

int getMeshIndexIfAvailable(float* vertices,int vertDataCnt,int* indices,int indDataCnt)
{
	for (int j=0;j<int(meshes_vertices.size());j++)
	{
		if ( (vertDataCnt==int(meshes_vertices[j].size()))&&(indDataCnt==int(meshes_indices[j].size())) )
		{
			bool same=true;
			for (int i=0;i<vertDataCnt;i++)
			{
				if (vertices[i]!=meshes_vertices[j][i])
				{
					same=false;
					break;
				}
			}
			if (same)
			{
				for (int i=0;i<indDataCnt;i++)
				{
					if (indices[i]!=meshes_indices[j][i])
					{
						same=false;
						break;
					}
				}
			}
			if (same)
				return(j);
		}
	}
	return(-1);
}


LIBRARY vrepLib; // the V-REP library that we will dynamically load and bind

#define CONCAT(x,y,z) x y z
#define strConCat(x,y,z)	CONCAT(x,y,z)
#define LUA_START_COMMAND "simExtCHAI3D_start"
#define LUA_STOP_COMMAND "simExtCHAI3D_stop"
#define LUA_READ_BUTTONS_COMMAND "simExtCHAI3D_readButtons"
#define LUA_READ_POSITION_COMMAND "simExtCHAI3D_readPosition"
#define LUA_READ_FORCE_COMMAND "simExtCHAI3D_readForce"
#define LUA_ADD_MESH_SHAPE_COMMAND "simExtCHAI3D_addMeshShape"
#define LUA_SET_SHAPE_EFFECT_COMMAND "simExtCHAI3D_setShapeEffect"
#define LUA_ADD_CONSTANT_FORCE_COMMAND "simExtCHAI3D_addConstantForce"
#define LUA_ADD_SPRING_FORCE_COMMAND "simExtCHAI3D_addSpringForce"
#define LUA_REMOVE_OBJECT_COMMAND "simExtCHAI3D_removeObject"
#define LUA_SET_SHAPE_POSE_COMMAND "simExtCHAI3D_setShapePose"
#define LUA_FLUSH_COMMAND "simExtCHAI3D_flush"

void LUA_START_CALLBACK(SLuaCallBack* p)
{
	/*
	cMatrix3d m(0.5f,2.5f,-1.2f,C_EULER_ORDER_ZYX,false);
	printf("***************Plugin**********\n");
	printf("%f\n",m(0,0));
	printf("%f\n",m(0,1));
	printf("%f\n",m(0,2));
	printf("%f\n",m(1,0));
	printf("%f\n",m(1,1));
	printf("%f\n",m(1,2));
	printf("%f\n",m(2,0));
	printf("%f\n",m(2,1));
	printf("%f\n",m(2,2));
	printf("-------------------------------\n");
	
	int hh=simGetObjectHandle("a");
	float euler[3]={0.5f,2.5f,-1.2f};
	simSetObjectOrientation(hh,-1,euler);
	float m2[12];
	simGetObjectMatrix(hh,-1,m2);

	printf("%f\n",m2[0]);
	printf("%f\n",m2[4]);
	printf("%f\n",m2[8]);
	printf("%f\n",m2[1]);
	printf("%f\n",m2[5]);
	printf("%f\n",m2[9]);
	printf("%f\n",m2[2]);
	printf("%f\n",m2[6]);
	printf("%f\n",m2[10]);

	printf("*******************************\n");
*/
	int commandResult=-1;

	if (p->inputArgCount>=2)
	{ // Ok, we have at least 2 input argument. We are expecting an integer and a float
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int) && // the integer value
			(p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_float) )
		{ // Ok, we have all required arguments
			int intArgInd=0;
			int floatArgInd=0;
			int deviceIndex=p->inputInt[intArgInd++]; 
			float sphereRadius=p->inputFloat[floatArgInd++]*float(wsScaleFactor);
			if (init(sphereRadius))
				commandResult=1;
			else
				simSetLastError(LUA_START_COMMAND,"Initialization failed."); // output an error
		}
		else
			simSetLastError(LUA_START_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_START_COMMAND,"Not enough arguments."); // output an error


	p->outputArgCount=1;
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt));
	p->outputArgTypeAndSize[0*2+0]=sim_lua_arg_int;
	p->outputArgTypeAndSize[0*2+1]=1;
	// Now create the int buffer and populate it:
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(int));
	p->outputInt[0]=commandResult;
}

void LUA_STOP_CALLBACK(SLuaCallBack* p)
{
	int commandResult=-1;

	if (p->inputArgCount>=1)
	{ // Ok, we have at least 1 input argument. We are expecting an integer
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have all required arguments
			int intArgInd=0;

			int deviceIndex=p->inputInt[intArgInd++]; 
			if (deinit())
				commandResult=1;
			else
				simSetLastError(LUA_STOP_COMMAND,"Deinitialization failed."); // output an error
		}
		else
			simSetLastError(LUA_STOP_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_STOP_COMMAND,"Not enough arguments."); // output an error


	p->outputArgCount=1;
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt));
	p->outputArgTypeAndSize[0*2+0]=sim_lua_arg_int;
	p->outputArgTypeAndSize[0*2+1]=1;
	// Now create the int buffer and populate it:
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(int));
	p->outputInt[0]=commandResult;
}

void LUA_FLUSH_CALLBACK(SLuaCallBack* p)
{
	int commandResult=-1;

	if (p->inputArgCount>=1)
	{ // Ok, we have at least 1 input argument. We are expecting an integer
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have all required arguments
			int intArgInd=0;

			int deviceIndex=p->inputInt[intArgInd++];
			flush();
			commandResult=1;
		}
		else
			simSetLastError(LUA_FLUSH_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_FLUSH_COMMAND,"Not enough arguments."); // output an error


	p->outputArgCount=0;
}

void LUA_ADD_MESH_SHAPE_CALLBACK(SLuaCallBack* p)
{
	int shapeHandle=-1;

	if (p->inputArgCount>=5)
	{
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int) &&
			(p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_float+sim_lua_arg_table)&&(p->inputArgTypeAndSize[1*2+1]>=3)&&
			(p->inputArgTypeAndSize[2*2+0]==sim_lua_arg_int+sim_lua_arg_table)&&(p->inputArgTypeAndSize[2*2+1]>=3)&&
			(p->inputArgTypeAndSize[3*2+0]==sim_lua_arg_float+sim_lua_arg_table)&&(p->inputArgTypeAndSize[3*2+1]==3)&&
			(p->inputArgTypeAndSize[4*2+0]==sim_lua_arg_float+sim_lua_arg_table)&&(p->inputArgTypeAndSize[4*2+1]==3) )
		{
			int intArgInd=0;
			int floatArgInd=0;

			int deviceIndex=p->inputInt[intArgInd++];
			int meshIndex=getMeshIndexIfAvailable(p->inputFloat+floatArgInd,p->inputArgTypeAndSize[1*2+1],p->inputInt+intArgInd,p->inputArgTypeAndSize[2*2+1]);
			if (meshIndex==-1)
			{ // do not add several times the same mesh, just reuse a previously added one!
				std::vector<float> vertices;
				vertices.assign(p->inputFloat+floatArgInd,p->inputFloat+floatArgInd+p->inputArgTypeAndSize[1*2+1]);
				for (int i=0;i<int(vertices.size());i++)
					vertices[i]*=float(wsScaleFactor);
				floatArgInd+=p->inputArgTypeAndSize[1*2+1];
				std::vector<int> indices;
				indices.assign(p->inputInt+intArgInd,p->inputInt+intArgInd+p->inputArgTypeAndSize[2*2+1]);
				intArgInd+=p->inputArgTypeAndSize[2*2+1];
				meshes_vertices.push_back(vertices);
				meshes_indices.push_back(indices);
				meshIndex=int(meshes_vertices.size()-1);
			}
			else
			{
				floatArgInd+=p->inputArgTypeAndSize[1*2+1];
				intArgInd+=p->inputArgTypeAndSize[2*2+1];
			}
			double pos[3];
			pos[0]=(double)p->inputFloat[floatArgInd++]*wsScaleFactor;
			pos[1]=(double)p->inputFloat[floatArgInd++]*wsScaleFactor;
			pos[2]=(double)p->inputFloat[floatArgInd++]*wsScaleFactor;
			double orient[3];
			orient[2]=(double)p->inputFloat[floatArgInd++];
			orient[1]=(double)p->inputFloat[floatArgInd++];
			orient[0]=(double)p->inputFloat[floatArgInd++];

			shapeHandle=addMesh(meshIndex,pos,orient);
		}
		else
			simSetLastError(LUA_ADD_MESH_SHAPE_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_ADD_MESH_SHAPE_COMMAND,"Not enough arguments."); // output an error


	p->outputArgCount=1;
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt));
	p->outputArgTypeAndSize[0*2+0]=sim_lua_arg_int;
	p->outputArgTypeAndSize[0*2+1]=1;
	// Now create the int buffer and populate it:
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(int));
	p->outputInt[0]=shapeHandle;
}

void LUA_SET_SHAPE_EFFECT_CALLBACK(SLuaCallBack* p)
{
	if (p->inputArgCount>=4)
	{
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int) && (p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_int) && (p->inputArgTypeAndSize[2*2+0]==sim_lua_arg_int) &&
			(p->inputArgTypeAndSize[3*2+0]==sim_lua_arg_float+sim_lua_arg_table)&&(p->inputArgTypeAndSize[3*2+1]==3) )
		{
			int intArgInd=0;
			int floatArgInd=0;

			int deviceIndex=p->inputInt[intArgInd++];
			int objHandle=p->inputInt[intArgInd++];
			int effectType=p->inputInt[intArgInd++];
			double effectParam[3];
			effectParam[0]=(double)p->inputFloat[floatArgInd++];
			effectParam[1]=(double)p->inputFloat[floatArgInd++];
			effectParam[2]=(double)p->inputFloat[floatArgInd++];
			setShapeEffect(objHandle,effectType,effectParam);
		}
		else
			simSetLastError(LUA_SET_SHAPE_EFFECT_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_SET_SHAPE_EFFECT_COMMAND,"Not enough arguments."); // output an error

	p->outputArgCount=0;
}

void LUA_REMOVE_OBJECT_CALLBACK(SLuaCallBack* p)
{
	if (p->inputArgCount>=2)
	{
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int) && (p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_int) )
		{
			int intArgInd=0;
			int floatArgInd=0;

			int deviceIndex=p->inputInt[intArgInd++];
			int objectHandle=p->inputInt[intArgInd++];

			removeObject(objectHandle);
		}
		else
			simSetLastError(LUA_REMOVE_OBJECT_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_REMOVE_OBJECT_COMMAND,"Not enough arguments."); // output an error

	p->outputArgCount=0;
}

void LUA_ADD_CONSTANT_FORCE_CALLBACK(SLuaCallBack* p)
{
	int forceHandle=-1;

	if (p->inputArgCount>=2)
	{
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int) &&
			(p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_float+sim_lua_arg_table)&&(p->inputArgTypeAndSize[1*2+1]==3) )
		{
			int intArgInd=0;
			int floatArgInd=0;

			int deviceIndex=p->inputInt[intArgInd++];
			double forceVector[3];
			forceVector[0]=(double)p->inputFloat[floatArgInd++];
			forceVector[1]=(double)p->inputFloat[floatArgInd++];
			forceVector[2]=(double)p->inputFloat[floatArgInd++];

			forceHandle=addConstantForce(forceVector);
		}
		else
			simSetLastError(LUA_ADD_CONSTANT_FORCE_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_ADD_CONSTANT_FORCE_COMMAND,"Not enough arguments."); // output an error


	p->outputArgCount=1;
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt));
	p->outputArgTypeAndSize[0*2+0]=sim_lua_arg_int;
	p->outputArgTypeAndSize[0*2+1]=1;
	// Now create the int buffer and populate it:
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(int));
	p->outputInt[0]=forceHandle;
}

void LUA_ADD_SPRING_FORCE_CALLBACK(SLuaCallBack* p)
{
	int forceHandle=-1;

	if (p->inputArgCount>=5)
	{
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int) &&
			(p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_float+sim_lua_arg_table)&&(p->inputArgTypeAndSize[1*2+1]==3)&&
			(p->inputArgTypeAndSize[2*2+0]==sim_lua_arg_float)&&
			(p->inputArgTypeAndSize[3*2+0]==sim_lua_arg_float)&&
			(p->inputArgTypeAndSize[4*2+0]==sim_lua_arg_float) )
		{
			int intArgInd=0;
			int floatArgInd=0;

			int deviceIndex=p->inputInt[intArgInd++];
			double pos[3];
			pos[0]=(double)p->inputFloat[floatArgInd++]*wsScaleFactor;
			pos[1]=(double)p->inputFloat[floatArgInd++]*wsScaleFactor;
			pos[2]=(double)p->inputFloat[floatArgInd++]*wsScaleFactor;
			double k=p->inputFloat[floatArgInd++];
			double c=p->inputFloat[floatArgInd++];
			double maxForce=p->inputFloat[floatArgInd++];

			forceHandle=addCentralSpring(pos,k,c,maxForce);
		}
		else
			simSetLastError(LUA_ADD_SPRING_FORCE_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_ADD_SPRING_FORCE_COMMAND,"Not enough arguments."); // output an error


	p->outputArgCount=1;
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt));
	p->outputArgTypeAndSize[0*2+0]=sim_lua_arg_int;
	p->outputArgTypeAndSize[0*2+1]=1;
	// Now create the int buffer and populate it:
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(int));
	p->outputInt[0]=forceHandle;
}

void LUA_READ_BUTTONS_CALLBACK(SLuaCallBack* p)
{
	bool success=false;

	if (p->inputArgCount>=1)
	{
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{
			int intArgInd=0;
			int deviceIndex=p->inputInt[intArgInd++];
			success=true;
		}
		else
			simSetLastError(LUA_READ_BUTTONS_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_READ_BUTTONS_COMMAND,"Not enough arguments."); // output an error

	if (currentButtons==-1)
		p->outputArgCount=0;
	else
	{
		p->outputArgCount=2;
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt));
		p->outputArgTypeAndSize[0*2+0]=sim_lua_arg_int;
		p->outputArgTypeAndSize[0*2+1]=1;
		p->outputArgTypeAndSize[1*2+0]=sim_lua_arg_int;
		p->outputArgTypeAndSize[1*2+1]=1;
		// Now create the int buffer and populate it:
		p->outputInt=(simInt*)simCreateBuffer(2*sizeof(int));
		p->outputInt[0]=currentButtons;
		p->outputInt[1]=eventButtons;
	}
}

void LUA_READ_POSITION_CALLBACK(SLuaCallBack* p)
{
	bool success=false;

	if (p->inputArgCount>=1)
	{
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{
			int intArgInd=0;
			int deviceIndex=p->inputInt[intArgInd++];
			success=true;
		}
		else
			simSetLastError(LUA_READ_POSITION_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_READ_POSITION_COMMAND,"Not enough arguments."); // output an error

	if (!success)
		p->outputArgCount=0;
	else
	{
		p->outputArgCount=1;
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt));
		p->outputArgTypeAndSize[0*2+0]=sim_lua_arg_float|sim_lua_arg_table;
		p->outputArgTypeAndSize[0*2+1]=3;
		// Now create the int buffer and populate it:
		p->outputFloat=(simFloat*)simCreateBuffer(3*sizeof(float));
		p->outputFloat[0]=position[0];
		p->outputFloat[1]=position[1];
		p->outputFloat[2]=position[2];
	}
}

void LUA_READ_FORCE_CALLBACK(SLuaCallBack* p)
{
	bool success=false;

	if (p->inputArgCount>=1)
	{
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{
			int intArgInd=0;
			int deviceIndex=p->inputInt[intArgInd++];
			success=true;
		}
		else
			simSetLastError(LUA_READ_FORCE_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_READ_FORCE_COMMAND,"Not enough arguments."); // output an error

	if (!success)
		p->outputArgCount=0;
	else
	{
		p->outputArgCount=1;
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt));
		p->outputArgTypeAndSize[0*2+0]=sim_lua_arg_float|sim_lua_arg_table;
		p->outputArgTypeAndSize[0*2+1]=3;
		// Now create the int buffer and populate it:
		p->outputFloat=(simFloat*)simCreateBuffer(3*sizeof(float));
		p->outputFloat[0]=interactionForce[0];
		p->outputFloat[1]=interactionForce[1];
		p->outputFloat[2]=interactionForce[2];
	}
}

void LUA_SET_SHAPE_POSE_CALLBACK(SLuaCallBack* p)
{
	if (p->inputArgCount>=4)
	{
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int) && (p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_int) &&
			(p->inputArgTypeAndSize[2*2+0]==sim_lua_arg_float+sim_lua_arg_table)&&(p->inputArgTypeAndSize[2*2+1]==3)&&
			(p->inputArgTypeAndSize[3*2+0]==sim_lua_arg_float+sim_lua_arg_table)&&(p->inputArgTypeAndSize[3*2+1]==3) )
		{ // Ok, all required arguments are there
			int interpolationTimeInMs=0;
			
			if (p->inputArgCount>=5)
			{ // We have an optional argument:
				if (p->inputArgTypeAndSize[4*2+0]==sim_lua_arg_int)
				{
					interpolationTimeInMs=1; // to indicate we need to read that value
				}
				else
				{ // bad argument type!
					simSetLastError(LUA_SET_SHAPE_POSE_COMMAND,"Wrong argument type/size."); // output an error
					interpolationTimeInMs=-1;
				}
			}
			
			if (interpolationTimeInMs>=0)
			{
				int intArgInd=0;
				int floatArgInd=0;

				int deviceIndex=p->inputInt[intArgInd++]; 
				int shapeId=p->inputInt[intArgInd++]; 
				double pos[3];
				pos[0]=(double)p->inputFloat[floatArgInd++]*wsScaleFactor;
				pos[1]=(double)p->inputFloat[floatArgInd++]*wsScaleFactor;
				pos[2]=(double)p->inputFloat[floatArgInd++]*wsScaleFactor;
				double orient[3];
				orient[2]=(double)p->inputFloat[floatArgInd++];
				orient[1]=(double)p->inputFloat[floatArgInd++];
				orient[0]=(double)p->inputFloat[floatArgInd++];
				
				if (interpolationTimeInMs==1)
					interpolationTimeInMs=p->inputInt[intArgInd++];

				setShapePose(shapeId,pos,orient,interpolationTimeInMs);
			}
		}
		else
			simSetLastError(LUA_SET_SHAPE_POSE_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_SET_SHAPE_POSE_COMMAND,"Not enough arguments."); // output an error

	p->outputArgCount=0;
}

VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt)
{
	char curDirAndFile[1024];
#ifdef _WIN32
	GetModuleFileName(NULL,curDirAndFile,1023);
	PathRemoveFileSpec(curDirAndFile);
#elif defined (__linux) || defined (__APPLE__)
	getcwd(curDirAndFile, sizeof(curDirAndFile));
#endif
	std::string currentDirAndPath(curDirAndFile);
	std::string temp(currentDirAndPath);
#ifdef _WIN32
	temp+="\\v_rep.dll";
#elif defined (__linux)
	temp+="/libv_rep.so";
#elif defined (__APPLE__)
	temp+="/libv_rep.dylib";
#endif /* __linux || __APPLE__ */
	vrepLib=loadVrepLibrary(temp.c_str());
	if (vrepLib==NULL)
	{
		std::cout << "Error, could not find or correctly load the V-REP library. Cannot start 'CHAI3D' plugin.\n";
		return(0); // Means error, V-REP will unload this plugin
	}
	if (getVrepProcAddresses(vrepLib)==0)
	{
		std::cout << "Error, could not find all required functions in the V-REP library. Cannot start 'CHAI3D' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}

	int vrepVer;
	simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
	if (vrepVer<30103) // if V-REP version is smaller than 3.01.03
	{
		std::cout << "Sorry, your V-REP copy is somewhat old. Cannot start 'CHAI3D' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}


	// Register the new Lua commands
	int inArgs_start[]={2,sim_lua_arg_int,sim_lua_arg_float};
	simRegisterCustomLuaFunction(LUA_START_COMMAND,strConCat("number result=",LUA_START_COMMAND,"(number deviceIndex,number sphereRadius)"),inArgs_start,LUA_START_CALLBACK);

	int inArgs_stop[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_STOP_COMMAND,strConCat("number result=",LUA_STOP_COMMAND,"(number deviceIndex)"),inArgs_stop,LUA_STOP_CALLBACK);

	int inArgs_flush[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_FLUSH_COMMAND,strConCat("",LUA_FLUSH_COMMAND,"(number deviceIndex)"),inArgs_flush,LUA_FLUSH_CALLBACK);

	int inArgs_addMeshShape[]={5,sim_lua_arg_int,sim_lua_arg_float|sim_lua_arg_table,sim_lua_arg_int|sim_lua_arg_table,sim_lua_arg_float|sim_lua_arg_table,sim_lua_arg_float|sim_lua_arg_table};
	simRegisterCustomLuaFunction(LUA_ADD_MESH_SHAPE_COMMAND,strConCat("number objectHandle=",LUA_ADD_MESH_SHAPE_COMMAND,"(number deviceIndex,table vertices,table indices,table_3 position,table_3 orientation)"),inArgs_addMeshShape,LUA_ADD_MESH_SHAPE_CALLBACK);

	int inArgs_setShapeEffect[]={4,sim_lua_arg_int,sim_lua_arg_int,sim_lua_arg_int,sim_lua_arg_float|sim_lua_arg_table};
	simRegisterCustomLuaFunction(LUA_SET_SHAPE_EFFECT_COMMAND,strConCat("",LUA_SET_SHAPE_EFFECT_COMMAND,"(number deviceIndex,number objectHandle,number effectType,table_3 effectParameters)"),inArgs_setShapeEffect,LUA_SET_SHAPE_EFFECT_CALLBACK);

	int inArgs_removeShape[]={2,sim_lua_arg_int,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_REMOVE_OBJECT_COMMAND,strConCat("",LUA_REMOVE_OBJECT_COMMAND,"(number deviceIndex,number objectHandle)"),inArgs_removeShape,LUA_REMOVE_OBJECT_CALLBACK);

	int inArgs_addConstantForce[]={2,sim_lua_arg_int,sim_lua_arg_float|sim_lua_arg_table};
	simRegisterCustomLuaFunction(LUA_ADD_CONSTANT_FORCE_COMMAND,strConCat("number objectHandle=",LUA_ADD_CONSTANT_FORCE_COMMAND,"(number deviceIndex,table_3 forceVector)"),inArgs_addConstantForce,LUA_ADD_CONSTANT_FORCE_CALLBACK);

	int inArgs_addSpringForce[]={5,sim_lua_arg_int,sim_lua_arg_float|sim_lua_arg_table,sim_lua_arg_float,sim_lua_arg_float,sim_lua_arg_float};
	simRegisterCustomLuaFunction(LUA_ADD_SPRING_FORCE_COMMAND,strConCat("number objectHandle=",LUA_ADD_SPRING_FORCE_COMMAND,"(number deviceIndex,table_3 position,number kConstant,number cConstant,number maxForce)"),inArgs_addSpringForce,LUA_ADD_SPRING_FORCE_CALLBACK);

	int inArgs_readButtons[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_READ_BUTTONS_COMMAND,strConCat("number currentButton,number eventButton=",LUA_READ_BUTTONS_COMMAND,"(number deviceIndex)"),inArgs_readButtons,LUA_READ_BUTTONS_CALLBACK);

	int inArgs_readPosition[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_READ_POSITION_COMMAND,strConCat("table_3 position=",LUA_READ_POSITION_COMMAND,"(number deviceIndex)"),inArgs_readPosition,LUA_READ_POSITION_CALLBACK);

	int inArgs_readForce[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_READ_FORCE_COMMAND,strConCat("table_3 force=",LUA_READ_FORCE_COMMAND,"(number deviceIndex)"),inArgs_readForce,LUA_READ_FORCE_CALLBACK);

	int inArgs_setShapePose[]={5,sim_lua_arg_int,sim_lua_arg_int,sim_lua_arg_float|sim_lua_arg_table,sim_lua_arg_float|sim_lua_arg_table,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_SET_SHAPE_POSE_COMMAND,strConCat("",LUA_SET_SHAPE_POSE_COMMAND,"(number deviceIndex,number shapeHandle,table_3 position,table_3 orientation,number interpolationTimeInMs=0)"),inArgs_setShapePose,LUA_SET_SHAPE_POSE_CALLBACK);

#ifdef _WIN32
	_mutex=CreateMutex(0,FALSE,0);
#elif defined (__linux) || defined (__APPLE__)
	pthread_mutex_init(&_mutex, NULL);
#endif /* __linux || __APPLE__ */

	return(PLUGIN_VERSION); // initialization went fine, we return the version number of this plugin (can be queried with simGetModuleName)
}

VREP_DLLEXPORT void v_repEnd()
{
	if (handler!=NULL)
		delete handler;

	unloadVrepLibrary(vrepLib); // release the library
}

VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{ // This is called quite often. Just watch out for messages/events you want to handle
	// Keep following 4 lines at the beginning and unchanged:
	int errorModeSaved;
	simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
	simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);
	void* retVal=NULL;

	simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
	return(retVal);
}

