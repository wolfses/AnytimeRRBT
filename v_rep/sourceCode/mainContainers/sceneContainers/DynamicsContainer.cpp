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
#include "DynamicsContainer.h"

#include "dynInterface.h"
#include "IloIlo.h"
#include "App.h"
#include "v_repStringTable.h"
#include "OGL.h"
#include "Tt.h"

// index0=very precise, index1=precise (default), index2=fast, index3=very fast
const float DYNAMIC_BULLET_DEFAULT_STEP_SIZE[4]={0.005f,0.005f,0.005f,0.005f};
const int DYNAMIC_BULLET_DEFAULT_CONSTRAINT_SOLVING_ITERATIONS[4]={200,100,50,20};
const bool DYNAMIC_BULLET_DEFAULT_FULL_INTERNAL_SCALING[4]={true,true,true,true};
const float DYNAMIC_BULLET_DEFAULT_INTERNAL_SCALING_FACTOR[4]={10.0f,10.0f,10.0f,10.0f};
const float DYNAMIC_BULLET_DEFAULT_COLLISION_MARGIN_FACTOR[4]={0.1f,0.1f,0.1f,0.1f};

const float DYNAMIC_ODE_DEFAULT_STEP_SIZE[4]={0.005f,0.005f,0.005f,0.005f};
const int DYNAMIC_ODE_DEFAULT_CONSTRAINT_SOLVING_ITERATIONS[4]={200,100,50,20};
const bool DYNAMIC_ODE_DEFAULT_FULL_INTERNAL_SCALING[4]={true,true,true,true};
const float DYNAMIC_ODE_DEFAULT_INTERNAL_SCALING_FACTOR[4]={1.0f,1.0f,1.0f,1.0f};
const bool DYNAMIC_ODE_USE_QUICKSTEP[4]={true,true,true,true};
const float DYNAMIC_ODE_GLOBAL_CFM[4]={0.00001f,0.00001f,0.00001f,0.00001f};
const float DYNAMIC_ODE_GLOBAL_ERP[4]={0.6f,0.6f,0.6f,0.6f};

const float DYNAMIC_VORTEX_DEFAULT_STEP_SIZE[4]={0.0025f,0.005f,0.01f,0.025f};
const bool DYNAMIC_VORTEX_DEFAULT_AUTO_SLEEP[4]={true,true,true,true};
const float DYNAMIC_VORTEX_DEFAULT_INTERNAL_SCALING_FACTOR[4]={1.0f,1.0f,1.0f,1.0f};
const float DYNAMIC_VORTEX_DEFAULT_CONTACT_TOLERANCE[4]={0.001f,0.001f,0.001f,0.001f};
const bool DYNAMIC_VORTEX_DEFAULT_MULTITHREADING[4]={true,true,true,true};
const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_LIN_COMPLIANCE[4]={1.0e-7,1.0e-7,1.0e-7,1.0e-7};
const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_LIN_DAMPING[4]={8.0e+6,8.0e+6,8.0e+6,8.0e+6};
const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_LIN_KIN_LOSS[4]={6.0e-5,6.0e-5,6.0e-5,6.0e-5};
const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_ANG_COMPLIANCE[4]={1.0e-9,1.0e-9,1.0e-9,1.0e-9};
const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_ANG_DAMPING[4]={8.0e+8,8.0e+8,8.0e+8,8.0e+8};
const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_ANG_KIN_LOSS[4]={6.0e-7,6.0e-7,6.0e-7,6.0e-7};
//const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_LIN_COMPLIANCE[4]={1.0e-10,1.0e-10,1.0e-10,1.0e-10};
//const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_LIN_DAMPING[4]={8.33e+8,8.33e+8,8.33e+8,8.33e+8};
//const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_LIN_KIN_LOSS[4]={6.0e-9,6.0e-9,6.0e-9,6.0e-9};
//const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_ANG_COMPLIANCE[4]={1.0e-10,1.0e-10,1.0e-10,1.0e-10};
//const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_ANG_DAMPING[4]={8.33e+8,8.33e+8,8.33e+8,8.33e+8};
//const float DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_ANG_KIN_LOSS[4]={6.0e-9,6.0e-9,6.0e-9,6.0e-9};





const float SPHEREVERTICES[24*3]={
-0.4142f,-1.0000f,-0.4142f,
+0.4142f,-1.0000f,-0.4142f,
+0.4142f,-1.0000f,+0.4142f,
-0.4142f,-1.0000f,+0.4142f,

-0.4142f,-0.4142f,-1.0000f,
-0.4142f,+0.4142f,-1.0000f,
+0.4142f,+0.4142f,-1.0000f,
+0.4142f,-0.4142f,-1.0000f,

+0.4142f,+1.0000f,-0.4142f,
-0.4142f,+1.0000f,-0.4142f,
-0.4142f,+1.0000f,+0.4142f,
+0.4142f,+1.0000f,+0.4142f,

+0.4142f,-0.4142f,+1.0000f,
+0.4142f,+0.4142f,+1.0000f,
-0.4142f,+0.4142f,+1.0000f,
-0.4142f,-0.4142f,+1.0000f,

+1.0000f,-0.4142f,-0.4142f,
+1.0000f,+0.4142f,-0.4142f,
+1.0000f,+0.4142f,+0.4142f,
+1.0000f,-0.4142f,+0.4142f,

-1.0000f,-0.4142f,+0.4142f,
-1.0000f,+0.4142f,+0.4142f,
-1.0000f,+0.4142f,-0.4142f,
-1.0000f,-0.4142f,-0.4142f
};

const int SPHEREQUADINDICES[18*4]={
0,1,2,3,
4,5,6,7,
8,9,10,11,
12,13,14,15,
16,17,18,19,
20,21,22,23,

0,4,7,1,
1,16,19,2,
3,2,12,15,
0,3,20,23,

6,5,9,8,
18,17,8,11,
14,13,11,10,
10,9,22,21,

7,6,17,16,
19,18,13,12,
15,14,21,20,
23,22,5,4
};

const float SPHEREQUADNORMALS[18*3]={
+0.0f,-1.0f,+0.0f,
+0.0f,+0.0f,-1.0f,
+0.0f,+1.0f,+0.0f,
+0.0f,+0.0f,+1.0f,
+1.0f,+0.0f,+0.0f,
-1.0f,+0.0f,+0.0f,

+0.0f,-0.7071f,-0.7071f,
+0.7071f,-0.7071f,+0.0f,
+0.0f,-0.7071f,+0.7071f,
-0.7071f,-0.7071f,+0.0f,

+0.0f,+0.7071f,-0.7071f,
+0.7071f,+0.7071f,+0.0f,
+0.0f,+0.7071f,+0.7071f,
-0.7071f,+0.7071f,+0.0f,

+0.7071f,0.0f,-0.7071f,
+0.7071f,0.0f,+0.7071f,
-0.7071f,0.0f,+0.7071f,
-0.7071f,0.0f,-0.7071f
};

const int SPHERETRIANGLEINDICES[8*3]={
1,7,16,
2,19,12,
3,15,20,
0,23,4,

17,6,8,
18,11,13,
14,10,21,
22,9,5
};

const float SPHERETRIANGLENORMALS[8*3]={
+0.5773f,-0.5773f,-0.5773f,
+0.5773f,-0.5773f,+0.5773f,
-0.5773f,-0.5773f,+0.5773f,
-0.5773f,-0.5773f,-0.5773f,

+0.5773f,+0.5773f,-0.5773f,
+0.5773f,+0.5773f,+0.5773f,
-0.5773f,+0.5773f,+0.5773f,
-0.5773f,+0.5773f,-0.5773f,
};

CDynamicsContainer::CDynamicsContainer()
{
	_dynamicsEnabled=true;
	_dynamicDefaultTypeCalculationParameters=1; // means precise

	_dynamicsBULLETStepSize=DYNAMIC_BULLET_DEFAULT_STEP_SIZE[_dynamicDefaultTypeCalculationParameters];
	_dynamicBULLETConstraintSolvingIterations=DYNAMIC_BULLET_DEFAULT_CONSTRAINT_SOLVING_ITERATIONS[_dynamicDefaultTypeCalculationParameters];
	_dynamicBULLETFullInternalScaling=true;//DYNAMIC_BULLET_DEFAULT_FULL_INTERNAL_SCALING[_dynamicDefaultTypeCalculationParameters];
	_dynamicBULLETInternalScalingFactor=DYNAMIC_BULLET_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters];
	_dynamicBULLETCollisionMarginFactor=DYNAMIC_BULLET_DEFAULT_COLLISION_MARGIN_FACTOR[_dynamicDefaultTypeCalculationParameters];

	_dynamicsODEStepSize=DYNAMIC_ODE_DEFAULT_STEP_SIZE[_dynamicDefaultTypeCalculationParameters];
	_dynamicODEInternalScalingFactor=DYNAMIC_ODE_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters];
	_dynamicODEConstraintSolvingIterations=DYNAMIC_ODE_DEFAULT_CONSTRAINT_SOLVING_ITERATIONS[_dynamicDefaultTypeCalculationParameters];
	_dynamicODEFullInternalScaling=true;//DYNAMIC_ODE_DEFAULT_FULL_INTERNAL_SCALING[_dynamicDefaultTypeCalculationParameters];
	_dynamicODEUseQuickStep=DYNAMIC_ODE_USE_QUICKSTEP[_dynamicDefaultTypeCalculationParameters];
	_dynamicODEGlobalCFM=DYNAMIC_ODE_GLOBAL_CFM[_dynamicDefaultTypeCalculationParameters];
	_dynamicODEGlobalERP=DYNAMIC_ODE_GLOBAL_ERP[_dynamicDefaultTypeCalculationParameters];

	getVortexDefaultFloatParams(_vortexFloatParams,1);
	getVortexDefaultIntParams(_vortexIntParams,1);

	_dynamicEngineToUse=sim_physics_bullet; // Bullet is default

	contactPointColor.setDefaultValues();
	contactPointColor.setColor(1.0f,1.0f,0.0f,AMBIENT_MODE);
	contactPointColor.setColor(0.0f,0.0f,0.0f,DIFFUSE_MODE);
	contactPointColor.setColor(0.0f,0.0f,0.0f,SPECULAR_MODE);
	contactPointColor.setColor(1.0f,1.0f,0.0f,AMBIENT_MODE);
	_displayContactPoints=false;

	_gravity=C3Vector(0.0f,0.0f,-9.81f);
	_resetWarningFlags();
}

CDynamicsContainer::~CDynamicsContainer()
{
}

void CDynamicsContainer::simulationAboutToStart()
{
	_resetWarningFlags();

	removeWorld(); // not really needed

	// Following is just in case:
	for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
		it->disableDynamicTreeForManipulation(false);
	}

	// Keep following (important that it is initialized BEFORE simHandleDynamics is called!!)
	if (getDynamicsEnabled())
		addWorldIfNotThere();
}

void CDynamicsContainer::simulationEnded()
{
	removeWorld();

	// Following is because some shapes might have been disabled because below z=-1000 meters:
	for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
		it->disableDynamicTreeForManipulation(false);
	}
	
	_resetWarningFlags();
}

void CDynamicsContainer::_resetWarningFlags()
{
	_containsNonPureNonConvexShapes=0;
	_containsStaticShapesOnDynamicConstruction=0;
	_pureSpheroidNotSupportedMark=0;
	_pureConeNotSupportedMark=0;
	_pureHollowShapeNotSupportedMark=0;
	_physicsEngineNotSupportedWarning=0;
	_nonDefaultEngineSettingsWarning=0;
	_vortexPluginIsDemoWarning=0;
}

void CDynamicsContainer::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{ // Has to be displayed as overlay!
	int index=0;
	float* cols;
	int objectType;
	int particlesCount;
	C4X4Matrix m(renderingObject->getCumulativeTransformation().getMatrix());
	void** particlesPointer=CDynInterface::getParticles(index++,&particlesCount,&objectType,&cols);
	while (particlesCount!=-1)
	{
		if ((particlesPointer!=NULL)&&(particlesCount>0)&&((objectType&sim_particle_invisible)==0))
		{
			if ( ((displayAttrib&sim_displayattribute_forvisionsensor)==0)||(objectType&sim_particle_painttag) )
			{
				ogl::setColor(cols,48,0);
				C3Vector pos;
				float size;
				int particleType;
				float* additionalColor;
				for (int i=0;i<particlesCount;i++)
				{
					if (particlesPointer[i]!=NULL)
					{
						if (CDynInterface::getParticleData(particlesPointer[i],pos.data,&size,&particleType,&additionalColor)!=0)
							_renderParticle(displayAttrib,m,pos,size,particleType,additionalColor);
					}
				}
			}
		}
		particlesPointer=CDynInterface::getParticles(index++,&particlesCount,&objectType,&cols);
	}
}

void CDynamicsContainer::renderYour3DStuff_overlay(CViewableBase* renderingObject,int displayAttrib)
{ // Has to be displayed as overlay!
	if ((displayAttrib&sim_displayattribute_renderpass)&&((displayAttrib&sim_displayattribute_forvisionsensor)==0) )
	{
		if (getDisplayContactPoints())
		{
			int cnt=0;
			float* pts=CDynInterface::getContactPoints(&cnt);
			
			glDisable(GL_DEPTH_TEST);
			glLoadName(-1);
			glPointSize(4.0f);
			contactPointColor.makeCurrentColor();
			for (int i=0;i<cnt;i++)
			{ // Contact points come in pairs!
				glBegin(GL_POINTS);
				glVertex3d(pts[3*i+0],pts[3*i+1],pts[3*i+2]);
				glEnd();
			}
			glLineWidth(1.0f);
			glPointSize(1.0f);
			glEnable(GL_DEPTH_TEST);
		}
	}
}

void CDynamicsContainer::handleDynamics(float dt)
{
	App::ct->infoDisplay->dynamicsStart();

	for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
		if (it!=NULL)
			it->setDynamicObjectFlag_forVisualization(0);
	}
	addWorldIfNotThere();

	if (getDynamicsEnabled())
		CDynInterface::step(dt,App::ct->simulation->getSimulationTime());

	if (CDynInterface::isDynamicContentAvailable()!=0)
		App::ct->infoDisplay->dynamicsEnd(CDynInterface::getDynamicStepDivider(),true);
	else
		App::ct->infoDisplay->dynamicsEnd(0,false);
}

bool CDynamicsContainer::getContactForce(int dynamicPass,int objectHandle,int index,int objectHandles[2],float contactInfo[6])
{
	if (getDynamicsEnabled())
		return(CDynInterface::getContactForce(dynamicPass,objectHandle,index,objectHandles,contactInfo)!=0);
	return(false);
}

void CDynamicsContainer::reportDynamicWorldConfiguration()
{
	if (getDynamicsEnabled())
		CDynInterface::reportDynamicWorldConfiguration(-1,1,0.0f);
}

void CDynamicsContainer::addWorldIfNotThere()
{
	if (getDynamicsEnabled()&&(CDynInterface::isInitialized()==0))
	{
		float floatParams[50];
		int intParams[50];
		char boolParams[50];
		int floatIndex=0;
		int intIndex=0;
		int boolIndex=0;
		floatParams[floatIndex++]=getDynamicsStepSize();
		floatParams[floatIndex++]=getBulletDynamicCollisionMarginFactor();
		floatParams[floatIndex++]=getODEDynamicsStepSize();
		floatParams[floatIndex++]=getODEDynamicGlobalCFM();
		floatParams[floatIndex++]=getODEDynamicGlobalERP();

		floatParams[floatIndex++]=getPositionScalingFactorDyn();
		floatParams[floatIndex++]=getLinearVelocityScalingFactorDyn();
		floatParams[floatIndex++]=getMassScalingFactorDyn();
		floatParams[floatIndex++]=getMasslessInertiaScalingFactorDyn();
		floatParams[floatIndex++]=getForceScalingFactorDyn();
		floatParams[floatIndex++]=getTorqueScalingFactorDyn();
		floatParams[floatIndex++]=getGravityScalingFactorDyn();
		floatParams[floatIndex++]=App::userSettings->dynamicActivityRange;
		std::vector<float> v;
		getVortexFloatParams(v);
		floatParams[floatIndex++]=v[0];


		intParams[intIndex++]=getDynamicConstraintSolvingIterations();
		intParams[intIndex++]=getODEDynamicConstraintSolvingIterations();
		intParams[intIndex++]=SIM_IDEND_3DOBJECT+1;
		intParams[intIndex++]=SIM_IDSTART_3DOBJECT;
		intParams[intIndex++]=SIM_IDEND_3DOBJECT;


		boolParams[boolIndex++]=getODEUseQuickStep();




		CDynInterface::startSimulation(getDynamicEngineType(),floatParams,intParams,boolParams);
	}
}

void CDynamicsContainer::removeWorld()
{
	if (CDynInterface::isInitialized()!=0)
		CDynInterface::endSimulation();
}

bool CDynamicsContainer::isWorldThere()
{
	return(CDynInterface::isInitialized()!=0);
}

void CDynamicsContainer::markForWarningDisplay_pureSpheroidNotSupported()
{
	if (_pureSpheroidNotSupportedMark==0)
		_pureSpheroidNotSupportedMark++;
}

void CDynamicsContainer::markForWarningDisplay_pureConeNotSupported()
{
	if (_pureConeNotSupportedMark==0)
		_pureConeNotSupportedMark++;
}

void CDynamicsContainer::markForWarningDisplay_pureHollowShapeNotSupported()
{
	if (_pureHollowShapeNotSupportedMark==0)
		_pureHollowShapeNotSupportedMark++;
}

void CDynamicsContainer::markForWarningDisplay_physicsEngineNotSupported()
{
	if (_physicsEngineNotSupportedWarning==0)
		_physicsEngineNotSupportedWarning++;
}

void CDynamicsContainer::markForWarningDisplay_vortexPluginIsDemo()
{
	if (_vortexPluginIsDemoWarning==0)
		_vortexPluginIsDemoWarning++;
}

bool CDynamicsContainer::displayVortexPluginIsDemoRequired()
{
	if (_vortexPluginIsDemoWarning==1)
	{
		_vortexPluginIsDemoWarning++;
		return(true);
	}
	return(false);
}

void CDynamicsContainer::markForWarningDisplay_containsNonPureNonConvexShapes()
{
	if (_containsNonPureNonConvexShapes==0)
		_containsNonPureNonConvexShapes++;
}

void CDynamicsContainer::markForWarningDisplay_containsStaticShapesOnDynamicConstruction()
{
	if (_containsNonPureNonConvexShapes==0)
		_containsNonPureNonConvexShapes++;
}

bool CDynamicsContainer::displayNonPureNonConvexShapeWarningRequired()
{
	if (_containsNonPureNonConvexShapes==1)
	{
		_containsNonPureNonConvexShapes++;
		return(true);
	}
	return(false);
}

bool CDynamicsContainer::displayStaticShapeOnDynamicConstructionWarningRequired()
{
	if (_containsStaticShapesOnDynamicConstruction==1)
	{
		_containsStaticShapesOnDynamicConstruction++;
		return(true);
	}
	return(false);
}

bool CDynamicsContainer::displayNonDefaultParameterWarningRequired()
{
	if (getUseDynamicDefaultCalculationParameters()!=1)
	{
		if (_nonDefaultEngineSettingsWarning==0)
		{
			_nonDefaultEngineSettingsWarning++;
			return(true);
		}
	}
	return(false);
}

void CDynamicsContainer::displayWarningsIfNeeded()
{
	if (_pureSpheroidNotSupportedMark==1)
	{
		App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_PURE_SPHEROID),strTranslate(IDS_WARNING_WHEN_PURE_SPHEROID_NOT_SUPPORTED),VMessageBox::OKELI);
		_pureSpheroidNotSupportedMark++;
	}
	if (_pureConeNotSupportedMark==1)
	{
		App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_PURE_CONE),strTranslate(IDS_WARNING_WHEN_PURE_CONE_NOT_SUPPORTED),VMessageBox::OKELI);
		_pureConeNotSupportedMark++;
	}
	if (_pureHollowShapeNotSupportedMark==1)
	{
		App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_PURE_HOLLOW_SHAPES),strTranslate(IDS_WARNING_WHEN_PURE_HOLLOW_SHAPE_NOT_SUPPORTED),VMessageBox::OKELI);
		_pureHollowShapeNotSupportedMark++;
	}
	if (_physicsEngineNotSupportedWarning==1)
	{
		if (_dynamicEngineToUse==sim_physics_vortex)
		{
#ifdef WIN_VREP
			App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_PHYSICS_ENGINE),strTranslate(IDS_WARNING_WHEN_PHYSICS_ENGINE_NOT_SUPPORTED),VMessageBox::OKELI);
#endif
#ifdef LIN_VREP
#ifdef QT_VREP_64BIT
			App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_PHYSICS_ENGINE),strTranslate(IDS_WARNING_WHEN_PHYSICS_ENGINE_NOT_SUPPORTED),VMessageBox::OKELI);
#else
			App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_PHYSICS_ENGINE),strTranslate(IDS_WARNING_WHEN_VORTEX_NOT_YET_SUPPORTED),VMessageBox::OKELI);
#endif
#endif
#ifdef MAC_VREP
			App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_PHYSICS_ENGINE),strTranslate(IDS_WARNING_WHEN_VORTEX_NOT_YET_SUPPORTED),VMessageBox::OKELI);
#endif
		}
		else
			App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_PHYSICS_ENGINE),strTranslate(IDS_WARNING_WHEN_PHYSICS_ENGINE_NOT_SUPPORTED),VMessageBox::OKELI);

		_physicsEngineNotSupportedWarning++;
	}
}

void CDynamicsContainer::_renderParticle(int displayAttrib,const C4X4Matrix& cameraCTM,const C3Vector& _currentPosition,float _size,int _objectType,const float* _additionalColor)
{
	glPushMatrix();
	glPushAttrib(GL_POLYGON_BIT);
	glLoadName(-1);

	bool wire=( (_objectType&sim_drawing_wireframe)||(displayAttrib&sim_displayattribute_forcewireframe))&&((displayAttrib&sim_displayattribute_depthpass)==0);
	if (displayAttrib&sim_displayattribute_forbidwireframe)
		wire=false;
	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glEnable(GL_CULL_FACE);


	int tmp=_objectType&0x001f;
	if ((tmp==sim_particle_points1)||(tmp==sim_particle_points2)||(tmp==sim_particle_points4))
		_drawPoints(displayAttrib,cameraCTM,_currentPosition,_size,_objectType,_additionalColor);
	if (tmp==sim_particle_roughspheres)
		_drawRoughSphere(displayAttrib,_currentPosition,_size,_objectType,_additionalColor);
	if (tmp==sim_particle_spheres)
		_drawSphere(displayAttrib,_currentPosition,_size,_objectType,_additionalColor);

	glDisable(GL_CULL_FACE);
	glPopAttrib();
	glPopMatrix();
}


void CDynamicsContainer::_drawPoints(int displayAttrib,const C4X4Matrix& cameraRTM,const C3Vector& _currentPosition,float _size,int _objectType,const float* _additionalColor)
{
	C3Vector n(cameraRTM.M.axis[2]*-1.0f);
	int tmp=_objectType&0x001f;
	if (tmp==sim_particle_points1)
		glPointSize(1.0f);
	if (tmp==sim_particle_points2)
		glPointSize(2.0f);
	if (tmp==sim_particle_points4)
		glPointSize(4.0f);
	glBegin(GL_POINTS);
	C3Vector v;
	float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
	if (_objectType&sim_particle_itemcolors)
	{
		rgbaAmb[0]=_additionalColor[0];
		rgbaAmb[1]=_additionalColor[1];
		rgbaAmb[2]=_additionalColor[2];
		if (_objectType&sim_particle_emissioncolor)
			glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
		else
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
	}
	glNormal3fv(n.data);
	glVertex3fv(_currentPosition.data);
	glEnd();
	glPointSize(1.0f);
}


void CDynamicsContainer::_drawRoughSphere(int displayAttrib,const C3Vector& _currentPosition,float _size,int _objectType,const float* _additionalColor)
{
	float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
	if (_objectType&sim_particle_itemcolors)
	{
		rgbaAmb[0]=_additionalColor[0];
		rgbaAmb[1]=_additionalColor[1];
		rgbaAmb[2]=_additionalColor[2];
		if (_objectType&sim_particle_emissioncolor)
			glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
		else
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
	}
	glBegin(GL_QUADS);
	for (int j=0;j<18;j++)
	{
		C3Vector x0(SPHEREVERTICES+3*SPHEREQUADINDICES[4*j+0]);
		C3Vector x1(SPHEREVERTICES+3*SPHEREQUADINDICES[4*j+1]);
		C3Vector x2(SPHEREVERTICES+3*SPHEREQUADINDICES[4*j+2]);
		C3Vector x3(SPHEREVERTICES+3*SPHEREQUADINDICES[4*j+3]);
		x0=_currentPosition+x0*_size*0.5f;
		x1=_currentPosition+x1*_size*0.5f;
		x2=_currentPosition+x2*_size*0.5f;
		x3=_currentPosition+x3*_size*0.5f;
		glNormal3fv(SPHEREQUADNORMALS+3*j);
		glVertex3fv(x0.data);
		glVertex3fv(x1.data);
		glVertex3fv(x2.data);
		glVertex3fv(x3.data);
	}
	glEnd();
	glBegin(GL_TRIANGLES);
	for (int j=0;j<8;j++)
	{
		C3Vector x0(SPHEREVERTICES+3*SPHERETRIANGLEINDICES[3*j+0]);
		C3Vector x1(SPHEREVERTICES+3*SPHERETRIANGLEINDICES[3*j+1]);
		C3Vector x2(SPHEREVERTICES+3*SPHERETRIANGLEINDICES[3*j+2]);
		x0=_currentPosition+x0*_size*0.5f;
		x1=_currentPosition+x1*_size*0.5f;
		x2=_currentPosition+x2*_size*0.5f;
		glNormal3fv(SPHERETRIANGLENORMALS+3*j);
		glVertex3fv(x0.data);
		glVertex3fv(x1.data);
		glVertex3fv(x2.data);
	}
	glEnd();
}

void CDynamicsContainer::_drawSphere(int displayAttrib,const C3Vector& _currentPosition,float _size,int _objectType,const float* _additionalColor)
{
	glPushMatrix();
	glTranslatef(_currentPosition(0),_currentPosition(1),_currentPosition(2));

	float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
	if (_objectType&sim_particle_itemcolors)
	{
		rgbaAmb[0]=_additionalColor[0];
		rgbaAmb[1]=_additionalColor[1];
		rgbaAmb[2]=_additionalColor[2];
		if (_objectType&sim_particle_emissioncolor)
			glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
		else
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
	}
	ogl::displaySphere(_size*0.5f,10,5,true);
	glPopMatrix();
}

void CDynamicsContainer::setDynamicEngineType(int t)
{
	_dynamicEngineToUse=t;
	App::ct->objCont->setLightDialogRefreshFlag(); // will trigger a refresh
}

int CDynamicsContainer::getDynamicEngineType()
{
	return(_dynamicEngineToUse);
}

void CDynamicsContainer::setDisplayContactPoints(bool d)
{
	_displayContactPoints=d;
}

bool CDynamicsContainer::getDisplayContactPoints()
{
	return(_displayContactPoints);
}


void CDynamicsContainer::setUseDynamicDefaultCalculationParameters(int def)
{
	def=tt::getLimitedInt(0,4,def);
	_dynamicDefaultTypeCalculationParameters=def;
}

int CDynamicsContainer::getUseDynamicDefaultCalculationParameters()
{
	return(_dynamicDefaultTypeCalculationParameters);
}

void CDynamicsContainer::setBulletDynamicFullInternalScaling(bool b,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_dynamicBULLETFullInternalScaling=b;
}

bool CDynamicsContainer::getBulletDynamicFullInternalScaling()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicBULLETFullInternalScaling);
	return(DYNAMIC_BULLET_DEFAULT_FULL_INTERNAL_SCALING[_dynamicDefaultTypeCalculationParameters]);
}

void CDynamicsContainer::setODEDynamicFullInternalScaling(bool b,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_dynamicODEFullInternalScaling=b;
}

bool CDynamicsContainer::getODEDynamicFullInternalScaling()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicODEFullInternalScaling);
	return(DYNAMIC_ODE_DEFAULT_FULL_INTERNAL_SCALING[_dynamicDefaultTypeCalculationParameters]);
}

void CDynamicsContainer::setDynamicInternalScalingFactor(float f,bool direct)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||direct)
		_dynamicBULLETInternalScalingFactor=tt::getLimitedFloat(0.0001f,10000.0f,f);
}

void CDynamicsContainer::setODEDynamicInternalScalingFactor(float f,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_dynamicODEInternalScalingFactor=tt::getLimitedFloat(0.0001f,10000.0f,f);
}

void CDynamicsContainer::setBulletDynamicCollisionMarginFactor(float f,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_dynamicBULLETCollisionMarginFactor=tt::getLimitedFloat(0.001f,100.0f,f);
}

float CDynamicsContainer::getBulletDynamicCollisionMarginFactor()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicBULLETCollisionMarginFactor);
	return(DYNAMIC_BULLET_DEFAULT_COLLISION_MARGIN_FACTOR[_dynamicDefaultTypeCalculationParameters]);
}

void CDynamicsContainer::getVortexFloatParams(std::vector<float>& p)
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		p.assign(_vortexFloatParams.begin(),_vortexFloatParams.end());
	else
		getVortexDefaultFloatParams(p,_dynamicDefaultTypeCalculationParameters);
}

void CDynamicsContainer::getVortexDefaultFloatParams(std::vector<float>& p,int defType)
{
	p.clear();
	p.push_back(DYNAMIC_VORTEX_DEFAULT_STEP_SIZE[defType]); // step size
	p.push_back(DYNAMIC_VORTEX_DEFAULT_INTERNAL_SCALING_FACTOR[defType]); // internal scaling factor
	p.push_back(DYNAMIC_VORTEX_DEFAULT_CONTACT_TOLERANCE[defType]); // contact tolerance
	p.push_back(DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_LIN_COMPLIANCE[defType]); // constraint linear compliance
	p.push_back(DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_LIN_DAMPING[defType]); // constraint linear damping
	p.push_back(DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_LIN_KIN_LOSS[defType]); // constraint linear kinetic loss
	p.push_back(DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_ANG_COMPLIANCE[defType]); // constraint angular compliance
	p.push_back(DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_ANG_DAMPING[defType]); // constraint angular damping
	p.push_back(DYNAMIC_VORTEX_DEFAULT_CONSTRAINT_ANG_KIN_LOSS[defType]); // constraint angular kinetic loss
	p.push_back(0.01f); // RESERVED. used to be auto angular damping tension ratio, not used anymore
	// VORTEX_FLOAT_PARAM_CNT_2=10
}

void CDynamicsContainer::setVortexFloatParams(const std::vector<float>& p,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_vortexFloatParams.assign(p.begin(),p.end());
	_vortexFloatParams[0]=tt::getLimitedFloat(0.0001f,1.0f,_vortexFloatParams[0]); // step size
	_vortexFloatParams[1]=tt::getLimitedFloat(0.0001f,10000.0f,_vortexFloatParams[1]); // internal scaling factor
	_vortexFloatParams[2]=tt::getLimitedFloat(0.0f,10.0f,_vortexFloatParams[2]); // contact tolerance
	// _vortexFloatParams[9] is RESERVED (used to be the auto angular damping tension ratio)
}

void CDynamicsContainer::getVortexIntParams(std::vector<int>& p)
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		p.assign(_vortexIntParams.begin(),_vortexIntParams.end());
	else
		getVortexDefaultIntParams(p,_dynamicDefaultTypeCalculationParameters);
}

void CDynamicsContainer::getVortexDefaultIntParams(std::vector<int>& p,int defType)
{
	p.clear();
	int v=0;
	if (DYNAMIC_VORTEX_DEFAULT_AUTO_SLEEP[defType])
		v|=1;
	if (DYNAMIC_VORTEX_DEFAULT_MULTITHREADING[defType])
		v|=2;
	v|=4; // always on by default (full internal scaling)
	// bit4 (8) is RESERVED!! (was auto-angular damping)
	p.push_back(v); // bit-coded: 0set: autoSleep, 1set: dynamics multithreaded, 2set: full internal scaling, 3set: RESERVED
	// VORTEX_INT_PARAM_CNT_2=1
}

void CDynamicsContainer::setVortexIntParams(const std::vector<int>& p,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_vortexIntParams.assign(p.begin(),p.end());
}


void CDynamicsContainer::setDynamicConstraintSolvingIterations(int iterations,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_dynamicBULLETConstraintSolvingIterations=tt::getLimitedInt(1,10000,iterations);
}

void CDynamicsContainer::setODEDynamicConstraintSolvingIterations(int iterations,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_dynamicODEConstraintSolvingIterations=tt::getLimitedInt(1,10000,iterations);
}

int CDynamicsContainer::getDynamicConstraintSolvingIterations()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicBULLETConstraintSolvingIterations);
	return(DYNAMIC_BULLET_DEFAULT_CONSTRAINT_SOLVING_ITERATIONS[_dynamicDefaultTypeCalculationParameters]);
}

int CDynamicsContainer::getODEDynamicConstraintSolvingIterations()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicODEConstraintSolvingIterations);
	return(DYNAMIC_ODE_DEFAULT_CONSTRAINT_SOLVING_ITERATIONS[_dynamicDefaultTypeCalculationParameters]);
}

float CDynamicsContainer::getDynamicInternalScalingFactor()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicBULLETInternalScalingFactor);
	return(DYNAMIC_BULLET_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters]);
}

float CDynamicsContainer::getODEDynamicInternalScalingFactor()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicODEInternalScalingFactor);
	return(DYNAMIC_ODE_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters]);
}

void CDynamicsContainer::setODEDynamicGlobalCFM(float p,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_dynamicODEGlobalCFM=tt::getLimitedFloat(0.0f,1.0f,p);
}

float CDynamicsContainer::getODEDynamicGlobalCFM()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicODEGlobalCFM);
	return(DYNAMIC_ODE_GLOBAL_CFM[_dynamicDefaultTypeCalculationParameters]);
}

void CDynamicsContainer::setODEDynamicGlobalERP(float p,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_dynamicODEGlobalERP=tt::getLimitedFloat(0.0f,1.0f,p);
}

float CDynamicsContainer::getODEDynamicGlobalERP()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicODEGlobalERP);
	return(DYNAMIC_ODE_GLOBAL_ERP[_dynamicDefaultTypeCalculationParameters]);
}


float CDynamicsContainer::getPositionScalingFactorDyn()
{
	if (_dynamicEngineToUse==sim_physics_bullet)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			return(_dynamicBULLETInternalScalingFactor);
		return(DYNAMIC_BULLET_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters]);
	}
	if (_dynamicEngineToUse==sim_physics_ode)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			return(_dynamicODEInternalScalingFactor);
		return(DYNAMIC_ODE_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters]);
	}
	if (_dynamicEngineToUse==sim_physics_vortex)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			return(_vortexFloatParams[1]);
		return(DYNAMIC_VORTEX_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters]);
	}
	return(1.0f);
}

float CDynamicsContainer::getGravityScalingFactorDyn()
{
	if (_dynamicEngineToUse==sim_physics_bullet)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			return(_dynamicBULLETInternalScalingFactor);
		return(DYNAMIC_BULLET_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters]);
	}
	if (_dynamicEngineToUse==sim_physics_ode)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			return(_dynamicODEInternalScalingFactor);
		return(DYNAMIC_ODE_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters]);
	}
	if (_dynamicEngineToUse==sim_physics_vortex)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			return(_vortexFloatParams[1]);
		return(DYNAMIC_VORTEX_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters]);
	}
	return(1.0f);
}

float CDynamicsContainer::getLinearVelocityScalingFactorDyn()
{
	if (_dynamicEngineToUse==sim_physics_bullet)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			return(_dynamicBULLETInternalScalingFactor);
		return(DYNAMIC_BULLET_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters]);
	}
	if (_dynamicEngineToUse==sim_physics_ode)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			return(_dynamicODEInternalScalingFactor);
		return(DYNAMIC_ODE_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters]);
	}
	if (_dynamicEngineToUse==sim_physics_vortex)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			return(_vortexFloatParams[1]);
		return(DYNAMIC_VORTEX_DEFAULT_INTERNAL_SCALING_FACTOR[_dynamicDefaultTypeCalculationParameters]);
	}
	return(1.0f);
}

float CDynamicsContainer::getMassScalingFactorDyn()
{
	bool full=false;
	if (_dynamicEngineToUse==sim_physics_bullet)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			full=_dynamicBULLETFullInternalScaling;
		else
			full=DYNAMIC_BULLET_DEFAULT_FULL_INTERNAL_SCALING[_dynamicDefaultTypeCalculationParameters];
	}
	if (_dynamicEngineToUse==sim_physics_ode)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			full=_dynamicODEFullInternalScaling;
		else
			full=DYNAMIC_ODE_DEFAULT_FULL_INTERNAL_SCALING[_dynamicDefaultTypeCalculationParameters];
	}
	if (_dynamicEngineToUse==sim_physics_vortex)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			full=true;
		else
			full=(_vortexIntParams[0]&4)!=0;
	}
	if (full)
	{
		float f=getPositionScalingFactorDyn();
		return(f*f*f);
	}
	return(1.0f);
}

float CDynamicsContainer::getMasslessInertiaScalingFactorDyn()
{
	float f=getPositionScalingFactorDyn();
	return(f*f);
}

float CDynamicsContainer::getForceScalingFactorDyn()
{
	bool full=false;
	if (_dynamicEngineToUse==sim_physics_bullet)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			full=_dynamicBULLETFullInternalScaling;
		else
			full=DYNAMIC_BULLET_DEFAULT_FULL_INTERNAL_SCALING[_dynamicDefaultTypeCalculationParameters];
	}
	if (_dynamicEngineToUse==sim_physics_ode)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			full=_dynamicODEFullInternalScaling;
		else
			full=DYNAMIC_ODE_DEFAULT_FULL_INTERNAL_SCALING[_dynamicDefaultTypeCalculationParameters];
	}
	if (_dynamicEngineToUse==sim_physics_vortex)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			full=true;
		else
			full=(_vortexIntParams[0]&4)!=0;
	}

	float f=getPositionScalingFactorDyn();
	if (full)
		return(f*f*f*f);
	return(f);
}

float CDynamicsContainer::getTorqueScalingFactorDyn()
{
	bool full=false;
	if (_dynamicEngineToUse==sim_physics_bullet)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			full=_dynamicBULLETFullInternalScaling;
		else
			full=DYNAMIC_BULLET_DEFAULT_FULL_INTERNAL_SCALING[_dynamicDefaultTypeCalculationParameters];
	}
	if (_dynamicEngineToUse==sim_physics_ode)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			full=_dynamicODEFullInternalScaling;
		else
			full=DYNAMIC_ODE_DEFAULT_FULL_INTERNAL_SCALING[_dynamicDefaultTypeCalculationParameters];
	}
	if (_dynamicEngineToUse==sim_physics_vortex)
	{
		if (_dynamicDefaultTypeCalculationParameters==4)
			full=true;
		else
			full=(_vortexIntParams[0]&4)!=0;
	}

	float f=getPositionScalingFactorDyn();
	if (full)
		return(f*f*f*f*f);
	return(f*f);
}


void CDynamicsContainer::setDynamicsStepSize(float f,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_dynamicsBULLETStepSize=tt::getLimitedFloat(0.0001f,1.0f,f);
}

void CDynamicsContainer::setODEDynamicsStepSize(float f,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_dynamicsODEStepSize=tt::getLimitedFloat(0.0001f,1.0f,f);
}

float CDynamicsContainer::getDynamicsStepSize()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicsBULLETStepSize);
	return(DYNAMIC_BULLET_DEFAULT_STEP_SIZE[_dynamicDefaultTypeCalculationParameters]);
}

float CDynamicsContainer::getODEDynamicsStepSize()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicsODEStepSize);
	return(DYNAMIC_ODE_DEFAULT_STEP_SIZE[_dynamicDefaultTypeCalculationParameters]);
}

void CDynamicsContainer::setODEUseQuickStep(bool u,bool setDirect)
{
	if ((_dynamicDefaultTypeCalculationParameters==4)||setDirect)
		_dynamicODEUseQuickStep=u;
}

bool CDynamicsContainer::getODEUseQuickStep()
{
	if (_dynamicDefaultTypeCalculationParameters==4)
		return(_dynamicODEUseQuickStep);
	return(DYNAMIC_ODE_USE_QUICKSTEP[_dynamicDefaultTypeCalculationParameters]);
}

void CDynamicsContainer::setDynamicsEnabled(bool e)
{
	_dynamicsEnabled=e;
	if (!e)
		App::ct->dynamicsContainer->removeWorld();
	else
	{
		if (App::ct->simulation->isSimulationRunning())
			App::ct->dynamicsContainer->addWorldIfNotThere();
	}

}

bool CDynamicsContainer::getDynamicsEnabled()
{
	return(_dynamicsEnabled);
}

void CDynamicsContainer::setGravity(const C3Vector& gr)
{
	_gravity=gr;
	_gravity(0)=tt::getLimitedFloat(-1000.0f,+1000.0f,_gravity(0));
	_gravity(1)=tt::getLimitedFloat(-1000.0f,+1000.0f,_gravity(1));
	_gravity(2)=tt::getLimitedFloat(-1000.0f,+1000.0f,_gravity(2));
}

C3Vector CDynamicsContainer::getGravity()
{
	return(_gravity);
}

void CDynamicsContainer::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("En2"); // En2 since we have the Vortex option
		ar << _dynamicEngineToUse << _gravity(0) << _gravity(1) << _gravity(2) << _dynamicDefaultTypeCalculationParameters;
		ar.flush();

		ar.storeDataName("Bul");
		ar << _dynamicsBULLETStepSize << _dynamicBULLETInternalScalingFactor << _dynamicBULLETConstraintSolvingIterations << _dynamicBULLETCollisionMarginFactor;
		ar.flush();

		ar.storeDataName("Ode");
		ar << _dynamicsODEStepSize << _dynamicODEInternalScalingFactor << _dynamicODEConstraintSolvingIterations << _dynamicODEGlobalCFM << _dynamicODEGlobalERP;
		ar.flush();

		ar.storeDataName("Vo5"); // vortex params:
		ar << int(_vortexFloatParams.size()) << int(_vortexIntParams.size());
		for (int i=0;i<int(_vortexFloatParams.size());i++)
			ar << _vortexFloatParams[i];
		for (int i=0;i<int(_vortexIntParams.size());i++)
			ar << _vortexIntParams[i];
		ar.flush();

		ar.storeDataName("Var");
		BYTE dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,_dynamicsEnabled);
		SIM_SET_CLEAR_BIT(dummy,1,_displayContactPoints);
		SIM_SET_CLEAR_BIT(dummy,2,_dynamicBULLETFullInternalScaling);
		SIM_SET_CLEAR_BIT(dummy,3,_dynamicODEFullInternalScaling);
		SIM_SET_CLEAR_BIT(dummy,4,_dynamicODEUseQuickStep);
		// reserved SIM_SET_CLEAR_BIT(dummy,5,_dynamicVORTEXFullInternalScaling);
		ar << dummy;
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

				if (theName.compare("Eng")==0)
				{ // keep for backward compatibility (23/09/2013)
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynamicEngineToUse >> _gravity(0) >> _gravity(1) >> _gravity(2) >>_dynamicDefaultTypeCalculationParameters;
				}

				if (theName.compare("En2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynamicEngineToUse >> _gravity(0) >> _gravity(1) >> _gravity(2) >>_dynamicDefaultTypeCalculationParameters;
				}

				if (theName.compare("Bul")==0)
				{ 
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynamicsBULLETStepSize >> _dynamicBULLETInternalScalingFactor >> _dynamicBULLETConstraintSolvingIterations >> _dynamicBULLETCollisionMarginFactor;
				}

				if (theName.compare("Ode")==0)
				{ 
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynamicsODEStepSize >> _dynamicODEInternalScalingFactor >> _dynamicODEConstraintSolvingIterations >> _dynamicODEGlobalCFM >> _dynamicODEGlobalERP;
				}
				if (theName.compare("Vo5")==0)
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
				if (theName.compare("Var")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					
					_dynamicsEnabled=SIM_IS_BIT_SET(dummy,0);
					_displayContactPoints=SIM_IS_BIT_SET(dummy,1);
					_dynamicBULLETFullInternalScaling=SIM_IS_BIT_SET(dummy,2);
					_dynamicODEFullInternalScaling=SIM_IS_BIT_SET(dummy,3);
					_dynamicODEUseQuickStep=SIM_IS_BIT_SET(dummy,4);
					// reserved _dynamicVORTEXFullInternalScaling=SIM_IS_BIT_SET(dummy,5);
				}

				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
