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
#include "OGL.h"
#include "Simulation.h"
#include "CollisionRoutine.h"
#include "DistanceRoutine.h"
#include "proxSensorRoutine.h"
#include "IloIlo.h"
#include "Tt.h"
#include "FileMenu.h"
#include "persistentDataContainer.h"
#include "GraphingRoutines.h"
#include "ObjectEdition.h"
#include "ThreadPool.h"
#include "DxfFile.h"
#include "ObjFile.h"
#include "StlFile.h"
#include "AddMenu.h"
#include "App.h"
#include "pluginContainer.h"
#include "geometric.h"
#include "VDateTime.h"
#include "ttUtil.h"
#include "VVarious.h"
#include "sensingThreadPool.h"
#include "pluginContainer.h"
#include "collDistInterface.h"
#include "dynInterface.h"
#include "pathPlanningInterface.h"

#include "qdlgprimitives.h"
#include <QSplashScreen>
#include <QBitmap>
#include "imgLoader.h"


VMutex _lockForExtLockList;
std::vector<CSimAndUiThreadSync*> _extLockList;

bool isSimulatorInitialized(const char* functionName)
{
	if (!App::isSimulatorRunning())
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_SIMULATOR_NOT_INITIALIZED);
		return(false);
	}
	return(true);
}

bool ifEditModeActiveGenerateErrorAndReturnTrue(const char* functionName)
{
	if (App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERRROR_EDIT_MODE_ACTIVE);
		return(true);
	}
	return(false);
}

bool canBoolIntOrFloatParameterBeSetOrGet(const char* functionName,int when)
{ // 'when' coding: bit0: sim not launched(1), bit1: sim launched(2), bit2: mainWindow not present(4),
	//bit3: mainWIndow present(8),bit4: sim not running(16), bit5: sim running(32)
	int st=0;
	if (App::isSimulatorRunning())
	{
		st|=2;
		if (App::mainWindow==NULL)
			st|=4;
		else
			st|=8;
		if (App::ct->simulation->isSimulationStopped())
			st|=16;
		else
			st|=32;
	}
	else
		st|=1+4+16;
	if (((st&3)&when)==0)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_CANNOT_SET_GET_PARAM_LAUNCH);
		return(false);
	}
	if (((st&12)&when)==0)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_CANNOT_SET_GET_PARAM_WINDOW);
		return(false);
	}
	if (((st&48)&when)==0)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_CANNOT_SET_GET_PARAM_SIM);
		return(false);
	}
	return(true);
}

bool doesObjectExist(const char* functionName,int identifier)
{
	C3DObject* it=App::ct->objCont->getObject(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_INEXISTANT);
		return(false);
	}
	else
		return(true);
}

bool doesEntityExist(const char* functionName,int identifier)
{
	if (identifier>=SIM_IDSTART_COLLECTION)
	{
		if (App::ct->collections->getGroup(identifier)==NULL)
		{
			CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_ENTITY_INEXISTANT);
			return(false);
		}
		return(true);
	}
	else
	{
		if (App::ct->objCont->getObject(identifier)==NULL)
		{
			CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_ENTITY_INEXISTANT);
			return(false);
		}
		return(true);
	}
}

bool doesCollectionExist(const char* functionName,int identifier)
{
	if (App::ct->collections->getGroup(identifier)==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_COLLECTION_INEXISTANT);
		return(false);
	}
	else
		return(true);
}

bool doesCollisionObjectExist(const char* functionName,int identifier)
{
	if (App::ct->collisions->getObject(identifier)==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_COLLISION_INEXISTANT);
		return(false);
	}
	else
		return(true);
}

bool doesDistanceObjectExist(const char* functionName,int identifier)
{
	if (App::ct->distances->getObject(identifier)==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_DISTANCE_INEXISTANT);
		return(false);
	}
	else
		return(true);
}

bool isJoint(const char* functionName,int identifier)
{
	C3DObject* it=App::ct->objCont->getObject(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_INEXISTANT);
		return(false);
	}
	else if (it->getObjectType()!=sim_object_joint_type)
	{ 
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_NOT_JOINT);
		return(false);
	}
	return(true);
}
bool isShape(const char* functionName,int identifier)
{
	C3DObject* it=App::ct->objCont->getObject(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_INEXISTANT);
		return(false);
	}
	else if (it->getObjectType()!=sim_object_shape_type)
	{ 
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_NOT_SHAPE);
		return(false);
	}
	return(true);
}
bool isSensor(const char* functionName,int identifier)
{
	C3DObject* it=App::ct->objCont->getObject(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_INEXISTANT);
		return(false);
	}
	else if (it->getObjectType()!=sim_object_proximitysensor_type)
	{ 
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_NOT_PROX_SENSOR);
		return(false);
	}
	return(true);
}
bool isMill(const char* functionName,int identifier)
{
	C3DObject* it=App::ct->objCont->getObject(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_INEXISTANT);
		return(false);
	}
	else if (it->getObjectType()!=sim_object_mill_type)
	{ 
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_NOT_MILL);
		return(false);
	}
	return(true);
}
bool isForceSensor(const char* functionName,int identifier)
{
	C3DObject* it=App::ct->objCont->getObject(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_INEXISTANT);
		return(false);
	}
	else if (it->getObjectType()!=sim_object_forcesensor_type)
	{ 
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_NOT_FORCE_SENSOR);
		return(false);
	}
	return(true);
}
bool isVisionSensor(const char* functionName,int identifier)
{
	C3DObject* it=App::ct->objCont->getObject(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_INEXISTANT);
		return(false);
	}
	else if (it->getObjectType()!=sim_object_visionsensor_type)
	{ 
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_NOT_VISION_SENSOR);
		return(false);
	}
	return(true);
}
bool isGraph(const char* functionName,int identifier)
{
	C3DObject* it=App::ct->objCont->getObject(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_INEXISTANT);
		return(false);
	}
	else if (it->getObjectType()!=sim_object_graph_type)
	{ 
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_NOT_GRAPH);
		return(false);
	}
	return(true);
}
bool isPath(const char* functionName,int identifier)
{
	C3DObject* it=App::ct->objCont->getObject(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_INEXISTANT);
		return(false);
	}
	else if (it->getObjectType()!=sim_object_path_type)
	{ 
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_NOT_PATH);
		return(false);
	}
	return(true);
}
bool isLight(const char* functionName,int identifier)
{
	C3DObject* it=App::ct->objCont->getObject(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_INEXISTANT);
		return(false);
	}
	else if (it->getObjectType()!=sim_object_light_type)
	{ 
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_NOT_LIGHT);
		return(false);
	}
	return(true);
}
bool isDummy(const char* functionName,int identifier)
{
	C3DObject* it=App::ct->objCont->getObject(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_INEXISTANT);
		return(false);
	}
	else if (it->getObjectType()!=sim_object_dummy_type)
	{ 
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_OBJECT_NOT_DUMMY);
		return(false);
	}
	return(true);
}
bool doesUIExist(const char* functionName,int elementHandle)
{
	CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_UI_INEXISTANT);
		return(false);
	}
	return(true);
}

bool doesUIButtonExist(const char* functionName,int elementHandle,int buttonHandle)
{
	CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_UI_INEXISTANT);
		return(false);
	}
	else if (it->getButtonWithID(buttonHandle)==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_UI_BUTTON_INEXISTANT);
		return(false);
	}
	return(true);
}

bool doesIKGroupExist(const char* functionName,int identifier)
{
	CikGroup* it=App::ct->ikGroups->getIkGroup(identifier);
	if (it==NULL)
	{
		CApiErrors::setLastErrorMessage(functionName,SIM_ERROR_IK_GROUP_INEXISTANT);
		return(false);
	}
	else
		return(true);
}

char* _extractCustomDataFromBuffer(std::vector<char>& buffer,const char* dataName,int* extractedBufSize)
{
	int off=0;
	while (off+8<int(buffer.size()))
	{
		int sizeIncr=((int*)((&buffer[0])+off))[0];
		int nameLength=((int*)((&buffer[0])+off))[1];
		std::string datName(((&buffer[0])+off)+4+4);
		if (datName.compare(dataName)==0)
		{ // ok, we have the correct data here
			int datLength=sizeIncr-4-4-nameLength;
			char* returnBuffer=new char[datLength];
			extractedBufSize[0]=datLength;
			for (int i=0;i<datLength;i++)
				returnBuffer[i]=buffer[off+4+4+nameLength+i];
			buffer.erase(buffer.begin()+off,buffer.begin()+off+sizeIncr);
			return(returnBuffer);
		}
		// continue searching
		off+=sizeIncr;
	}
	extractedBufSize[0]=0;
	return(NULL);
}

void _appendCustomDataToBuffer(std::vector<char>& buffer,const char* dataName,const char* data,int dataSize)
{
	int nameLength=strlen(dataName)+1; // with zero char
	int sizeIncr=4+4+nameLength+dataSize;
	buffer.push_back(((char*)&sizeIncr)[0]);
	buffer.push_back(((char*)&sizeIncr)[1]);
	buffer.push_back(((char*)&sizeIncr)[2]);
	buffer.push_back(((char*)&sizeIncr)[3]);
	buffer.push_back(((char*)&nameLength)[0]);
	buffer.push_back(((char*)&nameLength)[1]);
	buffer.push_back(((char*)&nameLength)[2]);
	buffer.push_back(((char*)&nameLength)[3]);
	for (int i=0;i<nameLength;i++)
		buffer.push_back(dataName[i]);
	for (int i=0;i<dataSize;i++)
		buffer.push_back(data[i]);
}



simInt simRunSimulator_internal(const simChar* applicationName,simInt options,simVoid(*initCallBack)(),simVoid(*loopCallBack)(),simVoid(*deinitCallBack)())
{
	C_API_FUNCTION_DEBUG;
	App::operationalUIParts=options;
	App applicationBasicInitialization((App::operationalUIParts&sim_gui_headless)!=0);
	if (!applicationBasicInitialization.wasInitSuccessful())
		return(0);

	if (options&sim_gui_headless)
		options=sim_gui_headless;
	std::string mainDirectory(VVarious::splitPath_path(VVarious::getModulePathAndFile()));
	COglSurface::_hierarchyEnabled=((App::operationalUIParts&sim_gui_hierarchy)!=0);
	COglSurface::_browserEnabled=((App::operationalUIParts&sim_gui_browser)!=0);



	if ((App::operationalUIParts&sim_gui_headless)==0)
		App::showSplashScreen();

	App::setIcon();



	App::setApplicationName(applicationName); // Make sure you mention in the name that it is a customized V-REP version (i.e. something like: "BenderSimulator (based on the V-REP platform)")

	App::createMainContainer();
	CFileMenu::createNewScene(false,false);
	if ((App::operationalUIParts&sim_gui_headless)==0)
		App::createMainWindow();
/*	else
	{
		//QMainWindow* m=new QMainWindow();
		//m->show();
		QGLWidget* q=new QGLWidget();
		q->show();
		q->makeCurrent();
		App::initGl_ifNeeded();
		q->doneCurrent();

	} */

	App::run(initCallBack,loopCallBack,deinitCallBack); // We stay in here until we quit the application!

	App::deleteMainWindow();
	App::deleteMainContainer();


	CThreadPool::cleanUp();
	return(1);
}

simVoid* simGetMainWindow_internal(simInt type)
{ // 0=window handle , otherwise Qt pointer 
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (App::mainWindow!=NULL)
		{
			if (type==0)
			{
#ifdef WIN_VREP
				return((simVoid*)App::mainWindow->winId());
#endif
			}
			if (type==1)
			{
				return(App::mainWindow);
			}
		}
		return(NULL);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simRefreshDialogs_internal(simInt refreshDegree)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (refreshDegree==0)
			App::ct->objCont->setLightDialogRefreshFlag();
		if (refreshDegree==2)
			App::ct->objCont->setFullDialogRefreshFlag();
		App::ct->objCont->dialogRefreshDontPublishFlag=true;
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simGetLastError_internal()
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		std::string lastErr=CApiErrors::getLastErrorMessage();
		if (lastErr==SIM_API_CALL_NO_ERROR)
		{
			return(NULL);
		}
		char* retVal=new char[lastErr.length()+1];
		for (unsigned int i=0;i<lastErr.length();i++)
			retVal[i]=lastErr[i];
		retVal[lastErr.length()]=0;
		CApiErrors::setLastErrorMessage("",SIM_API_CALL_NO_ERROR);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simSetLastError_internal(const simChar* funcName,const simChar* errorMessage)
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CApiErrors::setLastErrorMessage(funcName,errorMessage);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simLockInterface_internal(simBool locked)
{ // DEPRECATED since release 3.1.0
	return(0);
}

simInt simGetObjectHandle_internal(const simChar* objectName)
{
	C_API_FUNCTION_DEBUG;

	std::string objectNameAdjusted=CIloIlo::getCNameSuffixAdjustedName(objectName);
	CIloIlo::enableCNameSuffixAdjustment();
	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getObject(objectNameAdjusted);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_INEXISTANT);
			return(-1);
		}
		int retVal=it->getID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simRemoveObject_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (objectHandle==sim_handle_all)
		{
			App::ct->objCont->removeAllObjects();
			return(1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_INEXISTANT);
			return(-1);
		}
		App::ct->objCont->eraseObject(it);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simChar* simGetObjectName_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(NULL);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		char* retVal=new char[it->getName().length()+1];
		for (unsigned int i=0;i<it->getName().length();i++)
			retVal[i]=it->getName()[i];
		retVal[it->getName().length()]=0;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simGetObjects_internal(simInt index,simInt objectType)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int cnter=0;
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			if (objectType!=sim_handle_all)
			{
				if (it->getObjectType()!=objectType)
					it=NULL;
			}
			if (it!=NULL)
			{
				if (cnter==index)
				{
					int retVal=it->getID();
					return(retVal);
				}
				cnter++;
			}
		}
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetObjectName_internal(simInt objectHandle,const simChar* objectName)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		std::string originalText(objectName);
		if (originalText.length()>127)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_ILLEGAL_OBJECT_NAME);
			return(-1);
		}
		if (originalText.length()<1)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_ILLEGAL_OBJECT_NAME);
			return(-1);
		}
		std::string text(objectName);
		tt::removeIllegalCharacters(text,true);
		if (originalText!=text)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_ILLEGAL_OBJECT_NAME);
			return(-1);
		}
		if (it->getName().compare(text)==0)
		{ // that name was already set
			return(1);
		}
		if (App::ct->objCont->getObject(text)!=NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_ILLEGAL_OBJECT_NAME);
			return(-1);
		}
		if ( (VREP_LOWCASE_STRING_COMPARE("world",text.c_str())==0)||(VREP_LOWCASE_STRING_COMPARE("none",text.c_str())==0) )
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_ILLEGAL_OBJECT_NAME);
			return(-1);
		}

		it->setName(originalText);
		App::ct->objCont->setFullDialogRefreshFlag();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);

}

simInt simGetCollectionHandle_internal(const simChar* collectionName)
{
	C_API_FUNCTION_DEBUG;

	std::string collectionNameAdjusted=CIloIlo::getCNameSuffixAdjustedName(collectionName);
	CIloIlo::enableCNameSuffixAdjustment();
	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CRegGroup* it=App::ct->collections->getGroup(collectionNameAdjusted);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COLLECTION_INEXISTANT);
			return(-1);
		}
		int retVal=it->getGroupID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simRemoveCollection_internal(simInt collectionHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (collectionHandle==sim_handle_all)
		{
			App::ct->objCont->deselectObjects();
			for (int i=0;i<int(App::ct->collections->allGroups.size());i++)
				App::ct->collections->addGroupToSelection(App::ct->collections->allGroups[i]->getGroupID());
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			App::ct->objCont->deselectObjects();
			for (int i=0;i<int(sel.size());i++)
				App::ct->objCont->eraseObject(App::ct->objCont->getObject(sel[i]));
			return(1);
		}
		CRegGroup* it=App::ct->collections->getGroup(collectionHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COLLECTION_INEXISTANT);
			return(-1);
		}
		App::ct->objCont->deselectObjects();
		App::ct->collections->addGroupToSelection(it->getGroupID());
		std::vector<int> sel;
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
			sel.push_back(App::ct->objCont->getSelID(i));
		App::ct->objCont->deselectObjects();
		for (int i=0;i<int(sel.size());i++)
			App::ct->objCont->eraseObject(App::ct->objCont->getObject(sel[i]));
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simChar* simGetCollectionName_internal(simInt collectionHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesCollectionExist(__func__,collectionHandle))
		{
			return(NULL);
		}
		CRegGroup* it=App::ct->collections->getGroup(collectionHandle);
		char* retVal=new char[it->getGroupName().length()+1];
		for (unsigned int i=0;i<it->getGroupName().length();i++)
			retVal[i]=it->getGroupName()[i];
		retVal[it->getGroupName().length()]=0;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simSetCollectionName_internal(simInt collectionHandle,const simChar* collectionName)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!doesCollectionExist(__func__,collectionHandle))
		{
			return(-1);
		}
		CRegGroup* it=App::ct->collections->getGroup(collectionHandle);
		std::string originalText(collectionName);
		if (originalText.length()>127)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_ILLEGAL_COLLECTION_NAME);
			return(-1);
		}
		std::string text(collectionName);
		tt::removeIllegalCharacters(text,true);
		if (originalText!=text)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_ILLEGAL_COLLECTION_NAME);
			return(-1);
		}
		if (it->getGroupName().compare(text)==0)
		{
			return(1);
		}
		if (App::ct->collections->getGroup(text)!=NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_ILLEGAL_COLLECTION_NAME);
			return(-1);
		}
		it->setGroupName(originalText);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);

}

simInt simGetObjectMatrix_internal(simInt objectHandle,simInt relativeToObjectHandle,simFloat* matrix)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		if (relativeToObjectHandle==sim_handle_parent)
		{
			relativeToObjectHandle=-1;
			C3DObject* parent=it->getParent();
			if (parent!=NULL)
				relativeToObjectHandle=parent->getID();
		}
		if (relativeToObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,relativeToObjectHandle))
			{
				return(-1);
			}
		}
		C3DObject* relObj=App::ct->objCont->getObject(relativeToObjectHandle);
		C7Vector tr;
		if (relObj==NULL)
			tr=it->getCumulativeTransformationPart1();
		else
		{
			C7Vector relTr(relObj->getCumulativeTransformation());
			tr=relTr.getInverse()*it->getCumulativeTransformationPart1();
		}
		tr.getMatrix().copyToInterface(matrix);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetObjectMatrix_internal(simInt objectHandle,simInt relativeToObjectHandle,const simFloat* matrix)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		if (relativeToObjectHandle==sim_handle_parent)
		{
			relativeToObjectHandle=-1;
			C3DObject* parent=it->getParent();
			if (parent!=NULL)
				relativeToObjectHandle=parent->getID();
		}
		if (relativeToObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,relativeToObjectHandle))
			{
				return(-1);
			}
		}
		if (it->getObjectType()==sim_object_shape_type)
		{
			CShape* shape=(CShape*)it;
			if (!shape->getShapeIsDynamicallyStatic()) // condition new since 5/5/2013
				shape->setDynamicsFullRefreshFlag(true); // dynamically enabled objects have to be reset first!
		}
		else
			it->setDynamicsFullRefreshFlag(true); // dynamically enabled objects have to be reset first!
		C4X4Matrix m;
		m.copyFromInterface(matrix);
		if (relativeToObjectHandle==-1)
			App::ct->objCont->setAbsoluteConfiguration(it->getID(),m.getTransformation(),false);
		else
		{
			C3DObject* objRel=App::ct->objCont->getObject(relativeToObjectHandle);
			C7Vector relTr(objRel->getCumulativeTransformation());
			App::ct->objCont->setAbsoluteConfiguration(it->getID(),relTr*m.getTransformation(),false);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetObjectPosition_internal(simInt objectHandle,simInt relativeToObjectHandle,simFloat* position)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		if (relativeToObjectHandle==sim_handle_parent)
		{
			relativeToObjectHandle=-1;
			C3DObject* parent=it->getParent();
			if (parent!=NULL)
				relativeToObjectHandle=parent->getID();
		}
		if (relativeToObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,relativeToObjectHandle))
			{
				return(-1);
			}
		}
		C7Vector tr;
		if (relativeToObjectHandle==-1)
			tr=it->getCumulativeTransformationPart1();
		else
		{
			C3DObject* relObj=App::ct->objCont->getObject(relativeToObjectHandle);
			C7Vector relTr(relObj->getCumulativeTransformationPart1());
			tr=relTr.getInverse()*it->getCumulativeTransformationPart1();
		}
		tr.X.copyTo(position);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetObjectPosition_internal(simInt objectHandle,simInt relativeToObjectHandle,const simFloat* position)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		if (relativeToObjectHandle==sim_handle_parent)
		{
			relativeToObjectHandle=-1;
			C3DObject* parent=it->getParent();
			if (parent!=NULL)
				relativeToObjectHandle=parent->getID();
		}
		if (relativeToObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,relativeToObjectHandle))
			{
				return(-1);
			}
		}
		if (it->getObjectType()==sim_object_shape_type)
		{
			CShape* shape=(CShape*)it;
			if (!shape->getShapeIsDynamicallyStatic()) // condition new since 5/5/2013
				shape->setDynamicsFullRefreshFlag(true); // dynamically enabled objects have to be reset first!
		}
		else
			it->setDynamicsFullRefreshFlag(true); // dynamically enabled objects have to be reset first!
		if (relativeToObjectHandle==-1)
			App::ct->objCont->setAbsolutePosition(it->getID(),C3Vector(position));
		else
		{
			C3DObject* relObj=App::ct->objCont->getObject(relativeToObjectHandle);
			C7Vector absTr(it->getCumulativeTransformationPart1());
			C7Vector relTr(relObj->getCumulativeTransformationPart1());
			C7Vector x(relTr.getInverse()*absTr);
			x.X.set(position);
			absTr=relTr*x;
			App::ct->objCont->setAbsolutePosition(it->getID(),absTr.X);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetObjectOrientation_internal(simInt objectHandle,simInt relativeToObjectHandle,simFloat* eulerAngles)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		if (relativeToObjectHandle==sim_handle_parent)
		{
			relativeToObjectHandle=-1;
			C3DObject* parent=it->getParent();
			if (parent!=NULL)
				relativeToObjectHandle=parent->getID();
		}
		if (relativeToObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,relativeToObjectHandle))
			{
				return(-1);
			}
		}
		C7Vector tr;
		if (relativeToObjectHandle==-1)
			tr=it->getCumulativeTransformationPart1();
		else
		{
			C3DObject* relObj=App::ct->objCont->getObject(relativeToObjectHandle);
			C7Vector relTr(relObj->getCumulativeTransformationPart1());
			tr=relTr.getInverse()*it->getCumulativeTransformationPart1();
		}
		C3Vector(tr.Q.getEulerAngles()).copyTo(eulerAngles);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetObjectOrientation_internal(simInt objectHandle,simInt relativeToObjectHandle,const simFloat* eulerAngles)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		if (relativeToObjectHandle==sim_handle_parent)
		{
			relativeToObjectHandle=-1;
			C3DObject* parent=it->getParent();
			if (parent!=NULL)
				relativeToObjectHandle=parent->getID();
		}
		if (relativeToObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,relativeToObjectHandle))
			{
				return(-1);
			}
		}
		if (it->getObjectType()==sim_object_shape_type)
		{
			CShape* shape=(CShape*)it;
			if (!shape->getShapeIsDynamicallyStatic()) // condition new since 5/5/2013
				shape->setDynamicsFullRefreshFlag(true); // dynamically enabled objects have to be reset first!
		}
		else
			it->setDynamicsFullRefreshFlag(true); // dynamically enabled objects have to be reset first!
		if (relativeToObjectHandle==-1)
			App::ct->objCont->setAbsoluteAngles(it->getID(),C3Vector(eulerAngles));
		else
		{
			C3DObject* relObj=App::ct->objCont->getObject(relativeToObjectHandle);
			C7Vector absTr(it->getCumulativeTransformationPart1());
			C7Vector relTr(relObj->getCumulativeTransformationPart1());
			C7Vector x(relTr.getInverse()*absTr);
			x.Q.setEulerAngles(eulerAngles);
			absTr=relTr*x;
			App::ct->objCont->setAbsoluteAngles(it->getID(),absTr.Q.getEulerAngles());
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetJointPosition_internal(simInt objectHandle,simFloat* position)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		if (it->getJointType()==sim_joint_spherical_subtype)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_JOINT_SPHERICAL);
			return(-1);
		}
		position[0]=it->getPosition();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetJointPosition_internal(simInt objectHandle,simFloat position)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		if (it->getJointType()==sim_joint_spherical_subtype)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_JOINT_SPHERICAL);
			return(-1);
		}
		it->setPosition(position);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetJointTargetPosition_internal(simInt objectHandle,simFloat targetPosition)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		if (it->getJointType()==sim_joint_spherical_subtype)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_JOINT_SPHERICAL);
			return(-1);
		}
		if (it->getJointMode()==sim_jointmode_force)
			it->setDynamicMotorPositionControlTargetPosition(targetPosition);
		else
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_JOINT_NOT_IN_FORCE_TORQUE_MODE);
			return(-1);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetJointTargetPosition_internal(simInt objectHandle,simFloat* targetPosition)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		if (it->getJointType()==sim_joint_spherical_subtype)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_JOINT_SPHERICAL);
			return(-1);
		}
		targetPosition[0]=it->getDynamicMotorPositionControlTargetPosition();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetJointForce_internal(simInt objectHandle,simFloat forceOrTorque)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		it->setDynamicMotorMaximumForce(forceOrTorque);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetPathPosition_internal(simInt objectHandle,simFloat* position)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isPath(__func__,objectHandle))
		{
			return(-1);
		}
		CPath* it=App::ct->objCont->getPath(objectHandle);
		position[0]=float(it->pathContainer->getPosition());
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetPathPosition_internal(simInt objectHandle,simFloat position)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isPath(__func__,objectHandle))
		{
			return(-1);
		}
		CPath* it=App::ct->objCont->getPath(objectHandle);
		it->pathContainer->setPosition(position);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetPathLength_internal(simInt objectHandle,simFloat* length)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isPath(__func__,objectHandle))
		{
			return(-1);
		}
		CPath* it=App::ct->objCont->getPath(objectHandle);
		length[0]=it->pathContainer->getBezierVirtualPathLength();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetJointTargetVelocity_internal(simInt objectHandle,simFloat targetVelocity)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		if (it->getJointType()==sim_joint_spherical_subtype)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_JOINT_SPHERICAL);
			return(-1);
		}
		if (it->getJointMode()==sim_jointmode_force)
			it->setDynamicMotorTargetVelocity(targetVelocity);
		else
			it->setTargetVelocity(targetVelocity);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetJointTargetVelocity_internal(simInt objectHandle,simFloat* targetVelocity)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		if (it->getJointType()==sim_joint_spherical_subtype)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_JOINT_SPHERICAL);
			return(-1);
		}
		if (it->getJointMode()==sim_jointmode_force)
			targetVelocity[0]=it->getDynamicMotorTargetVelocity();
		else
			targetVelocity[0]=it->getTargetVelocity();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetPathTargetNominalVelocity_internal(simInt objectHandle,simFloat targetNominalVelocity)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isPath(__func__,objectHandle))
		{
			return(-1);
		}
		CPath* it=App::ct->objCont->getPath(objectHandle);
		it->pathContainer->setTargetNominalVelocity(targetNominalVelocity);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetJointMatrix_internal(simInt objectHandle,simFloat* matrix)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		C7Vector trFull(it->getLocalTransformation());
		C7Vector trPart1(it->getLocalTransformationPart1());
		C7Vector tr(trPart1.getInverse()*trFull);
		tr.getMatrix().copyToInterface(matrix);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetSphericalJointMatrix_internal(simInt objectHandle,const simFloat* matrix)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		if (it->getJointType()!=sim_joint_spherical_subtype)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_JOINT_NOT_SPHERICAL);
			return(-1);
		}
		C4X4Matrix m;
		m.copyFromInterface(matrix);
		it->setSphericalTransformation(C4Vector(m.M.getQuaternion()));
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetJointInterval_internal(simInt objectHandle,simBool* cyclic,simFloat* interval)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		cyclic[0]=0;
		if (it->getPositionIsCyclic())
			cyclic[0]=1;
		interval[0]=it->getPositionIntervalMin();
		interval[1]=it->getPositionIntervalRange();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetJointInterval_internal(simInt objectHandle,simBool cyclic,const simFloat* interval)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		it->setPositionIsCyclic(cyclic!=0);
		it->setPositionIntervalMin(interval[0]);
		it->setPositionIntervalRange(interval[1]);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetObjectParent_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		int retVal=-1;
		if (it->getParent()!=NULL)
			retVal=it->getParent()->getID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetObjectChild_internal(simInt objectHandle,simInt index)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		int retVal=-1;
		if (int(it->childList.size())>index)
			retVal=it->childList[index]->getID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetObjectParent_internal(simInt objectHandle,simInt parentObjectHandle,simBool keepInPlace)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (parentObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,parentObjectHandle))
			{
				return(-1);
			}
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		C3DObject* parentIt=App::ct->objCont->getObject(parentObjectHandle);
		if (keepInPlace)
			App::ct->objCont->makeObjectChildOf(it,parentIt);
		else
			it->setParent(parentIt);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simGetObjectType_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		int retVal=it->getObjectType();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetJointType_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,objectHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(objectHandle);
		int retVal=it->getJointType();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simBuildIdentityMatrix_internal(simFloat* matrix)
{
	C_API_FUNCTION_DEBUG;

	C4X4Matrix m;
	m.setIdentity();
	m.copyToInterface(matrix);
	return(1);
}

simInt simCopyMatrix_internal(const simFloat* matrixIn,simFloat* matrixOut)
{
	C_API_FUNCTION_DEBUG;

	for (int i=0;i<12;i++)
		matrixOut[i]=matrixIn[i];
	return(1);
}

simInt simBuildMatrix_internal(const simFloat* position,const simFloat* eulerAngles,simFloat* matrix)
{
	C_API_FUNCTION_DEBUG;

	C4X4Matrix m;
	m.M.setEulerAngles(C3Vector(eulerAngles));
	m.X.set(position);
	m.copyToInterface(matrix);
	return(1);
}

simInt simBuildMatrixQ_internal(const simFloat* position,const simFloat* quaternion,simFloat* matrix)
{
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w
	C_API_FUNCTION_DEBUG;

	C4X4Matrix m;
	C4Vector q(quaternion[3],quaternion[0],quaternion[1],quaternion[2]);
	m.M=q.getMatrix();
	m.X.set(position);
	m.copyToInterface(matrix);
	return(1);
}

simInt simGetEulerAnglesFromMatrix_internal(const simFloat* matrix,simFloat* eulerAngles)
{
	C_API_FUNCTION_DEBUG;

	C4X4Matrix m;
	m.copyFromInterface(matrix);
	m.M.getEulerAngles().copyTo(eulerAngles);
	return(1);
}

simInt simGetQuaternionFromMatrix_internal(const simFloat* matrix,simFloat* quaternion)
{
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w
	C_API_FUNCTION_DEBUG;

	C4X4Matrix m;
	m.copyFromInterface(matrix);
	C4Vector q(m.M.getQuaternion());
	quaternion[0]=q(1);
	quaternion[1]=q(2);
	quaternion[2]=q(3);
	quaternion[3]=q(0);
	return(1);
}

simInt simInvertMatrix_internal(simFloat* matrix)
{
	C_API_FUNCTION_DEBUG;

	C4X4Matrix m;
	m.copyFromInterface(matrix);
	m.inverse();
	m.copyToInterface(matrix);
	return(1);
}

simInt simMultiplyMatrices_internal(const simFloat* matrixIn1,const simFloat* matrixIn2,simFloat* matrixOut)
{
	C_API_FUNCTION_DEBUG;

	C4X4Matrix mIn1;
	mIn1.copyFromInterface(matrixIn1);
	C4X4Matrix mIn2;
	mIn2.copyFromInterface(matrixIn2);
	(mIn1*mIn2).copyToInterface(matrixOut);
	return(1);
}

simInt simInterpolateMatrices_internal(const simFloat* matrixIn1,const simFloat* matrixIn2,simFloat interpolFactor,simFloat* matrixOut)
{
	C_API_FUNCTION_DEBUG;

	C4X4Matrix mIn1;
	mIn1.copyFromInterface(matrixIn1);
	C4X4Matrix mIn2;
	mIn2.copyFromInterface(matrixIn2);
	C7Vector tr;
	tr.buildInterpolation(mIn1.getTransformation(),mIn2.getTransformation(),interpolFactor);
	(tr.getMatrix()).copyToInterface(matrixOut);
	return(1);
}

simInt simTransformVector_internal(const simFloat* matrix,simFloat* vect)
{
	C_API_FUNCTION_DEBUG;

	C4X4Matrix m;
	m.copyFromInterface(matrix);
	C3Vector v(vect);
	(m*v).copyTo(vect);
	return(1);
}

simInt simReservedCommand_internal(simInt v,simInt w)
{
	C_API_FUNCTION_DEBUG;

	if (v==0)
		VThread::sleep(w);
	if (v==2)
	{
		int retVal=App::ct->getModificationFlags(true);
		return(retVal);
	}

	return(1);
}

simInt simSetBooleanParameter_internal(simInt parameter,simBool boolState)
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (parameter==sim_boolparam_hierarchy_visible)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			if (App::mainWindow!=NULL)
			{
				if (App::mainWindow->dlgCont->isVisible(HIERARCHY_DLG)!=(boolState!=0))
					App::mainWindow->dlgCont->toggle(HIERARCHY_DLG);
			}
			else
				return(-1);
			return(1);
		}
		if (parameter==sim_boolparam_browser_visible)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			if (App::mainWindow!=NULL)
			{
				if (App::mainWindow->dlgCont->isVisible(BROWSER_DLG)!=(boolState!=0))
					App::mainWindow->dlgCont->toggle(BROWSER_DLG);
			}
			else
				return(-1);
			return(1);
		}

		if (parameter==sim_boolparam_shape_textures_are_visible)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			App::ct->environment->setShapeTexturesEnabled(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_display_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			if (App::mainWindow!=NULL)
			{
				App::mainWindow->openglWidget->setOpenGlDisplayEnabled(boolState!=0);
				return(1);
			}
			else
				return(-1);
		}

		if (parameter==sim_boolparam_infotext_visible)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->infoWindowOpenState=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_threaded_rendering_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+32))
			{
				return(-1);
			}
			if (App::ct->simulation->getThreadedRendering())
			{
				if (boolState==0)
					App::ct->simulation->toggleThreadedRendering(true);
			}
			else
			{
				if (boolState!=0)
					App::ct->simulation->toggleThreadedRendering(true);
			}
			return(1);
		}

		if (parameter==sim_boolparam_fullscreen)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+32))
			{
				return(-1);
			}
			if (App::isFullScreen())
			{
				if (boolState==0)
					App::uiThread->setFullscreen(false);
			}
			else
			{
				if (boolState!=0)
					App::uiThread->setFullscreen(true);
			}
			return(1);
		}

		if (parameter==sim_boolparam_statustext_open)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->statusBoxOpenState=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_fog_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			App::ct->environment->fogEnabled=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_scene_and_model_load_messages)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,1+2+4+8+16+32))
			{
				return(-1);
			}
			CIloIlo::outputSceneOrModelLoadMessagesWithApiCall=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_console_visible)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			App::userSettings->alwaysShowConsole=(boolState!=0);
			App::setShowConsole(App::userSettings->alwaysShowConsole);
			return(1);
		}

		if (parameter==sim_boolparam_collision_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->collisionDetectionEnabled=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_distance_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->distanceCalculationEnabled=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_ik_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->ikCalculationEnabled=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_gcs_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->gcsCalculationEnabled=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_dynamics_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->dynamicsContainer->setDynamicsEnabled(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_joint_motion_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->jointMotionHandlingEnabled=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_path_motion_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->pathMotionHandlingEnabled=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_proximity_sensor_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->proximitySensorsEnabled=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_vision_sensor_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->visionSensorsEnabled=(boolState!=0);
			return(1);
		}

		if (parameter==sim_boolparam_mirrors_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->mirrorsDisabled=(boolState==0);
			return(1);
		}

		if (parameter==sim_boolparam_aux_clip_planes_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->clippingPlanesDisabled=(boolState==0);
			return(1);
		}
		if (parameter==sim_boolparam_full_model_copy_from_api)
		{
			CIloIlo::fullModelCopyFromApi=(boolState!=0);
			return(1);
		}
		if (parameter==sim_boolparam_realtime_simulation)
		{
			if (App::ct->simulation->isSimulationStopped())
			{
				App::ct->simulation->setRealTimeSimulation(boolState!=0);
				return(1);
			}
			return(0);
		}
		if (parameter==sim_boolparam_use_glfinish_cmd)
		{
			App::userSettings->useGlFinish=(boolState!=0);
			return(1);
		}
		if (parameter==sim_boolparam_force_show_wireless_emission)
		{
			CIloIlo::wirelessForceShow_emission=(boolState!=0);
			return(1);
		}
		if (parameter==sim_boolparam_force_show_wireless_reception)
		{
			CIloIlo::wirelessForceShow_reception=(boolState!=0);
			return(1);
		}
		if (parameter==sim_boolparam_mill_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->millsEnabled=(boolState!=0);
			return(1);
		}
		if (parameter==sim_boolparam_video_recording_triggered)
		{
			if ((App::mainWindow!=NULL)&&(!App::mainWindow->simulationRecorder->getIsRecording())&&App::ct->simulation->isSimulationStopped() )
			{
				App::mainWindow->simulationRecorder->setRecorderEnabled(boolState!=0);
				App::mainWindow->simulationRecorder->setShowSavedMessage(boolState==0); // avoid displaying a message at simulation end in this case!
				return(1);
			}
			return(-1);
		}
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PARAMETER);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetBooleanParameter_internal(simInt parameter)
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (parameter==sim_boolparam_realtime_simulation)
		{
			int retVal=0;
			if (App::ct->simulation->getRealTimeSimulation())
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_use_glfinish_cmd)
		{
			int retVal=0;
			if (App::userSettings->useGlFinish)
				retVal=1;
			return(retVal);
		}

		if (parameter==sim_boolparam_force_show_wireless_emission)
		{
			int retVal=0;
			if (CIloIlo::wirelessForceShow_emission)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_force_show_wireless_reception)
		{
			int retVal=0;
			if (CIloIlo::wirelessForceShow_reception)
				retVal=1;
			return(retVal);
		}

		if (parameter==sim_boolparam_hierarchy_visible)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if ((App::mainWindow!=NULL)&&App::mainWindow->dlgCont->isVisible(HIERARCHY_DLG))
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_browser_visible)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if ((App::mainWindow!=NULL)&&App::mainWindow->dlgCont->isVisible(BROWSER_DLG))
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_shape_textures_are_visible)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->environment->getShapeTexturesEnabled())
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_threaded_rendering_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->simulation->getThreadedRendering())
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_fullscreen)
		{
			int retVal=0;
			if (App::isFullScreen())
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_headless)
		{
			int retVal=0;
			if (App::mainWindow==NULL)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_display_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))

			{
				return(-1);
			}
			int retVal=0;
			if ((App::mainWindow!=NULL)&&App::mainWindow->openglWidget->getOpenGlDisplayEnabled())
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_infotext_visible)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->mainSettings->infoWindowOpenState)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_statustext_open)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->mainSettings->statusBoxOpenState)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_fog_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->environment->fogEnabled)
				retVal=1;
			return(retVal);
		}

		if (parameter==sim_boolparam_rml2_available)
		{
			int retVal=0;
			if (CPluginContainer::getPluginFromName("ReflexxesTypeII")!=NULL)
				retVal=1;
			return(retVal);
		}

		if (parameter==sim_boolparam_rml4_available)
		{
			int retVal=0;
			if (CPluginContainer::getPluginFromName("ReflexxesTypeIV")!=NULL)
				retVal=1;
			return(retVal);
		}

		if (parameter==sim_boolparam_scene_and_model_load_messages)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,1+2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (CIloIlo::outputSceneOrModelLoadMessagesWithApiCall)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_console_visible)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::userSettings->alwaysShowConsole)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_collision_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->mainSettings->collisionDetectionEnabled)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_distance_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->mainSettings->distanceCalculationEnabled)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_ik_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->mainSettings->ikCalculationEnabled)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_gcs_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->mainSettings->gcsCalculationEnabled)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_dynamics_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->dynamicsContainer->getDynamicsEnabled())
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_joint_motion_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->mainSettings->jointMotionHandlingEnabled)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_path_motion_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->mainSettings->pathMotionHandlingEnabled)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_proximity_sensor_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->mainSettings->proximitySensorsEnabled)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_vision_sensor_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->mainSettings->visionSensorsEnabled)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_mirrors_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=1;
			if (App::ct->mainSettings->mirrorsDisabled)
				retVal=0;
			return(retVal);
		}
		if (parameter==sim_boolparam_aux_clip_planes_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=1;
			if (App::ct->mainSettings->clippingPlanesDisabled)
				retVal=0;
			return(retVal);
		}
		if (parameter==sim_boolparam_full_model_copy_from_api)
		{
			int retVal=0;
			if (CIloIlo::fullModelCopyFromApi)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_mill_handling_enabled)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			int retVal=0;
			if (App::ct->mainSettings->millsEnabled)
				retVal=1;
			return(retVal);
		}
		if (parameter==sim_boolparam_video_recording_triggered)
		{
			int retVal=0;
			if ((App::mainWindow!=NULL)&&App::mainWindow->simulationRecorder->getRecorderEnabled())
				retVal=1;
			return(retVal);
		}

		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PARAMETER);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetArrayParameter_internal(simInt parameter,const simVoid* arrayOfValues)
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (parameter==sim_arrayparam_gravity)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->dynamicsContainer->setGravity(C3Vector((float*)arrayOfValues));
			return(1);
		}

		if (parameter==sim_arrayparam_fog)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->environment->fogStart=((float*)arrayOfValues)[0];
			App::ct->environment->fogEnd=((float*)arrayOfValues)[1];
			App::ct->environment->fogDensity=((float*)arrayOfValues)[2];
			return(1);
		}
		if (parameter==sim_arrayparam_fog_color)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->environment->fogBackgroundColor[0]=((float*)arrayOfValues)[0];
			App::ct->environment->fogBackgroundColor[1]=((float*)arrayOfValues)[1];
			App::ct->environment->fogBackgroundColor[2]=((float*)arrayOfValues)[2];
			return(1);
		}
		if (parameter==sim_arrayparam_background_color1)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->environment->backGroundColorDown[0]=((float*)arrayOfValues)[0];
			App::ct->environment->backGroundColorDown[1]=((float*)arrayOfValues)[1];
			App::ct->environment->backGroundColorDown[2]=((float*)arrayOfValues)[2];
			return(1);
		}
		if (parameter==sim_arrayparam_background_color2)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->environment->backGroundColor[0]=((float*)arrayOfValues)[0];
			App::ct->environment->backGroundColor[1]=((float*)arrayOfValues)[1];
			App::ct->environment->backGroundColor[2]=((float*)arrayOfValues)[2];
			return(1);
		}
		if (parameter==sim_arrayparam_ambient_light)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			App::ct->environment->ambientLightColor[0]=((float*)arrayOfValues)[0];
			App::ct->environment->ambientLightColor[1]=((float*)arrayOfValues)[1];
			App::ct->environment->ambientLightColor[2]=((float*)arrayOfValues)[2];
			return(1);
		}

		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PARAMETER);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetArrayParameter_internal(simInt parameter,simVoid* arrayOfValues)
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (parameter==sim_arrayparam_gravity)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			C3Vector g(App::ct->dynamicsContainer->getGravity());
			g.copyTo((float*)arrayOfValues);
			return(1);
		}
		if (parameter==sim_arrayparam_fog)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			((float*)arrayOfValues)[0]=App::ct->environment->fogStart;
			((float*)arrayOfValues)[1]=App::ct->environment->fogEnd;
			((float*)arrayOfValues)[2]=App::ct->environment->fogDensity;
			return(1);
		}
		if (parameter==sim_arrayparam_fog_color)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			((float*)arrayOfValues)[0]=App::ct->environment->fogBackgroundColor[0];
			((float*)arrayOfValues)[1]=App::ct->environment->fogBackgroundColor[1];
			((float*)arrayOfValues)[2]=App::ct->environment->fogBackgroundColor[2];
			return(1);
		}
		if (parameter==sim_arrayparam_background_color1)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			((float*)arrayOfValues)[0]=App::ct->environment->backGroundColorDown[0];
			((float*)arrayOfValues)[1]=App::ct->environment->backGroundColorDown[1];
			((float*)arrayOfValues)[2]=App::ct->environment->backGroundColorDown[2];
			return(1);
		}
		if (parameter==sim_arrayparam_background_color2)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			((float*)arrayOfValues)[0]=App::ct->environment->backGroundColor[0];
			((float*)arrayOfValues)[1]=App::ct->environment->backGroundColor[1];
			((float*)arrayOfValues)[2]=App::ct->environment->backGroundColor[2];
			return(1);
		}
		if (parameter==sim_arrayparam_ambient_light)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			((float*)arrayOfValues)[0]=App::ct->environment->ambientLightColor[0];
			((float*)arrayOfValues)[1]=App::ct->environment->ambientLightColor[1];
			((float*)arrayOfValues)[2]=App::ct->environment->ambientLightColor[2];
			return(1);
		}

		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PARAMETER);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}



simInt simSetIntegerParameter_internal(simInt parameter,simInt intState)
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (parameter==sim_intparam_error_report_mode)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			CApiErrors::setErrorReportingMode(intState);
			return(1);
		}
		if (parameter==sim_intparam_server_port_next)
		{
			CIloIlo::nextfreeServerPortToUse=intState;
			return(1);
		}
		if (parameter==sim_intparam_current_page)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			if (App::ct->oglSurface!=NULL)
			{
				App::ct->oglSurface->pageCont->setActivePage(intState);
			}
			return(1);
		}
		if (parameter==sim_intparam_visible_layers)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->activeLayers=WORD(intState);
			return(1);
		}
		if (parameter==sim_intparam_infotext_style)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			App::ct->mainSettings->infoWindowColorStyle=intState;
			return(1);
		}
		if (parameter==sim_intparam_work_thread_count)
		{
			App::userSettings->workThreadCount=intState;
			return(1);
		}
		if (parameter==sim_intparam_work_thread_calc_time_ms)
		{
			App::ct->infoDisplay->setWorkThreadExecutionDuration_inMs(intState);
			return(1);
		}
		if (parameter==sim_intparam_settings)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			App::userSettings->antiAliasing=(intState&1);
			App::userSettings->displayWorldReference=((intState&2)!=0);
			App::userSettings->displayBoundingBoxeWhenObjectSelected=((intState&4)!=0);
			// reserved
			App::userSettings->setUndoRedoEnabled((intState&16)!=0);
			return(1);
		}
		if (parameter==sim_intparam_dynamic_engine)
		{
			if (App::ct->simulation->isSimulationStopped())
			{
				App::ct->dynamicsContainer->setDynamicEngineType(intState);
				return(1);
			}
			return(0);
		}
		if (parameter==sim_intparam_idle_fps)
		{
			App::userSettings->setIdleFps(intState);
			return(1);
		}
		if ( (parameter==sim_intparam_prox_sensor_select_down)||(parameter==sim_intparam_prox_sensor_select_up) )
		{
			if (parameter==sim_intparam_prox_sensor_select_down)
				CIloIlo::proxSensorClickSelectDown=intState;
			else
				CIloIlo::proxSensorClickSelectUp=intState;
			if (intState!=0)
			{
				int displayAttrib=sim_displayattribute_renderpass;
				for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
				{
					int shapeHandle=App::ct->objCont->shapeList[i];
					CShape* shape=App::ct->objCont->getShape(shapeHandle);
					if (shape->getShouldObjectBeDisplayed(displayAttrib))
					{
						if ( (intState==-1)||(shape->getMainPropertyLocal()&intState) )
							shape->geomData->initializeCalculationStructureIfNeeded();
					}
				}
			}
			return(1);
		}
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PARAMETER);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetIntegerParameter_internal(simInt parameter,simInt* intState)
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (parameter==sim_intparam_stop_request_counter)
		{
			intState[0]=App::ct->simulation->getStopRequestCounter();
			return(1);
		}
		if (parameter==sim_intparam_error_report_mode)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+4+8+16+32))
			{
				return(-1);
			}
			intState[0]=CApiErrors::getErrorReportingMode();
			return(1);
		}
		if (parameter==sim_intparam_program_version)
		{
			intState[0]=VREP_PROGRAM_VERSION_NB;
			return(1);
		}
		if (parameter==sim_intparam_program_revision)
		{
			intState[0]=VREP_PROGRAM_REVISION_NB;
			return(1);
		}
		if (parameter==sim_intparam_scene_unique_id)
		{
			intState[0]=App::ct->environment->getSceneUniqueID();
			return(1);
		}
		if (parameter==sim_intparam_platform)
		{
	#ifdef WIN_VREP
			intState[0]=0;
	#endif
	#ifdef MAC_VREP
			intState[0]=1;
	#endif
	#ifdef LIN_VREP
			intState[0]=2;
	#endif
			return(1);
		}
		if (parameter==sim_intparam_event_flags_read)
		{
			intState[0]=App::ct->getModificationFlags(false);
			return(1);
		}
		if (parameter==sim_intparam_event_flags_read_clear)
		{
			intState[0]=App::ct->getModificationFlags(true);
			return(1);
		}

		if (parameter==sim_intparam_qt_version)
		{
			intState[0]=0;
			intState[0]=(QT_VERSION>>16)*10000+((QT_VERSION>>8)&255)*100+(QT_VERSION&255)*1;
			return(1);
		}
		if (parameter==sim_intparam_compilation_version)
		{
			intState[0]=6;
			return(1);
		}
		if (parameter==sim_intparam_instance_count)
		{
			intState[0]=1;
			return(1);
		}
		if (parameter==sim_intparam_custom_cmd_start_id)
		{
			intState[0]=OUTSIDE_CUSTOMIZED_COMMANDS_START_CMD;
			return(1);
		}
		if (parameter==sim_intparam_current_page)
		{
			if (App::ct->oglSurface==NULL)
			{
				return(-1);
			}
			intState[0]=App::ct->oglSurface->pageCont->getActivePageIndex();
			return(1);
		}
		if (parameter==sim_intparam_visible_layers)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			intState[0]=int(App::ct->mainSettings->activeLayers);
			return(1);
		}
		if (parameter==sim_intparam_infotext_style)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			intState[0]=int(App::ct->mainSettings->infoWindowColorStyle);
			return(1);
		}
		if (parameter==sim_intparam_edit_mode_type)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			int editMode=App::ct->objCont->getEditModeType();
			if (editMode==NO_EDIT_MODE)
				intState[0]=0;
			if (editMode==TRIANGLE_EDIT_MODE)
				intState[0]=1;
			if (editMode==VERTEX_EDIT_MODE)
				intState[0]=2;
			if (editMode==EDGE_EDIT_MODE)
				intState[0]=3;
			if (editMode==PATH_EDIT_MODE)
				intState[0]=4;
			if (editMode==BUTTON_EDIT_MODE)
				intState[0]=5;
			return(1);
		}
		if (parameter==sim_intparam_work_thread_count)
		{
			intState[0]=App::userSettings->workThreadCount;
			return(1);
		}
		if (parameter==sim_intparam_work_thread_calc_time_ms)
		{
			intState[0]=App::ct->infoDisplay->getWorkThreadExecutionDuration_inMs();
			return(1);
		}
		if (parameter==sim_intparam_core_count)
		{
			intState[0]=App::getCoreCount();
			return(1);
		}
		if (parameter==sim_intparam_idle_fps)
		{
			intState[0]=App::userSettings->getIdleFps();
			return(1);
		}
		if (parameter==sim_intparam_prox_sensor_select_down)
		{
			intState[0]=CIloIlo::proxSensorClickSelectDown;
			return(1);
		}
		if (parameter==sim_intparam_prox_sensor_select_up)
		{
			intState[0]=CIloIlo::proxSensorClickSelectUp;
			return(1);
		}
		if (parameter==sim_intparam_mouse_buttons)
		{
			if (App::mainWindow!=NULL)
			{
				intState[0]=App::mainWindow->getMouseButtonState();
				return(1);
			}
			else
				return(-1);
		}
		if (parameter==sim_intparam_mouse_x)
		{
			if (App::mainWindow!=NULL)
			{
				int p[2];
				App::mainWindow->getOglMousePos(p);
				intState[0]=p[0];
				return(1);
			}
			else
				return(-1);
		}
		if (parameter==sim_intparam_mouse_y)
		{
			if (App::mainWindow!=NULL)
			{
				int p[2];
				App::mainWindow->getOglMousePos(p);
				intState[0]=p[1];
				return(1);
			}
			else
				return(-1);
		}
		if (parameter==sim_intparam_settings)
		{
			if (!canBoolIntOrFloatParameterBeSetOrGet(__func__,2+8+16+32))
			{
				return(-1);
			}
			intState[0]=0;
			if (App::userSettings->antiAliasing)
				intState[0]|=1;
			if (App::userSettings->displayWorldReference)
				intState[0]|=2;
			if (App::userSettings->displayBoundingBoxeWhenObjectSelected)
				intState[0]|=4;
			// reserved 	intState[0]|=8;
			if (App::userSettings->getUndoRedoEnabled())
				intState[0]|=16;
			return(1);
		}
		if (parameter==sim_intparam_flymode_camera_handle)
		{
			if (App::ct->getCurrentInstanceIndex()==CIloIlo::flyModeCameraHandleInstance)
			{
				intState[0]=CIloIlo::flyModeCameraHandle;
				return(1);
			}
			return(-1);
		}
		if (parameter==sim_intparam_dynamic_step_divider)
		{
			intState[0]=CDynInterface::getDynamicStepDivider();
			if (intState[0]>0)
			{
				return(1);
			}
			return(-1);
		}
		if (parameter==sim_intparam_dynamic_engine)
		{
			int a=App::ct->dynamicsContainer->getDynamicEngineType();
			if (a==sim_physics_bullet)
				intState[0]=0;
			if (a==sim_physics_ode)
				intState[0]=1;
			if (a==sim_physics_vortex)
				intState[0]=2;
			return(1);
		}
		if (parameter==sim_intparam_server_port_start)
		{
			intState[0]=App::userSettings->freeServerPortStart;
			return(1);
		}
		if (parameter==sim_intparam_server_port_next)
		{
			intState[0]=CIloIlo::nextfreeServerPortToUse;
			return(1);
		}
		if (parameter==sim_intparam_server_port_range)
		{
			intState[0]=App::userSettings->freeServerPortRange;
			return(1);
		}
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PARAMETER);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetFloatingParameter_internal(simInt parameter,simFloat floatState)
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (parameter==sim_floatparam_simulation_time_step)
		{
			if (App::ct->simulation->isSimulationStopped())
			{
				App::ct->simulation->setSimulationTimeStep_raw(floatState);
				return(1);
			}
			return(0);
		}
		if (parameter==sim_floatparam_stereo_distance)
		{
			if (App::mainWindow!=NULL)
			{
				App::mainWindow->setStereoDistance(floatState);
				return(1);
			}
			return(0);
		}

		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PARAMETER);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetFloatingParameter_internal(simInt parameter,simFloat* floatState)
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (parameter==sim_floatparam_rand)
		{
			floatState[0]=SIM_RAND_FLOAT;
			return(1);
		}
		if (parameter==sim_floatparam_simulation_time_step)
		{
			floatState[0]=App::ct->simulation->getSimulationTimeStep_speedModified();
			return(1);
		}
		if (parameter==sim_floatparam_stereo_distance)
		{
			if (App::mainWindow!=NULL)
			{
				floatState[0]=App::mainWindow->getStereoDistance();
				return(1);
			}
			return(0);
		}
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PARAMETER);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetStringParameter_internal(simInt parameter,const simChar* str)
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (parameter==sim_stringparam_video_filename)
		{
			if (App::mainWindow!=NULL)
			{
				App::mainWindow->simulationRecorder->setFilenameAndPath(str);
				return(1);
			}
			else
			{
				return(0);
			}
		}
		if ((parameter>=sim_stringparam_app_arg1)&&(parameter<=sim_stringparam_app_arg9))
		{
			App::setApplicationArgument(parameter-sim_stringparam_app_arg1,str);
			return(1);
		}
		if (parameter==sim_stringparam_additional_addonscript_firstscene)
		{
			CLuaScriptContainer::setAdditionalAddOnScript(str,true);
			return(1);
		}
		if (parameter==sim_stringparam_additional_addonscript)
		{
			CLuaScriptContainer::setAdditionalAddOnScript(str,false);
			return(1);
		}

		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PARAMETER);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simGetStringParameter_internal(simInt parameter)
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (parameter==sim_stringparam_application_path)
		{
			char* retVal=new char[App::directories->executableDirectory.length()+1];
			for (int i=0;i<int(App::directories->executableDirectory.length());i++)
				retVal[i]=App::directories->executableDirectory[i];
			retVal[App::directories->executableDirectory.length()]=0;
			return(retVal);
		}
		if (parameter==sim_stringparam_scene_path_and_name)
		{
			char* retVal=new char[App::ct->mainSettings->scenePathAndName.length()+1];
			for (int i=0;i<int(App::ct->mainSettings->scenePathAndName.length());i++)
				retVal[i]=App::ct->mainSettings->scenePathAndName[i];
			retVal[App::ct->mainSettings->scenePathAndName.length()]=0;
			return(retVal);
		}
		if (parameter==sim_stringparam_video_filename)
		{
			char* retVal=NULL;
			if (App::mainWindow!=NULL)
			{
				std::string s(App::mainWindow->simulationRecorder->getFilenameAndPath());
				retVal=new char[s.length()+1];
				for (int i=0;i<int(s.length());i++)
					retVal[i]=s[i];
				retVal[s.length()]=0;
			}
			return(retVal);
		}
		if ((parameter>=sim_stringparam_app_arg1)&&(parameter<=sim_stringparam_app_arg9))
		{
			std::string s(App::getApplicationArgument(parameter-sim_stringparam_app_arg1));
			char* retVal=new char[s.length()+1];
			for (int i=0;i<int(s.length());i++)
				retVal[i]=s[i];
			retVal[s.length()]=0;
			return(retVal);
		}
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PARAMETER);
		return(NULL);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simFloat simGetSimulationTime_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1.0f);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		float retVal=App::ct->simulation->getSimulationTime();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1.0f);
}

simInt simGetSimulationState_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=App::ct->simulation->getSimulationState();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simFloat simGetSystemTime_internal()
{
	C_API_FUNCTION_DEBUG;
//	float retVal=float(float(VDateTime::getTimeInMs())/1000.0f);
	float retVal=float(VDateTime::getOSTimeInMs(0))/1000.0f;
	return(retVal);
}

simInt simGetSystemTimeInMilliseconds_internal()
{
	C_API_FUNCTION_DEBUG;
//	int retVal=VDateTime::getTimeInMs();
	int retVal=int(VDateTime::getOSTimeInMs(0));
	return(retVal);
}

simUInt simGetSystemTimeInMs_internal(simInt previousTime)
{
	C_API_FUNCTION_DEBUG;
	unsigned int retVal;
	if (previousTime<-1)
		retVal=VDateTime::getOSTimeInMs(-previousTime-2);
	else
	{
		if (previousTime==-1)
			retVal=(unsigned int)VDateTime::getTimeInMs();
		else
			retVal=(unsigned int)VDateTime::getTimeDiffInMs(previousTime);
	}
	return(retVal);
}

simInt simLoadScene_internal(const simChar* filename)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!App::ct->simulation->isSimulationStopped())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_STOPPED);
			return(-1);
		}
		if (strlen(filename)!=0)
		{
			if (!VFile::doesFileExist(filename))
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_FILE_NOT_FOUND);
				return(-1);
			}
		}

		if (!App::uiThread->loadScene_fromApi(filename,CIloIlo::outputSceneOrModelLoadMessagesWithApiCall,false)) // can trigger a texture delete. That is only ok from the UI thread!
		//if (!CFileMenu::loadScene(filename,CIloIlo::outputSceneOrModelLoadMessagesWithApiCall,false))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCENE_COULD_NOT_BE_READ);
			return(-1);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simCloseScene_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!App::ct->simulation->isSimulationStopped())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_STOPPED);
			return(-1);
		}
		App::uiThread->closeScene_fromApi(false); // can trigger a texture delete. That is only ok from the UI thread!
		// CFileMenu::closeScene(false);
		return(App::ct->getCurrentInstanceIndex());
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simLoadModel_internal(const simChar* filename)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!VFile::doesFileExist(filename))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_FILE_NOT_FOUND);
			return(-1);
		}
		if (!CFileMenu::loadModel(filename,CIloIlo::outputSceneOrModelLoadMessagesWithApiCall,false,true,NULL,true))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MODEL_COULD_NOT_BE_READ);
			return(-1);
		}
		int retVal=App::ct->objCont->getLastSelectionID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simLoadUI_internal(const simChar* filename,int maxCount,int* uiHandles)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!VFile::doesFileExist(filename))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_FILE_NOT_FOUND);
			return(-1);
		}
		std::vector<int> handles;
		bool res=CFileMenu::loadUserInterfaces(filename,false,false,&handles,true);
		if (!res)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_UI_COULD_NOT_BE_READ);
			return(-1);
		}
		int m=SIM_MIN(maxCount,int(handles.size()));
		for (int i=0;i<m;i++)
			uiHandles[i]=handles[i];
		return(handles.size());
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simCreateUI_internal(const simChar* elementName,simInt menuAttributes,const simInt* clientSize,const simInt* cellSize,simInt* buttonHandles)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		int s[2]={clientSize[0],clientSize[1]};
		if (menuAttributes!=0)
			s[1]++;
		int b=0;
		for (int i=0;i<8;i++)
		{
			if (menuAttributes&(1<<i))
				b++;
		}
		if (b>s[0])
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_DIMENSIONS);
			return(-1);
		}
		CButtonBlock* it=new CButtonBlock(s[0],s[1],cellSize[0],cellSize[1],-1);
		it->setBlockName(elementName);
		if ((menuAttributes&sim_ui_menu_systemblock)!=0)
			it->setAttributes(it->getAttributes()|sim_ui_property_systemblock);
		App::ct->buttonBlockContainer->insertBlock(it,false);
		int retVal=it->getBlockID();
		int retHandlesP=0;
		float white[3]={1.0f,1.0f,1.0f};
		if (menuAttributes&sim_ui_menu_title)
		{ // We have a title bar:
			int p[2]={0,0};
			int s2[2]={s[0]-b+1,1};
			float blue[3]={0.36f,0.35f,0.87f};
			buttonHandles[retHandlesP]=simCreateUIButton_internal(retVal,p,s2,sim_buttonproperty_label|sim_buttonproperty_enabled|sim_buttonproperty_verticallycentered);
			simSetUIButtonColor_internal(retVal,buttonHandles[retHandlesP],blue,blue,white);
			retHandlesP++;
		}
		if (menuAttributes&sim_ui_menu_minimize)
		{ // We have a minimize button:
			int p[2]={s[0]-b,0};
			if (menuAttributes&sim_ui_menu_title)
				p[0]++;
			int s2[2]={1,1};
			float blue[3]={0.18f,0.16f,0.84f};
			buttonHandles[retHandlesP]=simCreateUIButton_internal(retVal,p,s2,sim_buttonproperty_button|sim_buttonproperty_enabled|sim_buttonproperty_staydown|sim_buttonproperty_horizontallycentered|sim_buttonproperty_verticallycentered|sim_buttonproperty_isdown|sim_buttonproperty_rollupaction);
			simSetUIButtonColor_internal(retVal,buttonHandles[retHandlesP],blue,blue,white);
			simSetUIButtonLabel_internal(retVal,buttonHandles[retHandlesP],"&&fg999&&Square","&&fg999&&Minimize");
			it->setRollupMin(VPoint(0,0));
			it->setRollupMax(VPoint(s[0],0));
			retHandlesP++;
		}
		if (menuAttributes&sim_ui_menu_close)
		{ // We have a close button:
			int p[2]={s[0]-1,0};
			int s2[2]={1,1};
			float red[3]={0.84f,0.16f,0.17f};
			buttonHandles[retHandlesP]=simCreateUIButton_internal(retVal,p,s2,sim_buttonproperty_button|sim_buttonproperty_enabled|sim_buttonproperty_horizontallycentered|sim_buttonproperty_verticallycentered|sim_buttonproperty_closeaction);
			simSetUIButtonColor_internal(retVal,buttonHandles[retHandlesP],red,red,white);
			simSetUIButtonLabel_internal(retVal,buttonHandles[retHandlesP],"&&fg999&&Check","");
			retHandlesP++;
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simCreateUIButton_internal(simInt elementHandle,const simInt* position,const simInt* size,simInt buttonProperty)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!doesUIExist(__func__,elementHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=new CSoftButton("",position[0],position[1],size[0],size[1]);
		if (!it->insertButton(but))
		{
			delete but;
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_WRONG_POS_SIZE_PARAMS);
			return(-1);
		}
		int retVal=but->buttonID;
		if (simSetUIButtonProperty_internal(elementHandle,retVal,buttonProperty)==-1)
			retVal=-1; // should not happen!
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simGetUIHandle_internal(const simChar* elementName)
{
	C_API_FUNCTION_DEBUG;

	std::string elementNameAdjusted=CIloIlo::getCNameSuffixAdjustedName(elementName);
	CIloIlo::enableCNameSuffixAdjustment();
	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithName(elementNameAdjusted);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_UI_INEXISTANT);
			return(-1);
		}
		int retVal=it->getBlockID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetUIProperty_internal(simInt elementHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIExist(__func__,elementHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		int retVal=it->getAttributes();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetUIEventButton_internal(simInt elementHandle,simInt* auxiliaryValues)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIExist(__func__,elementHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		int retVal=it->getLastEventButtonID(auxiliaryValues);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetUIProperty_internal(simInt elementHandle,simInt elementProperty)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIExist(__func__,elementHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		// Following few new since 4/2/2013 (to bring newly made visible UI to the front)
		int attrib=it->getAttributes();
		it->setAttributes(elementProperty);
		int attribNew=it->getAttributes();
		if ( ((attrib&sim_ui_property_visible)==0)&&((attribNew&sim_ui_property_visible)!=0) )
			App::ct->buttonBlockContainer->sendBlockToFront(it->getBlockID());
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetUIButtonSize_internal(simInt elementHandle,simInt buttonHandle,simInt* size)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		size[0]=but->getLength();
		size[1]=but->getHeight();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simGetUIButtonProperty_internal(simInt elementHandle,simInt buttonHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		int retVal=but->getAttributes();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetUIButtonProperty_internal(simInt elementHandle,simInt buttonHandle,simInt buttonProperty)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		but->setAttributes(buttonProperty);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetUIButtonLabel_internal(simInt elementHandle,simInt buttonHandle,const simChar* upStateLabel,const simChar* downStateLabel)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		if (upStateLabel!=NULL)
			but->label=std::string(upStateLabel);
		if (downStateLabel!=NULL)
			but->downLabel=std::string(downStateLabel);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simGetUIButtonLabel_internal(simInt elementHandle,simInt buttonHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(NULL);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		char* retVal=new char[but->label.length()+1];
		for (unsigned int i=0;i<but->label.length();i++)
			retVal[i]=but->label[i];
		retVal[but->label.length()]=0;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simSetUISlider_internal(simInt elementHandle,simInt buttonHandle,simInt position)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		if (but->getButtonType()!=sim_buttonproperty_slider)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_BUTTON_NOT_SLIDER);
			return(-1);
		}
		but->setSliderPos((float(position)/500.0f)-1.0f);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetUISlider_internal(simInt elementHandle,simInt buttonHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		if (but->getButtonType()!=sim_buttonproperty_slider)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_BUTTON_NOT_SLIDER);
			return(-1);
		}
		int retVal=int((but->getSliderPos()+1.0f)*500.0f);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetUIButtonColor_internal(simInt elementHandle,simInt buttonHandle,const simFloat* upStateColor,const simFloat* downStateColor,const simFloat* labelColor)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		for (int i=0;i<3;i++)
		{
			if (upStateColor!=NULL)
				but->backgroundColor[i]=upStateColor[i];
			if (downStateColor!=NULL)
				but->downBackgroundColor[i]=downStateColor[i];
			if (labelColor!=NULL)
				but->textColor[i]=labelColor[i];
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simRemoveUI_internal(simInt elementHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (elementHandle==sim_handle_all)
		{
			App::ct->buttonBlockContainer->removeAllBlocks(false);
			return(1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_UI_INEXISTANT);
			return(-1);
		}
		App::ct->buttonBlockContainer->removeBlockFromID(elementHandle);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simChar* simGetSimulatorMessage_internal(simInt* messageID,simInt* auxiliaryData,simInt* returnedDataSize)
{
	C_API_FUNCTION_DEBUG;

	char* retVal=App::ct->simulatorMessageQueue->extractOneCommand(messageID[0],auxiliaryData,returnedDataSize[0]);
	return(retVal);
}

simInt simSaveScene_internal(const simChar* filename)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}
	if (App::ct->environment->getSceneLocked())
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCENE_LOCKED);
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (App::ct->environment->getRequestFinalSave())
			App::ct->environment->setSceneLocked(); // silent locking!

		if (!CFileMenu::saveScene(filename,false,false))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCENE_COULD_NOT_BE_SAVED);
			return(-1);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simSaveModel_internal(int baseOfModelHandle,const simChar* filename)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}
	if (App::ct->environment->getSceneLocked())
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCENE_LOCKED);
		return(-1);
	}
	if (!doesObjectExist(__func__,baseOfModelHandle))
	{
		return(-1);
	}
	C3DObject* it=App::ct->objCont->getObject(baseOfModelHandle);
	if (!it->getModelBase())
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_MODEL_BASE);
		return(-1);
	}
	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!CFileMenu::saveModel(baseOfModelHandle,filename,false,false))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MODEL_COULD_NOT_BE_SAVED);
			return(-1);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simSaveUI_internal(int count,const int* uiHandles,const simChar* filename)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (App::ct->environment->getSceneLocked())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCENE_LOCKED);
			return(-1);
		}
		bool result;
		if (count<1)
			result=CFileMenu::saveUserInterfaces(filename,false,false,NULL);
		else
		{
			std::vector<int> h;
			for (int i=0;i<count;i++)
				h.push_back(uiHandles[i]);
			result=CFileMenu::saveUserInterfaces(filename,false,false,&h);
		}
		if (!result)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_UI_COULD_NOT_BE_SAVED);
			return(-1);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simDoesFileExist_internal(const simChar* filename)
{
	C_API_FUNCTION_DEBUG;

	if (!VFile::doesFileExist(filename))
	{
		return(0);
	}
	return(1);
}

simInt simIsObjectInSelection_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=0;
		if (App::ct->objCont->isObjectInSelection(objectHandle))
			retVal|=1;
		C3DObject* lastSel=App::ct->objCont->getLastSelection();
		if (lastSel!=NULL)
		{
			if (lastSel->getID()==objectHandle)
				retVal|=2;
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAddObjectToSelection_internal(simInt what,simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (ifEditModeActiveGenerateErrorAndReturnTrue(__func__))
		{
			return(-1);
		}
		if (what==sim_handle_all)
			App::ct->objCont->selectAllObjects();
		else
		{
			if (!doesObjectExist(__func__,objectHandle))
			{
				return(-1);
			}
			C3DObject* it=App::ct->objCont->getObject(objectHandle);
			if (what==sim_handle_single)
				App::ct->objCont->addObjectToSelection(objectHandle);
			else
			{
				if ((what==sim_handle_tree)||(what==sim_handle_chain))
				{
					std::vector<C3DObject*> allObjects;
					if (what==sim_handle_tree)
						it->getAllObjectsRecursive(&allObjects,true,true);
					if (what==sim_handle_chain)
						it->getChain(allObjects,true,true);
					for (int i=0;i<int(allObjects.size());i++)
						App::ct->objCont->addObjectToSelection(allObjects[i]->getID());
				}
				else
				{
					CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ARGUMENT);
					return(-1);
				}
			}
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);

}

simInt simRemoveObjectFromSelection_internal(simInt what,simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (ifEditModeActiveGenerateErrorAndReturnTrue(__func__))
		{
			return(-1);
		}
		if (what==sim_handle_all)
			App::ct->objCont->deselectObjects();
		else
		{
			if (!doesObjectExist(__func__,objectHandle))
			{
				return(-1);
			}
			C3DObject* it=App::ct->objCont->getObject(objectHandle);
			if (what==sim_handle_single)
				App::ct->objCont->removeObjectFromSelection(objectHandle);
			else
			{
				if ((what==sim_handle_tree)||(what==sim_handle_chain))
				{
					std::vector<C3DObject*> allObjects;
					if (what==sim_handle_tree)
						it->getAllObjectsRecursive(&allObjects,true,true);
					if (what==sim_handle_chain)
						it->getChain(allObjects,true,true);
					for (int i=0;i<int(allObjects.size());i++)
						App::ct->objCont->removeObjectFromSelection(allObjects[i]->getID());
				}
				else
				{
					CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ARGUMENT);
					return(-1);
				}
			}
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);

}

simInt simGetObjectSelectionSize_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=App::ct->objCont->getSelSize();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetObjectLastSelection_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=-1;
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it!=NULL)
			retVal=it->getID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simGetObjectSelection_internal(simInt* objectHandles)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
			objectHandles[i]=App::ct->objCont->getSelID(i);
		return(App::ct->objCont->getSelSize());
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSearchPath_internal(simInt pathPlanningObjectHandle,simFloat maximumSearchTime)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CPathPlanningTask* it=App::ct->pathPlanning->getObject(pathPlanningObjectHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PATH_PLANNING_OBJECT_INEXISTANT);
			return(-1);
		}
		int retVal=0;
		if (it->performSearch(false,maximumSearchTime))
			retVal=1;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simInitializePathSearch_internal(simInt pathPlanningObjectHandle,simFloat maximumSearchTime,simFloat searchTimeStep)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CPathPlanningTask* it=App::ct->pathPlanning->getObject(pathPlanningObjectHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PATH_PLANNING_OBJECT_INEXISTANT);
			return(-1);
		}
		if (App::ct->pathPlanning->getTemporaryPathSearchObjectCount()>100)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_TOO_MANY_TEMP_OBJECTS);
			return(-1);
		}

		maximumSearchTime=tt::getLimitedFloat(0.01f,36000.0f,maximumSearchTime);
		searchTimeStep=tt::getLimitedFloat(0.001f,SIM_MIN(1.0f,maximumSearchTime),searchTimeStep);
		CPathPlanningTask* oldIt=it;
		it=oldIt->copyYourself(); // we copy it because the original might be destroyed at any time
		it->setOriginalTask(oldIt);
		int retVal=-1; // error
		if (it->initiateSteppedSearch(false,maximumSearchTime,searchTimeStep))
			retVal=App::ct->pathPlanning->addTemporaryPathSearchObject(it);
		else
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PATH_PLANNING_OBJECT_NOT_CONSISTENT);
			delete it;
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simPerformPathSearchStep_internal(simInt temporaryPathSearchObject,simBool abortSearch)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CPathPlanningTask* it=App::ct->pathPlanning->getTemporaryPathSearchObject(temporaryPathSearchObject);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_TEMP_PATH_SEARCH_OBJECT_INEXISTANT);
			return(-1);
		}
		if (abortSearch)
		{
			App::ct->pathPlanning->removeTemporaryPathSearchObjectButDontDestroyIt(it);
			delete it;
			return(0);
		}
		int retVal=it->performSteppedSearch();
		if (retVal!=-2)
		{
			App::ct->pathPlanning->removeTemporaryPathSearchObjectButDontDestroyIt(it);
			CPathPlanningTask* originalIt=it->getOriginalTask();
			int tree1Handle,tree2Handle;
			it->getAndDisconnectSearchTrees(tree1Handle,tree2Handle); // to keep trees visible!
			delete it;
			// Now we connect the trees only if the originalTask still exists:
			bool found=false;
			for (int ot=0;ot<int(App::ct->pathPlanning->allObjects.size());ot++)
			{
				if (App::ct->pathPlanning->allObjects[ot]==originalIt)
				{
					found=true;
					break;
				}
			}
			if (found)
				originalIt->connectExternalSearchTrees(tree1Handle,tree2Handle);
			else
			{
				App::ct->drawingCont->removeObject(tree1Handle);
				App::ct->drawingCont->removeObject(tree2Handle);
			}
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simHandleCollision_internal(simInt collisionObjectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (collisionObjectHandle!=sim_handle_all)&&(collisionObjectHandle!=sim_handle_all_except_explicit) )
		{
			if (!doesCollisionObjectExist(__func__,collisionObjectHandle))
			{
				return(-1);
			}
		}
		int colCnt=0;
		if (collisionObjectHandle<0)
			colCnt=App::ct->collisions->handleAllCollisions(collisionObjectHandle==sim_handle_all_except_explicit); // implicit handling
		else
		{ // explicit handling
			CRegColl* it=App::ct->collisions->getObject(collisionObjectHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			if (it->handleCollision(false))
				colCnt++;
		}
		return(colCnt);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simReadCollision_internal(simInt collisionObjectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesCollisionObjectExist(__func__,collisionObjectHandle))
		{
			return(-1);
		}
		CRegColl* it=App::ct->collisions->getObject(collisionObjectHandle);
		int retVal=it->readCollision();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simHandleDistance_internal(simInt distanceObjectHandle,simFloat* smallestDistance)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (distanceObjectHandle!=sim_handle_all)&&(distanceObjectHandle!=sim_handle_all_except_explicit) )
		{
			if (!doesDistanceObjectExist(__func__,distanceObjectHandle))
			{
				return(-1);
			}
		}
		float d;
		if (distanceObjectHandle<0)
			d=App::ct->distances->handleAllDistances(distanceObjectHandle==sim_handle_all_except_explicit); // implicit handling
		else
		{ // explicit handling
			CRegDist* it=App::ct->distances->getObject(distanceObjectHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			d=it->handleDistance(false);
		}
		if (d>=0.0f)
		{
			if (smallestDistance!=NULL)
				smallestDistance[0]=d;
			return(1);
		}
		else
			return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simReadDistance_internal(simInt distanceObjectHandle,simFloat* smallestDistance)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesDistanceObjectExist(__func__,distanceObjectHandle))
		{
			return(-1);
		}
		float d;
		CRegDist* it=App::ct->distances->getObject(distanceObjectHandle);
		d=it->readDistance();
		if (d>=0.0f)
		{
			smallestDistance[0]=d;
			return(1);
		}
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simHandleMill_internal(simInt millHandle,simFloat* removedSurfaceAndVolume)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{ // writing/reading of cut data is protected by another mutex!
		if ( (millHandle!=sim_handle_all)&&(millHandle!=sim_handle_all_except_explicit) )
		{
			if (!isMill(__func__,millHandle))
			{
				return(-1);
			}
		}
		int totalCutCount=0;
		float totalSurface=0.0f;
		float totalVolume=0.0f;
		for (int i=0;i<int(App::ct->objCont->millList.size());i++)
		{
			float surface=0.0f;
			float volume=0.0f;
			CMill* it=(CMill*)App::ct->objCont->getObject(App::ct->objCont->millList[i]);
			if (millHandle>=0)
			{ // explicit handling
				it=(CMill*)App::ct->objCont->getObject(millHandle);
				if (!it->getExplicitHandling())
				{
					CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
					return(-1);
				}
				totalCutCount+=it->handleMill(false,surface,volume,false);
			}
			else
				totalCutCount+=it->handleMill(millHandle==sim_handle_all_except_explicit,surface,volume,false);
			totalSurface+=surface;
			totalVolume+=volume;
			if (millHandle>=0)
				break;
		}
		if (removedSurfaceAndVolume!=NULL)
		{
			removedSurfaceAndVolume[0]=totalSurface;
			removedSurfaceAndVolume[1]=totalVolume;
		}
		return(totalCutCount);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}



simInt simHandleProximitySensor_internal(simInt sensorHandle,simFloat* detectedPoint,simInt* detectedObjectHandle,simFloat* normalVector)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (sensorHandle>=0)
		{ // handle just one sensor (this is explicit handling)
			CProxSensor* it=App::ct->objCont->getProximitySensor(sensorHandle);
			if (it==NULL)
			{
				return(-1);
			}
			else
			{
				if (!it->getExplicitHandling())
				{
					CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
					return(-1);
				}

				int retVal=0;
				C3Vector smallest;
				float smallestL=SIM_MAX_FLOAT;
				int detectedObj;
				C3Vector detectedSurf;
				bool detected=it->handleSensor(false,false,detectedObj,detectedSurf);
				if (detected)
				{
					smallest=it->getDetectedPoint();
					smallestL=smallest.getLength();
					retVal=1;
					if (detectedPoint!=NULL)
					{
						detectedPoint[0]=smallest(0);
						detectedPoint[1]=smallest(1);
						detectedPoint[2]=smallest(2);
						detectedPoint[3]=smallestL;
					}
					if (detectedObjectHandle!=NULL)
						detectedObjectHandle[0]=detectedObj;
					if (normalVector!=NULL)
					{
						normalVector[0]=detectedSurf(0);
						normalVector[1]=detectedSurf(1);
						normalVector[2]=detectedSurf(2);
					}
				}
				return(retVal);
			}
		}
		else
		{ // handle several sensors at once (with sim_handle_all or sim_handle_all_except_explicit
			if (CSensingThreadPool::getThreadCount()==0)
			{ // we want to process the sensor handling sequentially (i.e. not in parallel)
				int retVal=0;
				C3Vector allSmallest;
				int detectedObjectID=-1;
				C3Vector detectedSurfaceNormal;
				float allSmallestL=SIM_MAX_FLOAT;
				for (int i=0;i<int(App::ct->objCont->proximitySensorList.size());i++)
				{
					int detectedObj;
					C3Vector detectedSurf;
					CProxSensor* it=(CProxSensor*)App::ct->objCont->getObject(App::ct->objCont->proximitySensorList[i]);
					bool detected=false;
					detected=it->handleSensor(false,sensorHandle==sim_handle_all_except_explicit,detectedObj,detectedSurf);
					if (detected)
					{
						C3Vector smallest(it->getDetectedPoint());
						float smallestL=smallest.getLength();

						if (smallestL<allSmallestL)
						{
							allSmallest=smallest;
							allSmallestL=smallestL;
							detectedObjectID=detectedObj;
							detectedSurfaceNormal=detectedSurf;
							retVal=1;
						}
					}
				}
				if (retVal==1)
				{
					if (detectedPoint!=NULL)
					{
						detectedPoint[0]=allSmallest(0);
						detectedPoint[1]=allSmallest(1);
						detectedPoint[2]=allSmallest(2);
						detectedPoint[3]=allSmallestL;
					}
					if (detectedObjectHandle!=NULL)
						detectedObjectHandle[0]=detectedObjectID;
					if (normalVector!=NULL)
					{
						normalVector[0]=detectedSurfaceNormal(0);
						normalVector[1]=detectedSurfaceNormal(1);
						normalVector[2]=detectedSurfaceNormal(2);
					}
				}
				return(retVal);
			}
			else
			{ // we want to handle the sensors concurrently (i.e. in parallel)
				for (int i=0;i<int(App::ct->objCont->proximitySensorList.size());i++)
				{
					CProxSensor* it=(CProxSensor*)App::ct->objCont->getObject(App::ct->objCont->proximitySensorList[i]);
					SParallelWorkDat e;
					e.object=it;
					e.objectType=2; // prox. sensor
					e.exceptExplicit=sensorHandle==sim_handle_all_except_explicit;
					e.handlingStarted=false;
					e.freeSlot=false;
					CSensingThreadPool::addTask(e);
				}
				return(0);
			}
		}
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simReadProximitySensor_internal(simInt sensorHandle,simFloat* detectedPoint,simInt* detectedObjectHandle,simFloat* normalVector)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!isSensor(__func__,sensorHandle))
		{
			return(-1);
		}
		CProxSensor* it=App::ct->objCont->getProximitySensor(sensorHandle);
		C3Vector dPt;
		int dObjHandle;
		C3Vector nVect;
		int retVal=it->readSensor(dPt,dObjHandle,nVect);
		if (retVal==1)
		{
			if (detectedPoint!=NULL)
			{
				dPt.copyTo(detectedPoint);
				detectedPoint[3]=dPt.getLength();
			}
			if (detectedObjectHandle!=NULL)
				detectedObjectHandle[0]=dObjHandle;
			if (normalVector!=NULL)
				nVect.copyTo(normalVector);
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simHandleIkGroup_internal(simInt ikGroupHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (ikGroupHandle!=sim_handle_all)&&(ikGroupHandle!=sim_handle_all_except_explicit) )
		{
			if (!doesIKGroupExist(__func__,ikGroupHandle))
			{
				return(-1);
			}
		}
		int returnValue=0;
		if (ikGroupHandle<0)
			returnValue=App::ct->ikGroups->computeAllIkGroups(ikGroupHandle==sim_handle_all_except_explicit);
		else
		{ // explicit handling
			CikGroup* it=App::ct->ikGroups->getIkGroup(ikGroupHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			returnValue=it->computeGroupIk(false);
		}
		return(returnValue);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simCheckIkGroup_internal(simInt ikGroupHandle,simInt jointCnt,const simInt* jointHandles,simFloat* jointValues,simVoid* reserved)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesIKGroupExist(__func__,ikGroupHandle))
		{
			return(-1);
		}
		CikGroup* it=App::ct->ikGroups->getIkGroup(ikGroupHandle);
		if (!it->getExplicitHandling())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
			return(-1);
		}
		// Check if the handles are valid:
		bool ok=true;
		for (int i=0;i<jointCnt;i++)
		{
			if (App::ct->objCont->getJoint(jointHandles[i])==NULL)
				ok=false;
		}
		if (!ok)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_HANDLES);
			return(-1);
		}

		// Save joint positions/modes (all of them, just in case)
		std::vector<CJoint*> sceneJoints;
		std::vector<float> initSceneJointValues;
		std::vector<int> initSceneJointModes;
		for (int i=0;i<int(App::ct->objCont->jointList.size());i++)
		{
			CJoint* aj=App::ct->objCont->getJoint(App::ct->objCont->jointList[i]);
			sceneJoints.push_back(aj);
			initSceneJointValues.push_back(aj->getPosition());
			initSceneJointModes.push_back(aj->getJointMode());
		}

		// Set the correct mode for the joints involved:
		for (int i=0;i<jointCnt;i++)
		{
			CJoint* joint=App::ct->objCont->getJoint(jointHandles[i]);
			if (joint->getJointMode()!=sim_jointmode_ik)
				joint->setJointMode(sim_jointmode_ik,false);
		}

		// do the calculation:
		int returnValue=it->computeGroupIk(false);

		// Prepare the return array with the computed joint values:
		for (int i=0;i<jointCnt;i++)
		{
			CJoint* joint=App::ct->objCont->getJoint(jointHandles[i]);
			jointValues[i]=joint->getPosition();
		}

		// Restore joint positions/modes:
		for (int i=0;i<int(sceneJoints.size());i++)
		{
			if (sceneJoints[i]->getPosition()!=initSceneJointValues[i])
				sceneJoints[i]->setPosition(initSceneJointValues[i]);
			if (sceneJoints[i]->getJointMode()!=initSceneJointModes[i])
				sceneJoints[i]->setJointMode(initSceneJointModes[i],false);
		}

		return(returnValue);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simHandleDynamics_internal(simFloat deltaTime)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		App::ct->dynamicsContainer->handleDynamics(deltaTime);
		if ((!App::ct->dynamicsContainer->isWorldThere())&&App::ct->dynamicsContainer->getDynamicsEnabled())
		{
			App::ct->dynamicsContainer->markForWarningDisplay_physicsEngineNotSupported();
			return(0);
		}
		return(CDynInterface::getDynamicStepDivider());
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetScriptHandle_internal(const simChar* scriptName)
{
	C_API_FUNCTION_DEBUG;

	std::string scriptNameAdjusted=CIloIlo::getCNameSuffixAdjustedName(scriptName);
	CIloIlo::enableCNameSuffixAdjustment();
	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromPseudoName(scriptNameAdjusted);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(-1);
		}
		if (it->getDestroyAtEndOfSimulation())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_WAS_DESTROYED);
			return(-1);
		}
		int retVal=it->getScriptID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simHandleMainScript_internal()
{
	C_API_FUNCTION_DEBUG;
	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	int data[4]={0,0,0,0};
	int rtVal[4]={-1,-1,-1,-1};
	void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_mainscriptabouttobecalled,data,NULL,rtVal);
	delete[] (char*)returnVal;
	if (rtVal[0]!=-1)
	{ // a plugin doesn't want to run the main script!
		// Handle add-on execution (during non-simulation, it happens elsewhere!):
		App::ct->luaScriptContainer->handleAddOnScriptExecution();

		return(sim_script_main_script_not_called); // this should not generate an error
	}

	App::ct->infoDisplay->simulationPassStart();
	CLuaScriptObject* it=App::ct->luaScriptContainer->getMainScript();
	if (it==NULL)
	{
		// Handle add-on execution (during non-simulation, it happens elsewhere!):
		App::ct->luaScriptContainer->handleAddOnScriptExecution();

		return(sim_script_main_script_nonexistent); // this should not generate an error
	}
	int retVal=0;
	App::ct->luaScriptContainer->removeLastExecutedMainScriptReturnValues();
	App::ct->luaScriptContainer->lastExecutedMainScriptReturnValues=new CLuaObjectGroup();


	if (App::ct->luaScriptContainer->mainScriptInputValues==NULL)
	{ // no input values
		CLuaObjectGroup dummyGroup;
		retVal=it->runOneStep(dummyGroup,*App::ct->luaScriptContainer->lastExecutedMainScriptReturnValues);
	}
	else
		retVal=it->runOneStep(*App::ct->luaScriptContainer->mainScriptInputValues,*App::ct->luaScriptContainer->lastExecutedMainScriptReturnValues);
	App::ct->infoDisplay->simulationPassEnd();

	// Handle add-on execution (during non-simulation, it happens elsewhere!):
	App::ct->luaScriptContainer->handleAddOnScriptExecution();

	return(retVal);
}

simInt simResetScript_internal(simInt scriptHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (scriptHandle==sim_handle_all)
		{
			App::ct->luaScriptContainer->resetAllSimulationScripts();
			return(1);
		}
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(-1);
		}
		it->reset();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simSetScriptText_internal(simInt scriptHandle,const simChar* scriptText)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(-1);
		}
		if (!App::ct->simulation->isSimulationStopped())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_STOPPED);
			return(-1);
		}
		if (App::ct->scintillaEditorContainer->closeEditor(scriptHandle))
		{ // We close and reopen the editor
			it->setScriptText(scriptText);
			App::ct->scintillaEditorContainer->openEditorForScript(scriptHandle);
		}
		else
			it->setScriptText(scriptText);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simChar* simGetScriptText_internal(simInt scriptHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (App::ct->environment->getSceneLocked())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCENE_LOCKED);
			return(NULL);
		}
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(NULL);
		}
		char* retVal=NULL;
		if (App::ct->scintillaEditorContainer->closeEditor(scriptHandle))
		{ // We close and reopen the editor
			retVal=it->getScriptText();
			App::ct->scintillaEditorContainer->openEditorForScript(scriptHandle);
		}
		else
			retVal=it->getScriptText();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simGetScript_internal(simInt index)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (index<0)||(index>=int(App::ct->luaScriptContainer->allScripts.size())) )
		{
			return(-1);
		}
		CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[index];
		int retVal=it->getScriptID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetScriptAssociatedWithObject_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo(objectHandle);
		if (it==NULL)
		{
			return(-1);
		}
		int retVal=it->getScriptID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetObjectAssociatedWithScript_internal(simInt scriptHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(-1);
		}
		int retVal=it->getObjectIDThatScriptIsAttachedTo();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simGetScriptName_internal(simInt scriptHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(NULL);
		}
		std::string name(it->getScriptPseudoName());
		char* retVal=new char[name.length()+1];
		for (int i=0;i<int(name.length());i++)
			retVal[i]=name[i];
		retVal[name.length()]=0;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}


simInt simGetScriptProperty_internal(simInt scriptHandle,simInt* scriptProperty,simInt* associatedObjectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(-1);
		}
		scriptProperty[0]=it->getScriptType();
		associatedObjectHandle[0]=it->getObjectIDThatScriptIsAttachedTo();
		if (it->getThreadedExecution())
			scriptProperty[0]|=sim_scripttype_threaded;
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAssociateScriptWithObject_internal(simInt scriptHandle,simInt associatedObjectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(-1);
		}
		if (it->getScriptType()!=sim_scripttype_childscript)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_NOT_CHILD_SCRIPT);
			return(-1);
		}
		if (associatedObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,associatedObjectHandle))
			{
				return(-1);
			}
		}
		if (!App::ct->simulation->isSimulationStopped())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_STOPPED);
			return(-1);
		}
		it->setObjectIDThatScriptIsAttachedTo(associatedObjectHandle);
		App::ct->objCont->setLightDialogRefreshFlag();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);

}

simInt simAddScript_internal(simInt scriptProperty)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CLuaScriptObject* it=new CLuaScriptObject(true);
		if (scriptProperty&sim_scripttype_threaded)
		{
			it->setScriptType(scriptProperty-sim_scripttype_threaded);
			it->setThreadedExecution(true);
			it->setExecuteJustOnce(true);
		}
		else
			it->setScriptType(scriptProperty);
		int retVal=App::ct->luaScriptContainer->insertScript(it);
		App::ct->objCont->setFullDialogRefreshFlag();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simRemoveScript_internal(simInt scriptHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (scriptHandle==sim_handle_all)
		{ // We wanna remove all scripts!
			if (!App::ct->simulation->isSimulationStopped())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_STOPPED);
				return(-1);
			}
			// we close the script editor windows:
			App::ct->scintillaEditorContainer->closeAllEditors();
			App::ct->luaScriptContainer->removeAllScripts(true);
			return(1);
		}
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(-1);
		}
		if (!App::ct->simulation->isSimulationStopped())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_STOPPED);
			return(-1);
		}
		App::ct->scintillaEditorContainer->closeEditor(scriptHandle);
		App::ct->luaScriptContainer->removeScript(scriptHandle);
		App::ct->objCont->setFullDialogRefreshFlag();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simGetCollisionHandle_internal(const simChar* collisionObjectName)
{
	C_API_FUNCTION_DEBUG;

	std::string collisionObjectNameAdjusted=CIloIlo::getCNameSuffixAdjustedName(collisionObjectName);
	CIloIlo::enableCNameSuffixAdjustment();
	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CRegColl* it=App::ct->collisions->getObject(collisionObjectNameAdjusted);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COLLISION_INEXISTANT);
			return(-1);
		}
		int retVal=it->getObjectID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetDistanceHandle_internal(const simChar* distanceObjectNameI)
{
	C_API_FUNCTION_DEBUG;

	std::string distanceObjectNameAdjusted=CIloIlo::getCNameSuffixAdjustedName(distanceObjectNameI);
	CIloIlo::enableCNameSuffixAdjustment();
	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CRegDist* it=App::ct->distances->getObject(distanceObjectNameAdjusted);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_DISTANCE_INEXISTANT);
			return(-1);
		}
		int retVal=it->getObjectID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetIkGroupHandle_internal(const simChar* ikGroupName)
{
	C_API_FUNCTION_DEBUG;

	std::string ikGroupNameAdjusted=CIloIlo::getCNameSuffixAdjustedName(ikGroupName);
	CIloIlo::enableCNameSuffixAdjustment();
	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(ikGroupNameAdjusted);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_IK_GROUP_INEXISTANT);
			return(-1);
		}
		int retVal=it->getObjectID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetIkGroupProperties_internal(simInt ikGroupHandle,simInt resolutionMethod,simInt maxIterations,simFloat damping,void* reserved)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(ikGroupHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_IK_GROUP_INEXISTANT);
			return(-1);
		}
		it->setCalculationMethod(resolutionMethod);
		it->setMaxIterations(maxIterations);
		it->setDlsFactor(damping);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetIkElementProperties_internal(simInt ikGroupHandle,simInt tipDummyHandle,simInt constraints,const simFloat* precision,const simFloat* weight,void* reserved)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(ikGroupHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_IK_GROUP_INEXISTANT);
			return(-1);
		}
		if (!isDummy(__func__,tipDummyHandle))
		{
			return(-1);
		}
		CikEl* el=it->getIkElementWithTooltipID(tipDummyHandle);
		if (el==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_IK_ELEMENT_INEXISTANT);
			return(-1);
		}
		el->setConstraints(constraints);
		if (precision!=NULL)
		{
			el->setMinLinearPrecision(precision[0]);
			el->setMinAngularPrecision(precision[1]);
		}
		if (weight!=NULL)
		{
			el->setPositionWeight(weight[0]);
			el->setOrientationWeight(weight[1]);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simResetCollision_internal(simInt collisionObjectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (collisionObjectHandle!=sim_handle_all)&&(collisionObjectHandle!=sim_handle_all_except_explicit) )
		{
			if (!doesCollisionObjectExist(__func__,collisionObjectHandle))
			{
				return(-1);
			}
		}
		if (collisionObjectHandle<0)
			App::ct->collisions->resetAllCollisions(collisionObjectHandle==sim_handle_all_except_explicit);
		else
		{ // Explicit handling
			CRegColl* it=App::ct->collisions->getObject(collisionObjectHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			it->clearCollisionResult();
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simResetDistance_internal(simInt distanceObjectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (distanceObjectHandle!=sim_handle_all)&&(distanceObjectHandle!=sim_handle_all_except_explicit) )
		{
			if (!doesDistanceObjectExist(__func__,distanceObjectHandle))
			{
				return(-1);
			}
		}
		if (distanceObjectHandle<0)
			App::ct->distances->resetAllDistances(distanceObjectHandle==sim_handle_all_except_explicit);
		else
		{ // Explicit handling
			CRegDist* it=App::ct->distances->getObject(distanceObjectHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			it->clearDistanceResult();
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simResetProximitySensor_internal(simInt sensorHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (sensorHandle!=sim_handle_all)&&(sensorHandle!=sim_handle_all_except_explicit) )
		{
			if (!isSensor(__func__,sensorHandle))
			{
				return(-1);
			}
		}
		for (int i=0;i<int(App::ct->objCont->proximitySensorList.size());i++)
		{
			CProxSensor* it=(CProxSensor*)App::ct->objCont->getObject(App::ct->objCont->proximitySensorList[i]);
			if (sensorHandle>=0)
			{ // Explicit handling
				it=(CProxSensor*)App::ct->objCont->getObject(sensorHandle);
				if (!it->getExplicitHandling())
				{
					CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
					return(-1);
				}
				it->resetSensor(false);
				break;
			}
			else
				it->resetSensor(sensorHandle==sim_handle_all_except_explicit);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simResetMill_internal(simInt millHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if ( (millHandle!=sim_handle_all)&&(millHandle!=sim_handle_all_except_explicit) )
		{
			if (!isMill(__func__,millHandle))
			{
				return(-1);
			}
		}
		for (int i=0;i<int(App::ct->objCont->millList.size());i++)
		{
			CMill* it=(CMill*)App::ct->objCont->getObject(App::ct->objCont->millList[i]);
			if (millHandle>=0)
			{ // Explicit handling
				it=(CMill*)App::ct->objCont->getObject(millHandle);
				if (!it->getExplicitHandling())
				{
					CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
					return(-1);
				}
				it->resetMill(false);
				break;
			}
			else
				it->resetMill(millHandle==sim_handle_all_except_explicit);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}


simInt simCheckProximitySensor_internal(simInt sensorHandle,simInt entityHandle,simFloat* detectedPoint)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,sensorHandle))
		{
			return(-1);
		}
		if (!isSensor(__func__,sensorHandle))
		{
			return(-1);
		}
		if ( (entityHandle!=sim_handle_all)&&(!doesEntityExist(__func__,entityHandle)) )
		{
			return(-1);
		}
		int retVal=simCheckProximitySensorEx_internal(sensorHandle,entityHandle,3,SIM_MAX_FLOAT,0,detectedPoint,NULL,NULL);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simCheckProximitySensorEx_internal(simInt sensorHandle,simInt entityHandle,simInt detectionMode,simFloat detectionThreshold,simFloat maxAngle,simFloat* detectedPoint,simInt* detectedObjectHandle,simFloat* normalVector)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,sensorHandle))
		{
			return(-1);
		}
		if (!isSensor(__func__,sensorHandle))
		{
			return(-1);
		}
		if ( (entityHandle!=sim_handle_all)&&(!doesEntityExist(__func__,entityHandle)) )
		{
			return(-1);
		}
		if (entityHandle==sim_handle_all)
			entityHandle=-1;

		if (!App::ct->mainSettings->proximitySensorsEnabled)
		{
			return(0);
		}

		bool frontFace=SIM_IS_BIT_SET(detectionMode,0);
		bool backFace=SIM_IS_BIT_SET(detectionMode,1);
		bool fastDetection=SIM_IS_BIT_SET(detectionMode,2);
		bool limitedAngle=SIM_IS_BIT_SET(detectionMode,3);
		bool checkForOcclusions=SIM_IS_BIT_SET(detectionMode,4);
		if (!(frontFace||backFace))
			frontFace=true;
		if (detectionThreshold<0.0f)
			detectionThreshold=0.0f;
		tt::limitValue(0.0f,piValD2,maxAngle);
		int detectedObj;
		C3Vector dPoint;
		float minThreshold=-1.0f;
		CProxSensor* it=App::ct->objCont->getProximitySensor(sensorHandle);
		if ( (it!=NULL)&&(it->convexVolume->getSmallestDistanceEnabled()) )
			minThreshold=it->convexVolume->getSmallestDistanceAllowed();
		C3Vector normV;
		bool returnValue;
		if (it->getRandomizedDetection())
		{
			it->calculateFreshRandomizedRays();
			returnValue=CProxSensorRoutine::randomizedDetectEntity(sensorHandle,entityHandle,!fastDetection,limitedAngle,maxAngle,
				dPoint,detectionThreshold,frontFace,backFace,detectedObj,minThreshold,normV,true,checkForOcclusions,it->getPointerToRandomizedRays()[0],it->getPointerToRandomizedRayDetectionStates()[0],it->getRandomizedDetectionCountForDetection(),false);
		}
		else
		{
			returnValue=CProxSensorRoutine::detectEntity(sensorHandle,entityHandle,!fastDetection,limitedAngle,maxAngle,dPoint,detectionThreshold,frontFace,backFace,detectedObj,minThreshold,normV,true,checkForOcclusions,false);
		}
		if (returnValue)
		{
			if (detectedPoint!=NULL)
			{
				detectedPoint[0]=dPoint(0);
				detectedPoint[1]=dPoint(1);
				detectedPoint[2]=dPoint(2);
				detectedPoint[3]=detectionThreshold;
			}
			if (detectedObjectHandle!=NULL)
				detectedObjectHandle[0]=detectedObj;
			if (normalVector!=NULL)
			{
				normalVector[0]=normV(0);
				normalVector[1]=normV(1);
				normalVector[2]=normV(2);
			}
			return(1);
		}
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simCheckProximitySensorEx2_internal(simInt sensorHandle,simFloat* vertexPointer,simInt itemType,simInt itemCount,simInt detectionMode,simFloat detectionThreshold,simFloat maxAngle,simFloat* detectedPoint,simFloat* normalVector)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!isSensor(__func__,sensorHandle))
		{
			return(-1);
		}

		if (!App::ct->mainSettings->proximitySensorsEnabled)
		{
			return(0);
		}
		bool frontFace=SIM_IS_BIT_SET(detectionMode,0);
		bool backFace=SIM_IS_BIT_SET(detectionMode,1);
		bool fastDetection=SIM_IS_BIT_SET(detectionMode,2);
		bool limitedAngle=SIM_IS_BIT_SET(detectionMode,3);
		if (!(frontFace||backFace))
			frontFace=true;
		if (detectionThreshold<0.0f)
			detectionThreshold=0.0f;
		tt::limitValue(0.0f,piValD2,maxAngle);
		C3Vector dPoint;
		float minThreshold=-1.0f;
		CProxSensor* it=App::ct->objCont->getProximitySensor(sensorHandle);
		if ( (it!=NULL)&&(it->convexVolume->getSmallestDistanceEnabled()) )
			minThreshold=it->convexVolume->getSmallestDistanceAllowed();
		C3Vector normV;
		bool returnValue;
		if (it->getRandomizedDetection())
		{
			it->calculateFreshRandomizedRays();
			returnValue=CProxSensorRoutine::randomizedDetectPrimitive(sensorHandle,vertexPointer,itemType,itemCount,!fastDetection,limitedAngle,maxAngle,
				dPoint,detectionThreshold,frontFace,backFace,minThreshold,normV,it->getPointerToRandomizedRays()[0],it->getPointerToRandomizedRayDetectionStates()[0],it->getRandomizedDetectionCountForDetection());
		}
		else
			returnValue=CProxSensorRoutine::detectPrimitive(sensorHandle,vertexPointer,itemType,itemCount,!fastDetection,limitedAngle,maxAngle,dPoint,detectionThreshold,frontFace,backFace,minThreshold,normV);
		if (returnValue)
		{
			if (detectedPoint!=NULL)
			{
				detectedPoint[0]=dPoint(0);
				detectedPoint[1]=dPoint(1);
				detectedPoint[2]=dPoint(2);
				detectedPoint[3]=detectionThreshold;
			}
			if (normalVector!=NULL)
			{
				normalVector[0]=normV(0);
				normalVector[1]=normV(1);
				normalVector[2]=normV(2);
			}
			return(1);
		}
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simLoadModule_internal(const simChar* filenameAndPath,const simChar* pluginName)
{ // -3: could not load, -2: missing entry points, -1: could not initialize. 0=< : handle of the plugin
  // we cannot lock/unlock, because this function might trigger another thread (GUI) that itself will initialize the plugin and call sim-functions --> forever locked!!
	C_API_FUNCTION_DEBUG;
	int handle=App::uiThread->pluginLoadAndStart(filenameAndPath,pluginName);
	return(handle);
}

simInt simUnloadModule_internal(simInt pluginhandle)
{ // we cannot lock/unlock, because this function might trigger another thread (GUI) that itself will initialize the plugin and call sim-functions --> forever locked!!
	C_API_FUNCTION_DEBUG;
	int retVal=0;
	if (App::uiThread->pluginStopAndUnload(pluginhandle))
		retVal=1;
	return(retVal);
}

simVoid* simSendModuleMessage_internal(simInt message,simInt* auxiliaryData,simVoid* customData,simInt* replyData)
{
	C_API_FUNCTION_DEBUG;

	if (replyData!=NULL)
	{
		replyData[0]=-1;
		replyData[1]=-1;
		replyData[2]=-1;
		replyData[3]=-1;
	}
	void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(message,auxiliaryData,customData,replyData);
	return(retVal);
}

simVoid* simBroadcastMessage_internal(simInt* auxiliaryData,simVoid* customData,simInt* replyData)
{
	C_API_FUNCTION_DEBUG;

	if (replyData!=NULL)
	{
		replyData[0]=-1;
		replyData[1]=-1;
		replyData[2]=-1;
		replyData[3]=-1;
	}
	void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_broadcast,auxiliaryData,customData,replyData);
	return(retVal);
}

simInt simRegisterCustomLuaFunction_internal(const simChar* funcName,const simChar* callTips,const simInt* inputArgumentTypes,simVoid(*callBack)(struct SLuaCallBack* p))
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		bool retVal=1;
		if (CIloIlo::luaCustomFunctionContainer.removeCustomFunctionFromName(funcName))
			retVal=0;// that function already existed. We remove it and replace it!
		std::vector<int> inputV;
		if (inputArgumentTypes!=NULL)
		{
			for (int i=0;i<inputArgumentTypes[0];i++)
				inputV.push_back(inputArgumentTypes[i+1]);
		}
		CLuaCustomFunction* newFunction=new CLuaCustomFunction(funcName,callTips,inputV,callBack);
		if (!CIloIlo::luaCustomFunctionContainer.insertCustomFunction(newFunction))
		{
			delete newFunction;
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_CUSTOM_LUA_FUNC_COULD_NOT_BE_REGISTERED);
			return(-1);
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simRegisterCustomLuaVariable_internal(const simChar* varName,const simChar* varValue)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		bool retVal=1;
		if (CIloIlo::luaCustomFunctionContainer.removeCustomVariable(varName))
			retVal=0;// that variable already existed. We remove it and replace it!
		if (!CIloIlo::luaCustomFunctionContainer.insertCustomVariable(varName,varValue))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_CUSTOM_LUA_VAR_COULD_NOT_BE_REGISTERED);
			return(-1);
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAddSceneCustomData_internal(simInt header,const simChar* data,simInt dataLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		App::ct->customSceneData->setData(header,data,dataLength);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simGetSceneCustomDataLength_internal(simInt header)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=App::ct->customSceneData->getDataLength(header);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetSceneCustomData_internal(simInt header,simChar* data)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		App::ct->customSceneData->getData(header,data);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}



simInt simAddObjectCustomData_internal(simInt objectHandle,simInt header,const simChar* data,simInt dataLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		it->setObjectCustomData(header,data,dataLength);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simGetObjectCustomDataLength_internal(simInt objectHandle,simInt header)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		int retVal=it->getObjectCustomDataLength(header);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetObjectCustomData_internal(simInt objectHandle,simInt header,simChar* data)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		it->getObjectCustomData(header,data);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simCreateBuffer_internal(simInt size)
{
	C_API_FUNCTION_DEBUG;

	simChar* retVal=new char[size];
	return(retVal);
}

simInt simReleaseBuffer_internal(simChar* buffer)
{
	C_API_FUNCTION_DEBUG;

	delete[] buffer;
	return(1);
}

simInt simCheckCollision_internal(simInt entity1Handle,simInt entity2Handle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (!doesEntityExist(__func__,entity1Handle))||
			((entity2Handle!=sim_handle_all)&&(!doesEntityExist(__func__,entity2Handle))))
			{
				return(-1);
			}
		if (entity2Handle==sim_handle_all)
			entity2Handle=-1;

		if (!App::ct->mainSettings->collisionDetectionEnabled)
		{
			return(0);
		}

		int buffer[4];
		App::ct->cacheData->getCacheDataColl(entity1Handle,entity2Handle,buffer);
		bool returnValue=CCollisionRoutine::doEntitiesCollide(entity1Handle,entity2Handle,NULL,buffer,true,true,false,false);
		App::ct->cacheData->setCacheDataColl(entity1Handle,entity2Handle,buffer);
		return(returnValue);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simCheckCollisionEx_internal(simInt entity1Handle,simInt entity2Handle,simFloat** intersectionSegments)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (!doesEntityExist(__func__,entity1Handle))||
			((entity2Handle!=sim_handle_all)&&(!doesEntityExist(__func__,entity2Handle))))
		{
			return(-1);
		}
		if (entity2Handle==sim_handle_all)
			entity2Handle=-1;

		if (!App::ct->mainSettings->collisionDetectionEnabled)
		{
			return(0);
		}

		int buffer[4];
		std::vector<float> intersect;
		App::ct->cacheData->getCacheDataColl(entity1Handle,entity2Handle,buffer);
		CCollisionRoutine::doEntitiesCollide(entity1Handle,entity2Handle,&intersect,buffer,true,true,false,false);
		App::ct->cacheData->setCacheDataColl(entity1Handle,entity2Handle,buffer);
		if ( (intersectionSegments!=NULL)&&(intersect.size()!=0) )
		{
			intersectionSegments[0]=new float[intersect.size()];
			for (int i=0;i<int(intersect.size());i++)
				(*intersectionSegments)[i]=intersect[i];
		}
		return(intersect.size()/6);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simCheckDistance_internal(simInt entity1Handle,simInt entity2Handle,simFloat threshold,simFloat* distanceData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (!doesEntityExist(__func__,entity1Handle))||
			((entity2Handle!=sim_handle_all)&&(!doesEntityExist(__func__,entity2Handle))) )
		{
			return(-1);
		}
		if (entity2Handle==sim_handle_all)
			entity2Handle=-1;

		if (!App::ct->mainSettings->distanceCalculationEnabled)
		{
			return(0);
		}

		int buffer[4];
		App::ct->cacheData->getCacheDataDist(entity1Handle,entity2Handle,buffer);
		if (threshold<=0.0f)
			threshold=SIM_MAX_FLOAT;
		bool result=CDistanceRoutine::getDistanceBetweenEntitiesIfSmaller(entity1Handle,entity2Handle,threshold,distanceData,buffer,true,true,false,false);
		App::ct->cacheData->setCacheDataDist(entity1Handle,entity2Handle,buffer);
		if (result)
		{
			return(1);
		}
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simGetConfigurationTree_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (objectHandle!=sim_handle_all)
		{
			if (!doesObjectExist(__func__,objectHandle))
			{
				return(NULL);
			}
		}
		if (objectHandle==sim_handle_all)
			objectHandle=-1;
		char* retVal=App::ct->confContainer->getConfigurationTree(objectHandle);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simSetConfigurationTree_internal(const simChar* data)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		App::ct->confContainer->setConfigurationTree(data);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simChar* simGetObjectConfiguration_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(NULL);
		}
		char* retVal=App::ct->confContainer->getObjectConfiguration(objectHandle);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simSetObjectConfiguration_internal(const simChar* data)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		App::ct->confContainer->setObjectConfiguration(data);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simAdvanceSimulationByOneStep_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!App::ct->simulation->isSimulationRunning())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_RUNNING);
			return(-1);
		}
		App::ct->simulation->advanceSimulationByOneStep();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetSimulationTimeStep_internal(simFloat timeStep)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!App::ct->simulation->isSimulationStopped())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_STOPPED);
			return(-1);
		}
		App::ct->simulation->setSimulationTimeStep_raw(timeStep);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simFloat simGetSimulationTimeStep_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1.0f);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		float retVal=App::ct->simulation->getSimulationTimeStep_speedModified();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1.0f);
}

simInt simGetRealTimeSimulation_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (App::ct->simulation->getRealTimeSimulation())
		{
			return(1);
		}
		else
		{
			return(0);
		}
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAdjustRealTimeTimer_internal(simInt instanceIndex,simFloat deltaTime)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		App::ct->simulation->adjustRealTimeTimer(deltaTime);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simIsRealTimeSimulationStepNeeded_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!App::ct->simulation->isSimulationRunning())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_RUNNING);
			return(-1);
		}
		if (!App::ct->simulation->getRealTimeSimulation())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_REAL_TIME);
			return(-1);
		}
		if (App::ct->simulation->isRealTimeCalculationStepNeeded())
		{
			return(1);
		}
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetSimulationPassesPerRenderingPass_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=App::ct->simulation->getSimulationPassesPerRendering_speedModified();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetSimulationPassesPerRenderingPass_internal(int p)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		p=tt::getLimitedInt(1,512,p);
		App::ct->simulation->setSimulationPassesPerRendering_raw(p);
		return(App::ct->simulation->getSimulationPassesPerRendering_raw());
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simStartSimulation_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!App::ct->simulation->isSimulationRunning())
		{
			App::ct->simulation->startOrResumeSimulation();
			return(1);
		}
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simStopSimulation_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!App::ct->simulation->isSimulationStopped())
		{
			App::ct->simulation->incrementStopRequestCounter();
			App::ct->simulation->stopSimulation();
			return(1);
		}
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simPauseSimulation_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (App::ct->simulation->isSimulationRunning())
		{
			App::ct->simulation->pauseSimulation();
			return(1);
		}
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetMechanismHandle_internal(const simChar* mechanismName)
{
	C_API_FUNCTION_DEBUG;

	std::string mechanismNameAdjusted=CIloIlo::getCNameSuffixAdjustedName(mechanismName);
	CIloIlo::enableCNameSuffixAdjustment();
	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CConstraintSolverObject* it=App::ct->constraintSolver->getObject(mechanismNameAdjusted.c_str());
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MECHANISM_INEXISTANT);
			return(-1);
		}
		int retVal=it->getObjectID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetPathPlanningHandle_internal(const simChar* pathPlanningObjectName)
{
	C_API_FUNCTION_DEBUG;

	std::string pathPlanningObjectNameAdjusted=CIloIlo::getCNameSuffixAdjustedName(pathPlanningObjectName);
	CIloIlo::enableCNameSuffixAdjustment();
	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CPathPlanningTask* it=App::ct->pathPlanning->getObject(pathPlanningObjectNameAdjusted);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PATH_PLANNING_OBJECT_INEXISTANT);
			return(-1);
		}
		int retVal=it->getObjectID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetMotionPlanningHandle_internal(const simChar* motionPlanningObjectName)
{
	C_API_FUNCTION_DEBUG;

	std::string motionPlanningObjectNameAdjusted=CIloIlo::getCNameSuffixAdjustedName(motionPlanningObjectName);
	CIloIlo::enableCNameSuffixAdjustment();
	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CMotionPlanningTask* it=App::ct->motionPlanning->getObject(motionPlanningObjectNameAdjusted);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT);
			return(-1);
		}
		int retVal=it->getObjectID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetMpConfigForTipPose_internal(simInt motionPlanningObjectHandle,simInt options,simFloat closeNodesDistance,simInt trialCount,const simFloat* tipPose,simInt maxTimeInMs,simFloat* outputJointPositions,const simFloat* referenceConfigs,simInt referenceConfigCount,const simFloat* jointWeights,const simInt* jointBehaviour,simInt correctionPasses)
{ // referenceConfigs can be NULL, as well as jointWeights
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CMotionPlanningTask* it=App::ct->motionPlanning->getObject(motionPlanningObjectHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT);
			return(-1);
		}
		// V-REP quaternion, internally: w x y z
		// V-REP quaternion, at interfaces: x y z w
		C7Vector tipTr;
		tipTr.X.set(tipPose);
		tipTr.Q(0)=tipPose[6];
		tipTr.Q(1)=tipPose[3];
		tipTr.Q(2)=tipPose[4];
		tipTr.Q(3)=tipPose[5];

		int retVal=it->getRobotConfigFromTipPose(tipTr,options,outputJointPositions,trialCount,closeNodesDistance,jointWeights,referenceConfigs,referenceConfigCount,jointBehaviour,correctionPasses,maxTimeInMs);

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simFloat* simFindMpPath_internal(simInt motionPlanningObjectHandle,const simFloat* startConfig,const simFloat* goalConfig,simInt options,simFloat stepSize,simInt* outputConfigsCnt,simInt maxTimeInMs,simFloat* reserved,const simInt* auxIntParams,const simFloat* auxFloatParams)
{ // auxIntParams and auxFloatParams can be NULL
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CMotionPlanningTask* it=App::ct->motionPlanning->getObject(motionPlanningObjectHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT);
			return(NULL);
		}

		float* retVal=it->findPath(startConfig,goalConfig,options,stepSize,outputConfigsCnt,maxTimeInMs,auxIntParams,auxFloatParams);

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simFloat* simSimplifyMpPath_internal(simInt motionPlanningObjectHandle,const simFloat* pathBuffer,simInt configCnt,simInt options,simFloat stepSize,simInt increment,simInt* outputConfigsCnt,simInt maxTimeInMs,simFloat* reserved,const simInt* auxIntParams,const simFloat* auxFloatParams)
{ // auxIntParams and auxFloatParams can be NULL
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CMotionPlanningTask* it=App::ct->motionPlanning->getObject(motionPlanningObjectHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT);
			return(NULL);
		}
		if (configCnt<2)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ARGUMENT);
			return(NULL);
		}

		float* retVal=it->simplifyPath(pathBuffer,configCnt,options,stepSize,increment,outputConfigsCnt,maxTimeInMs,auxIntParams,auxFloatParams);

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simFloat* simFindIkPath_internal(simInt motionPlanningObjectHandle,const simFloat* startConfig,const simFloat* goalPose,simInt options,simFloat stepSize,simInt* outputConfigsCnt,simFloat* reserved,const simInt* auxIntParams,const simFloat* auxFloatParams)
{ // pathLengh can be NULL, as well as auxIntParams and auxFloatParams
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CMotionPlanningTask* it=App::ct->motionPlanning->getObject(motionPlanningObjectHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT);
			return(NULL);
		}

		// V-REP quaternion, internally: w x y z
		// V-REP quaternion, at interfaces: x y z w
		C7Vector gPose;
		gPose.X.set(goalPose);
		gPose.Q(0)=goalPose[6];
		gPose.Q(1)=goalPose[3];
		gPose.Q(2)=goalPose[4];
		gPose.Q(3)=goalPose[5];


		float* retVal=it->findIkPath(startConfig,gPose,options,stepSize,outputConfigsCnt,auxIntParams,auxFloatParams);

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simFloat* simGetMpConfigTransition_internal(simInt motionPlanningObjectHandle,const simFloat* startConfig,const simFloat* goalConfig,simInt options,const simInt* select,simFloat calcStepSize,simFloat maxOutStepSize,simInt wayPointCnt,const simFloat* wayPoints,simInt* outputConfigsCnt,const simInt* auxIntParams,const simFloat* auxFloatParams)
{ // auxIntParams and auxFloatParams can be NULL
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CMotionPlanningTask* it=App::ct->motionPlanning->getObject(motionPlanningObjectHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT);
			return(NULL);
		}

		float* retVal=NULL;
		retVal=it->getConfigTransition(startConfig,goalConfig,options,select,calcStepSize,maxOutStepSize,wayPointCnt,wayPoints,outputConfigsCnt,auxIntParams,auxFloatParams);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}


simInt simHandleMechanism_internal(simInt mechanismHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (mechanismHandle!=sim_handle_all)&&(mechanismHandle!=sim_handle_all_except_explicit) )
		{
			CConstraintSolverObject* it=App::ct->constraintSolver->getObject(mechanismHandle);
			if (it==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MECHANISM_INEXISTANT);
				return(-1);
			}
		}
		int calcCnt=0;
		if (mechanismHandle<0)
			calcCnt=App::ct->constraintSolver->computeAllMechanisms(mechanismHandle==sim_handle_all_except_explicit);
		else
		{ // explicit handling
			CConstraintSolverObject* it=App::ct->constraintSolver->getObject(mechanismHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			if (it->computeGcs())
				calcCnt++;
		}
		return(calcCnt);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simResetPath_internal(simInt pathHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (pathHandle!=sim_handle_all)&&(pathHandle!=sim_handle_all_except_explicit) )
		{
			if (!isPath(__func__,pathHandle))
			{
				return(-1);
			}
		}
		if (pathHandle>=0)
		{ // Explicit handling
			CPath* it=App::ct->objCont->getPath(pathHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			it->resetPath();
		}
		else
		{
			for (int i=0;i<int(App::ct->objCont->pathList.size());i++)
			{
				CPath* p=App::ct->objCont->getPath(App::ct->objCont->pathList[i]);
				if ( (pathHandle==sim_handle_all)||(!p->getExplicitHandling()) )
					p->resetPath();
			}
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simHandlePath_internal(simInt pathHandle,simFloat deltaTime)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (pathHandle!=sim_handle_all)&&(pathHandle!=sim_handle_all_except_explicit) )
		{
			if (!isPath(__func__,pathHandle))
			{
				return(-1);
			}
		}
		if (pathHandle>=0)
		{ // explicit handling
			CPath* it=App::ct->objCont->getPath(pathHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			it->handlePath(deltaTime);
		}
		else
		{
			for (int i=0;i<int(App::ct->objCont->pathList.size());i++)
			{
				CPath* p=App::ct->objCont->getPath(App::ct->objCont->pathList[i]);
				if ( (pathHandle==sim_handle_all)||(!p->getExplicitHandling()) )
					p->handlePath(deltaTime);
			}
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simResetJoint_internal(simInt jointHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (jointHandle!=sim_handle_all)&&(jointHandle!=sim_handle_all_except_explicit) )
		{
			if (!isJoint(__func__,jointHandle))
			{
				return(-1);
			}
		}
		if (jointHandle>=0)
		{ // Explicit handling
			CJoint* it=App::ct->objCont->getJoint(jointHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			it->resetJoint();
		}
		else
		{
			for (int i=0;i<int(App::ct->objCont->jointList.size());i++)
			{
				CJoint* p=App::ct->objCont->getJoint(App::ct->objCont->jointList[i]);
				if ( (jointHandle==sim_handle_all)||(!p->getExplicitHandling()) )
					p->resetJoint();
			}
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simHandleJoint_internal(simInt jointHandle,simFloat deltaTime)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (jointHandle!=sim_handle_all)&&(jointHandle!=sim_handle_all_except_explicit) )
		{
			if (!isJoint(__func__,jointHandle))
			{
				return(-1);
			}
		}
		if (jointHandle>=0)
		{ // explicit handling
			CJoint* it=App::ct->objCont->getJoint(jointHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			it->handleJoint(deltaTime);
		}
		else
		{
			for (int i=0;i<int(App::ct->objCont->jointList.size());i++)
			{
				CJoint* p=App::ct->objCont->getJoint(App::ct->objCont->jointList[i]);
				if ( (jointHandle==sim_handle_all)||(!p->getExplicitHandling()) )
					p->handleJoint(deltaTime);
			}
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAddStatusbarMessage_internal(const simChar* message)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (App::mainWindow==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MAIN_WINDOW_NOT_INITIALIZED);
			return(-1);
		}
		App::uiThread->addStatusLine(message);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAddModuleMenuEntry_internal(const simChar* entryLabel,simInt itemCount,simInt* itemHandles)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (App::mainWindow!=NULL)
		{
			std::vector<int> commandIDs;
			if (App::mainWindow->customMenuBarItemContainer->addMenuBarItem(entryLabel,itemCount,commandIDs))
			{
				for (unsigned int i=0;i<commandIDs.size();i++)
					itemHandles[i]=commandIDs[i];
				App::mainWindow->createDefaultMenuBar(false);
				return(1);
			}
		}
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OPERATION_FAILED);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetModuleMenuItemState_internal(simInt itemHandle,simInt state,const simChar* label)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (App::mainWindow!=NULL)
		{
			if (App::mainWindow->customMenuBarItemContainer->setItemState(itemHandle,(state&2)!=0,(state&1)!=0,label))
			{
				return(1);
			}
		}
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ITEM_HANDLE);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simHandleGraph_internal(simInt graphHandle,simFloat simulationTime)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (graphHandle!=sim_handle_all)&&(graphHandle!=sim_handle_all_except_explicit) )
		{
			if (!isGraph(__func__,graphHandle))
			{
				return(-1);
			}
		}
		if (graphHandle<0)
			CGraphingRoutines::handleAllGraphs(graphHandle==sim_handle_all_except_explicit,simulationTime);
		else
		{ // explicit handling
			CGraph* it=App::ct->objCont->getGraph(graphHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			it->addNextPoint(simulationTime);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simResetGraph_internal(simInt graphHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (graphHandle!=sim_handle_all)&&(graphHandle!=sim_handle_all_except_explicit) )
		{
			if (!isGraph(__func__,graphHandle))
			{
				return(-1);
			}
		}
		if (graphHandle<0)
			CGraphingRoutines::resetAllGraphs(graphHandle==sim_handle_all_except_explicit);
		else
		{ // explicit handling
			CGraph* it=App::ct->objCont->getGraph(graphHandle);
			if (!it->getExplicitHandling())
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
				return(-1);
			}
			it->resetGraph();
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simCreateUIButtonArray_internal(simInt elementHandle,simInt buttonHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		but->enableArray(true);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simSetUIButtonArrayColor_internal(simInt elementHandle,simInt buttonHandle,const simInt* position,const simFloat* color)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		if (!but->setArrayColor(position[0],position[1],color))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OPERATION_FAILED);
			return(-1);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simDeleteUIButtonArray_internal(simInt elementHandle,simInt buttonHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		but->enableArray(false);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simSetUIButtonTexture_internal(simInt elementHandle,simInt buttonHandle,const simInt* size,const simChar* textureData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!doesUIButtonExist(__func__,elementHandle,buttonHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		CSoftButton* but=it->getButtonWithID(buttonHandle);
		CTextureProperty* tp=but->getTextureProperty();
		if (tp!=NULL)
		{ // We already have a texture. Is it the same size/type? or do we wanna remove the texture anyway?
			int tob=tp->getTextureObjectID();
			bool remove=true;
			if ((tob>=SIM_IDSTART_TEXTURE)&&(tob<=SIM_IDSTART_TEXTURE)&&(size!=NULL))
			{ // we have the correct type (i.e. non-vision sensor)
				CTextureObject* to=App::ct->textureCont->getObject(tob);
				if (to!=NULL)
				{
					int sizeX,sizeY;
					to->getTextureSize(sizeX,sizeY);
					if ( (size[0]==sizeX)&&(size[1]==sizeY) )
					{ // we just need to actualize the texture content:
						to->setImage(false,false,true,(unsigned char*)textureData);
						remove=false;
					}
				}
			}
			if (remove)
			{
				App::ct->textureCont->announceGeneralObjectWillBeErased(elementHandle,but->getUniqueID());
				delete tp;
				tp=NULL;
				but->setTextureProperty(NULL);
			}
		}
		if ((tp==NULL)&&(size!=NULL))
		{ // add an existing texture
			CTextureObject* textureObj=new CTextureObject(size[0],size[1]);
			textureObj->setImage(false,false,true,(unsigned char*)textureData); // keep false,true
			textureObj->setObjectName("textureSetThroughAPI");
			textureObj->addDependentObject(it->getBlockID(),but->getUniqueID()); // Unique ID starts exceptionnally at 1
			int textureID=App::ct->textureCont->addObject(textureObj,false); // might erase the textureObj and return a similar object already present!!
			tp=new CTextureProperty(textureID);
			but->setTextureProperty(tp);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simChar* simGetModuleName_internal(simInt index,simUChar* moduleVersion)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CPlugin* plug=CPluginContainer::getPluginFromIndex(index);
		if (plug==NULL)
		{
			return(NULL);
		}
		char* name=new char[plug->name.length()+1];
		for (int i=0;i<int(plug->name.length());i++)
			name[i]=plug->name[i];
		name[plug->name.length()]=0;
		if (moduleVersion!=NULL)
			moduleVersion[0]=plug->pluginVersion;
		return(name);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simChar* simGetScriptSimulationParameter_internal(simInt scriptHandle,const simChar* parameterName,simInt* parameterLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if ( (it==NULL)&&(scriptHandle!=sim_handle_main_script)&&(scriptHandle!=sim_handle_all) )
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(NULL);
		}
		std::string parameterValue;
		int retVal=App::ct->luaScriptContainer->getScriptSimulationParameter_mainAndChildScriptsOnly(scriptHandle,parameterName,parameterValue);
		if (retVal!=0)
		{
			char* retVal=new char[parameterValue.length()+1];
			for (int i=0;i<int(parameterValue.length());i++)
				retVal[i]=parameterValue[i];
			retVal[parameterValue.length()]=0;
			parameterLength[0]=parameterValue.length();
			return(retVal);
		}
		return(NULL);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simSetScriptSimulationParameter_internal(simInt scriptHandle,const simChar* parameterName,const simChar* parameterValue,simInt parameterLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if ( (it==NULL)&&(scriptHandle!=sim_handle_main_script)&&(scriptHandle!=sim_handle_all) )
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(-1);
		}
		int retVal=App::ct->luaScriptContainer->setScriptSimulationParameter_mainAndChildScriptsOnly(scriptHandle,parameterName,parameterValue,parameterLength);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simDisplayDialog_internal(const simChar* titleText,const simChar* mainText,simInt dialogType,const simChar* initialText,const simFloat* titleColors,const simFloat* dialogColors,simInt* elementHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CGenericDialog* it=new CGenericDialog(titleText,mainText,dialogType,false,initialText,titleColors,dialogColors);
		it->setPauseActive(true);
		int retVal=App::ct->genericDialogContainer->addDialog(it);
		if (elementHandle!=NULL)
			elementHandle[0]=it->getDialogBlockHandle();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simGetDialogResult_internal(simInt genericDialogHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=App::ct->genericDialogContainer->getDialogResult(genericDialogHandle);
		if (retVal==-1)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_HANDLE);
			return(-1);
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simGetDialogInput_internal(simInt genericDialogHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		std::string tmp;
		if (!App::ct->genericDialogContainer->getDialogString(genericDialogHandle,tmp))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_HANDLE);
			return(NULL);
		}
		char* retVal=new char[tmp.length()+1];
		retVal[tmp.length()]=0;
		for (int i=0;i<int(tmp.length());i++)
			retVal[i]=tmp[i];
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simEndDialog_internal(simInt genericDialogHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!App::ct->genericDialogContainer->removeDialogFromID(genericDialogHandle))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_HANDLE);
			return(-1);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simSetNavigationMode_internal(simInt navigationMode)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	App::ct->setMouseMode(navigationMode);
	return(1);
}

simInt simGetNavigationMode_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	int retVal=App::ct->getMouseMode();
	return(retVal);
}

simInt simSetPage_internal(simInt index)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		App::ct->oglSurface->pageCont->setActivePage(index);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetPage_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=App::ct->oglSurface->pageCont->getActivePageIndex();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simChar* simGetScriptRawBuffer_internal(simInt scriptHandle,simInt bufferHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(NULL);
		}
		char* buffer=(char*)it->getUserData(bufferHandle);
		if (buffer==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_BUFFER_INEXISTANT);
			return(NULL);
		}
		return(buffer);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simSetScriptRawBuffer_internal(simInt scriptHandle,const simChar* buffer,simInt bufferSize)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(-1);
		}
		char* bufferCopy=new char[bufferSize];
		for (int i=0;i<bufferSize;i++)
			bufferCopy[i]=buffer[i];
		int retVal=it->setUserData(bufferCopy);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simReleaseScriptRawBuffer_internal(simInt scriptHandle,simInt bufferHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (scriptHandle==sim_handle_all)
		{
			for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
			{
				CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
				it->clearAllUserData();
			}
			return(1);
		}
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptHandle);
		if (scriptHandle==sim_handle_main_script)
			it=App::ct->luaScriptContainer->getMainScript();
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
			return(-1);
		}
		if (bufferHandle==sim_handle_all)
		{
			it->clearAllUserData();
			return(1);
		}
		char* buffer=(char*)it->getUserData(bufferHandle);
		if (buffer==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_BUFFER_INEXISTANT);
			return(-1);
		}
		it->releaseUserData(bufferHandle);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simCopyPasteSelectedObjects_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (ifEditModeActiveGenerateErrorAndReturnTrue(__func__))
		{
			return(-1);
		}
		std::vector<int> sel;
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
			sel.push_back(App::ct->objCont->getSelID(i));
		if (CIloIlo::fullModelCopyFromApi)
			CObjectEdition::addRootObjectChildrenToSelection(sel);
		App::ct->copyBuffer->memorizeBuffer();
		App::ct->copyBuffer->copyCurrentSelection(&sel,App::ct->environment->getSceneLocked());
		App::ct->objCont->deselectObjects();
		App::ct->copyBuffer->pasteBuffer(App::ct->environment->getSceneLocked());
		App::ct->objCont->removeFromSelectionAllExceptModelBase(true);
		App::ct->copyBuffer->restoreBuffer();
		App::ct->copyBuffer->clearMemorizedBuffer();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simScaleSelectedObjects_internal(simFloat scalingFactor,simBool scalePositionsToo)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (ifEditModeActiveGenerateErrorAndReturnTrue(__func__))
		{
			return(-1);
		}
		std::vector<int> sel;
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
			sel.push_back(App::ct->objCont->getSelID(i));
		CObjectEdition::scaleObjects(sel,scalingFactor,scalePositionsToo!=0);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simDeleteSelectedObjects_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (ifEditModeActiveGenerateErrorAndReturnTrue(__func__))
		{
			return(-1);
		}
		std::vector<int> sel;
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
			sel.push_back(App::ct->objCont->getSelID(i));
		CObjectEdition::addRootObjectChildrenToSelection(sel);
		std::vector<int> sel2(sel);
		for (int i=0;i<int(sel2.size());i++)
		{
			C3DObject* it=App::ct->objCont->getObject(sel2[i]);
			if (it!=NULL)
				App::ct->objCont->eraseObject(it);
		}
		App::ct->objCont->deselectObjects();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simGetObjectUniqueIdentifier_internal(simInt objectHandle,simInt* uniqueIdentifier)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (objectHandle!=sim_handle_all)&&(!doesObjectExist(__func__,objectHandle)) )
		{
			return(-1);
		}
		int p=0;
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			if ( (it->getID()==objectHandle)||(objectHandle==sim_handle_all) )
				uniqueIdentifier[p++]=it->getUniqueID();
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetNameSuffix_internal(const simChar* name)
{
	C_API_FUNCTION_DEBUG;

	int suffixNumber;
	if (name!=NULL)
		suffixNumber=tt::getNameSuffixNumber(name,true);
	else
		suffixNumber=CIloIlo::getCNameSuffixNumber();
	return(suffixNumber);
}

simInt simSendData_internal(simInt targetID,simInt dataHeader,const simChar* dataName,const simChar* data,simInt dataLength,simInt antennaHandle,simFloat actionRadius,simFloat emissionAngle1,simFloat emissionAngle2,simFloat persistence)
{
	C_API_FUNCTION_DEBUG;

	if (App::ct->simulation->getSimulationState()==sim_simulation_stopped)
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_RUNNING);
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (targetID!=0)&&(targetID!=sim_handle_all) )
		{
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(targetID);
			if (it==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_TARGET_HANDLE);
				return(-1);
			}
		}
		if (dataHeader<0)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_DATA_HEADER);
			return(-1);
		}
		if (strlen(dataName)<1)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_DATA_NAME);
			return(-1);
		}
		if (dataLength<1)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_DATA);
			return(-1);
		}
		if (antennaHandle!=sim_handle_default)
		{
			C3DObject* it=App::ct->objCont->getObject(antennaHandle);
			if (it==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ANTENNA_HANDLE);
				return(-1);
			}
		}
		actionRadius=tt::getLimitedFloat(0.0f,SIM_MAX_FLOAT,actionRadius);
		emissionAngle1=tt::getLimitedFloat(0.0f,piValue,emissionAngle1);
		emissionAngle2=tt::getLimitedFloat(0.0f,piValTimes2,emissionAngle2);
		persistence=tt::getLimitedFloat(0.0f,99999999999999.9f,persistence);
		if (persistence==0.0f)
			persistence=App::ct->simulation->getSimulationTimeStep_speedModified()*1.5f;
		std::string datN(dataName);
		App::ct->luaScriptContainer->broadcastDataContainer.broadcastData(0,targetID,dataHeader,datN,
						App::ct->simulation->getSimulationTime()+persistence,actionRadius,antennaHandle,
						emissionAngle1,emissionAngle2,data,dataLength);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simReceiveData_internal(simInt dataHeader,const simChar* dataName,simInt antennaHandle,simInt index,simInt* dataLength,simInt* senderID,simInt* dataHeaderR,simChar** dataNameR)
{
	C_API_FUNCTION_DEBUG;

	if (App::ct->simulation->getSimulationState()==sim_simulation_stopped)
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_RUNNING);
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (dataHeader<0)
			dataHeader=-1;
		if (dataName!=NULL)
		{
			if (strlen(dataName)<1)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_DATA_NAME);
				return(NULL);
			}
		}
		if (antennaHandle!=sim_handle_default)
		{
			C3DObject* it=App::ct->objCont->getObject(antennaHandle);
			if (it==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ANTENNA_HANDLE);
				return(NULL);
			}
		}
		if (index<0)
			index=-1;
		std::string datNm;
		if (dataName!=NULL)
			datNm=dataName;
		int theIndex=index;
		int theSenderID;
		int theDataHeader;
		std::string theDataName;
		char* data0=App::ct->luaScriptContainer->broadcastDataContainer.receiveData(0,App::ct->simulation->getSimulationTime(),
				dataHeader,datNm,antennaHandle,dataLength[0],theIndex,theSenderID,theDataHeader,theDataName);
		char* retData=NULL;
		if (data0!=NULL)
		{
			retData=new char[dataLength[0]];
			for (int i=0;i<dataLength[0];i++)
				retData[i]=data0[i];
			if (senderID!=NULL)
				senderID[0]=theSenderID;
			if (dataHeaderR!=NULL)
				dataHeaderR[0]=theDataHeader;
			if (dataNameR!=NULL)
			{
				dataNameR[0]=new char[theDataName.length()+1];
				for (int i=0;i<int(theDataName.length());i++)
					dataNameR[0][i]=theDataName[i];
				dataNameR[0][theDataName.length()]=0; // terminal zero
			}
		}
		return(retData);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simSetGraphUserData_internal(simInt graphHandle,const simChar* dataStreamName,simFloat data)
{
	C_API_FUNCTION_DEBUG;

	if (!App::ct->simulation->isSimulationRunning())
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_RUNNING);
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!isGraph(__func__,graphHandle))
		{
			return(-1);
		}
		CGraph* it=App::ct->objCont->getGraph(graphHandle);
		CGraphData* stream=it->getGraphData(dataStreamName);
		if (stream==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_DATA_STREAM);
			return(-1);
		}
		if (stream->getDataType()!=GRAPH_NOOBJECT_USER_DEFINED)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_DATA_STREAM_NOT_USER_DEFINED);
			return(-1);
		}
		stream->setUserData(data);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetNameSuffix_internal(simInt nameSuffixNumber)
{
	C_API_FUNCTION_DEBUG;

	if (nameSuffixNumber<-1)
		nameSuffixNumber=-1;
	CIloIlo::setCNameSuffixNumber(nameSuffixNumber);
	return(1);
}

simInt simGetUIPosition_internal(simInt elementHandle,simInt* position)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIExist(__func__,elementHandle))
		{
			return(-1);
		}

		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		VPoint p;
		it->getBlockPositionAbsolute(p);
		position[0]=p.x;
		position[1]=p.y;
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetUIPosition_internal(simInt elementHandle,const simInt* position)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesUIExist(__func__,elementHandle))
		{
			return(-1);
		}
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(elementHandle);
		it->setDesiredBlockPosition(position[0],position[1]);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAddDrawingObject_internal(simInt objectType,simFloat size,simFloat duplicateTolerance,simInt parentObjectHandle,simInt maxItemCount,const simFloat* ambient,const simFloat* diffuse,const simFloat* specular,const simFloat* emission)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (parentObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,parentObjectHandle))
			{
				return(-1);
			}
		}
		CDrawingObject* it=new CDrawingObject(objectType,size,duplicateTolerance,parentObjectHandle,maxItemCount,false);
		if (ambient!=NULL)
			it->color.setColor(ambient,AMBIENT_MODE);
		if (diffuse!=NULL)
			it->color.setColor(diffuse,DIFFUSE_MODE);
		if (specular!=NULL)
			it->color.setColor(specular,SPECULAR_MODE);
		if (emission!=NULL)
			it->color.setColor(emission,EMISSION_MODE);
		int retVal=App::ct->drawingCont->addObject(it);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simRemoveDrawingObject_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (objectHandle==sim_handle_all)
			App::ct->drawingCont->removeAllObjects(false);
		else
		{
			CDrawingObject* it=App::ct->drawingCont->getObject(objectHandle);
			if (it==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_INEXISTANT);
				return(-1);
			}
			App::ct->drawingCont->removeObject(objectHandle);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simAddDrawingObjectItem_internal(simInt objectHandle,const simFloat* itemData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{ // protected with an additional mutex in CDrawingObject
		CDrawingObject* it=App::ct->drawingCont->getObject(objectHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_INEXISTANT);
			return(-1);
		}
		it->addItem(itemData);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}
simInt simAddParticleObject_internal(simInt objectType,simFloat size,simFloat massOverVolume,const simVoid* params,simFloat lifeTime,simInt maxItemCount,const simFloat* ambient,const simFloat* diffuse,const simFloat* specular,const simFloat* emission)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!App::ct->simulation->isSimulationRunning())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_RUNNING);
			return(-1);
		}
		int retVal=CDynInterface::addParticleObject(objectType,size,massOverVolume,params,lifeTime,maxItemCount,ambient,diffuse,specular,emission);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simRemoveParticleObject_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!App::ct->simulation->isSimulationRunning())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_RUNNING);
			return(-1);
		}
		int retVal=-1;
		if (CDynInterface::removeParticleObject(objectHandle)!=0)
			retVal=1;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simAddParticleObjectItem_internal(simInt objectHandle,const simFloat* itemData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}
	if (!App::ct->simulation->isSimulationRunning())
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_RUNNING);
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		int retVal=-1;
		if (CDynInterface::addParticleObjectItem(objectHandle,itemData,App::ct->simulation->getSimulationTime())!=0)
			retVal=1;
		else
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_INEXISTANT);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simFloat simGetObjectSizeFactor_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1.0f);

	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1.0f);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);

		float retVal=it->getSizeFactor();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAnnounceSceneContentChange_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (App::ct->undoBufferContainer->announceChange())
		{
			return(-1);
		}
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simResetMilling_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (objectHandle!=sim_handle_all)
		{
			if (!doesObjectExist(__func__,objectHandle))
			{
				return(-1);
			}
			C3DObject* it=App::ct->objCont->getObject(objectHandle);
			CShape* shape=NULL;
			CVolumeObject* volume=NULL;
			if (it->getObjectType()==sim_object_shape_type)
				shape=(CShape*)it;
			if (it->getObjectType()==sim_object_volume_type)
				volume=(CVolumeObject*)it;
			if (shape!=NULL)
				shape->resetMilling();
			if (volume!=NULL)
				volume->resetMilling();
			return(1);
		}
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			CShape* shape=NULL;
			CVolumeObject* volume=NULL;
			if (it->getObjectType()==sim_object_shape_type)
				shape=(CShape*)it;
			if (it->getObjectType()==sim_object_volume_type)
				volume=(CVolumeObject*)it;
			if (shape!=NULL)
				shape->resetMilling();
			if (volume!=NULL)
				volume->resetMilling();
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simApplyMilling_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (objectHandle!=sim_handle_all)
		{
			if (!doesObjectExist(__func__,objectHandle))
			{
				return(-1);
			}
			C3DObject* it=App::ct->objCont->getObject(objectHandle);
			CShape* shape=NULL;
			CVolumeObject* volume=NULL;
			if (it->getObjectType()==sim_object_shape_type)
				shape=(CShape*)it;
			if (it->getObjectType()==sim_object_volume_type)
				volume=(CVolumeObject*)it;
			if (shape!=NULL)
			{
				if (shape->applyMilling())
				{
					App::ct->objCont->eraseObject(shape);
					return(0);
				}
			}
			if (volume!=NULL)
			{
				if (volume->applyMilling())
				{
					App::ct->objCont->eraseObject(volume);
					return(0);
				}
			}
			return(1);
		}
		std::vector<C3DObject*> toErase;
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			CShape* shape=NULL;
			CVolumeObject* volume=NULL;
			if (it->getObjectType()==sim_object_shape_type)
				shape=(CShape*)it;
			if (it->getObjectType()==sim_object_volume_type)
				volume=(CVolumeObject*)it;
			if (shape!=NULL)
			{
				if (shape->applyMilling())
					toErase.push_back(shape);
			}
			if (volume!=NULL)
			{
				if (volume->applyMilling())
					toErase.push_back(volume);
			}
		}
		// Now erase what needs to be erased:
		for (int i=0;i<int(toErase.size());i++)
			App::ct->objCont->eraseObject(toErase[i]);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simSetIntegerSignal_internal(const simChar* signalName,simInt signalValue)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		App::ct->signalContainer->setIntegerSignal(signalName,signalValue,false);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetIntegerSignal_internal(const simChar* signalName,simInt* signalValue)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=0;

		if (App::ct->signalContainer->getIntegerSignal(signalName,signalValue[0]))
			retVal=1;

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simClearIntegerSignal_internal(const simChar* signalName)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal;

		if (signalName==NULL)
			retVal=App::ct->signalContainer->clearAllIntegerSignals(false);
		else
			retVal=App::ct->signalContainer->clearIntegerSignal(signalName);

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetFloatSignal_internal(const simChar* signalName,simFloat signalValue)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		App::ct->signalContainer->setFloatSignal(signalName,signalValue,false);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetFloatSignal_internal(const simChar* signalName,simFloat* signalValue)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=0;

		if (App::ct->signalContainer->getFloatSignal(signalName,signalValue[0]))
			retVal=1;

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simClearFloatSignal_internal(const simChar* signalName)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal;

		if (signalName==NULL)
			retVal=App::ct->signalContainer->clearAllFloatSignals(false);
		else
			retVal=App::ct->signalContainer->clearFloatSignal(signalName);

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetStringSignal_internal(const simChar* signalName,const simChar* signalValue,simInt stringLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		App::ct->signalContainer->setStringSignal(signalName,std::string(signalValue,stringLength),false);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simGetStringSignal_internal(const simChar* signalName,simInt* stringLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		std::string sigVal;

		if (App::ct->signalContainer->getStringSignal(signalName,sigVal))
		{
			char* retVal=new char[sigVal.length()];
			for (unsigned int i=0;i<sigVal.length();i++)
				retVal[i]=sigVal[i];
			stringLength[0]=sigVal.length();
			return(retVal);
		}

		return(NULL); // signal does not exist
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simClearStringSignal_internal(const simChar* signalName)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal;

		if (signalName==NULL)
			retVal=App::ct->signalContainer->clearAllStringSignals(false);
		else
			retVal=App::ct->signalContainer->clearStringSignal(signalName);

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simGetSignalName_internal(simInt signalIndex,simInt signalType)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (signalType!=0)&&(signalType!=1)&&(signalType!=2) )
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ARGUMENT);
			return(NULL);
		}
		std::string sigName;
		bool res;

		if (signalType==0)
			res=App::ct->signalContainer->getIntegerSignalNameAtIndex(signalIndex,sigName);
		if (signalType==1)
			res=App::ct->signalContainer->getFloatSignalNameAtIndex(signalIndex,sigName);
		if (signalType==2)
			res=App::ct->signalContainer->getStringSignalNameAtIndex(signalIndex,sigName);

		if (res)
		{
			char* retVal=new char[sigName.length()+1];
			for (unsigned int i=0;i<sigName.length();i++)
				retVal[i]=sigName[i];
			retVal[sigName.length()]=0;
			return(retVal);
		}
		return(NULL); // signal does not exist
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simSetObjectProperty_internal(simInt objectHandle,simInt prop)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		it->setObjectProperty(prop);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetObjectProperty_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		int retVal=it->getObjectProperty();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetObjectSpecialProperty_internal(simInt objectHandle,simInt prop)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		it->setMainPropertyLocal(prop);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetObjectSpecialProperty_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		int retVal=it->getMainPropertyLocal();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetModelProperty_internal(simInt objectHandle,simInt modelProperty)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		if ((modelProperty&sim_modelproperty_not_model)!=0)
			it->setModelBase(false);
		else
		{
			if (!it->getModelBase())
				it->setModelBase(true);
			it->setMainPropertyOverride(modelProperty);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetModelProperty_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		int retVal;
		if (it->getModelBase())
			retVal=it->getMainPropertyOverride();
		else
			retVal=sim_modelproperty_not_model;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetDataOnPath_internal(simInt pathHandle,simFloat relativeDistance,simInt dataType,simInt* intData,simFloat* floatData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,pathHandle))
		{
			return(-1);
		}
		if (!isPath(__func__,pathHandle))
		{
			return(-1);
		}
		CPath* it=App::ct->objCont->getPath(pathHandle);
		float auxChannels[4];
		int auxFlags;
		if (dataType==0)
		{
			if (it->pathContainer->getAuxDataOnBezierCurveAtNormalizedVirtualDistance(relativeDistance,auxFlags,auxChannels))
			{
				intData[0]=auxFlags;
				for (int i=0;i<4;i++)
					floatData[i]=auxChannels[i];
				return(1);
			}
			else
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PATH_EMPTY);
				return(-1);
			}
		}
		else
			return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simGetPositionOnPath_internal(simInt pathHandle,simFloat relativeDistance,simFloat* position)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,pathHandle))
		{
			return(-1);
		}
		if (!isPath(__func__,pathHandle))
		{
			return(-1);
		}
		CPath* it=App::ct->objCont->getPath(pathHandle);
		C7Vector tr;
		if (it->pathContainer->getTransformationOnBezierCurveAtNormalizedVirtualDistance(relativeDistance,tr))
		{
			tr=it->getCumulativeTransformationPart1()*tr;
			tr.X.copyTo(position);
			return(1);
		}
		else
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PATH_EMPTY);
			return(-1);
		}
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetOrientationOnPath_internal(simInt pathHandle,simFloat relativeDistance,simFloat* eulerAngles)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,pathHandle))
		{
			return(-1);
		}
		if (!isPath(__func__,pathHandle))
		{
			return(-1);
		}
		CPath* it=App::ct->objCont->getPath(pathHandle);
		C7Vector tr;
		if (it->pathContainer->getTransformationOnBezierCurveAtNormalizedVirtualDistance(relativeDistance,tr))
		{
			tr=it->getCumulativeTransformationPart1()*tr;
			C3Vector(tr.Q.getEulerAngles()).copyTo(eulerAngles);
			return(1);
		}
		else
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PATH_EMPTY);
			return(-1);
		}
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetClosestPositionOnPath_internal(simInt pathHandle,simFloat* absolutePosition,simFloat* pathPosition)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,pathHandle))
		{
			return(-1);
		}
		if (!isPath(__func__,pathHandle))
		{
			return(-1);
		}
		CPath* it=App::ct->objCont->getPath(pathHandle);
		C3Vector p(absolutePosition);
		if (it->pathContainer->getPositionOnPathClosestTo(p,*pathPosition))
		{
			float pl=it->pathContainer->getBezierVirtualPathLength();
			if (pl!=0.0f)
				*pathPosition/=pl;
			return(1);
		}
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PATH_EMPTY);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simReadForceSensor_internal(simInt objectHandle,simFloat* forceVector,simFloat* torqueVector)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isForceSensor(__func__,objectHandle))
		{
			return(-1);
		}
		if (App::ct->simulation->isSimulationStopped())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_RUNNING);
			return(-1);
		}
		CForceSensor* it=App::ct->objCont->getForceSensor(objectHandle);
		int retVal=0;

		C3Vector f,t;
		if (it->getDynamicForces(f))
		{
			it->getDynamicTorques(t);
			if (forceVector!=NULL)
				f.copyTo(forceVector);
			if (torqueVector!=NULL)
				t.copyTo(torqueVector);
			retVal|=1;
		}

		if (it->getForceSensorIsBroken())
			retVal|=2;

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simBreakForceSensor_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isForceSensor(__func__,objectHandle))
		{
			return(-1);
		}
		if (App::ct->simulation->isSimulationStopped())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_RUNNING);
			return(-1);
		}
		CForceSensor* it=App::ct->objCont->getForceSensor(objectHandle);
		it->setForceSensorIsBroken();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetShapeVertex_internal(simInt shapeHandle,simInt groupElementIndex,simInt vertexIndex,simFloat* relativePosition)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,shapeHandle))
		{
			return(-1);
		}
		if (!isShape(__func__,shapeHandle))
		{
			return(-1);
		}
		if (App::ct->environment->getSceneLocked())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCENE_LOCKED);
			return(-1);
		}
		CShape* it=App::ct->objCont->getShape(shapeHandle);
		CGeometric* cc=it->geomData->geomInfo->getShapeComponentAtIndex(groupElementIndex);
		if (cc==NULL)
		{
			return(0);
		}
		std::vector<float> wvert;
		cc->getCumulativeMeshes(wvert,NULL,NULL);
		if ( (vertexIndex<0)||(vertexIndex>=int(wvert.size())/3) )
		{
			return(0);
		}
		relativePosition[0]=wvert[3*vertexIndex+0];
		relativePosition[1]=wvert[3*vertexIndex+1];
		relativePosition[2]=wvert[3*vertexIndex+2];
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetShapeTriangle_internal(simInt shapeHandle,simInt groupElementIndex,simInt triangleIndex,simInt* vertexIndices,simFloat* triangleNormals)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,shapeHandle))
		{
			return(-1);
		}
		if (!isShape(__func__,shapeHandle))
		{
			return(-1);
		}
		if (App::ct->environment->getSceneLocked())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCENE_LOCKED);
			return(-1);
		}
		CShape* it=App::ct->objCont->getShape(shapeHandle);
		CGeometric* cc=it->geomData->geomInfo->getShapeComponentAtIndex(groupElementIndex);
		if (cc==NULL)
		{
			return(0);
		}
		std::vector<float> wvert;
		std::vector<int> wind;
		std::vector<float> wnorm;
		cc->getCumulativeMeshes(wvert,&wind,&wnorm);
		if ( (triangleIndex<0)||(triangleIndex>=int(wind.size())/3) )
		{
			return(0);
		}
		if (vertexIndices!=NULL)
		{
			vertexIndices[0]=wind[3*triangleIndex+0];
			vertexIndices[1]=wind[3*triangleIndex+1];
			vertexIndices[2]=wind[3*triangleIndex+2];
		}
		if (triangleNormals!=NULL)
		{
			triangleNormals[0]=wnorm[9*triangleIndex+0];
			triangleNormals[1]=wnorm[9*triangleIndex+1];
			triangleNormals[2]=wnorm[9*triangleIndex+2];
			triangleNormals[3]=wnorm[9*triangleIndex+3];
			triangleNormals[4]=wnorm[9*triangleIndex+4];
			triangleNormals[5]=wnorm[9*triangleIndex+5];
			triangleNormals[6]=wnorm[9*triangleIndex+6];
			triangleNormals[7]=wnorm[9*triangleIndex+7];
			triangleNormals[8]=wnorm[9*triangleIndex+8];
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetLightParameters_internal(simInt objectHandle,simFloat* ambientPart,simFloat* diffusePart,simFloat* specularPart)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isLight(__func__,objectHandle))
		{
			return(-1);
		}
		CLight* it=App::ct->objCont->getLight(objectHandle);
		int retVal=0;
		if (it->getLightActive())
			retVal|=1;
		for (int i=0;i<3;i++)
		{
			if (ambientPart!=NULL)
				ambientPart[0+i]=it->lightColor.colors[0+i];
			if (diffusePart!=NULL)
				diffusePart[0+i]=it->lightColor.colors[3+i];
			if (specularPart!=NULL)
				specularPart[0+i]=it->lightColor.colors[6+i];
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetLightParameters_internal(simInt objectHandle,simInt state,const simFloat* ambientPart,const simFloat* diffusePart,const simFloat* specularPart)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		if (!isLight(__func__,objectHandle))
		{
			return(-1);
		}
		CLight* it=App::ct->objCont->getLight(objectHandle);
		it->setLightActive(state&1);
		for (int i=0;i<3;i++)
		{
			if (ambientPart!=NULL)
				it->lightColor.colors[0+i]=ambientPart[0+i];
			if (diffusePart!=NULL)
				it->lightColor.colors[3+i]=diffusePart[0+i];
			if (specularPart!=NULL)
				it->lightColor.colors[6+i]=specularPart[0+i];
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simHandleVarious_internal()
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		// Following is for camera tracking!
		for (int i=0;i<int(App::ct->objCont->cameraList.size());i++)
		{
			CCamera*  it=App::ct->objCont->getCamera(App::ct->objCont->cameraList[i]);
			it->handleTrackingAndHeadAlwaysUp();
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetVelocity_internal(simInt shapeHandle,simFloat* linearVelocity,simFloat* angularVelocity)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,shapeHandle))
		{
			return(-1);
		}
		if (!isShape(__func__,shapeHandle))
		{
			return(-1);
		}
		CShape* it=App::ct->objCont->getShape(shapeHandle);
		C3Vector lv(it->getDynamicLinearVelocity());
		C3Vector av(it->getDynamicAngularVelocity());
		if (linearVelocity!=NULL)
			lv.copyTo(linearVelocity);
		if (angularVelocity!=NULL)
			av.copyTo(angularVelocity);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetObjectVelocity_internal(simInt objectHandle,simFloat* linearVelocity,simFloat* angularVelocity)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		C3Vector lv(it->getMeasuredLinearVelocity());
		C3Vector av(it->getMeasuredAngularVelocity3());
		if (linearVelocity!=NULL)
			lv.copyTo(linearVelocity);
		if (angularVelocity!=NULL)
			av.copyTo(angularVelocity);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAddForceAndTorque_internal(simInt shapeHandle,const simFloat* force,const simFloat* torque)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,shapeHandle))
		{
			return(-1);
		}
		if (!isShape(__func__,shapeHandle))
		{
			return(-1);
		}
		CShape* it=App::ct->objCont->getShape(shapeHandle);
		C3Vector f;
		C3Vector t;
		f.clear();
		t.clear();
		if (force!=NULL)
			f.set(force);
		if (torque!=NULL)
			t.set(torque);
		it->addAdditionalForceAndTorque(f,t);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAddForce_internal(simInt shapeHandle,const simFloat* position,const simFloat* force)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,shapeHandle))
		{
			return(-1);
		}
		if (!isShape(__func__,shapeHandle))
		{
			return(-1);
		}
		CShape* it=App::ct->objCont->getShape(shapeHandle);
		C3Vector r(position);
		C3Vector f(force);
		C3Vector t(r^f);
		// f & t are relative to the shape's frame now. We have to make them absolute:
		C4Vector q(it->getCumulativeTransformationPart1().Q);
		f=q*f;
		t=q*t;
		it->addAdditionalForceAndTorque(f,t);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simSetExplicitHandling_internal(simInt generalObjectHandle,int explicitFlags)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (generalObjectHandle>=SIM_IDSTART_3DOBJECT)&&(generalObjectHandle<SIM_IDEND_3DOBJECT) )
		{ // scene objects
			if (!doesObjectExist(__func__,generalObjectHandle))
			{
				return(-1);
			}
			C3DObject* it=App::ct->objCont->getObject(generalObjectHandle);
			if (it->getObjectType()==sim_object_joint_type)
			{ // Joints
				((CJoint*)it)->setExplicitHandling(explicitFlags&1);
				return(1);
			}
			if (it->getObjectType()==sim_object_graph_type)
			{ // Graphs
				((CGraph*)it)->setExplicitHandling(explicitFlags&1);
				return(1);
			}
			if (it->getObjectType()==sim_object_mill_type)
			{ // Mills
				((CMill*)it)->setExplicitHandling(explicitFlags&1);
				return(1);
			}
			if (it->getObjectType()==sim_object_path_type)
			{ // Paths
				((CPath*)it)->setExplicitHandling(explicitFlags&1);
				return(1);
			}
			if (it->getObjectType()==sim_object_visionsensor_type)
			{ // vision sensors
				((CVisionSensor*)it)->setExplicitHandling(explicitFlags&1);
				return(1);
			}
			if (it->getObjectType()==sim_object_proximitysensor_type)
			{ // Proximity sensors
				((CProxSensor*)it)->setExplicitHandling(explicitFlags&1);
				return(1);
			}
		}
		if ( (generalObjectHandle>=SIM_IDSTART_COLLISION)&&(generalObjectHandle<SIM_IDEND_COLLISION) )
		{ // collision objects
			if (!doesCollisionObjectExist(__func__,generalObjectHandle))
			{
				return(-1);
			}
			CRegColl* it=App::ct->collisions->getObject(generalObjectHandle);
			it->setExplicitHandling(explicitFlags&1);
			return(1);
		}
		if ( (generalObjectHandle>=SIM_IDSTART_DISTANCE)&&(generalObjectHandle<SIM_IDEND_DISTANCE) )
		{ // distance objects
			if (!doesDistanceObjectExist(__func__,generalObjectHandle))
			{
				return(-1);
			}
			CRegDist* it=App::ct->distances->getObject(generalObjectHandle);
			it->setExplicitHandling(explicitFlags&1);
			return(1);
		}
		if ( (generalObjectHandle>=SIM_IDSTART_IKGROUP)&&(generalObjectHandle<SIM_IDEND_IKGROUP) )
		{ // IK objects
			if (!doesIKGroupExist(__func__,generalObjectHandle))
			{
				return(-1);
			}
			CikGroup* it=App::ct->ikGroups->getIkGroup(generalObjectHandle);
			it->setExplicitHandling(explicitFlags&1);
			return(1);
		}
		if ( (generalObjectHandle>=SIM_IDSTART_GCSOBJECT)&&(generalObjectHandle<SIM_IDEND_GCSOBJECT) )
		{ // GCS objects
			CConstraintSolverObject* it=App::ct->constraintSolver->getObject(generalObjectHandle);
			if (it==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MECHANISM_INEXISTANT);
				return(-1);
			}
			it->setExplicitHandling(explicitFlags&1);
			return(1);
		}
		if ( (generalObjectHandle>=SIM_IDSTART_COLLECTION)&&(generalObjectHandle<SIM_IDEND_COLLECTION) )
		{ // collection objects

		}
		if ( (generalObjectHandle>=SIM_IDSTART_2DELEMENT)&&(generalObjectHandle<SIM_IDEND_2DELEMENT) )
		{ // UI objects

		}
		if ( (generalObjectHandle>=SIM_IDSTART_LUASCRIPT)&&(generalObjectHandle<SIM_IDEND_LUASCRIPT) )
		{ // Script objects
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(generalObjectHandle);
			if (it==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
				return(-1);
			}
			it->setExplicitHandling(explicitFlags&1);
			return(1);
		}
		if ( (generalObjectHandle>=SIM_IDSTART_PATHPLANNINGTASK)&&(generalObjectHandle<SIM_IDEND_PATHPLANNINGTASK) )
		{ // Path planning objects

		}
		if ( (generalObjectHandle>=SIM_IDSTART_MOTIONPLANNINGTASK)&&(generalObjectHandle<SIM_IDEND_MOTIONPLANNINGTASK) )
		{ // Motion planning objects

		}
		if ( (generalObjectHandle>=SIM_IDSTART_TEXTURE)&&(generalObjectHandle<SIM_IDEND_TEXTURE) )
		{ // Texture objects

		}
		if ( (generalObjectHandle>=SIM_IDSTART_DYNMATERIAL)&&(generalObjectHandle<SIM_IDEND_DYNMATERIAL) )
		{ // Dyn material objects

		}
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ARGUMENT);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetExplicitHandling_internal(simInt generalObjectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (generalObjectHandle>=SIM_IDSTART_3DOBJECT)&&(generalObjectHandle<SIM_IDEND_3DOBJECT) )
		{ // scene objects
			if (!doesObjectExist(__func__,generalObjectHandle))
			{
				return(-1);
			}
			C3DObject* it=App::ct->objCont->getObject(generalObjectHandle);
			if (it->getObjectType()==sim_object_joint_type)
			{ // Joints
				bool exp=((CJoint*)it)->getExplicitHandling();
				return(exp);
			}
			if (it->getObjectType()==sim_object_graph_type)
			{ // Graphs
				bool exp=((CGraph*)it)->getExplicitHandling();
				return(exp);
			}
			if (it->getObjectType()==sim_object_mill_type)
			{ // Mills
				bool exp=((CMill*)it)->getExplicitHandling();
				return(exp);
			}
			if (it->getObjectType()==sim_object_path_type)
			{ // Paths
				bool exp=((CPath*)it)->getExplicitHandling();
				return(exp);
			}
			if (it->getObjectType()==sim_object_visionsensor_type)
			{ // vision sensors
				bool exp=((CVisionSensor*)it)->getExplicitHandling();
				return(exp);
			}
			if (it->getObjectType()==sim_object_proximitysensor_type)
			{ // Proximity sensors
				bool exp=((CProxSensor*)it)->getExplicitHandling();
				return(exp);
			}
		}
		if ( (generalObjectHandle>=SIM_IDSTART_COLLISION)&&(generalObjectHandle<SIM_IDEND_COLLISION) )
		{ // collision objects
			if (!doesCollisionObjectExist(__func__,generalObjectHandle))
			{
				return(-1);
			}
			CRegColl* it=App::ct->collisions->getObject(generalObjectHandle);
			bool exp=it->getExplicitHandling();
			return(exp);
		}
		if ( (generalObjectHandle>=SIM_IDSTART_DISTANCE)&&(generalObjectHandle<SIM_IDEND_DISTANCE) )
		{ // distance objects
			if (!doesDistanceObjectExist(__func__,generalObjectHandle))
			{
				return(-1);
			}
			CRegDist* it=App::ct->distances->getObject(generalObjectHandle);
			bool exp=it->getExplicitHandling();
			return(exp);
		}
		if ( (generalObjectHandle>=SIM_IDSTART_IKGROUP)&&(generalObjectHandle<SIM_IDEND_IKGROUP) )
		{ // IK objects
			if (!doesIKGroupExist(__func__,generalObjectHandle))
			{
				return(-1);
			}
			CikGroup* it=App::ct->ikGroups->getIkGroup(generalObjectHandle);
			bool exp=it->getExplicitHandling();
			return(exp);
		}
		if ( (generalObjectHandle>=SIM_IDSTART_GCSOBJECT)&&(generalObjectHandle<SIM_IDEND_GCSOBJECT) )
		{ // GCS objects
			CConstraintSolverObject* it=App::ct->constraintSolver->getObject(generalObjectHandle);
			if (it==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MECHANISM_INEXISTANT);
				return(-1);
			}
			bool exp=it->getExplicitHandling();
			return(exp);
		}
		if ( (generalObjectHandle>=SIM_IDSTART_COLLECTION)&&(generalObjectHandle<SIM_IDEND_COLLECTION) )
		{ // collection objects

		}
		if ( (generalObjectHandle>=SIM_IDSTART_2DELEMENT)&&(generalObjectHandle<SIM_IDEND_2DELEMENT) )
		{ // UI objects

		}
		if ( (generalObjectHandle>=SIM_IDSTART_LUASCRIPT)&&(generalObjectHandle<SIM_IDEND_LUASCRIPT) )
		{ // Script objects
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(generalObjectHandle);
			if (it==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCRIPT_INEXISTANT);
				return(-1);
			}
			bool exp=it->getExplicitHandling();
			return(exp);
		}
		if ( (generalObjectHandle>=SIM_IDSTART_PATHPLANNINGTASK)&&(generalObjectHandle<SIM_IDEND_PATHPLANNINGTASK) )
		{ // Path planning objects

		}
		if ( (generalObjectHandle>=SIM_IDSTART_MOTIONPLANNINGTASK)&&(generalObjectHandle<SIM_IDEND_MOTIONPLANNINGTASK) )
		{ // Motion planning objects

		}
		if ( (generalObjectHandle>=SIM_IDSTART_TEXTURE)&&(generalObjectHandle<SIM_IDEND_TEXTURE) )
		{ // Texture objects

		}
		if ( (generalObjectHandle>=SIM_IDSTART_DYNMATERIAL)&&(generalObjectHandle<SIM_IDEND_DYNMATERIAL) )
		{ // Dyn material objects

		}
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ARGUMENT);
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetLinkDummy_internal(simInt dummyHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!isDummy(__func__,dummyHandle))
		{
			return(-1);
		}
		CDummy* it=App::ct->objCont->getDummy(dummyHandle);
		int retVal=it->getLinkedDummyID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetLinkDummy_internal(simInt dummyHandle,simInt linkedDummyHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!isDummy(__func__,dummyHandle))
		{
			return(-1);
		}
		if (linkedDummyHandle!=-1)
		{
			if (!isDummy(__func__,linkedDummyHandle))
			{
				return(-1);
			}
		}
		CDummy* it=App::ct->objCont->getDummy(dummyHandle);
		it->setLinkedDummyID(linkedDummyHandle,false);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetShapeColor_internal(simInt shapeHandle,const simChar* colorName,simInt colorComponent,const simFloat* rgbData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (shapeHandle!=sim_handle_all)&&(!isShape(__func__,shapeHandle)) )
		{
			return(-1);
		}
		for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
		{
			CShape* it=App::ct->objCont->getShape(App::ct->objCont->shapeList[i]);
			if ( (it->getID()==shapeHandle)||(shapeHandle==sim_handle_all) )
				it->setColor(colorName,colorComponent,rgbData);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetShapeColor_internal(simInt shapeHandle,const simChar* colorName,simInt colorComponent,simFloat* rgbData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!isShape(__func__,shapeHandle))
		{
			return(-1);
		}
		CShape* it=App::ct->objCont->getShape(shapeHandle);
		int retVal=0;
		if (it->getColor(colorName,colorComponent,rgbData))
			retVal=1;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simResetDynamicObject_internal(simInt objectHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (objectHandle!=sim_handle_all)&&(!doesObjectExist(__func__,objectHandle)) )
		{
			return(-1);
		}
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			if ( (it->getID()==objectHandle)||(objectHandle==sim_handle_all) )
			{
				it->setDynamicsFullRefreshFlag(true);
			}
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetJointMode_internal(simInt jointHandle,simInt jointMode,simInt options)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!isJoint(__func__,jointHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(jointHandle);
		it->setJointMode(jointMode);
		it->setHybridFunctionality(options&1);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetJointMode_internal(simInt jointHandle,simInt* options)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!isJoint(__func__,jointHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(jointHandle);
		int retVal=it->getJointMode();
		options[0]=0;
		if (it->getHybridFunctionality())
			options[0]|=1;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSerialOpen_internal(const simChar* portString,simInt baudRate,simVoid* reserved1,simVoid* reserved2)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CSerialPort* port=new CSerialPort(false);
		if (port->open(portString,baudRate)==0)
		{
			delete port;
			return(-1);
		}
		int handle=App::ct->serialPortContainer->addOpenedPort(port);
		return(handle);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSerialClose_internal(simInt portHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CSerialPort* port=App::ct->serialPortContainer->getPortFromHandle(portHandle);
		if (port==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PORT_HANDLE);
			return(-1);
		}
		App::ct->serialPortContainer->closePortFromHandle(port->getPortHandle());
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSerialSend_internal(simInt portHandle,const simChar* data,simInt dataLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}
	CSerialPort* port=App::ct->serialPortContainer->getPortFromHandle(portHandle);
	if (port==NULL)
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PORT_HANDLE);
		return(-1);
	}
	int retVal=port->sendData(data,dataLength);
	return(retVal);
}

simInt simSerialRead_internal(simInt portHandle,simChar* buffer,simInt dataLengthToRead)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}
	CSerialPort* port=App::ct->serialPortContainer->getPortFromHandle(portHandle);
	if (port==NULL)
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PORT_HANDLE);
		return(-1);
	}
	int retVal=0;
	int availableData=port->readDataWaiting();
	if (availableData!=0)
		retVal=port->readData(buffer,SIM_MIN(availableData,dataLengthToRead));
	return(retVal);
}

simInt simSerialCheck_internal(simInt portHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}
	CSerialPort* port=App::ct->serialPortContainer->getPortFromHandle(portHandle);
	if (port==NULL)
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_PORT_HANDLE);
		return(-1);
	}
	int retVal=port->readDataWaiting();
	return(retVal);
}


simInt simSerialPortOpen_internal(simInt portNumber,simInt baudRate,simVoid* reserved1,simVoid* reserved2)
{ // deprecated (10/04/2012)
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}
	CSerialPort* port=new CSerialPort(false);
	std::string theString("\\\\.\\COM");
	theString+=tt::FNb(portNumber);
	if (port->open(theString.c_str(),baudRate)==0)
	{
		return(-1);
	}
	port->setPortNumber_backwardCompatibility(portNumber);
	App::ct->serialPortContainer->addOpenedPort(port);
	return(1);
}

simInt simSerialPortClose_internal(simInt portNumber)
{ // deprecated (10/04/2012)
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}
	CSerialPort* port=App::ct->serialPortContainer->getPortFromPortNumber_backwardCompatibility(portNumber);
	if (port==NULL)
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PORT_NOT_OPEN);
		return(-1);
	}
	App::ct->serialPortContainer->closePortFromHandle(port->getPortHandle());
	return(1);
}

simInt simSerialPortSend_internal(simInt portNumber,const simChar* data,simInt dataLength)
{ // deprecated (10/04/2012)
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}
	CSerialPort* port=App::ct->serialPortContainer->getPortFromPortNumber_backwardCompatibility(portNumber);
	if (port==NULL)
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PORT_NOT_OPEN);
		return(-1);
	}
	int retVal=port->sendData(data,dataLength);
	return(retVal);
}

simInt simSerialPortRead_internal(simInt portNumber,simChar* buffer,simInt dataLengthToRead)
{ // deprecated (10/04/2012)
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}
	CSerialPort* port=App::ct->serialPortContainer->getPortFromPortNumber_backwardCompatibility(portNumber);
	if (port==NULL)
	{
		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PORT_NOT_OPEN);
		return(-1);
	}
	int retVal=0;
	int availableData=port->readDataWaiting();
	if (availableData!=0)
		retVal=port->readData(buffer,SIM_MIN(availableData,dataLengthToRead));
	return(retVal);
}

simInt simGetContactInfo_internal(simInt dynamicPass,simInt objectHandle,simInt index,simInt* objectHandles,simFloat* contactInfo)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=0;
		if (App::ct->dynamicsContainer->getContactForce(dynamicPass,objectHandle,index,objectHandles,contactInfo)!=0)
			retVal=1;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetThreadIsFree_internal(simBool freeMode)
{
	C_API_FUNCTION_DEBUG;

	if (VThread::isCurrentThreadTheMainThread())
	{

		CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_CANNOT_BE_CALLED_FROM_MAIN_THREAD);
		return(-1);
	}
	if (CThreadPool::setThreadFreeMode(freeMode!=0))
		return(1);
	return(0);
}


simInt simTubeOpen_internal(simInt dataHeader,const simChar* dataName,simInt readBufferSize,simBool notUsedButKeepFalse)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal;
		retVal=App::ct->commTubeContainer->openTube(dataHeader,dataName,false,readBufferSize);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simTubeClose_internal(simInt tubeHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=0;
		if (tubeHandle&1)
		{ // not used anymore
		}
		else
		{
			if (App::ct->commTubeContainer->closeTube(tubeHandle))
				retVal=1;
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simTubeWrite_internal(simInt tubeHandle,const simChar* data,simInt dataLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=0;
		if (tubeHandle&1)
		{ // not used anymore
		}
		else
		{
			if (App::ct->commTubeContainer->writeToTube_copyBuffer(tubeHandle,data,dataLength))
				retVal=1;
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simTubeRead_internal(simInt tubeHandle,simInt* dataLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		char* retVal;
		retVal=App::ct->commTubeContainer->readFromTube_bufferNotCopied(tubeHandle,dataLength[0]);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simTubeStatus_internal(simInt tubeHandle,simInt* readPacketsCount,simInt* writePacketsCount)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int readP=0;
		int writeP=0;
		int retVal;
		retVal=App::ct->commTubeContainer->getTubeStatus(tubeHandle,readP,writeP);
		if (readPacketsCount!=NULL)
			readPacketsCount[0]=readP;
		if (writePacketsCount!=NULL)
			writePacketsCount[0]=writeP;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAuxiliaryConsoleOpen_internal(const simChar* title,simInt maxLines,simInt mode,const simInt* position,const simInt* size,const simFloat* textColor,const simFloat* backgroundColor)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CConsoleInitInfo* info=new CConsoleInitInfo(mode,title,maxLines,position,size,textColor,backgroundColor,App::ct->getCurrentInstanceIndex());
		int retVal=App::ct->scintillaConsoleContainer->addConsoleInfo(info);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simAuxiliaryConsoleClose_internal(simInt consoleHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (App::ct->scintillaConsoleContainer->removeConsole(consoleHandle))
		{
			return(1);
		}
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simAuxiliaryConsoleShow_internal(simInt consoleHandle,simBool showState)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (App::ct->scintillaConsoleContainer->consoleSetShowState(consoleHandle,showState!=0))
		{
			return(1);
		}
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simAuxiliaryConsolePrint_internal(simInt consoleHandle,const simChar* text)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CScintillaConsoleDlg* it=App::ct->scintillaConsoleContainer->getConsoleFromHandle(consoleHandle);
		if (it==NULL)
		{
			CConsoleInitInfo* it=App::ct->scintillaConsoleContainer->getConsoleInfoFromHandle(consoleHandle);
			if (it==NULL)
				return(0); // Kind of silent error
			else
				it->addText(text);
		}
		else
			it->info->addText(text);

		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simImportShape_internal(simInt fileformat,const simChar* pathAndFilename,simInt options,simFloat identicalVerticeTolerance,simFloat scalingFactor)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		int importType=-1;
		if (fileformat==0)
			importType=FILE_FORMAT_OBJ;
		if (fileformat==1)
			importType=FILE_FORMAT_DXF;
		if (fileformat==2)
			importType=FILE_FORMAT_3DS;
		if (fileformat==3)
			importType=FILE_FORMAT_ASCII_STL;
		if (fileformat==4)
			importType=FILE_FORMAT_BINARY_STL;
		if (fileformat==5)
			importType=FILE_FORMAT_COLLADA;
		if (importType==-1)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_FILE_FORMAT);
			return(-1);
		}
		if (!VFile::doesFileExist(pathAndFilename))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_FILE_NOT_FOUND);
			return(-1);
		}
		if (importType==FILE_FORMAT_COLLADA)
		{ // collada file is handled by a plugin!
			CPlugin* plugin=CPluginContainer::getPluginFromName("Collada");
			bool error=true;
			int handle=-1;
			if (plugin!=NULL)
			{
				int auxVals[4]={1,(options&8)==0,int(scalingFactor*1000.0f),0};
				int retVals[4];
				plugin->sendEventCallbackMessage(sim_message_eventcallback_colladaplugin,auxVals,(void*)pathAndFilename,retVals);
				handle=retVals[0];
				error=false;
			}
			if (error||(handle<0))
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OPERATION_FAILED);
				return(-1);
			}
			return(handle); // success!
		}
		else
		{
			if ((options&16)!=0)
			{
				App::ct->objCont->deselectObjects();
				if (!CFileMenu::commonImportRoutine(importType,pathAndFilename,false,scalingFactor,options))
				{
					CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OPERATION_FAILED);
					return(-1);
				}
				// We have to group the shapes:
				std::vector<int> newShapeIds;
				for (int i=0;i<App::ct->objCont->getSelSize();i++)
					newShapeIds.push_back(App::ct->objCont->getSelID(i));
				int handle=simGroupShapes_internal(&newShapeIds[0],int(newShapeIds.size()));
				return(handle);
			}
			else
			{
				std::vector<std::vector<float>*> theVertices;
				std::vector<std::vector<int>*> theIndices;
				std::vector<std::string> theNames;
				if (!CFileMenu::apiImportRoutine(importType,pathAndFilename,options,identicalVerticeTolerance,scalingFactor,theVertices,theIndices,theNames))
				{
					CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OPERATION_FAILED);
					return(-1);
				}
				int elementCount=theVertices.size();

				std::vector<int> allShapes;
				for (int i=0;i<elementCount;i++)
				{
					if ((theVertices[i]->size()>=9)&&(theIndices[i]->size()>=3))
					{
						int handle=simCreateMeshShape_internal(2,20.0f*piValue/180.0f,&theVertices[i]->at(0),int(theVertices[i]->size()),&theIndices[i]->at(0),int(theIndices[i]->size()),NULL);
						allShapes.push_back(handle);
					}
				}

				if (allShapes.size()==0)
				{
					CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OPERATION_FAILED);
					return(-1);
				}
				if (allShapes.size()==1)
				{
					return(allShapes[0]);
				}
				// we have to merge the shapes:
				int handle=simGroupShapes_internal(&allShapes[0],-int(allShapes.size()));
				return(handle);
			}
		}
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}


simInt simImportMesh_internal(simInt fileformat,const simChar* pathAndFilename,simInt options,simFloat identicalVerticeTolerance,simFloat scalingFactor,simFloat*** vertices,simInt** verticesSizes,simInt*** indices,simInt** indicesSizes,simFloat*** reserved,simChar*** names)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		int importType=-1;
		if (fileformat==0)
			importType=FILE_FORMAT_OBJ;
		if (fileformat==1)
			importType=FILE_FORMAT_DXF;
		if (fileformat==2)
			importType=FILE_FORMAT_3DS;
		if (fileformat==3)
			importType=FILE_FORMAT_ASCII_STL;
		if (fileformat==4)
			importType=FILE_FORMAT_BINARY_STL;
		if (importType==-1)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_FILE_FORMAT);
			return(-1);
		}
		if (!VFile::doesFileExist(pathAndFilename))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_FILE_NOT_FOUND);
			return(-1);
		}
		std::vector<std::vector<float>*> theVertices;
		std::vector<std::vector<int>*> theIndices;
		std::vector<std::string> theNames;
		if (!CFileMenu::apiImportRoutine(importType,pathAndFilename,options,identicalVerticeTolerance,scalingFactor,theVertices,theIndices,theNames))
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OPERATION_FAILED);
			return(-1);
		}
		int elementCount=theVertices.size();

		vertices[0]=new float*[elementCount];
		verticesSizes[0]=new int[elementCount];
		if (indices!=NULL)
		{
			indices[0]=new int*[elementCount];
			indicesSizes[0]=new int[elementCount];
		}
		if (names!=NULL)
			names[0]=new char*[elementCount];
		for (int i=0;i<elementCount;i++)
		{
			vertices[0][i]=new float[theVertices[i]->size()];
			verticesSizes[0][i]=theVertices[i]->size();
			for (int j=0;j<int(theVertices[i]->size());j++)
				vertices[0][i][j]=theVertices[i]->at(j);
			delete theVertices[i];

			if (indices!=NULL)
			{
				indices[0][i]=new int[theIndices[i]->size()];
				indicesSizes[0][i]=theIndices[i]->size();
				for (int j=0;j<int(theIndices[i]->size());j++)
					indices[0][i][j]=theIndices[i]->at(j);
			}
			delete theIndices[i];

			if (names!=NULL)
			{
				names[0][i]=new char[theNames[i].length()+1];
				for (int j=0;j<int(theNames[i].length());j++)
					names[0][i][j]=theNames[i][j];
				names[0][i][theNames[i].length()]=0;
			}
		}
		return(elementCount);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simExportMesh_internal(simInt fileformat,const simChar* pathAndFilename,simInt options,simFloat scalingFactor,simInt elementCount,simFloat** vertices,const simInt* verticesSizes,simInt** indices,const simInt* indicesSizes,simFloat** reserved,simChar** names)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (App::ct->environment->getSceneLocked())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SCENE_LOCKED);
			return(-1);
		}
		int exportType=-1;
		if (fileformat==0)
			exportType=FILE_FORMAT_OBJ;
		if (fileformat==1)
			exportType=FILE_FORMAT_DXF;
		//if (fileformat==2) // not yet supported
		//	exportType=FILE_FORMAT_3DS;
		//if (fileformat==3) // not yet supported
		//	exportType=FILE_FORMAT_ASCII_STL;
		if (fileformat==4)
			exportType=FILE_FORMAT_BINARY_STL;
		if (exportType==-1)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_FILE_FORMAT);
			return(-1);
		}
		bool invalidValues=false;
		if (elementCount<1)
			invalidValues=true;
		if (!invalidValues)
		{
			for (int i=0;i<elementCount;i++)
			{
				if ( (verticesSizes[i]<9)||((verticesSizes[i]/3)*3!=verticesSizes[i]) )
					invalidValues=true;
				if ( (indicesSizes[i]<3)||((indicesSizes[i]/3)*3!=indicesSizes[i]) )
					invalidValues=true;
			}
		}
		if (invalidValues)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_DATA);
			return(-1);
		}
		std::vector<std::vector<float>*> theVertices;
		std::vector<std::vector<int>*> theIndices;
		std::vector<std::string> theNames;

		for (int i=0;i<elementCount;i++)
		{
			std::vector<float>* vert=new std::vector<float>;
			std::vector<int>* ind=new std::vector<int>;
			for (int j=0;j<verticesSizes[i];j++)
				vert->push_back(vertices[i][j]);
			for (int j=0;j<indicesSizes[i];j++)
				ind->push_back(indices[i][j]);
			theVertices.push_back(vert);
			theIndices.push_back(ind);
			theNames.push_back(names[i]);
		}

		CImportExport* exporter=NULL;
		//if (exportType==FILE_FORMAT_3DS) // not yet supported
		//	exporter=new L3DS();
		if (exportType==FILE_FORMAT_DXF)
			exporter=new CDxfFile();
		if (exportType==FILE_FORMAT_OBJ)
			exporter=new CObjFile();
		//if (exportType==FILE_FORMAT_ASCII_STL) // not yet supported
		//	exporter=new CStlFile(FILE_FORMAT_ASCII_STL);
		if (exportType==FILE_FORMAT_BINARY_STL)
			exporter=new CStlFile(FILE_FORMAT_BINARY_STL);
		exporter->exportFunc(pathAndFilename,true,theVertices,theIndices,theNames);
		delete exporter;

		for (int i=0;i<elementCount;i++)
		{
			delete theVertices[i];
			delete theIndices[i];
		}

		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simCreateMeshShape_internal(simInt options,simFloat shadingAngle,const simFloat* vertices,simInt verticesSize,const simInt* indices,simInt indicesSize,simFloat* reserved)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		std::vector<float> vert(vertices,vertices+verticesSize);
		std::vector<int> ind(indices,indices+indicesSize);
		CGeomProxy* geom=new CGeomProxy(NULL,vert,ind,NULL,NULL);
		CShape* shape=new CShape();
		shape->setLocalTransformation(geom->getCreationTransformation());
		geom->setCreationTransformation(C7Vector::identityTransformation);
		((CGeometric*)geom->geomInfo)->setGouraudShadingAngle(shadingAngle);
		((CGeometric*)geom->geomInfo)->setEdgeThresholdAngle(shadingAngle);
		geom->geomInfo->setLocalInertiaFrame(C7Vector::identityTransformation);
		shape->geomData=geom;
		shape->setCulling((options&1)!=0);
		shape->setVisibleEdges((options&2)!=0);
		App::ct->objCont->addObjectToScene(shape,false);
		return(shape->getID());
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simGetShapeMesh_internal(simInt shapeHandle,simFloat** vertices,simInt* verticesSize,simInt** indices,simInt* indicesSize,simFloat** normals)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!isShape(__func__,shapeHandle))
		{
			return(-1);
		}
		CShape* it=App::ct->objCont->getShape(shapeHandle);
		std::vector<float> wvert;
		std::vector<int> wind;
		std::vector<float> wnorm;
		it->geomData->geomInfo->getCumulativeMeshes(wvert,&wind,&wnorm);
		vertices[0]=new float[wvert.size()];
		verticesSize[0]=wvert.size();
		indices[0]=new int[wind.size()];
		indicesSize[0]=wind.size();
		if (normals!=NULL)
			normals[0]=new float[wnorm.size()];
		for (int i=0;i<int(wvert.size());i++)
			vertices[0][i]=wvert[i];
		for (int i=0;i<int(wind.size());i++)
			indices[0][i]=wind[i];
		if (normals!=NULL)
		{
			for (int i=0;i<int(wnorm.size());i++)
				normals[0][i]=wnorm[i];
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simCreatePureShape_internal(simInt primitiveType,simInt options,const simFloat* sizes,simFloat mass,const simInt* precision)
{ // options: bit: 0=culling, 1=edges, 2=smooth, 3=respondable, 4=static, 5=open
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		int pType=-1;
		int command;
		bool cone=false;
		if (primitiveType==0) // cuboid
		{
			command=ADD_PRIMITIVE_RECTANGLE_CMD;
			pType=1;
		}
		if (primitiveType==1) // sphere
		{
			command=ADD_PRIMITIVE_SPHERE_CMD;
			pType=2;
		}
		if (primitiveType==2) // cylinder
		{
			command=ADD_PRIMITIVE_CYLINDER_CMD;
			pType=3;
		}
		if (primitiveType==3) // cone
		{
			command=ADD_PRIMITIVE_CYLINDER_CMD;
			cone=true;
			pType=3;
		}
		if (pType==-1)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_TYPE);
			return(-1);
		}
		C3Vector s(tt::getLimitedFloat(0.00001f,100000.0f,sizes[0]),tt::getLimitedFloat(0.00001f,100000.0f,sizes[1]),tt::getLimitedFloat(0.00001f,100000.0f,sizes[2]));
		CQDlgPrimitives theDialog;
		theDialog.initialize(pType,&s);
		theDialog.smooth=(options&4)!=0;
		theDialog.open=(options&32)!=0;
		theDialog.cone=cone;

		if (precision!=NULL)
		{
			theDialog.faces=tt::getLimitedInt(3,100,precision[0]);
			if (pType==2)
				theDialog.sides=tt::getLimitedInt(3,50,precision[1]); // sphere
			else
				theDialog.sides=tt::getLimitedInt(0,50,precision[1]);
		}
		CShape* shape=CAddMenu::addPrimitiveWithoutDialog(command,theDialog);

		C7Vector identity;
		identity.setIdentity();
		shape->setLocalTransformation(identity);
		shape->setCulling((options&1)!=0);
		shape->setVisibleEdges((options&2)!=0);
		shape->setRespondable((options&8)!=0);
		shape->setShapeIsDynamicallyStatic((options&16)!=0);
		shape->geomData->geomInfo->setMass(tt::getLimitedFloat(0.000001f,10000.0f,mass));
		return(shape->getID());
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simAddBanner_internal(const simChar* label,simFloat size,simInt options,const simFloat* positionAndEulerAngles,simInt parentObjectHandle,const simFloat* labelColors,const simFloat* backgroundColors)
{ // from positionAndEulerAngles on, all can be NULL for default behaviour
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		int retVal=-1; // means error

		CBannerObject* it=new CBannerObject(label,options,parentObjectHandle,positionAndEulerAngles,labelColors,backgroundColors,size);
		retVal=App::ct->bannerCont->addObject(it);

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simRemoveBanner_internal(simInt bannerID)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (bannerID==sim_handle_all)
			App::ct->bannerCont->removeAllObjects(false);
		else
		{
			CBannerObject* it=App::ct->bannerCont->getObject(bannerID);
			if (it==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_INEXISTANT);
				return(-1);
			}
			App::ct->bannerCont->removeObject(bannerID);
		}

		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simCreateDummy_internal(simFloat size,const simFloat* color)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CDummy* it=new CDummy();
		it->setSize(size);
		if (color!=NULL)
		{
			it->color.setColor(color+0,AMBIENT_MODE);
			it->color.setColor(color+3,DIFFUSE_MODE);
			it->color.setColor(color+6,SPECULAR_MODE);
			it->color.setColor(color+9,EMISSION_MODE);
		}
		App::ct->objCont->addObjectToScene(it,false);
		int retVal=it->getID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simCreatePath_internal(simInt attributes,const simInt* intParams,const simFloat* floatParams,const simFloat* color)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CPath* newObject=new CPath();
		App::ct->objCont->addObjectToScene(newObject,false);
		if (attributes!=-1)
			newObject->pathContainer->setAttributes(attributes);

		if (intParams!=NULL)
		{
			newObject->pathContainer->setLineSize(intParams[0]);
			newObject->pathContainer->setPathLengthCalculationMethod(intParams[1]);
		}

		if (floatParams!=NULL)
		{
			newObject->pathContainer->setSquareSize(floatParams[0]);
			newObject->pathContainer->setAngleVarToDistanceCoeff(floatParams[1]);
			newObject->pathContainer->setOnSpotDistanceToDistanceCoeff(floatParams[2]);
		}

		if (color!=NULL)
		{
			newObject->pathContainer->_lineColor.setColor(color+0,AMBIENT_MODE);
			newObject->pathContainer->_lineColor.setColor(color+3,DIFFUSE_MODE);
			newObject->pathContainer->_lineColor.setColor(color+6,SPECULAR_MODE);
			newObject->pathContainer->_lineColor.setColor(color+9,EMISSION_MODE);
		}

		int retVal=newObject->getID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simInsertPathCtrlPoints_internal(simInt pathHandle,simInt options,simInt startIndex,simInt ptCnt,const simVoid* ptData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CPath* path=App::ct->objCont->getPath(pathHandle);
		if (path==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PATH_INEXISTANT);
			return(-1);
		}

		path->pathContainer->enableActualization(false);
		for (int i=0;i<ptCnt;i++)
		{
			CSimplePathPoint* pt=new CSimplePathPoint();
			C7Vector tr(C4Vector(((float*)ptData)[11*i+3],((float*)ptData)[11*i+4],((float*)ptData)[11*i+5]),C3Vector(((float*)ptData)+11*i+0));
			pt->setTransformation(tr,path->pathContainer->getAttributes());
			pt->setMaxRelAbsVelocity(((float*)ptData)[11*i+6]);
			pt->setOnSpotDistance(((float*)ptData)[11*i+7]);
			pt->setBezierPointCount(((int*)ptData)[11*i+8]);
			pt->setBezierFactors(((float*)ptData)[11*i+9],((float*)ptData)[11*i+10]);
			path->pathContainer->insertSimplePathPoint(pt,startIndex+i);
		}
		if (options&1)
			path->pathContainer->setAttributes(path->pathContainer->getAttributes()|sim_pathproperty_closed_path);
		else
			path->pathContainer->setAttributes((path->pathContainer->getAttributes()|sim_pathproperty_closed_path)-sim_pathproperty_closed_path);
		path->pathContainer->enableActualization(true);
		path->pathContainer->actualizePath();
		App::ct->objCont->setFullDialogRefreshFlag();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simCutPathCtrlPoints_internal(simInt pathHandle,simInt startIndex,simInt ptCnt)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CPath* path=App::ct->objCont->getPath(pathHandle);
		if (path==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PATH_INEXISTANT);
			return(-1);
		}
		if ((startIndex<0)||(ptCnt<0))
			path->pathContainer->removeAllSimplePathPoints();
		else
		{
			path->pathContainer->enableActualization(false);
			for (int i=0;i<ptCnt;i++)
				path->pathContainer->removeSimplePathPoint(startIndex);
			path->pathContainer->enableActualization(true);
			path->pathContainer->actualizePath();
		}
		App::ct->objCont->setFullDialogRefreshFlag();
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simFloat* simGetIkGroupMatrix_internal(simInt ikGroupHandle,simInt options,simInt* matrixSize)
{ 
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(ikGroupHandle);
		if (it==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_IK_GROUP_INEXISTANT);
			return(NULL);
		}
		float* retData=NULL;
		if (options==0)
			retData=it->getLastJacobianData(matrixSize);
		if (options==1)
			retData=it->getLastManipulabilityValue(matrixSize);
		return(retData);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}


simInt simCreateProximitySensor_internal(simInt sensorType,simInt subType,simInt options,const simInt* intParams,const simFloat* floatParams,const simFloat* color)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CProxSensor* it=new CProxSensor(sensorType);
		it->setSensableType(subType);
		it->setExplicitHandling((options&1)!=0);
		it->setShowVolumeWhenDetecting((options&2)==0);
		it->setShowVolumeWhenNotDetecting((options&4)==0);
		it->setFrontFaceDetection((options&8)==0);
		it->setBackFaceDetection((options&16)==0);
		it->setClosestObjectMode((options&32)==0);
		it->setNormalCheck((options&64)!=0);
		it->setCheckOcclusions((options&128)!=0);
		it->convexVolume->setSmallestDistanceEnabled((options&256)!=0);
		it->setRandomizedDetection((sensorType==sim_proximitysensor_ray_subtype)&&(options&512)!=0);

		if ( (sensorType==sim_proximitysensor_cylinder_subtype)||(sensorType==sim_proximitysensor_disc_subtype)||(sensorType==sim_proximitysensor_cone_subtype) )
			it->convexVolume->setFaceNumber(intParams[0]);
		if (sensorType==sim_proximitysensor_disc_subtype)
			it->convexVolume->setFaceNumberFar(intParams[1]);
		if (sensorType==sim_proximitysensor_cone_subtype)
		{
			it->convexVolume->setSubdivisions(intParams[2]);
			it->convexVolume->setSubdivisionsFar(intParams[3]);
		}

		if (it->getRandomizedDetection())
		{
			it->setRandomizedDetectionSampleCount(intParams[4]);
			it->setRandomizedDetectionCountForDetection(intParams[5]);
		}


		it->convexVolume->setOffset(floatParams[0]);
		it->convexVolume->setRange(floatParams[1]);
		if (sensorType==sim_proximitysensor_pyramid_subtype)
			it->convexVolume->setXSize(floatParams[2]);
		if ( (sensorType==sim_proximitysensor_pyramid_subtype)||(sensorType==sim_proximitysensor_disc_subtype) )
			it->convexVolume->setYSize(floatParams[3]);
		if (sensorType==sim_proximitysensor_pyramid_subtype)
		{
			it->convexVolume->setXSizeFar(floatParams[4]);
			it->convexVolume->setYSizeFar(floatParams[5]);
		}
		if ( (sensorType==sim_proximitysensor_cone_subtype)||(sensorType==sim_proximitysensor_disc_subtype) )
			it->convexVolume->setInsideAngleThing(floatParams[6]);

		if ( (sensorType==sim_proximitysensor_cylinder_subtype)||(sensorType==sim_proximitysensor_cone_subtype)||(sensorType==sim_proximitysensor_disc_subtype) )
			it->convexVolume->setRadius(floatParams[7]);
		if (sensorType==sim_proximitysensor_cylinder_subtype)
			it->convexVolume->setRadiusFar(floatParams[8]);
		if ( ((sensorType==sim_proximitysensor_ray_subtype)&&it->getRandomizedDetection())||(sensorType==sim_proximitysensor_cone_subtype)||(sensorType==sim_proximitysensor_disc_subtype) )
			it->convexVolume->setAngle(floatParams[9]);

		it->setAllowedNormal(floatParams[10]);
		it->convexVolume->setSmallestDistanceAllowed(floatParams[11]);
		it->setSize(floatParams[12]);

		if (color!=NULL)
		{
			it->passiveVolumeColor.setColor(color+0,AMBIENT_MODE);
			it->passiveVolumeColor.setColor(color+3,DIFFUSE_MODE);
			it->passiveVolumeColor.setColor(color+6,SPECULAR_MODE);
			it->passiveVolumeColor.setColor(color+9,EMISSION_MODE);

			it->activeVolumeColor.setColor(color+12,AMBIENT_MODE);
			it->activeVolumeColor.setColor(color+15,DIFFUSE_MODE);
			it->activeVolumeColor.setColor(color+18,SPECULAR_MODE);
			it->activeVolumeColor.setColor(color+21,EMISSION_MODE);

			it->detectionRayColor.setColor(color+24,AMBIENT_MODE);
			it->detectionRayColor.setColor(color+27,DIFFUSE_MODE);
			it->detectionRayColor.setColor(color+30,SPECULAR_MODE);
			it->detectionRayColor.setColor(color+33,EMISSION_MODE);

			it->closestDistanceVolumeColor.setColor(color+36,AMBIENT_MODE);
			it->closestDistanceVolumeColor.setColor(color+39,DIFFUSE_MODE);
			it->closestDistanceVolumeColor.setColor(color+42,SPECULAR_MODE);
			it->closestDistanceVolumeColor.setColor(color+45,EMISSION_MODE);
		}


		App::ct->objCont->addObjectToScene(it,false);
		int retVal=it->getID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simCreateForceSensor_internal(simInt options,const simInt* intParams,const simFloat* floatParams,const simFloat* color)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CForceSensor* it=new CForceSensor();

		it->setEnableForceThreshold((options&1)!=0);
		it->setEnableTorqueThreshold((options&2)!=0);

		it->setFilterType(intParams[0]);
		it->setValueCountForFilter(intParams[1]);
		it->setConsecutiveThresholdViolationsForBreaking(intParams[2]);

		it->setSize(floatParams[0]);
		it->setForceThreshold(floatParams[1]);
		it->setTorqueThreshold(floatParams[2]);

		if (color!=NULL)
		{
			it->colorPart1.setColor(color+0,AMBIENT_MODE);
			it->colorPart1.setColor(color+3,DIFFUSE_MODE);
			it->colorPart1.setColor(color+6,SPECULAR_MODE);
			it->colorPart1.setColor(color+9,EMISSION_MODE);

			it->colorPart2.setColor(color+12,AMBIENT_MODE);
			it->colorPart2.setColor(color+15,DIFFUSE_MODE);
			it->colorPart2.setColor(color+18,SPECULAR_MODE);
			it->colorPart2.setColor(color+21,EMISSION_MODE);
		}

		App::ct->objCont->addObjectToScene(it,false);
		int retVal=it->getID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simCreateVisionSensor_internal(simInt options,const simInt* intParams,const simFloat* floatParams,const simFloat* color)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CVisionSensor* it=new CVisionSensor();

		it->setExplicitHandling((options&1)!=0);
		it->setPerspectiveOperation((options&2)!=0);
		it->setShowVolumeWhenNotDetecting((options&4)==0);
		it->setShowVolumeWhenDetecting((options&8)==0);
		it->setUseExternalImage((options&16)!=0);
		it->setUseLocalLights((options&32)!=0);
		it->setShowFogIfAvailable((options&64)==0);
		it->setUseEnvironmentBackgroundColor((options&128)==0);

		int res[2]={intParams[0],intParams[1]};
		it->setDesiredResolution(res);

		it->setNearClippingPlane(floatParams[0]);
		it->setFarClippingPlane(floatParams[1]);
		if (it->getPerspectiveOperation())
			it->setViewAngle(floatParams[2]);
		else
			it->setOrthoViewSize(floatParams[2]);
		it->setSize(C3Vector(floatParams+3));
		it->setDefaultBufferValues(floatParams+6);

		if (color!=NULL)
		{
			it->bodyColor.setColor(color+0,AMBIENT_MODE);
			it->bodyColor.setColor(color+3,DIFFUSE_MODE);
			it->bodyColor.setColor(color+6,SPECULAR_MODE);
			it->bodyColor.setColor(color+9,EMISSION_MODE);

			it->bodyActiveColor.setColor(color+12,AMBIENT_MODE);
			it->bodyActiveColor.setColor(color+15,DIFFUSE_MODE);
			it->bodyActiveColor.setColor(color+18,SPECULAR_MODE);
			it->bodyActiveColor.setColor(color+21,EMISSION_MODE);

			it->detectionVolumeColor.setColor(color+24,AMBIENT_MODE);
			it->detectionVolumeColor.setColor(color+27,DIFFUSE_MODE);
			it->detectionVolumeColor.setColor(color+30,SPECULAR_MODE);
			it->detectionVolumeColor.setColor(color+33,EMISSION_MODE);

			it->detectionVolumeActiveColor.setColor(color+36,AMBIENT_MODE);
			it->detectionVolumeActiveColor.setColor(color+39,DIFFUSE_MODE);
			it->detectionVolumeActiveColor.setColor(color+42,SPECULAR_MODE);
			it->detectionVolumeActiveColor.setColor(color+45,EMISSION_MODE);
		}

		App::ct->objCont->addObjectToScene(it,false);
		int retVal=it->getID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}


simInt simCreateJoint_internal(simInt jointType,simInt jointMode,simInt options,const simFloat* sizes,const simFloat* colorA,const simFloat* colorB)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CJoint* it=new CJoint(jointType);
		it->setJointMode(jointMode);
		it->setHybridFunctionality(options&1);
		if (sizes!=NULL)
		{
			it->setLength(sizes[0]);
			it->setDiameter(sizes[1]);
		}
		if (colorA!=NULL)
		{
			it->colorPart1.setColor(colorA+0,AMBIENT_MODE);
			it->colorPart1.setColor(colorA+3,DIFFUSE_MODE);
			it->colorPart1.setColor(colorA+6,SPECULAR_MODE);
			it->colorPart1.setColor(colorA+9,EMISSION_MODE);
		}
		if (colorB!=NULL)
		{
			it->colorPart2.setColor(colorB+0,AMBIENT_MODE);
			it->colorPart2.setColor(colorB+3,DIFFUSE_MODE);
			it->colorPart2.setColor(colorB+6,SPECULAR_MODE);
			it->colorPart2.setColor(colorB+9,EMISSION_MODE);
		}
		App::ct->objCont->addObjectToScene(it,false);
		int retVal=it->getID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simFloatingViewAdd_internal(simFloat posX,simFloat posY,simFloat sizeX,simFloat sizeY,simInt options)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CSPage* page=App::ct->oglSurface->pageCont->getPage(App::ct->oglSurface->pageCont->getActivePageIndex());
		if (page==NULL)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PAGE_INEXISTANT);
			return(-1);
		}
		CSView* theFloatingView=new CSView(-1);
		posX=1.0f-posX;
		posY=1.0f-posY;
		if (posX<0.01f)
			posX=0.01f;
		if (posX>0.99f)
			posX=0.99f;
		if (posY<0.01f)
			posY=0.01f;
		if (posY>0.99f)
			posY=0.99f;
		sizeX=SIM_MIN(sizeX,2.0f*SIM_MIN(posX,1.0f-posX));
		sizeY=SIM_MIN(sizeY,2.0f*SIM_MIN(posY,1.0f-posY));
		float sizes[2]={sizeX,sizeY};
		float positions[2]={posX-sizeX*0.5f,posY-sizeY*0.5f};
		page->addFloatingView(theFloatingView,sizes,positions);
		theFloatingView->setCanSwapViewWithMainView(false);
		if (options&1)
			theFloatingView->setCanSwapViewWithMainView(true);
		if (options&2)
			theFloatingView->setCanBeClosed(false);
		if (options&4)
			theFloatingView->setCanBeShifted(false);
		if (options&8)
			theFloatingView->setCanBeResized(false);
		int retVal=theFloatingView->getUniqueID();
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simFloatingViewRemove_internal(simInt floatingViewHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		for (int i=0;i<8;i++)
		{
			CSPage* page=App::ct->oglSurface->pageCont->getPage(i);
			if (page!=NULL)
			{
				int viewIndex=page->getViewIndexFromViewUniqueID(floatingViewHandle);
				if (viewIndex!=-1)
				{
					if (viewIndex>=page->getRegularViewCount())
					{
						page->removeFloatingView(viewIndex);
						return(1);
					}
					break; // We can't remove the view because it is not floating (anymore?)
				}
			}
		}
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simCameraFitToView_internal(simInt viewHandleOrIndex,simInt objectCount,const simInt* objectHandles,simInt options,simFloat scaling)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CSView* view=NULL;
		if (viewHandleOrIndex>=10000)
		{
			for (int i=0;i<8;i++)
			{
				CSPage* page=App::ct->oglSurface->pageCont->getPage(i);
				int index=page->getViewIndexFromViewUniqueID(viewHandleOrIndex);
				if (index!=-1)
				{
					view=page->getView(index);
					break;
				}
			}
		}
		else
		{
			CSPage* page=App::ct->oglSurface->pageCont->getPage(App::ct->oglSurface->pageCont->getActivePageIndex());
			if (page==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PAGE_INEXISTANT);
				return(-1);
			}
			else
				view=page->getView(viewHandleOrIndex);
		}
		if (view==NULL)
		{ // silent error
			return(0);
		}
		CCamera* camera=App::ct->objCont->getCamera(view->getLinkedObjectID());
		if (camera==NULL)
		{ // silent error
			return(0);
		}
		std::vector<int> objectsToFrame;
		if ( (objectCount!=0)&&(objectHandles!=NULL) )
		{
			for (int i=0;i<objectCount;i++)
				objectsToFrame.push_back(objectHandles[i]);
		}
		int vs[2];
		view->getViewSize(vs);
		camera->frameSceneOrSelectedObjects(float(vs[0])/float(vs[1]),view->getPerspectiveDisplay(),&objectsToFrame,false,(options&1)==0,scaling);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simAdjustView_internal(simInt viewHandleOrIndex,simInt associatedViewableObjectHandle,simInt options,const simChar* viewLabel)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CSView* view=NULL;
		if (viewHandleOrIndex>=10000)
		{
			for (int i=0;i<8;i++)
			{
				CSPage* page=App::ct->oglSurface->pageCont->getPage(i);
				int index=page->getViewIndexFromViewUniqueID(viewHandleOrIndex);
				if (index!=-1)
				{
					view=page->getView(index);
					break;
				}
			}
		}
		else
		{
			CSPage* page=App::ct->oglSurface->pageCont->getPage(App::ct->oglSurface->pageCont->getActivePageIndex());
			if (page==NULL)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_PAGE_INEXISTANT);
				return(-1);
			}
			else
				view=page->getView(viewHandleOrIndex);
		}
		if (view==NULL)
		{
			return(0);
		}
		if (associatedViewableObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,associatedViewableObjectHandle))
			{
				return(-1);
			}
			C3DObject* it=App::ct->objCont->getObject(associatedViewableObjectHandle);
			int objType=it->getObjectType();
			if ( (objType!=sim_object_camera_type)&&(objType!=sim_object_graph_type)&&(objType!=sim_object_visionsensor_type) )
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_IS_NOT_VIEWABLE);
				return(-1);
			}
		}
		view->setLinkedObjectID(associatedViewableObjectHandle,true);
		view->setRenderingMode(options&0x000f);
		view->setPerspectiveDisplay((options&0x0010)==0);
		view->setTimeGraph((options&0x0020)==0);
		view->setRemoveFloatingViewAtSimulationEnd((options&0x0040)!=0);
		if (viewLabel!=NULL)
			view->setAlternativeViewName(viewLabel);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simRegisterContactCallback_internal(simInt(*callBack)(simInt,simInt,simInt,simInt*,simFloat*))
{
	C_API_FUNCTION_DEBUG;

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		for (int i=0;i<int(CIloIlo::allContactCallbacks.size());i++)
		{
			if (CIloIlo::allContactCallbacks[i]==callBack)
			{ // We unregister that callback
				CIloIlo::allContactCallbacks.erase(CIloIlo::allContactCallbacks.begin()+i);
				return(0);
			}
		}
		// We register that callback:
		CIloIlo::allContactCallbacks.push_back(callBack);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simCreateHeightfieldShape_internal(simInt options,simFloat shadingAngle,simInt xPointCount,simInt yPointCount,simFloat xSize,const simFloat* heights)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if ( (xPointCount<2)||(xPointCount>2048)||(yPointCount<2)||(yPointCount>2048)||(xSize<0.00001f) )
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ARGUMENTS);
			return(-1);
		}
		std::vector<std::vector<float>*> allData;
		for (int i=0;i<yPointCount;i++)
		{
			std::vector<float>* vect=new std::vector<float>;
			for (int j=0;j<xPointCount;j++)
				vect->push_back(heights[i*xPointCount+j]);
			allData.push_back(vect);
		}
		int retVal=CFileMenu::apiAddHeightfieldToScene(xPointCount,xSize/(xPointCount-1),allData,shadingAngle,options);
		for (int i=0;i<int(allData.size());i++)
			delete allData[i];
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simGetObjectIntParameter_internal(simInt objectHandle,simInt parameterID,simInt* parameter)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=0; // Means the parameter was not retrieved
		CVisionSensor* rendSens=App::ct->objCont->getVisionSensor(objectHandle);
		CCamera* camera=App::ct->objCont->getCamera(objectHandle);
		CJoint* joint=App::ct->objCont->getJoint(objectHandle);
		CShape* shape=App::ct->objCont->getShape(objectHandle);
		CMirror* mirror=App::ct->objCont->getMirror(objectHandle);
		CMotionPlanningTask* motionPlanningObject=App::ct->motionPlanning->getObject(objectHandle);
		if (parameterID<1000)
		{ // for all scene objects
			C3DObject* it=App::ct->objCont->getObject(objectHandle);
			if (it!=NULL)
			{
				if (parameterID==10)
				{
					parameter[0]=it->layer;
					retVal=1;
				}

				if (parameterID==27)
				{
					parameter[0]=it->getCollectionSelfCollisionIndicator();
					retVal=1;
				}
			}
		}
		if (rendSens!=NULL)
		{
			if (parameterID==1002)
			{
				int r[2];
				rendSens->getRealResolution(r);
				parameter[0]=r[0];
				retVal=1;
			}
			if (parameterID==1003)
			{
				int r[2];
				rendSens->getRealResolution(r);
				parameter[0]=r[1];
				retVal=1;
			}
			if (parameterID==1006)
			{
				parameter[0]=rendSens->getDisabledColorComponents();
				retVal=1;
			}
		}
		if (camera!=NULL)
		{
			if (parameterID==9000)
			{
				parameter[0]=camera->getDisabledColorComponents();
				retVal=1;
			}
		}
		if (joint!=NULL)
		{
			if (parameterID==2000)
			{
				parameter[0]=0;
				if (joint->getEnableDynamicMotor())
					parameter[0]=1;
				retVal=1;
			}
			if (parameterID==2001)
			{
				parameter[0]=0;
				if (joint->getEnableDynamicMotorControlLoop())
					parameter[0]=1;
				retVal=1;
			}
			if (parameterID==2020)
			{
				parameter[0]=joint->getJointCallbackCallOrder();
				retVal=1;
			}
		}
		if (shape!=NULL)
		{
			if (parameterID==3003)
			{
				parameter[0]=0;
				if (shape->getShapeIsDynamicallyStatic())
					parameter[0]=1;
				retVal=1;
			}
			if (parameterID==3004)
			{
				parameter[0]=0;
				if (shape->getRespondable())
					parameter[0]=1;
				retVal=1;
			}
			if (parameterID==3014)
			{
				parameter[0]=0;
				if (shape->getCulling())
					parameter[0]=1;
				retVal=1;
			}
			if (parameterID==3015)
			{
				parameter[0]=0;
				if (shape->getShapeWireframe())
					parameter[0]=1;
				retVal=1;
			}
			if (parameterID==3016)
			{
				parameter[0]=0;
				if (!shape->geomData->geomInfo->isGeometric())
					parameter[0]=1;
				retVal=1;
			}
			if (parameterID==3017)
			{
				parameter[0]=0;
				if (shape->geomData->geomInfo->isConvex())
					parameter[0]=1;
				retVal=1;
			}
			if (parameterID==3019)
			{
				parameter[0]=int(shape->getDynamicCollisionMask());
				retVal=1;
			}
		}
		if (mirror!=NULL)
		{
			if (parameterID==12003)
			{
				parameter[0]=mirror->getActive();
				retVal=1;
			}
		}
		if (motionPlanningObject!=NULL)
		{
			if (parameterID==25000)
			{
				if (motionPlanningObject->getPhase1NodeAreReady())
					parameter[0]=1;
				else
					parameter[0]=0;
				retVal=1;
			}
		}

		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetObjectIntParameter_internal(simInt objectHandle,simInt parameterID,simInt parameter)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=0; // Means the parameter was not set
		CVisionSensor* rendSens=App::ct->objCont->getVisionSensor(objectHandle);
		CJoint* joint=App::ct->objCont->getJoint(objectHandle);
		CShape* shape=App::ct->objCont->getShape(objectHandle);
		CMirror* mirror=App::ct->objCont->getMirror(objectHandle);
		CCamera* camera=App::ct->objCont->getCamera(objectHandle);
		CMotionPlanningTask* motionPlanningObject=App::ct->motionPlanning->getObject(objectHandle);
		if (parameterID<1000)
		{ // for all scene objects
			C3DObject* it=App::ct->objCont->getObject(objectHandle);
			if (it!=NULL)
			{
				if (parameterID==10)
				{
					it->layer=tt::getLimitedInt(0,65535,parameter);
					retVal=1;
				}
				if (parameterID==27)
				{
					it->setCollectionSelfCollisionIndicator(parameter);
					retVal=1;
				}
			}
		}
		if (rendSens!=NULL)
		{
			if ((parameterID==1002)||(parameterID==1003))
			{
				int r[2];
				rendSens->getRealResolution(r);
				if (parameterID==1002)
					r[0]=parameter;
				else
					r[1]=parameter;
				rendSens->setDesiredResolution(r);
				retVal=1;
			}
			if (parameterID==1006)
			{
				rendSens->setDisabledColorComponents(parameter);
			}
		}
		if (camera!=NULL)
		{
			if (parameterID==9000)
			{
				camera->setDisabledColorComponents(parameter);
			}
		}
		if (joint!=NULL)
		{
			if (parameterID==2000)
			{
				joint->setEnableDynamicMotor(parameter!=0);
				retVal=1;
			}
			if (parameterID==2001)
			{
				joint->setEnableDynamicMotorControlLoop(parameter!=0);
				retVal=1;
			}
			if (parameterID==2020)
			{
				joint->setJointCallbackCallOrder(parameter);
				retVal=1;
			}
		}
		if (shape!=NULL)
		{
			if (parameterID==3003)
			{
				shape->setShapeIsDynamicallyStatic(parameter!=0);
				retVal=1;
			}
			if (parameterID==3004)
			{
				shape->setRespondable(parameter!=0);
				retVal=1;
			}
			if (parameterID==3014)
			{
				shape->setCulling(parameter!=0);
				retVal=1;
			}
			if (parameterID==3015)
			{
				shape->setShapeWireframe(parameter!=0);
				retVal=1;
			}
			if (parameterID==3017)
			{ // very careful when setting this!
				shape->geomData->geomInfo->setConvex(parameter!=0);
				retVal=1;
			}
			if (parameterID==3018)
			{
				shape->geomData->geomInfo->checkIfConvex();
				retVal=1;
			}
			if (parameterID==3019)
			{
				shape->setDynamicCollisionMask(WORD(parameter));
				retVal=1;
			}
		}
		if (mirror!=NULL)
		{
			if (parameterID==12003)
			{
				mirror->setActive(parameter!=0);
				retVal=1;
			}
		}
		if (motionPlanningObject!=NULL)
		{
			if (parameterID==25001)
			{
				motionPlanningObject->calculateDataStructureIfNeeded(NULL,NULL,parameter!=0);
				retVal=1;
			}
			if (parameterID==25002)
			{
				motionPlanningObject->clearPhase2NodeVisualizationAndPathVisualization();
				retVal=1;
			}
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetObjectFloatParameter_internal(simInt objectHandle,simInt parameterID,simFloat* parameter)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=0; // Means the parameter was not retrieved
		CVisionSensor* rendSens=App::ct->objCont->getVisionSensor(objectHandle);
		CJoint* joint=App::ct->objCont->getJoint(objectHandle);
		CShape* shape=App::ct->objCont->getShape(objectHandle);
		CMirror* mirror=App::ct->objCont->getMirror(objectHandle);
		CPathPlanningTask* pathPlanningObject=App::ct->pathPlanning->getObject(objectHandle);
		if (parameterID<1000)
		{ // for all scene objects
			C3DObject* it=App::ct->objCont->getObject(objectHandle);
			if (it!=NULL)
			{
				if ((parameterID>=11)&&(parameterID<=13))
				{
					parameter[0]=it->getMeasuredLinearVelocity()(parameterID-11);
					retVal=1;
				}
				if (parameterID==14)
				{
					parameter[0]=it->getMeasuredAngularVelocity();
					retVal=1;
				}
				if ((parameterID>=15)&&(parameterID<=20))
				{
					C3Vector minV,maxV;
					it->getMarkingBoundingBox(minV,maxV);
					if (parameterID<=17)
						parameter[0]=minV(parameterID-15);
					else
						parameter[0]=maxV(parameterID-18);
					retVal=1;
				}
				if ((parameterID>=21)&&(parameterID<=26))
				{
					C3Vector minV,maxV;
					C7Vector ctmi(it->getCumulativeTransformationPart1().getInverse());
					bool b=true;
					if (!it->getGlobalMarkingBoundingBox(ctmi,minV,maxV,b,false))
						retVal=0;
					else
					if (parameterID<=23)
						parameter[0]=minV(parameterID-21);
					else
						parameter[0]=maxV(parameterID-24);
					retVal=1;
				}
			}
		}
		if (rendSens!=NULL)
		{
			if (parameterID==1000)
			{
				parameter[0]=rendSens->getNearClippingPlane();
				retVal=1;
			}
			if (parameterID==1001)
			{
				parameter[0]=rendSens->getFarClippingPlane();
				retVal=1;
			}
			if (parameterID==1004)
			{
				parameter[0]=rendSens->getViewAngle();
				retVal=1;
			}
			if (parameterID==1005)
			{
				parameter[0]=rendSens->getOrthoViewSize();
				retVal=1;
			}
		}
		if (joint!=NULL)
		{
			if ((parameterID==2002)||(parameterID==2003)||(parameterID==2004))
			{
				float pp,ip,dp;
				joint->getDynamicMotorPositionControlParameters(pp,ip,dp);
				if (parameterID==2002)
					parameter[0]=pp;
				if (parameterID==2003)
					parameter[0]=ip;
				if (parameterID==2004)
					parameter[0]=dp;
				retVal=1;
			}
			if ((parameterID==2018)||(parameterID==2019))
			{
				float kp,cp;
				joint->getDynamicMotorSpringControlParameters(kp,cp);
				if (parameterID==2018)
					parameter[0]=kp;
				if (parameterID==2019)
					parameter[0]=cp;
				retVal=1;
			}
			if ((parameterID>=2005)&&(parameterID<=2011))
			{
				C7Vector trFull(joint->getLocalTransformation());
				C7Vector trPart1(joint->getLocalTransformationPart1());
				C7Vector tr(trPart1.getInverse()*trFull);
				if (parameterID==2005)
					parameter[0]=tr.X(0);
				if (parameterID==2006)
					parameter[0]=tr.X(1);
				if (parameterID==2007)
					parameter[0]=tr.X(2);
				if (parameterID==2008)
					parameter[0]=tr.Q(1);
				if (parameterID==2009)
					parameter[0]=tr.Q(2);
				if (parameterID==2010)
					parameter[0]=tr.Q(3);
				if (parameterID==2011)
					parameter[0]=tr.Q(0);
				retVal=1;
			}
			if (parameterID==2012)
			{
				parameter[0]=joint->getMeasuredJointVelocity();
				retVal=1;
			}
			if (parameterID==2017)
			{
				parameter[0]=joint->getDynamicMotorUpperLimitVelocity();
				retVal=1;
			}
		}
		if (shape!=NULL)
		{
			if ((parameterID==3000)||(parameterID==3001)||(parameterID==3002))
			{
				C3Vector v(shape->getInitialDynamicVelocity());
				if (parameterID==3000)
					parameter[0]=v(0);
				if (parameterID==3001)
					parameter[0]=v(1);
				if (parameterID==3002)
					parameter[0]=v(2);
				retVal=1;
			}
			if ((parameterID==3020)||(parameterID==3021)||(parameterID==3022))
			{
				C3Vector v(shape->getInitialDynamicAngularVelocity());
				if (parameterID==3020)
					parameter[0]=v(0);
				if (parameterID==3021)
					parameter[0]=v(1);
				if (parameterID==3022)
					parameter[0]=v(2);
				retVal=1;
			}
			if (parameterID==3005)
			{
				parameter[0]=shape->geomData->geomInfo->getMass();
				retVal=1;
			}
			if ((parameterID>=3006)&&(parameterID<=3013))
			{
				if (shape->geomData->geomInfo->isGeometric())
				{
					CTextureProperty* tp=((CGeometric*)shape->geomData->geomInfo)->getTextureProperty();
					if (tp!=NULL)
					{
						if (parameterID==3006)
							parameter[0]=tp->getTextureRelativeConfig().X(0);
						if (parameterID==3007)
							parameter[0]=tp->getTextureRelativeConfig().X(1);
						if (parameterID==3008)
							parameter[0]=tp->getTextureRelativeConfig().X(2);
						if (parameterID==3009)
							parameter[0]=tp->getTextureRelativeConfig().Q.getEulerAngles()(0);
						if (parameterID==3010)
							parameter[0]=tp->getTextureRelativeConfig().Q.getEulerAngles()(1);
						if (parameterID==3011)
							parameter[0]=tp->getTextureRelativeConfig().Q.getEulerAngles()(2);
						if (parameterID==3012)
						{
							float dummyFloat;
							tp->getTextureScaling(parameter[0],dummyFloat);
						}
						if (parameterID==3013)
						{
							float dummyFloat;
							tp->getTextureScaling(dummyFloat,parameter[0]);
						}
						retVal=1;
					}
					else
						retVal=0;
				}
				else
					retVal=0;
			}
		}
		if (mirror!=NULL)
		{
			if (parameterID==12000)
			{
				parameter[0]=mirror->getMirrorWidth();
				retVal=1;
			}
			if (parameterID==12001)
			{
				parameter[0]=mirror->getMirrorHeight();
				retVal=1;
			}
			if (parameterID==12002)
			{
				parameter[0]=mirror->getReflectance();
				retVal=1;
			}
		}
		if (pathPlanningObject!=NULL)
		{
			if ((parameterID>=20000)&&(parameterID<=20007))
			{
				float sMin[4];
				float sRange[4];
				pathPlanningObject->getSearchRange(sMin,sRange);
				if (parameterID==20000)
					parameter[0]=sMin[0];
				if (parameterID==20001)
					parameter[0]=sRange[0];
				if (parameterID==20002)
					parameter[0]=sMin[1];
				if (parameterID==20003)
					parameter[0]=sRange[1];
				if (parameterID==20004)
					parameter[0]=sMin[2];
				if (parameterID==20005)
					parameter[0]=sRange[2];
				if (parameterID==20006)
					parameter[0]=sMin[3];
				if (parameterID==20007)
					parameter[0]=sRange[3];
				retVal=1;
			}
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetObjectFloatParameter_internal(simInt objectHandle,simInt parameterID,simFloat parameter)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=0; // Means the parameter was not set
		CVisionSensor* rendSens=App::ct->objCont->getVisionSensor(objectHandle);
		CJoint* joint=App::ct->objCont->getJoint(objectHandle);
		CShape* shape=App::ct->objCont->getShape(objectHandle);
		CMirror* mirror=App::ct->objCont->getMirror(objectHandle);
		CPathPlanningTask* pathPlanningObject=App::ct->pathPlanning->getObject(objectHandle);
		if (rendSens!=NULL)
		{
			if (parameterID==1000)
			{
				rendSens->setNearClippingPlane(parameter);
				retVal=1;
			}
			if (parameterID==1001)
			{
				rendSens->setFarClippingPlane(parameter);
				retVal=1;
			}
			if (parameterID==1004)
			{
				rendSens->setViewAngle(parameter);
				retVal=1;
			}
			if (parameterID==1005)
			{
				rendSens->setOrthoViewSize(parameter);
				retVal=1;
			}
		}
		if (joint!=NULL)
		{
			if ((parameterID==2002)||(parameterID==2003)||(parameterID==2004))
			{
				float pp,ip,dp;
				joint->getDynamicMotorPositionControlParameters(pp,ip,dp);
				if (parameterID==2002)
					pp=parameter;
				if (parameterID==2003)
					ip=parameter;
				if (parameterID==2004)
					dp=parameter;
				joint->setDynamicMotorPositionControlParameters(pp,ip,dp);
				retVal=1;
			}
			if ((parameterID==2018)||(parameterID==2019))
			{
				float kp,cp;
				joint->getDynamicMotorSpringControlParameters(kp,cp);
				if (parameterID==2018)
					kp=parameter;
				if (parameterID==2019)
					cp=parameter;
				joint->setDynamicMotorSpringControlParameters(kp,cp);
				retVal=1;
			}
			if ((parameterID>=2013)&&(parameterID<=2016))
			{
				if (joint->getJointType()==sim_joint_spherical_subtype)
				{
					static float buff[3];
					if (parameterID==2013)
						buff[0]=parameter;
					if (parameterID==2014)
						buff[1]=parameter;
					if (parameterID==2015)
						buff[2]=parameter;
					if (parameterID==2016)
					{
						C4Vector q(parameter,buff[0],buff[1],buff[2]);
						joint->setSphericalTransformation(q);
					}
					retVal=1;
				}
				else
					retVal=0;
			}
			if (parameterID==2017)
			{
				joint->setDynamicMotorUpperLimitVelocity(parameter);
				retVal=1;
			}
		}
		if (shape!=NULL)
		{
			if ((parameterID==3000)||(parameterID==3001)||(parameterID==3002))
			{
				C3Vector v(shape->getInitialDynamicVelocity());
				if (parameterID==3000)
					v(0)=parameter;
				if (parameterID==3001)
					v(1)=parameter;
				if (parameterID==3002)
					v(2)=parameter;
				shape->setInitialDynamicVelocity(v);
				retVal=1;
			}
			if ((parameterID==3020)||(parameterID==3021)||(parameterID==3022))
			{
				C3Vector v(shape->getInitialDynamicAngularVelocity());
				if (parameterID==3020)
					v(0)=parameter;
				if (parameterID==3021)
					v(1)=parameter;
				if (parameterID==3022)
					v(2)=parameter;
				shape->setInitialDynamicAngularVelocity(v);
				retVal=1;
			}
			if (parameterID==3005)
			{
				shape->geomData->geomInfo->setMass(parameter);
				retVal=1;
			}
			if ((parameterID>=3006)&&(parameterID<=3013))
			{
				if (shape->geomData->geomInfo->isGeometric())
				{
					CTextureProperty* tp=((CGeometric*)shape->geomData->geomInfo)->getTextureProperty();
					if (tp!=NULL)
					{
						C3Vector pos(tp->getTextureRelativeConfig().X);
						C3Vector euler(tp->getTextureRelativeConfig().Q.getEulerAngles());
						float scalingX,scalingY;
						tp->getTextureScaling(scalingX,scalingY);
						if (parameterID==3006)
							pos(0)=parameter;
						if (parameterID==3007)
							pos(1)=parameter;
						if (parameterID==3008)
							pos(2)=parameter;
						if (parameterID==3009)
							euler(0)=parameter;
						if (parameterID==3010)
							euler(1)=parameter;
						if (parameterID==3011)
							euler(2)=parameter;
						if (parameterID==3012)
							scalingX=parameter;
						if (parameterID==3013)
							scalingY=parameter;
						tp->setTextureRelativeConfig(C7Vector(C4Vector(euler),pos));
						tp->setTextureScaling(scalingX,scalingY);
						retVal=1;
					}
				}
			}
		}
		if (mirror!=NULL)
		{
			if (parameterID==12000)
			{
				mirror->setMirrorWidth(parameter);
				retVal=1;
			}
			if (parameterID==12001)
			{
				mirror->setMirrorHeight(parameter);
				retVal=1;
			}
			if (parameterID==12002)
			{
				mirror->setReflectance(parameter);
				retVal=1;
			}
		}
		if (pathPlanningObject!=NULL)
		{
			if ((parameterID>=20000)&&(parameterID<=20007))
			{
				float sMin[4];
				float sRange[4];
				pathPlanningObject->getSearchRange(sMin,sRange);
				if (parameterID==20000)
					sMin[0]=parameter;
				if (parameterID==20001)
					sRange[0]=parameter;
				if (parameterID==20002)
					sMin[1]=parameter;
				if (parameterID==20003)
					sRange[1]=parameter;
				if (parameterID==20004)
					sMin[2]=parameter;
				if (parameterID==20005)
					sRange[2]=parameter;
				if (parameterID==20006)
					sMin[3]=parameter;
				if (parameterID==20007)
					sRange[3]=parameter;
				pathPlanningObject->setSearchRange(sMin,sRange);
				retVal=1;
			}
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simGetObjectStringParameter_internal(simInt objectHandle,simInt parameterID,simInt* parameterLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(NULL);
		}
		simChar* retVal=NULL; // Means the parameter was not retrieved
		parameterLength[0]=0;
		CVisionSensor* rendSens=App::ct->objCont->getVisionSensor(objectHandle);
		if (rendSens!=NULL)
		{

		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simSetObjectStringParameter_internal(simInt objectHandle,simInt parameterID,simChar* parameter,simInt parameterLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		int retVal=0; // Means the parameter was not set
		CVisionSensor* rendSens=App::ct->objCont->getVisionSensor(objectHandle);
		if (rendSens!=NULL)
		{

		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetRotationAxis_internal(const simFloat* matrixStart,const simFloat* matrixGoal,simFloat* axis,simFloat* angle)
{
	C_API_FUNCTION_DEBUG;

	C4X4Matrix mStart;
	mStart.copyFromInterface(matrixStart);
	C4X4Matrix mGoal;
	mGoal.copyFromInterface(matrixGoal);

	// Following few lines taken from the quaternion interpolation part:
	C4Vector AA(mStart.M.getQuaternion());
	C4Vector BB(mGoal.M.getQuaternion());
	if (AA(0)*BB(0)+AA(1)*BB(1)+AA(2)*BB(2)+AA(3)*BB(3)<0.0f)
		AA=AA*-1.0f;
	C4Vector r((AA.getInverse()*BB).getAngleAndAxis());

	C3Vector v(r(1),r(2),r(3));
	v=AA*v;

	axis[0]=v(0);
	axis[1]=v(1);
	axis[2]=v(2);
	float l=sqrt(v(0)*v(0)+v(1)*v(1)+v(2)*v(2));
	if (l!=0.0f)
	{
		axis[0]/=l;
		axis[1]/=l;
		axis[2]/=l;
	}
	angle[0]=r(0);

	return(1);
}

simInt simRotateAroundAxis_internal(const simFloat* matrixIn,const simFloat* axis,const simFloat* axisPos,simFloat angle,simFloat* matrixOut)
{
	C_API_FUNCTION_DEBUG;

	C4X4Matrix mIn;
	mIn.copyFromInterface(matrixIn);
	C7Vector m(mIn);
	C3Vector ax(axis);
	C3Vector pos(axisPos);

	float alpha=-atan2(ax(1),ax(0));
	float beta=atan2(-sqrt(ax(0)*ax(0)+ax(1)*ax(1)),ax(2));
	m.X-=pos;
	C7Vector r;
	r.X.clear();
	r.Q.setEulerAngles(0.0f,0.0f,alpha);
	m=r*m;
	r.Q.setEulerAngles(0.0f,beta,0.0f);
	m=r*m;
	r.Q.setEulerAngles(0.0f,0.0f,angle);
	m=r*m;
	r.Q.setEulerAngles(0.0f,-beta,0.0f);
	m=r*m;
	r.Q.setEulerAngles(0.0f,0.0f,-alpha);
	m=r*m;
	m.X+=pos;
	m.getMatrix().copyToInterface(matrixOut);

	return(1);
}

simInt simGetJointForce_internal(simInt jointHandle,simFloat* forceOrTorque)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,jointHandle))
		{
			return(-1);
		}
		if (!isJoint(__func__,jointHandle))
		{
			return(-1);
		}
		CJoint* it=App::ct->objCont->getJoint(jointHandle);
		if (it->getJointType()==sim_joint_spherical_subtype)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_JOINT_SPHERICAL);
			return(-1);
		}
		float f;
		forceOrTorque[0]=0.0f;
		if (it->getDynamicForceOrTorque(f))
			forceOrTorque[0]=f;
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simPersistentDataWrite_internal(const simChar* dataName,const simChar* dataValue,simInt dataLength,simInt options)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		CPersistentDataContainer a;
		a.writeData(dataName,std::string(dataValue,dataLength),(options&1)!=0);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simChar* simPersistentDataRead_internal(const simChar* dataName,simInt* dataLength)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		std::string sigVal;
		CPersistentDataContainer a;
		if (a.readData(dataName,sigVal))
		{
			char* retVal=new char[sigVal.length()];
			for (unsigned int i=0;i<sigVal.length();i++)
				retVal[i]=sigVal[i];
			dataLength[0]=sigVal.length();
			return(retVal);
		}
		return(NULL); // data does not exist
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(NULL);
}

simInt simIsHandleValid_internal(simInt generalObjectHandle,simInt generalObjectType)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (((generalObjectType==-1)||(generalObjectType==sim_appobj_object_type))&&(App::ct->objCont->getObject(generalObjectHandle)!=NULL))
		{
			return(1);
		}
		if (((generalObjectType==-1)||(generalObjectType==sim_appobj_ui_type))&&(App::ct->buttonBlockContainer->getBlockWithID(generalObjectHandle)!=NULL))
		{
			return(1);
		}
		if (((generalObjectType==-1)||(generalObjectType==sim_appobj_collection_type))&&(App::ct->collections->getGroup(generalObjectHandle)!=NULL))
		{
			return(1);
		}
		if (((generalObjectType==-1)||(generalObjectType==sim_appobj_collision_type))&&(App::ct->collisions->getObject(generalObjectHandle)!=NULL))
		{
			return(1);
		}
		if (((generalObjectType==-1)||(generalObjectType==sim_appobj_distance_type))&&(App::ct->distances->getObject(generalObjectHandle)!=NULL))
		{
			return(1);
		}
		if (((generalObjectType==-1)||(generalObjectType==sim_appobj_constraintsolver_type))&&(App::ct->constraintSolver->getObject(generalObjectHandle)!=NULL))
		{
			return(1);
		}
		if (((generalObjectType==-1)||(generalObjectType==sim_appobj_ik_type))&&(App::ct->ikGroups->getIkGroup(generalObjectHandle)!=NULL))
		{
			return(1);
		}
		if (((generalObjectType==-1)||(generalObjectType==sim_appobj_script_type))&&(App::ct->luaScriptContainer->getScriptFromID(generalObjectHandle)!=NULL))
		{
			return(1);
		}
		if (((generalObjectType==-1)||(generalObjectType==sim_appobj_pathplanning_type))&&(App::ct->pathPlanning->getObject(generalObjectHandle)!=NULL))
		{
			return(1);
		}
		if (((generalObjectType==-1)||(generalObjectType==sim_appobj_motionplanning_type))&&(App::ct->motionPlanning->getObject(generalObjectHandle)!=NULL))
		{
			return(1);
		}
		if (((generalObjectType==-1)||(generalObjectType==sim_appobj_texture_type))&&(App::ct->textureCont->getObject(generalObjectHandle)!=NULL))
		{
			return(1);
		}
		return(0); // handle is not valid!
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simHandleVisionSensor_internal(simInt visionSensorHandle,simFloat** auxValues,simInt** auxValuesCount)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (visionSensorHandle!=sim_handle_all)&&(visionSensorHandle!=sim_handle_all_except_explicit) )
		{
			if (!isVisionSensor(__func__,visionSensorHandle))
			{
				return(-1);
			}
		}
		if (auxValues!=NULL)
			auxValues[0]=NULL;
		if (auxValuesCount!=NULL)
			auxValuesCount[0]=NULL;
		int retVal=0;
		for (int i=0;i<int(App::ct->objCont->visionSensorList.size());i++)
		{
			CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getObject(App::ct->objCont->visionSensorList[i]);
			if (visionSensorHandle>=0)
			{ // explicit handling
				it=(CVisionSensor*)App::ct->objCont->getObject(visionSensorHandle);
				if (!it->getExplicitHandling())
				{
					CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
					return(-1);
				}
				retVal=it->handleSensor();
				if ( (auxValues!=NULL)&&(auxValuesCount!=NULL) )
				{
					auxValuesCount[0]=new int[1+int(it->sensorAuxiliaryResult.size())];
					auxValuesCount[0][0]=int(it->sensorAuxiliaryResult.size());
					int fvs=0;
					for (int j=0;j<int(it->sensorAuxiliaryResult.size());j++)
						fvs+=it->sensorAuxiliaryResult[j].size();
					auxValues[0]=new float[fvs];
					fvs=0;
					for (int j=0;j<int(it->sensorAuxiliaryResult.size());j++)
					{
						auxValuesCount[0][j+1]=int(it->sensorAuxiliaryResult[j].size());
						for (int k=0;k<int(it->sensorAuxiliaryResult[j].size());k++)
							auxValues[0][fvs++]=it->sensorAuxiliaryResult[j][k];
					}
				}
			}
			else
			{
				if ( (!it->getExplicitHandling())||(visionSensorHandle==sim_handle_all) )
					retVal+=it->handleSensor();
			}
			if (visionSensorHandle>=0)
				break;
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simReadVisionSensor_internal(simInt visionSensorHandle,simFloat** auxValues,simInt** auxValuesCount)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!isVisionSensor(__func__,visionSensorHandle))
		{
			return(-1);
		}
		if (auxValues!=NULL)
			auxValues[0]=NULL;
		if (auxValuesCount!=NULL)
			auxValuesCount[0]=NULL;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getObject(visionSensorHandle);
		int retVal=-1;
		if (it->sensorResult.sensorResultIsValid)
		{
			if (it->sensorResult.sensorWasTriggered)
				retVal=1;
			else
				retVal=0;
			if ( (auxValues!=NULL)&&(auxValuesCount!=NULL) )
			{
				auxValuesCount[0]=new int[1+int(it->sensorAuxiliaryResult.size())];
				auxValuesCount[0][0]=int(it->sensorAuxiliaryResult.size());
				int fvs=0;
				for (int j=0;j<int(it->sensorAuxiliaryResult.size());j++)
					fvs+=it->sensorAuxiliaryResult[j].size();
				auxValues[0]=new float[fvs];
				fvs=0;
				for (int j=0;j<int(it->sensorAuxiliaryResult.size());j++)
				{
					auxValuesCount[0][j+1]=int(it->sensorAuxiliaryResult[j].size());
					for (int k=0;k<int(it->sensorAuxiliaryResult[j].size());k++)
						auxValues[0][fvs++]=it->sensorAuxiliaryResult[j][k];
				}
			}
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simResetVisionSensor_internal(simInt visionSensorHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if ( (visionSensorHandle!=sim_handle_all)&&(visionSensorHandle!=sim_handle_all_except_explicit) )
		{
			if (!isVisionSensor(__func__,visionSensorHandle))
			{
				return(-1);
			}
		}
		for (int i=0;i<int(App::ct->objCont->visionSensorList.size());i++)
		{
			CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getObject(App::ct->objCont->visionSensorList[i]);
			if (visionSensorHandle>=0)
			{ // Explicit handling
				it=(CVisionSensor*)App::ct->objCont->getObject(visionSensorHandle);
				if (!it->getExplicitHandling())
				{
					CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING);
					return(-1);
				}
				it->resetSensor();
				break;
			}
			else
			{
				if ( (!it->getExplicitHandling())||(visionSensorHandle==sim_handle_all) )
					it->resetSensor();
			}
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}


simInt simCheckVisionSensor_internal(simInt sensorHandle,simInt entityHandle,simFloat** auxValues,simInt** auxValuesCount)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,sensorHandle))
		{
			return(-1);
		}
		if (!isVisionSensor(__func__,sensorHandle))
		{
			return(-1);
		}
		if ( (entityHandle!=sim_handle_all)&&(!doesEntityExist(__func__,entityHandle)) )
		{
			return(-1);
		}
		if (entityHandle==sim_handle_all)
			entityHandle=-1;

		if (!App::ct->mainSettings->visionSensorsEnabled)
		{
			return(0);
		}

		if (auxValues!=NULL)
			auxValues[0]=NULL;
		if (auxValuesCount!=NULL)
			auxValuesCount[0]=NULL;
		int retVal=0;

		CVisionSensor* it=App::ct->objCont->getVisionSensor(sensorHandle);
		if (it->checkSensor(entityHandle,true))
			retVal=1;

		if ( (auxValues!=NULL)&&(auxValuesCount!=NULL) )
		{
			auxValuesCount[0]=new int[1+int(it->sensorAuxiliaryResult.size())];
			auxValuesCount[0][0]=int(it->sensorAuxiliaryResult.size());
			int fvs=0;
			for (int j=0;j<int(it->sensorAuxiliaryResult.size());j++)
				fvs+=it->sensorAuxiliaryResult[j].size();
			auxValues[0]=new float[fvs];
			fvs=0;
			for (int j=0;j<int(it->sensorAuxiliaryResult.size());j++)
			{
				auxValuesCount[0][j+1]=int(it->sensorAuxiliaryResult[j].size());
				for (int k=0;k<int(it->sensorAuxiliaryResult[j].size());k++)
					auxValues[0][fvs++]=it->sensorAuxiliaryResult[j][k];
			}
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simFloat* simCheckVisionSensorEx_internal(simInt sensorHandle,simInt entityHandle,simBool returnImage)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,sensorHandle))
		{
			return(NULL);
		}
		if (!isVisionSensor(__func__,sensorHandle))
		{
			return(NULL);
		}
		if ( (entityHandle!=sim_handle_all)&&(!doesEntityExist(__func__,entityHandle)) )
		{
			return(NULL);
		}
		if (entityHandle==sim_handle_all)
			entityHandle=-1;

		if (!App::ct->mainSettings->visionSensorsEnabled)
		{
			return(NULL);
		}

		CVisionSensor* it=App::ct->objCont->getVisionSensor(sensorHandle);
		float* retBuffer=it->checkSensorEx(entityHandle,returnImage!=0,false,false,true);
		return(retBuffer);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simGetVisionSensorResolution_internal(simInt sensorHandle,simInt* resolution)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,sensorHandle))
		{
			return(-1);
		}
		if (!isVisionSensor(__func__,sensorHandle))
		{
			return(-1);
		}
		CVisionSensor* it=App::ct->objCont->getVisionSensor(sensorHandle);
		it->getRealResolution(resolution);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simFloat* simGetVisionSensorImage_internal(simInt sensorHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,sensorHandle))
		{
			return(NULL);
		}
		if (!isVisionSensor(__func__,sensorHandle))
		{
			return(NULL);
		}
		CVisionSensor* it=App::ct->objCont->getVisionSensor(sensorHandle);
		int res[2];
		it->getRealResolution(res);
		float* buff=new float[res[0]*res[1]*3];
		for (int i=0;i<res[0]*res[1]*3;i++)
			buff[i]=it->_imageBuffer[i];
		return(buff);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simSetVisionSensorImage_internal(simInt sensorHandle,const simFloat* image)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (!doesObjectExist(__func__,sensorHandle))
		{
			return(-1);
		}
		if (!isVisionSensor(__func__,sensorHandle))
		{
			return(-1);
		}
		CVisionSensor* it=App::ct->objCont->getVisionSensor(sensorHandle);
		int retVal=0;
		if (it->setExternalImage(image))
			retVal=1;
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simFloat* simGetVisionSensorDepthBuffer_internal(simInt sensorHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,sensorHandle))
		{
			return(NULL);
		}
		if (!isVisionSensor(__func__,sensorHandle))
		{
			return(NULL);
		}
		CVisionSensor* it=App::ct->objCont->getVisionSensor(sensorHandle);
		int res[2];
		it->getRealResolution(res);
		float* buff=new float[res[0]*res[1]];
		for (int i=0;i<res[0]*res[1];i++)
			buff[i]=it->_depthBuffer[i];
		return(buff);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simRMLPosition_internal(simInt dofs,simDouble timeStep,simInt flags,const simDouble* currentPosVelAccel,const simDouble* maxVelAccelJerk,const simBool* selection,const simDouble* targetPosVel,simDouble* newPosVelAccel,simVoid* auxData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	BYTE auxDataCount=0;
	if (auxData!=NULL)
		auxDataCount=((BYTE*)auxData)[0]; // the first byte indicates how many values we have or we wanna have set!

	char* data=new char[4+8+24*dofs+24*dofs+dofs+16*dofs+4+1]; // for now we have no aux data to transmit (so just +1)
	int off=0;

	// dofs
	((int*)(data+off))[0]=dofs;
	off+=4;

	// timeStep
	((double*)(data+off))[0]=timeStep;
	off+=8;

	// currentPosVelAccel
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[0+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[dofs+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[2*dofs+i];
		off+=8;
	}

	// maxVelAccelJerk
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=maxVelAccelJerk[0+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=maxVelAccelJerk[dofs+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=maxVelAccelJerk[2*dofs+i];
		off+=8;
	}

	// selection
	for (int i=0;i<dofs;i++)
	{
		((char*)(data+off))[0]=selection[0+i];
		off++;
	}

	// targetPosVel
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=targetPosVel[0+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=targetPosVel[dofs+i];
		off+=8;
	}

	// Flags:
	((int*)(data+off))[0]=flags;
	off+=4;

	// Number of extension bytes (not used for now)
	data[off]=0;
	off++;

	int retVal=-42; // means no plugin
	int auxVals[4]={0,0,0,0}; // if first value is diff. from 0, we use the type 4 lib
		auxVals[0]=1;
	int replyData[4]={-1,-1,-1,-1};
	void* returnData=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_rmlposition,auxVals,data,replyData);
	if (returnData!=NULL)
	{
		retVal=replyData[0];
		off=0;
		char* returnDat=(char*)returnData;
		// newPosVelAccel
		for (int i=0;i<dofs;i++)
		{
			newPosVelAccel[0+i]=((double*)(returnDat+off))[0];
			off+=8;
		}
		for (int i=0;i<dofs;i++)
		{
			newPosVelAccel[dofs+i]=((double*)(returnDat+off))[0];
			off+=8;
		}
		for (int i=0;i<dofs;i++)
		{
			newPosVelAccel[2*dofs+i]=((double*)(returnDat+off))[0];
			off+=8;
		}

		// here we have 8*8 bytes for future extensions. 1*8 bytes are already used:
		double synchronizationTime=((double*)(returnDat+off))[0];
		off+=8*8;
		
		if (auxDataCount>=1)
			((double*)(((char*)auxData)+1))[0]=synchronizationTime;

		delete[] (char*)returnData;
	}

	delete[] data;
	return(retVal);
}

simInt simRMLVelocity_internal(simInt dofs,simDouble timeStep,simInt flags,const simDouble* currentPosVelAccel,const simDouble* maxAccelJerk,const simBool* selection,const simDouble* targetVel,simDouble* newPosVelAccel,simVoid* auxData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	BYTE auxDataCount=0;
	if (auxData!=NULL)
		auxDataCount=((BYTE*)auxData)[0]; // the first byte indicates how many values we have or we wanna have set!

	char* data=new char[4+8+24*dofs+16*dofs+dofs+8*dofs+4+1]; // for now we have no aux data to transmit (so just +1)
	int off=0;

	// dofs
	((int*)(data+off))[0]=dofs;
	off+=4;

	// timeStep
	((double*)(data+off))[0]=timeStep;
	off+=8;

	// currentPosVelAccel
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[0+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[dofs+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[2*dofs+i];
		off+=8;
	}

	// maxAccelJerk
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=maxAccelJerk[0+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=maxAccelJerk[dofs+i];
		off+=8;
	}

	// selection
	for (int i=0;i<dofs;i++)
	{
		((char*)(data+off))[0]=selection[0+i];
		off++;
	}

	// targetVel
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=targetVel[0+i];
		off+=8;
	}

	// Flags:
	((int*)(data+off))[0]=flags;
	off+=4;

	// Number of extension bytes (not used for now)
	data[off]=0;
	off++;

	int retVal=-42; // means no plugin
	int auxVals[4]={0,0,0,0}; // if first value is diff. from 0, we use the type 4 lib!
		auxVals[0]=1;
	int replyData[4]={-1,-1,-1,-1};
	void* returnData=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_rmlvelocity,auxVals,data,replyData);
	if (returnData!=NULL)
	{
		retVal=replyData[0];
		off=0;
		char* returnDat=(char*)returnData;
		// newPosVelAccel
		for (int i=0;i<dofs;i++)
		{
			newPosVelAccel[0+i]=((double*)(returnDat+off))[0];
			off+=8;
		}
		for (int i=0;i<dofs;i++)
		{
			newPosVelAccel[dofs+i]=((double*)(returnDat+off))[0];
			off+=8;
		}
		for (int i=0;i<dofs;i++)
		{
			newPosVelAccel[2*dofs+i]=((double*)(returnDat+off))[0];
			off+=8;
		}

		// here we have 8*8 bytes for future extensions. 1*8 bytes are already used:
		double synchronizationTime=((double*)(returnDat+off))[0];
		if (auxDataCount>=1)
			((double*)(((char*)auxData)+1))[0]=synchronizationTime;
		off+=8*8;

		delete[] (char*)returnData;
	}

	delete[] data;
	return(retVal);
}


simInt simRMLPos_internal(simInt dofs,simDouble smallestTimeStep,simInt flags,const simDouble* currentPosVelAccel,const simDouble* maxVelAccelJerk,const simBool* selection,const simDouble* targetPosVel,simVoid* auxData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	char* data=new char[4+8+24*dofs+24*dofs+dofs+16*dofs+4+1]; // for now we have no aux data to transmit (so just +1)
	int off=0;

	// dofs
	((int*)(data+off))[0]=dofs;
	off+=4;

	// timeStep
	((double*)(data+off))[0]=smallestTimeStep;
	off+=8;

	// currentPosVelAccel
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[0+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[dofs+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[2*dofs+i];
		off+=8;
	}

	// maxVelAccelJerk
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=maxVelAccelJerk[0+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=maxVelAccelJerk[dofs+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=maxVelAccelJerk[2*dofs+i];
		off+=8;
	}

	// selection
	for (int i=0;i<dofs;i++)
	{
		((char*)(data+off))[0]=selection[0+i];
		off++;
	}

	// targetPosVel
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=targetPosVel[0+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=targetPosVel[dofs+i];
		off+=8;
	}

	// Flags:
	((int*)(data+off))[0]=flags;
	off+=4;

	// Number of extension bytes (not used for now)
	data[off]=0;
	off++;

	int auxVals[4]={0,0,0,0}; // if first value is diff. from 0, we use the type 4 lib
	auxVals[0]=1;
	auxVals[1]=0; // do not destroy at simulation end
	if (auxData!=NULL)
	{
		if (((BYTE*)auxData)[0]>=1)
			auxVals[1]=((int*)(((BYTE*)auxData)+1))[0];
	}
	int replyData[4]={-1,-1,-1,-1};
	CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_rmlpos,auxVals,data,replyData);

	delete[] data;
	return(replyData[1]);
}

simInt simRMLVel_internal(simInt dofs,simDouble smallestTimeStep,simInt flags,const simDouble* currentPosVelAccel,const simDouble* maxAccelJerk,const simBool* selection,const simDouble* targetVel,simVoid* auxData)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	char* data=new char[4+8+24*dofs+16*dofs+dofs+8*dofs+4+1]; // for now we have no aux data to transmit (so just +1)
	int off=0;

	// dofs
	((int*)(data+off))[0]=dofs;
	off+=4;

	// timeStep
	((double*)(data+off))[0]=smallestTimeStep;
	off+=8;

	// currentPosVelAccel
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[0+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[dofs+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=currentPosVelAccel[2*dofs+i];
		off+=8;
	}

	// maxAccelJerk
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=maxAccelJerk[0+i];
		off+=8;
	}
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=maxAccelJerk[dofs+i];
		off+=8;
	}

	// selection
	for (int i=0;i<dofs;i++)
	{
		((char*)(data+off))[0]=selection[0+i];
		off++;
	}

	// targetVel
	for (int i=0;i<dofs;i++)
	{
		((double*)(data+off))[0]=targetVel[0+i];
		off+=8;
	}

	// Flags:
	((int*)(data+off))[0]=flags;
	off+=4;

	// Number of extension bytes (not used for now)
	data[off]=0;
	off++;

	int auxVals[4]={0,0,0,0}; // if first value is diff. from 0, we use the type 4 lib!
	auxVals[0]=1;
	auxVals[1]=0; // do not destroy at simulation end
	if (auxData!=NULL)
	{
		if (((BYTE*)auxData)[0]>=1)
			auxVals[1]=((int*)(((BYTE*)auxData)+1))[0];
	}
	int replyData[4]={-1,-1,-1,-1};
	CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_rmlvel,auxVals,data,replyData);

	delete[] data;
	return(replyData[1]);
}

simInt simRMLStep_internal(simInt handle,simDouble timeStep,simDouble* newPosVelAccel,simVoid* auxData,simVoid* reserved)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	BYTE auxDataCount=0;
	if (auxData!=NULL)
		auxDataCount=((BYTE*)auxData)[0]; // the first byte indicates how many values we have or we wanna have set!

	int retVal=-1;
	int auxVals[4]={0,0,0,0}; // if first value is diff. from 0, we use the type 4 lib
	auxVals[0]=1;
	auxVals[1]=handle;
	auxVals[2]=int(timeStep*100000.0);
	int replyData[4]={-1,-1,-1,-1};
	void* returnData=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_rmlstep,auxVals,NULL,replyData);
	if (returnData!=NULL)
	{
		retVal=replyData[0];
		int dofs=replyData[1];
		int off=0;
		char* returnDat=(char*)returnData;
		// newPosVelAccel
		for (int i=0;i<dofs;i++)
		{
			newPosVelAccel[0+i]=((double*)(returnDat+off))[0];
			off+=8;
		}
		for (int i=0;i<dofs;i++)
		{
			newPosVelAccel[dofs+i]=((double*)(returnDat+off))[0];
			off+=8;
		}
		for (int i=0;i<dofs;i++)
		{
			newPosVelAccel[2*dofs+i]=((double*)(returnDat+off))[0];
			off+=8;
		}

		// here we have 8*8 bytes for future extensions. 1*8 bytes are already used:
		double synchronizationTime=((double*)(returnDat+off))[0];
		off+=8*8;

		if (auxDataCount>=1)
			((double*)(((char*)auxData)+1))[0]=synchronizationTime;

		delete[] (char*)returnData;
	}

	return(retVal);
}

simInt simRMLRemove_internal(simInt handle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	int auxVals[4]={0,0,0,0};
	auxVals[1]=handle;
	int replyData[4]={-1,-1,-1,-1};
	CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_rmlremove,auxVals,NULL,replyData);
	return(replyData[1]);
}

simInt simGetObjectQuaternion_internal(simInt objectHandle,simInt relativeToObjectHandle,simFloat* quaternion)
{ 
	C_API_FUNCTION_DEBUG;
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		if (relativeToObjectHandle==sim_handle_parent)
		{
			relativeToObjectHandle=-1;
			C3DObject* parent=it->getParent();
			if (parent!=NULL)
				relativeToObjectHandle=parent->getID();
		}
		if (relativeToObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,relativeToObjectHandle))
			{
				return(-1);
			}
		}
		C7Vector tr;
		if (relativeToObjectHandle==-1)
			tr=it->getCumulativeTransformationPart1();
		else
		{
			C3DObject* relObj=App::ct->objCont->getObject(relativeToObjectHandle);
			C7Vector relTr(relObj->getCumulativeTransformationPart1());
			tr=relTr.getInverse()*it->getCumulativeTransformationPart1();
		}
		quaternion[0]=tr.Q(1);
		quaternion[1]=tr.Q(2);
		quaternion[2]=tr.Q(3);
		quaternion[3]=tr.Q(0);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetObjectQuaternion_internal(simInt objectHandle,simInt relativeToObjectHandle,const simFloat* quaternion)
{
	C_API_FUNCTION_DEBUG;
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		C3DObject* it=App::ct->objCont->getObject(objectHandle);
		if (relativeToObjectHandle==sim_handle_parent)
		{
			relativeToObjectHandle=-1;
			C3DObject* parent=it->getParent();
			if (parent!=NULL)
				relativeToObjectHandle=parent->getID();
		}
		if (relativeToObjectHandle!=-1)
		{
			if (!doesObjectExist(__func__,relativeToObjectHandle))
			{
				return(-1);
			}
		}
		if (it->getObjectType()==sim_object_shape_type)
		{
			CShape* shape=(CShape*)it;
			if (!shape->getShapeIsDynamicallyStatic()) // condition new since 5/5/2013
				shape->setDynamicsFullRefreshFlag(true); // dynamically enabled objects have to be reset first!
		}
		else
			it->setDynamicsFullRefreshFlag(true); // dynamically enabled objects have to be reset first!
		if (relativeToObjectHandle==-1)
		{
			C4Vector q;
			q(0)=quaternion[3];
			q(1)=quaternion[0];
			q(2)=quaternion[1];
			q(3)=quaternion[2];
			App::ct->objCont->setAbsoluteAngles(it->getID(),q.getEulerAngles());
		}
		else
		{
			C3DObject* relObj=App::ct->objCont->getObject(relativeToObjectHandle);
			C7Vector absTr(it->getCumulativeTransformationPart1());
			C7Vector relTr(relObj->getCumulativeTransformationPart1());
			C7Vector x(relTr.getInverse()*absTr);
			x.Q(0)=quaternion[3];
			x.Q(1)=quaternion[0];
			x.Q(2)=quaternion[1];
			x.Q(3)=quaternion[2];
			absTr=relTr*x;
			App::ct->objCont->setAbsoluteAngles(it->getID(),absTr.Q.getEulerAngles());
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simFileDialog_internal(simInt mode,const simChar* title,const simChar* startPath,const simChar* initName,const simChar* extName,const simChar* ext)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	std::string nameAndPath;
	char* retVal=NULL;
	std::string stPath(startPath);
	if (stPath.length()==0)
		stPath=App::directories->executableDirectory;

	nameAndPath=App::uiThread->fileDialog(mode==sim_filedlg_type_load,title,stPath.c_str(),initName,extName,ext);
	if (nameAndPath.length()!=0)
	{
		retVal=new char[nameAndPath.length()+1];
		for (int i=0;i<int(nameAndPath.length());i++)
			retVal[i]=nameAndPath[i];
		retVal[nameAndPath.length()]=0; // terminal 0
	}
	return(retVal);
}

simInt simMsgBox_internal(simInt dlgType,simInt buttons,const simChar* title,const simChar* message)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	int retVal=App::uiThread->msgBox(dlgType,buttons,title,message);
	return(retVal);
}

simInt simSetShapeMassAndInertia_internal(simInt shapeHandle,simFloat mass,const simFloat* inertiaMatrix,const simFloat* centerOfMass,const simFloat* transformation)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!App::ct->simulation->isSimulationStopped())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_STOPPED);
			return(-1);
		}
		if (!isShape(__func__,shapeHandle))
		{
			return(-1);
		}
		CShape* it=App::ct->objCont->getShape(shapeHandle);

		if (mass<0.0000001f)
			mass=0.0000001f;
		C3X3Matrix m;
		m.copyFromInterface(inertiaMatrix);
		m.axis[0](1)=m.axis[1](0);
		m.axis[0](2)=m.axis[2](0);
		m.axis[1](2)=m.axis[2](1);
		m/=mass; // in V-REP we work with the "massless inertia"
		it->geomData->geomInfo->setMass(mass);
		C3Vector com(centerOfMass);
		C4X4Matrix tr;
		if (transformation==NULL)
			tr.setIdentity();
		else
			tr.copyFromInterface(transformation);

		C4Vector rot;
		C3Vector pmoment;
		CGeomWrap::findPrincipalMomentOfInertia(m,rot,pmoment);
		if (pmoment(0)<0.0000001f)
			pmoment(0)=0.0000001f;
		if (pmoment(1)<0.0000001f)
			pmoment(1)=0.0000001f;
		if (pmoment(2)<0.0000001f)
			pmoment(0)=0.0000001f;
		it->geomData->geomInfo->setPrincipalMomentsOfInertia(pmoment);
		it->geomData->geomInfo->setLocalInertiaFrame(it->getCumulativeTransformation().getInverse()*tr.getTransformation()*C7Vector(rot,com));

		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetShapeMassAndInertia_internal(simInt shapeHandle,simFloat* mass,simFloat* inertiaMatrix,simFloat* centerOfMass,const simFloat* transformation)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!isShape(__func__,shapeHandle))
		{
			return(-1);
		}
		CShape* it=App::ct->objCont->getShape(shapeHandle);

		mass[0]=it->geomData->geomInfo->getMass();

		C7Vector tr(it->getCumulativeTransformation()*it->geomData->geomInfo->getLocalInertiaFrame());
		C4X4Matrix ref;
		if (transformation==NULL)
			ref.setIdentity();
		else
			ref.copyFromInterface(transformation);
		C3X3Matrix m(CGeomWrap::getNewTensor(it->geomData->geomInfo->getPrincipalMomentsOfInertia(),ref.getTransformation().getInverse()*tr));
		m*=mass[0]; // in V-REP we work with the "massless inertia"
		m.copyToInterface(inertiaMatrix);

		(ref.getTransformation().getInverse()*tr).X.copyTo(centerOfMass);

		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGroupShapes_internal(const simInt* shapeHandles,simInt shapeCount)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		/*
		if (!App::ct->simulation->isSimulationStopped())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_STOPPED);
			return(-1);
		}
		*/
		std::vector<int> shapes;
		bool merging=(shapeCount<0);
		if (shapeCount<0)
			shapeCount=-shapeCount;
		for (int i=0;i<shapeCount;i++)
		{
			CShape* it=App::ct->objCont->getShape(shapeHandles[i]);
			if (it!=NULL)
				shapes.push_back(it->getID());
		}
		if (shapes.size()<2)
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_NOT_ENOUGH_SHAPES);
			return(-1);
		}
		int retVal;
		if (merging)
			retVal=CObjectEdition::mergeSelection(&shapes,false);
		else
			retVal=CObjectEdition::groupSelection(&shapes,false);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt* simUngroupShape_internal(simInt shapeHandle,simInt* shapeCount)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		shapeCount[0]=0;
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		// Commented following because animation export won't work otherwise
		/*
		if (!App::ct->simulation->isSimulationStopped())
		{
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_SIMULATION_NOT_STOPPED);
			shapeCount[0]=0;
			return(NULL);
		}
		*/
		bool dividing=false;
		if (shapeHandle<=-2)
		{
			shapeHandle=-shapeHandle-2;
			dividing=true;
		}

		if (!isShape(__func__,shapeHandle))
		{
			shapeCount[0]=0;
			return(NULL);
		}
		CShape* it=App::ct->objCont->getShape(shapeHandle);
		if (it->geomData->geomInfo->isGeometric())
		{
			if (dividing)
			{
				std::vector<int> finalSel;
				std::vector<int> previousSel;
				std::vector<int> sel;
				previousSel.push_back(shapeHandle);
				sel.push_back(shapeHandle);
				CObjectEdition::divideSelection(&sel);
				for (int j=0;j<int(sel.size());j++)
				{
					if (sel[j]!=shapeHandle)
						finalSel.push_back(sel[j]);
				}
				finalSel.push_back(shapeHandle); // the original shape is added at the very end for correct ordering

				int* retVal=new int[finalSel.size()];
				for (int i=0;i<int(finalSel.size());i++)
					retVal[i]=finalSel[i];
				shapeCount[0]=int(finalSel.size());
				return(retVal);
			}
			else
			{
				shapeCount[0]=1;
				int* retVal=new int[1];
				retVal[0]=shapeHandle;
				return(retVal);
			}
		}
		else
		{
			if (dividing)
			{
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_CANNOT_DIVIDE_GROUPED_SHAPE);
				shapeCount[0]=0;
				return(NULL);
			}
		}
		std::vector<int> finalSel;
		std::vector<int> previousSel;
		std::vector<int> sel;
		previousSel.push_back(shapeHandle);
		sel.push_back(shapeHandle);
		while (sel.size()!=0)
		{
			CObjectEdition::ungroupSelection(&sel);
			for (int i=0;i<int(previousSel.size());i++)
			{
				int previousID=previousSel[i];
				bool present=false;
				for (int j=0;j<int(sel.size());j++)
				{
					if (sel[j]==previousID)
					{
						present=true;
						break;
					}
				}
				if ((!present)&&(shapeHandle!=previousID)) // the original shape will be added at the very end for correct ordering (see below)
					finalSel.push_back(previousID); // this is a simple shape (not a group)
			}
			previousSel.assign(sel.begin(),sel.end());
		}
		finalSel.push_back(shapeHandle);
		int* retVal=new int[finalSel.size()];
		for (int i=0;i<int(finalSel.size());i++)
			retVal[i]=finalSel[i];
		shapeCount[0]=int(finalSel.size());
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(NULL);
}

simInt simConvexDecompose_internal(simInt shapeHandle,simInt options,const simInt* intParams,const simFloat* floatParams)
{ // one shape at a time!
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	// Following call will automatically lock resources:
	int retVal=App::uiThread->convexDecompose(shapeHandle,options,intParams,floatParams);
	return(retVal);
}

simInt simRegisterJointCtrlCallback_internal(simInt(*callBack)(simInt,simInt,simInt,const simInt*,const simFloat*,simFloat*))
{
	C_API_FUNCTION_DEBUG;


	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		for (int i=0;i<int(CIloIlo::allJointCtrlCallbacks.size());i++)
		{
			if (CIloIlo::allJointCtrlCallbacks[i]==callBack)
			{ // We unregister that callback
				CIloIlo::allJointCtrlCallbacks.erase(CIloIlo::allJointCtrlCallbacks.begin()+i);
				return(0);
			}
		}
		// We register that callback:
		CIloIlo::allJointCtrlCallbacks.push_back(callBack);
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simAddGhost_internal(simInt ghostGroup,simInt objectHandle,simInt options,simFloat startTime,simFloat endTime,const simFloat* color)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (!doesObjectExist(__func__,objectHandle))
		{
			return(-1);
		}
		int retVal=App::ct->ghostObjectCont->addGhost(ghostGroup,objectHandle,options,startTime,endTime,color);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simModifyGhost_internal(simInt ghostGroup,simInt ghostId,simInt operation,simFloat floatValue,simInt options,simInt optionsMask,const simFloat* colorOrTransformation)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=App::ct->ghostObjectCont->modifyGhost(ghostGroup,ghostId,operation,floatValue,options,optionsMask,colorOrTransformation);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simVoid simQuitSimulator_internal(simBool doNotDisplayMessages)
{
	C_API_FUNCTION_DEBUG;

	App::setExitRequest(true);
}

simInt simGetThreadId_internal()
{
	C_API_FUNCTION_DEBUG;

	int retVal=VThread::getThreadId_apiQueried();
	return(retVal); // 0=GUI thread, 1=main sim thread, 2-n=aux. sim threads
}

simInt simLockResources_internal(simInt lockType,simInt reserved)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	CSimAndUiThreadSync* obj=new CSimAndUiThreadSync(__func__);
	bool res=false;
	int retVal=-1; // fail
	if (lockType==sim_lock_ui_wants_to_read)
		res=obj->uiThread_tryToLockForUiEventRead();
	if (lockType==sim_lock_ui_wants_to_write)
		res=obj->uiThread_tryToLockForUiEventWrite();
	if (lockType==sim_lock_nonui_wants_to_write)
	{
		obj->simThread_lockForSimThreadWrite();
		res=true;
	}
	if (res)
	{
		EASYLOCK(_lockForExtLockList);
		retVal=obj->getObjectHandle();
		_extLockList.push_back(obj);
	}
	return(retVal);
}

simInt simUnlockResources_internal(simInt lockHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	int retVal=0;
	{ // scope parenthesis are important here!
		EASYLOCK(_lockForExtLockList);
		for (int i=0;i<int(_extLockList.size());i++)
		{
			if (_extLockList[i]->getObjectHandle()==lockHandle)
			{
				delete _extLockList[i];
				_extLockList.erase(_extLockList.begin()+i);
				retVal=1;
				break;
			}
		}
	}
	return(retVal);
}

simInt simEnableEventCallback_internal(simInt eventCallbackType,const simChar* plugin,simInt reserved)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (CPluginContainer::enableOrDisableSpecificEventCallback(eventCallbackType,plugin))
			return(1);
		return(0);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}



simInt simGetMaterialId_internal(const simChar* materialName)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=-1; // means error
		CDynMaterialObject* mat=App::ct->dynMaterialCont->getObject(materialName);
		if (mat!=NULL)
			retVal=mat->getObjectID();
		else
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MATERIAL_INEXISTANT);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simSetShapeMaterial_internal(simInt shapeHandle,simInt materialId)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		int retVal=-1; // means error
		if (isShape(__func__,shapeHandle))
		{
			CDynMaterialObject* mat=App::ct->dynMaterialCont->getObject(materialId);
			if (mat!=NULL)
			{
				CShape* shape=App::ct->objCont->getShape(shapeHandle);
				shape->geomData->geomInfo->setDynMaterialId(materialId);
				retVal=1;
			}
			else
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_MATERIAL_INEXISTANT);
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetShapeMaterial_internal(simInt shapeHandle)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=-1; // means error
		if (isShape(__func__,shapeHandle))
		{
			CShape* shape=App::ct->objCont->getShape(shapeHandle);
			retVal=shape->geomData->geomInfo->getDynMaterialId();
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetTextureId_internal(const simChar* textureName,simInt* resolution)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=-1; // means error
		CTextureObject* to=App::ct->textureCont->getObject(textureName);
		if (to!=NULL)
		{
			retVal=to->getObjectID();
			if (resolution!=NULL)
				to->getTextureSize(resolution[0],resolution[1]);
		}
		else
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_TEXTURE_INEXISTANT);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simReadTexture_internal(simInt textureId,simInt options,simInt posX,simInt posY,simInt sizeX,simInt sizeY)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CTextureObject* to=App::ct->textureCont->getObject(textureId);
		if (to!=NULL)
		{
			int resX,resY;
			to->getTextureSize(resX,resY);
			if ( (posX>=0)&&(posY>=0)&&(sizeX>=0)&&(sizeY>=0)&&(posX+sizeX<=resX)&&(posY+sizeY<=resY) )
			{
				if (sizeX==0)
				{
					posX=0;
					sizeX=resX;
				}
				if (sizeY==0)
				{
					posY=0;
					sizeY=resY;
				}
				char* retVal=to->readPortionOfTexture(posX,posY,sizeX,sizeY);
				return(retVal);
			}
			else
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ARGUMENTS);
		}
		else
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_TEXTURE_INEXISTANT);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simWriteTexture_internal(simInt textureId,simInt options,const simChar* data,simInt posX,simInt posY,simInt sizeX,simInt sizeY,simFloat interpol)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		CTextureObject* to=App::ct->textureCont->getObject(textureId);
		if (to!=NULL)
		{
			int resX,resY;
			to->getTextureSize(resX,resY);
			if ( (posX>=0)&&(posY>=0)&&(sizeX>=0)&&(sizeY>=0)&&(posX+sizeX<=resX)&&(posY+sizeY<=resY) )
			{
				if (sizeX==0)
				{
					posX=0;
					sizeX=resX;
				}
				if (sizeY==0)
				{
					posY=0;
					sizeY=resY;
				}
				int retVal=-1;
				if (to->writePortionOfTexture((unsigned char*)data,posX,posY,sizeX,sizeY,(options&4)!=0,interpol))
					retVal=1;
				return(retVal);
			}
			else
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ARGUMENTS);
		}
		else
			CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_TEXTURE_INEXISTANT);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simCreateTexture_internal(const simChar* fileName,simInt options,const simFloat* planeSizes,const simFloat* scalingUV,const simFloat* xy_g,simInt fixedResolution,simInt* textureId,simInt* resolution,const simVoid* reserved)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_WRITE_DATA
	{
		if (strlen(fileName)!=0)
		{ // loading a texture
			if (VFile::doesFileExist(fileName))
			{
				int resX,resY,n;
				unsigned char* data=CImageLoader::load(fileName,&resX,&resY,&n,0,fixedResolution);
				bool rgba=(n==4);
				if (n<3)
				{
					delete[] data;
					data=NULL;
				}
				if (data!=NULL)
				{
					C3Vector s(0.1f,0.1f,0.00001f);
					if (planeSizes!=NULL)
						s=C3Vector(tt::getLimitedFloat(0.00001f,100000.0f,planeSizes[0]),tt::getLimitedFloat(0.00001f,100000.0f,planeSizes[1]),0.00001f);
					CQDlgPrimitives theDialog;
					theDialog.initialize(1,&s);
					CShape* shape=CAddMenu::addPrimitiveWithoutDialog(ADD_PRIMITIVE_PLANE_CMD,theDialog);
					C7Vector identity;
					identity.setIdentity();
					shape->setLocalTransformation(identity);
					shape->setCulling(false);
					shape->setVisibleEdges(true);
					shape->setRespondable(false);
					shape->setShapeIsDynamicallyStatic(true);
					shape->geomData->geomInfo->setMass(1.0f);

					if (resolution!=NULL)
					{
						resolution[0]=resX;
						resolution[1]=resY;
					}

					CTextureObject* textureObj=new CTextureObject(resX,resY);
					textureObj->setImage(rgba,false,false,data); // keep false,false
					textureObj->setObjectName(App::directories->getNameFromFull(fileName).c_str());
					delete[] data;
					textureObj->addDependentObject(shape->getID(),((CGeometric*)shape->geomData->geomInfo)->getUniqueID());
					int texID=App::ct->textureCont->addObject(textureObj,false); // might erase the textureObj and return a similar object already present!!
					CTextureProperty* tp=new CTextureProperty(texID);
					((CGeometric*)shape->geomData->geomInfo)->setTextureProperty(tp);
					tp->setInterpolateColors((options&1)==0);
					tp->setDecalTexture((options&2)!=0);
					tp->setRepeatU((options&4)!=0);
					tp->setRepeatV((options&8)!=0);
					if (scalingUV!=NULL)
						tp->setTextureScaling(scalingUV[0],scalingUV[1]);
					else
						tp->setTextureScaling(s(0),s(1));
					if (xy_g!=NULL)
					{
						C7Vector tr;
						tr.setIdentity();
						tr.X(0)=xy_g[0];
						tr.X(1)=xy_g[1];
						tr.Q=C4Vector(0.0f,0.0f,xy_g[2]);
						tp->setTextureRelativeConfig(tr);
					}
					if (textureId!=NULL)
						textureId[0]=texID;
					return(shape->getID());
				}
			}
			else
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_FILE_NOT_FOUND);
		}
		else
		{ // just creating a texture (not loading it)
			if (resolution!=NULL)
			{
				C3Vector s(0.1f,0.1f,0.00001f);
				if (planeSizes!=NULL)
					s=C3Vector(tt::getLimitedFloat(0.00001f,100000.0f,planeSizes[0]),tt::getLimitedFloat(0.00001f,100000.0f,planeSizes[1]),0.00001f);
				CQDlgPrimitives theDialog;
				theDialog.initialize(1,&s);
				CShape* shape=CAddMenu::addPrimitiveWithoutDialog(ADD_PRIMITIVE_RECTANGLE_CMD,theDialog);
				C7Vector identity;
				identity.setIdentity();
				shape->setLocalTransformation(identity);
				shape->setCulling(false);
				shape->setVisibleEdges(true);
				shape->setRespondable(false);
				shape->setShapeIsDynamicallyStatic(true);
				shape->geomData->geomInfo->setMass(1.0f);

				CTextureObject* textureObj=new CTextureObject(resolution[0],resolution[1]);
				textureObj->setRandomContent();
				textureObj->setObjectName(App::directories->getNameFromFull(fileName).c_str());
				textureObj->addDependentObject(shape->getID(),((CGeometric*)shape->geomData->geomInfo)->getUniqueID());
				int texID=App::ct->textureCont->addObject(textureObj,false); // might erase the textureObj and return a similar object already present!!
				CTextureProperty* tp=new CTextureProperty(texID);
				((CGeometric*)shape->geomData->geomInfo)->setTextureProperty(tp);
				tp->setInterpolateColors((options&1)==0);
				tp->setDecalTexture((options&2)!=0);
				tp->setRepeatU((options&4)!=0);
				tp->setRepeatV((options&8)!=0);
				if (scalingUV!=NULL)
					tp->setTextureScaling(scalingUV[0],scalingUV[1]);
				else
					tp->setTextureScaling(s(0),s(1));
				if (xy_g!=NULL)
				{
					C7Vector tr;
					tr.setIdentity();
					tr.X(0)=xy_g[0];
					tr.X(1)=xy_g[1];
					tr.Q=C4Vector(0.0f,0.0f,xy_g[2]);
					tp->setTextureRelativeConfig(tr);
				}
				if (textureId!=NULL)
					textureId[0]=texID;
				return(shape->getID());
			}
			else
				CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_INVALID_ARGUMENTS);
		}
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE);
	return(-1);
}

simInt simWriteCustomDataBlock_internal(simInt objectHandle,const simChar* dataName,const simChar* data,simInt dataSize)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		std::vector<char> buffer;
		int extractedBufSize;
		if (objectHandle!=sim_handle_scene)
		{
			if (!doesObjectExist(__func__,objectHandle))
				return(-1);
			C3DObject* it=App::ct->objCont->getObject(objectHandle);
			if (strlen(dataName)!=0)
			{
				int l=it->getObjectCustomDataLength(356248756);
				if (l>0)
				{
					buffer.resize(l,' ');
					it->getObjectCustomData(356248756,&buffer[0]);
				}
				delete[] _extractCustomDataFromBuffer(buffer,dataName,&extractedBufSize);
				_appendCustomDataToBuffer(buffer,dataName,data,dataSize);
				if (buffer.size()>0)
					it->setObjectCustomData(356248756,&buffer[0],buffer.size());
				else
					it->setObjectCustomData(356248756,NULL,0);
			}
			else
				it->setObjectCustomData(356248756,NULL,0);
		}
		else
		{
			if (strlen(dataName)!=0)
			{
				int l=App::ct->customSceneData->getDataLength(356248756);
				if (l>0)
				{
					buffer.resize(l,' ');
					App::ct->customSceneData->getData(356248756,&buffer[0]);
				}
				delete[] _extractCustomDataFromBuffer(buffer,dataName,&extractedBufSize);
				_appendCustomDataToBuffer(buffer,dataName,data,dataSize);
				if (buffer.size()>0)
					App::ct->customSceneData->setData(356248756,&buffer[0],buffer.size());
				else
					App::ct->customSceneData->setData(356248756,NULL,0);
			}
			else
				App::ct->customSceneData->setData(356248756,NULL,0);
		}
		return(1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simChar* simReadCustomDataBlock_internal(simInt objectHandle,const simChar* dataName,simInt* dataSize)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(NULL);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		char* retBuffer=NULL;
		dataSize[0]=0;
		std::vector<char> buffer;
		if (objectHandle!=sim_handle_scene)
		{
			if (!doesObjectExist(__func__,objectHandle))
				return(NULL);
			C3DObject* it=App::ct->objCont->getObject(objectHandle);
			int l=it->getObjectCustomDataLength(356248756);
			if (l>0)
			{
				buffer.resize(l,' ');
				it->getObjectCustomData(356248756,&buffer[0]);
			}
			retBuffer=_extractCustomDataFromBuffer(buffer,dataName,dataSize);
		}
		else
		{
			int l=App::ct->customSceneData->getDataLength(356248756);
			if (l>0)
			{
				buffer.resize(l,' ');
				App::ct->customSceneData->getData(356248756,&buffer[0]);
			}
			retBuffer=_extractCustomDataFromBuffer(buffer,dataName,dataSize);
		}
		return(retBuffer);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(NULL);
}

simInt simAddPointCloud_internal(simInt pageMask,simInt layerMask,simInt objectHandle,simInt options,simFloat pointSize,simInt ptCnt,const simFloat* pointCoordinates,const simChar* defaultColors,const simChar* pointColors,const simFloat* pointNormals)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=-1;
		CPtCloud* ptCloud=new CPtCloud(pageMask,layerMask,objectHandle,options,pointSize,ptCnt,pointCoordinates,(unsigned char*)pointColors,pointNormals,(unsigned char*)defaultColors);
		retVal=App::ct->pointCloudCont->addObject(ptCloud);
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simModifyPointCloud_internal(simInt pointCloudHandle,simInt operation,const simInt* intParam,const simFloat* floatParam)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		if (operation==0)
		{
			if (App::ct->pointCloudCont->removeObject(pointCloudHandle))
				return(1);
		}
		return(-1);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}

simInt simGetShapeGeomInfo_internal(simInt shapeHandle,simInt* intData,simFloat* floatData,simVoid* reserved)
{
	C_API_FUNCTION_DEBUG;

	if (!isSimulatorInitialized(__func__))
	{
		return(-1);
	}

	IF_C_API_SIM_OR_UI_THREAD_CAN_READ_DATA
	{
		int retVal=-1; // means error
		if (isShape(__func__,shapeHandle))
		{
			CShape* shape=App::ct->objCont->getShape(shapeHandle);
			retVal=0;
			if (shape->geomData->geomInfo->isGeometric())
			{
				CGeometric* geom=(CGeometric*)shape->geomData->geomInfo;
				if (intData!=NULL)
				{
					intData[0]=geom->getPurePrimitiveType();
				}
				if (floatData!=NULL)
				{
					C3Vector s;
					geom->getPurePrimitiveSizes(s);
					floatData[0]=s(0);
					floatData[1]=s(1);
					floatData[2]=s(2);
					floatData[3]=geom->getPurePrimitiveInsideScaling();
				}
				if (geom->isPure())
					retVal|=2;
				if (geom->isConvex())
					retVal|=4;
			}
			else
			{ // we have a grouping...
				retVal|=1;
				if (shape->geomData->geomInfo->isPure())
					retVal|=2;
			}
		}
		return(retVal);
	}
	CApiErrors::setLastErrorMessage(__func__,SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ);
	return(-1);
}




//************************************************************************************************************
//************************************************************************************************************
// FOLLOWING FUNCTIONS ARE VERY FAST, BUT NO SPECIFIC CHECKING IS DONE. ALSO, MANY OPERATE ON OBJECT POINTERS!
//************************************************************************************************************
//************************************************************************************************************


const simVoid* _simGetGeomWrapFromGeomProxy_internal(const simVoid* geomData)
{
	C_API_FUNCTION_DEBUG;
	return(((CGeomProxy*)geomData)->geomInfo);
}

simVoid _simGetLocalInertiaFrame_internal(const simVoid* geomInfo,simFloat* pos,simFloat* quat)
{
	C_API_FUNCTION_DEBUG;
	C7Vector tr(((CGeomWrap*)geomInfo)->getLocalInertiaFrame());
	tr.Q.getInternalData(quat);
	tr.X.getInternalData(pos);
}

simVoid _simGetPrincipalMomentOfInertia_internal(const simVoid* geomInfo,simFloat* inertia)
{
	C_API_FUNCTION_DEBUG;
	((CGeomWrap*)geomInfo)->getPrincipalMomentsOfInertia().getInternalData(inertia);
}


simInt _simGetPurePrimitiveType_internal(const simVoid* geomInfo)
{
	C_API_FUNCTION_DEBUG;
	return(((CGeomWrap*)geomInfo)->getPurePrimitiveType());
}

simVoid _simGetPurePrimitiveSizes_internal(const simVoid* geometric,simFloat* sizes)
{
	C_API_FUNCTION_DEBUG;
	C3Vector s;
	((CGeometric*)geometric)->getPurePrimitiveSizes(s);
	s.getInternalData(sizes);
}

simBool _simIsGeomWrapGeometric_internal(const simVoid* geomInfo)
{
	C_API_FUNCTION_DEBUG;
	return(((CGeomWrap*)geomInfo)->isGeometric());
}

simBool _simIsGeomWrapConvex_internal(const simVoid* geomInfo)
{
	C_API_FUNCTION_DEBUG;
	return(((CGeomWrap*)geomInfo)->isConvex());
}

simInt _simGetGeometricCount_internal(const simVoid* geomInfo)
{
	C_API_FUNCTION_DEBUG;
	std::vector<CGeometric*> all;
	((CGeomWrap*)geomInfo)->getAllShapeComponentsCumulative(all);
	return((int)all.size());
}

simVoid _simGetAllGeometrics_internal(const simVoid* geomInfo,simVoid** allGeometrics)
{
	C_API_FUNCTION_DEBUG;
	std::vector<CGeometric*> all;
	((CGeomWrap*)geomInfo)->getAllShapeComponentsCumulative(all);
	for (int i=0;i<int(all.size());i++)
		allGeometrics[i]=all[i];
}

simVoid _simMakeDynamicAnnouncement_internal(int announceType)
{
	C_API_FUNCTION_DEBUG;
	if (announceType==ANNOUNCE_PURE_CONE_NOT_SUPPORTED)
		App::ct->dynamicsContainer->markForWarningDisplay_pureConeNotSupported();
	if (announceType==ANNOUNCE_PURE_SPHEROID_NOT_SUPPORTED)
		App::ct->dynamicsContainer->markForWarningDisplay_pureSpheroidNotSupported();
	if (announceType==ANNOUNCE_CONTAINS_NON_PURE_NON_CONVEX_SHAPES)
		App::ct->dynamicsContainer->markForWarningDisplay_containsNonPureNonConvexShapes();
	if (announceType==ANNOUNCE_CONTAINS_STATIC_SHAPES_ON_DYNAMIC_CONSTRUCTION)
		App::ct->dynamicsContainer->markForWarningDisplay_containsStaticShapesOnDynamicConstruction();
	if (announceType==ANNOUNCE_PURE_HOLLOW_SHAPE_NOT_SUPPORTED)
		App::ct->dynamicsContainer->markForWarningDisplay_pureHollowShapeNotSupported();
	if (announceType==ANNOUNCE_VORTEX_PLUGIN_IS_DEMO)
		App::ct->dynamicsContainer->markForWarningDisplay_vortexPluginIsDemo();
}

simVoid _simGetVerticesLocalFrame_internal(const simVoid* geometric,simFloat* pos,simFloat* quat)
{
	C_API_FUNCTION_DEBUG;
	C7Vector tr(((CGeometric*)geometric)->getVerticeLocalFrame());
	tr.Q.getInternalData(quat);
	tr.X.getInternalData(pos);
}

const simFloat* _simGetHeightfieldData_internal(const simVoid* geometric,simInt* xCount,simInt* yCount,simFloat* minHeight,simFloat* maxHeight)
{
	C_API_FUNCTION_DEBUG;
	return(((CGeometric*)geometric)->getHeightfieldData(xCount[0],yCount[0],minHeight[0],maxHeight[0]));
}

simVoid _simGetCumulativeMeshes_internal(const simVoid* geomInfo,simFloat** vertices,simInt* verticesSize,simInt** indices,simInt* indicesSize)
{
	C_API_FUNCTION_DEBUG;
	std::vector<float> vert;
	std::vector<int> ind;
	((CGeomWrap*)geomInfo)->getCumulativeMeshes(vert,&ind,NULL);

	vertices[0]=new float[vert.size()];
	verticesSize[0]=vert.size();
	for (int i=0;i<int(vert.size());i++)
		vertices[0][i]=vert[i];
	indices[0]=new int[ind.size()];
	indicesSize[0]=ind.size();
	for (int i=0;i<int(ind.size());i++)
		indices[0][i]=ind[i];
}

simBool _simGetBulletCollisionMargin_internal(const simVoid* geomInfo,simFloat* margin,simInt* otherProp)
{
	C_API_FUNCTION_DEBUG;
	CGeomWrap* geomWrap=(CGeomWrap*)geomInfo;
	CDynMaterialObject* mat=geomWrap->getDynMaterialObject();
	bool convexAndNotPure=(geomWrap->isConvex()&&(!geomWrap->isPure()));
	margin[0]=mat->getBulletNonDefaultCollisionMarginFactor(convexAndNotPure);
	if (otherProp!=NULL)
	{
		otherProp[0]=0;
		if (mat->getBulletAutoShrinkConvexMesh())
			otherProp[0]|=1;
	}
	return(mat->getBulletNonDefaultCollisionMargin(convexAndNotPure));
}

simBool _simGetBulletStickyContact_internal(const simVoid* geomInfo)
{
	C_API_FUNCTION_DEBUG;
	CGeomWrap* geomWrap=(CGeomWrap*)geomInfo;
	CDynMaterialObject* mat=geomWrap->getDynMaterialObject();
	return(mat->getBulletStickyContact());	
}

simInt _simGetObjectID_internal(const simVoid* object)
{
	C_API_FUNCTION_DEBUG;
	return(((C3DObject*)object)->getID());
}

simVoid _simGetObjectLocalTransformation_internal(const simVoid* object,simFloat* pos,simFloat* quat,simBool excludeFirstJointTransformation)
{
	C_API_FUNCTION_DEBUG;
	C7Vector tr;
	if (excludeFirstJointTransformation)
		tr=((C3DObject*)object)->getLocalTransformationPart1();
	else
		tr=((C3DObject*)object)->getLocalTransformation();
	tr.X.getInternalData(pos);
	tr.Q.getInternalData(quat);
}

simVoid _simSetObjectLocalTransformation_internal(simVoid* object,const simFloat* pos,const simFloat* quat)
{
	C_API_FUNCTION_DEBUG;
	C7Vector tr;
	tr.X.setInternalData(pos);
	tr.Q.setInternalData(quat);
//	printf("%u, %s\n",object,((C3DObject*)object)->getName().c_str());
	((C3DObject*)object)->setLocalTransformation(tr);
//	printf("ok\n");
}

simVoid _simGetObjectCumulativeTransformation_internal(const simVoid* object,simFloat* pos,simFloat* quat,simBool excludeFirstJointTransformation)
{
	C_API_FUNCTION_DEBUG;
	C7Vector tr;
	if (excludeFirstJointTransformation!=0)
		tr=((C3DObject*)object)->getCumulativeTransformationPart1();
	else
		tr=((C3DObject*)object)->getCumulativeTransformation();
	if (pos!=NULL)
		tr.X.getInternalData(pos);
	if (quat!=NULL)
		tr.Q.getInternalData(quat);
}

const simVoid* _simGetGeomProxyFromShape_internal(const simVoid* shape)
{
	C_API_FUNCTION_DEBUG;
	return(((CShape*)shape)->geomData);
}

simFloat _simGetMass_internal(const simVoid* geomInfo)
{
	C_API_FUNCTION_DEBUG;
	return(((CGeomWrap*)geomInfo)->getMass());
}

simBool _simIsShapeDynamicallyStatic_internal(const simVoid* shape)
{
	C_API_FUNCTION_DEBUG;
	return(((CShape*)shape)->getShapeIsDynamicallyStatic());
}

simVoid _simGetInitialDynamicVelocity_internal(const simVoid* shape,simFloat* vel)
{
	C_API_FUNCTION_DEBUG;
	((CShape*)shape)->getInitialDynamicVelocity().getInternalData(vel);
}

simVoid _simSetInitialDynamicVelocity_internal(simVoid* shape,const simFloat* vel)
{
	C_API_FUNCTION_DEBUG;
	((CShape*)shape)->setInitialDynamicVelocity(C3Vector(vel));
}

simVoid _simGetInitialDynamicAngVelocity_internal(const simVoid* shape,simFloat* angularVel)
{
	C_API_FUNCTION_DEBUG;
	((CShape*)shape)->getInitialDynamicAngularVelocity().getInternalData(angularVel);
}

simVoid _simSetInitialDynamicAngVelocity_internal(simVoid* shape,const simFloat* angularVel)
{
	C_API_FUNCTION_DEBUG;
	((CShape*)shape)->setInitialDynamicAngularVelocity(C3Vector(angularVel));
}

simVoid _simGetDamping_internal(const simVoid* geomInfo,simFloat* linDamping,simFloat* angDamping)
{
	C_API_FUNCTION_DEBUG;
	CGeomWrap* geomWrap=(CGeomWrap*)geomInfo;
	CDynMaterialObject* mat=geomWrap->getDynMaterialObject();

	if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_bullet)
	{
		linDamping[0]=mat->getBulletLinearDamping();
		angDamping[0]=mat->getBulletAngularDamping();
	}
	if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_ode)
	{
		linDamping[0]=mat->getOdeLinearDamping();
		angDamping[0]=mat->getOdeAngularDamping();
	}
	// Vortex uses a different routine
}

simFloat _simGetFriction_internal(const simVoid* geomInfo)
{
	C_API_FUNCTION_DEBUG;
	CGeomWrap* geomWrap=(CGeomWrap*)geomInfo;
	CDynMaterialObject* mat=geomWrap->getDynMaterialObject();

	if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_bullet)
		return(mat->getBulletFriction());
	if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_ode)
		return(mat->getOdeFriction());
	// Vortex uses another routine
	return(0.0f);
}

simFloat _simGetBulletRestitution_internal(const simVoid* geomInfo)
{
	C_API_FUNCTION_DEBUG;
	CGeomWrap* geomWrap=(CGeomWrap*)geomInfo;
	CDynMaterialObject* mat=geomWrap->getDynMaterialObject();
	return(mat->getBulletRestitution());
}

simBool _simGetStartSleeping_internal(const simVoid* shape)
{
	C_API_FUNCTION_DEBUG;
	return(((CShape*)shape)->getStartInDynamicSleeping());
}

simBool _simGetWasPutToSleepOnce_internal(const simVoid* shape)
{ // flag is set to true whenever called!!!
	C_API_FUNCTION_DEBUG;
	bool a=((CShape*)shape)->getRigidBodyWasAlreadyPutToSleepOnce();
	((CShape*)shape)->setRigidBodyWasAlreadyPutToSleepOnce(true);
	return(a);
}

simBool _simIsShapeDynamicallyRespondable_internal(const simVoid* shape)
{
	C_API_FUNCTION_DEBUG;
	return(((CShape*)shape)->getRespondable());
}

simInt _simGetDynamicCollisionMask_internal(const simVoid* shape)
{
	C_API_FUNCTION_DEBUG;
	return(((CShape*)shape)->getDynamicCollisionMask());
}

const simVoid* _simGetLastParentForLocalGlobalCollidable_internal(const simVoid* shape)
{
	C_API_FUNCTION_DEBUG;
	return(((CShape*)shape)->getLastParentForLocalGlobalCollidable());
}

simVoid _simSetGeomProxyDynamicsFullRefreshFlag_internal(simVoid* geomData,simBool flag)
{
	C_API_FUNCTION_DEBUG;
	((CGeomProxy*)geomData)->setDynamicsFullRefreshFlag(flag!=0);
}

simBool _simGetGeomProxyDynamicsFullRefreshFlag_internal(const simVoid* geomData)
{
	C_API_FUNCTION_DEBUG;
	return(((CGeomProxy*)geomData)->getDynamicsFullRefreshFlag());
}

simBool _simGetDynamicsFullRefreshFlag_internal(const simVoid* object)
{
	C_API_FUNCTION_DEBUG;
	return(((C3DObject*)object)->getDynamicsFullRefreshFlag());
}

simVoid _simSetDynamicsFullRefreshFlag_internal(const simVoid* object,simBool flag)
{
	C_API_FUNCTION_DEBUG;
	((C3DObject*)object)->setDynamicsFullRefreshFlag(flag!=0);
}


simBool _simGetParentFollowsDynamic_internal(const simVoid* shape)
{
	C_API_FUNCTION_DEBUG;
	return(((CShape*)shape)->getParentFollowsDynamic());
}

const simVoid* _simGetParentObject_internal(const simVoid* object)
{
	C_API_FUNCTION_DEBUG;
	return(((C3DObject*)object)->getParent());
}

simVoid _simSetObjectCumulativeTransformation_internal(simVoid* object,const simFloat* pos,const simFloat* quat,simBool keepChildrenInPlace)
{
	C_API_FUNCTION_DEBUG;
	C7Vector tr;
	tr.X.setInternalData(pos);
	tr.Q.setInternalData(quat);
	App::ct->objCont->setAbsoluteConfiguration(((C3DObject*)object)->getID(),tr,keepChildrenInPlace!=0);
}

simVoid _simSetShapeDynamicVelocity_internal(simVoid* shape,const simFloat* linear,const simFloat* angular)
{
	C_API_FUNCTION_DEBUG;
	((CShape*)shape)->setDynamicVelocity(linear,angular);
}

simVoid _simGetAdditionalForceAndTorque_internal(const simVoid* shape,simFloat* force,simFloat* torque)
{
	C_API_FUNCTION_DEBUG;
	((CShape*)shape)->getAdditionalForce().getInternalData(force);
	((CShape*)shape)->getAdditionalTorque().getInternalData(torque);
}

simVoid _simClearAdditionalForceAndTorque_internal(const simVoid* shape)
{
	C_API_FUNCTION_DEBUG;
	((CShape*)shape)->clearAdditionalForceAndTorque();
}

simBool _simGetJointPositionInterval_internal(const simVoid* joint,simFloat* minValue,simFloat* rangeValue)
{
	C_API_FUNCTION_DEBUG;
	if (minValue!=NULL)
		minValue[0]=((CJoint*)joint)->getPositionIntervalMin();
	if (rangeValue!=NULL)
		rangeValue[0]=((CJoint*)joint)->getPositionIntervalRange();
	return(!((CJoint*)joint)->getPositionIsCyclic());
}

const simVoid* _simGetObject_internal(int objID)
{
	C_API_FUNCTION_DEBUG;
	return(App::ct->objCont->getObject(objID));
}

const simVoid* _simGetIkGroupObject_internal(int ikGroupID)
{
	C_API_FUNCTION_DEBUG;
	return(App::ct->ikGroups->getIkGroup(ikGroupID));
}

simInt _simMpHandleIkGroupObject_internal(const simVoid* ikGroup)
{
	C_API_FUNCTION_DEBUG;
	return(((CikGroup*)ikGroup)->computeGroupIk(true));
}

simInt _simGetJointType_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getJointType());
}

simVoid _simGetJointOdeParameters_internal(const simVoid* joint,simFloat* stopERP,simFloat* stopCFM,simFloat* bounce,simFloat* fudge,simFloat* normalCFM)
{
	C_API_FUNCTION_DEBUG;
	((CJoint*)joint)->getOdeParameters(stopERP[0],stopCFM[0],bounce[0],fudge[0],normalCFM[0]);
}

simVoid _simGetJointBulletParameters_internal(const simVoid* joint,simFloat* stopERP,simFloat* stopCFM,simFloat* normalCFM)
{
	C_API_FUNCTION_DEBUG;
	((CJoint*)joint)->getBulletParameters(stopERP[0],stopCFM[0],normalCFM[0]);
}

simBool _simIsForceSensorBroken_internal(const simVoid* forceSensor)
{
	C_API_FUNCTION_DEBUG;
	return(((CForceSensor*)forceSensor)->getForceSensorIsBroken());
}

simVoid _simGetDynamicForceSensorLocalTransformationPart2_internal(const simVoid* forceSensor,simFloat* pos,simFloat* quat)
{
	C_API_FUNCTION_DEBUG;
	C7Vector tr(((CForceSensor*)forceSensor)->getDynamicSecondPartLocalTransform());
	tr.X.getInternalData(pos);
	tr.Q.getInternalData(quat);
}

simBool _simIsDynamicMotorEnabled_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getEnableDynamicMotor());
}

simBool _simIsDynamicMotorPositionCtrlEnabled_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getEnableDynamicMotorControlLoop());
}

simVoid _simGetMotorPid_internal(const simVoid* joint,simFloat* pParam,simFloat* iParam,simFloat* dParam)
{
	C_API_FUNCTION_DEBUG;
	((CJoint*)joint)->getDynamicMotorPositionControlParameters(pParam[0],iParam[0],dParam[0]);
}

simFloat _simGetDynamicMotorTargetPosition_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getDynamicMotorPositionControlTargetPosition());
}

simFloat _simGetDynamicMotorTargetVelocity_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getDynamicMotorTargetVelocity());
}

simBool _simIsDynamicMotorTorqueModulationEnabled_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getEnableTorqueModulation());
}

simFloat _simGetDynamicMotorMaxForce_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getDynamicMotorMaximumForce());
}

simFloat _simGetDynamicMotorUpperLimitVelocity_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getDynamicMotorUpperLimitVelocity());
}

simVoid _simSetDynamicMotorReflectedPositionFromDynamicEngine_internal(simVoid* joint,simFloat pos)
{
	C_API_FUNCTION_DEBUG;
	((CJoint*)joint)->setDynamicMotorReflectedPosition_useOnlyFromDynamicPart(pos);
}

simVoid _simSetJointSphericalTransformation_internal(simVoid* joint,const simFloat* quat)
{
	C_API_FUNCTION_DEBUG;
	((CJoint*)joint)->setSphericalTransformation(quat);
}

simVoid _simSetDynamicJointLocalTransformationPart2_internal(simVoid* joint,const simFloat* pos,const simFloat* quat)
{
	C_API_FUNCTION_DEBUG;
	C7Vector tr;
	tr.X.setInternalData(pos);
	tr.Q.setInternalData(quat);
	((CJoint*)joint)->setDynamicSecondPartLocalTransform(tr);
}

simVoid _simSetDynamicForceSensorLocalTransformationPart2_internal(simVoid* forceSensor,const simFloat* pos,const simFloat* quat)
{
	C_API_FUNCTION_DEBUG;
	C7Vector tr;
	tr.X.setInternalData(pos);
	tr.Q.setInternalData(quat);
	((CForceSensor*)forceSensor)->setDynamicSecondPartLocalTransform(tr);
}

simVoid _simSetDynamicJointLocalTransformationPart2IsValid_internal(simVoid* joint,simBool valid)
{
	C_API_FUNCTION_DEBUG;
	((CJoint*)joint)->setDynamicSecondPartIsValid(valid!=0);
}

simVoid _simSetDynamicForceSensorLocalTransformationPart2IsValid_internal(simVoid* forceSensor,simBool valid)
{
	C_API_FUNCTION_DEBUG;
	((CForceSensor*)forceSensor)->setDynamicSecondPartIsValid(valid!=0);
}

simVoid _simAddForceSensorCumulativeForcesAndTorques_internal(simVoid* forceSensor,const simFloat* force,const simFloat* torque,int totalPassesCount)
{
	C_API_FUNCTION_DEBUG;
	((CForceSensor*)forceSensor)->addCumulativeForcesAndTorques(force,torque,totalPassesCount);
}

simVoid _simAddJointCumulativeForcesOrTorques_internal(simVoid* joint,float forceOrTorque,int totalPassesCount)
{
	C_API_FUNCTION_DEBUG;
	((CJoint*)joint)->addCumulativeForceOrTorque(forceOrTorque,totalPassesCount);
}

simInt _simGetObjectListSize_internal(simInt objType)
{
	C_API_FUNCTION_DEBUG;
	if (objType==sim_object_shape_type)
		return(int(App::ct->objCont->shapeList.size()));
	if (objType==sim_object_joint_type)
		return(int(App::ct->objCont->jointList.size()));
	if (objType==sim_handle_all)
		return(int(App::ct->objCont->objectList.size())); // we put it also here for faster access!
	if (objType==sim_object_dummy_type)
		return(int(App::ct->objCont->dummyList.size()));
	if (objType==sim_object_graph_type)
		return(int(App::ct->objCont->graphList.size()));
	if (objType==sim_object_camera_type)
		return(int(App::ct->objCont->cameraList.size()));
	if (objType==sim_object_proximitysensor_type)
		return(int(App::ct->objCont->proximitySensorList.size()));
	if (objType==sim_object_path_type)
		return(int(App::ct->objCont->pathList.size()));
	if (objType==sim_object_visionsensor_type)
		return(int(App::ct->objCont->visionSensorList.size()));
	if (objType==sim_object_volume_type)
		return(int(App::ct->objCont->volumeList.size()));
	if (objType==sim_object_mill_type)
		return(int(App::ct->objCont->millList.size()));
	if (objType==sim_object_forcesensor_type)
		return(int(App::ct->objCont->forceSensorList.size()));
	if (objType==sim_object_light_type)
		return(int(App::ct->objCont->lightList.size()));
	if (objType==sim_object_mirror_type)
		return(int(App::ct->objCont->mirrorList.size()));
	if (objType==-1)
		return(int(App::ct->objCont->orphanList.size()));
	return(int(App::ct->objCont->objectList.size()));
}

const simVoid* _simGetObjectFromIndex_internal(simInt objType,simInt index)
{
	C_API_FUNCTION_DEBUG;
	if (objType==sim_object_shape_type)
		return(App::ct->objCont->getObject(App::ct->objCont->shapeList[index]));
	if (objType==sim_object_joint_type)
		return(App::ct->objCont->getObject(App::ct->objCont->jointList[index]));
	if (objType==sim_handle_all)
		return(App::ct->objCont->getObject(App::ct->objCont->objectList[index])); // we put it also here for faster access!
	if (objType==sim_object_dummy_type)
		return(App::ct->objCont->getObject(App::ct->objCont->dummyList[index]));
	if (objType==sim_object_graph_type)
		return(App::ct->objCont->getObject(App::ct->objCont->graphList[index]));
	if (objType==sim_object_camera_type)
		return(App::ct->objCont->getObject(App::ct->objCont->cameraList[index]));
	if (objType==sim_object_proximitysensor_type)
		return(App::ct->objCont->getObject(App::ct->objCont->proximitySensorList[index]));
	if (objType==sim_object_path_type)
		return(App::ct->objCont->getObject(App::ct->objCont->pathList[index]));
	if (objType==sim_object_visionsensor_type)
		return(App::ct->objCont->getObject(App::ct->objCont->visionSensorList[index]));
	if (objType==sim_object_volume_type)
		return(App::ct->objCont->getObject(App::ct->objCont->volumeList[index]));
	if (objType==sim_object_mill_type)
		return(App::ct->objCont->getObject(App::ct->objCont->millList[index]));
	if (objType==sim_object_forcesensor_type)
		return(App::ct->objCont->getObject(App::ct->objCont->forceSensorList[index]));
	if (objType==sim_object_light_type)
		return(App::ct->objCont->getObject(App::ct->objCont->lightList[index]));
	if (objType==sim_object_mirror_type)
		return(App::ct->objCont->getObject(App::ct->objCont->mirrorList[index]));
	if (objType==-1)
		return(App::ct->objCont->getObject(App::ct->objCont->orphanList[index]));
	return(App::ct->objCont->getObject(App::ct->objCont->objectList[index]));
}

simInt _simGetContactCallbackCount_internal()
{
	C_API_FUNCTION_DEBUG;
	return(CIloIlo::allContactCallbacks.size());
}

const void* _simGetContactCallback_internal(simInt index)
{
	C_API_FUNCTION_DEBUG;
	return((const void*)CIloIlo::allContactCallbacks[index]);
}

simVoid _simSetDynamicSimulationIconCode_internal(simVoid* object,simInt code)
{
	C_API_FUNCTION_DEBUG;
	((C3DObject*)object)->setDynamicSimulationIconCode(code);
}

simVoid _simSetDynamicObjectFlagForVisualization_internal(simVoid* object,simInt flag)
{
	C_API_FUNCTION_DEBUG;
	((C3DObject*)object)->setDynamicObjectFlag_forVisualization(flag);
}

simInt _simGetTreeDynamicProperty_internal(const simVoid* object)
{
	C_API_FUNCTION_DEBUG;
	return(((C3DObject*)object)->getTreeDynamicProperty());
}

simVoid _simGetOdeMaxContactFrictionCFMandERP_internal(const simVoid* geomInfo,simInt* maxContacts,simFloat* friction,simFloat* cfm,simFloat* erp)
{
	C_API_FUNCTION_DEBUG;
	CGeomWrap* geomWrap=(CGeomWrap*)geomInfo;
	CDynMaterialObject* mat=geomWrap->getDynMaterialObject();
	maxContacts[0]=mat->getOdeMaxContacts();
	friction[0]=mat->getOdeFriction();
	cfm[0]=mat->getOdeSoftCFM();
	erp[0]=mat->getOdeSoftERP();
}

simInt _simGetObjectType_internal(const simVoid* object)
{
	C_API_FUNCTION_DEBUG;
	return(((C3DObject*)object)->getObjectType());
}

simVoid _simSetShapeIsStaticAndNotRespondableButDynamicTag_internal(const simVoid* shape,simBool tag)
{
	C_API_FUNCTION_DEBUG;
	((CShape*)shape)->setShapeIsStaticAndNotRespondableButDynamicTag(tag!=0);
}

simBool _simGetShapeIsStaticAndNotRespondableButDynamicTag_internal(const simVoid* shape)
{
	C_API_FUNCTION_DEBUG;
	return(((CShape*)shape)->getShapeIsStaticAndNotRespondableButDynamicTag());
}

const simVoid** _simGetObjectChildren_internal(const simVoid* object,simInt* count)
{
	C_API_FUNCTION_DEBUG;
	C3DObject* it=(C3DObject*)object;
	count[0]=int(it->childList.size());
	if (count[0]!=0)
		return((const void**)&it->childList[0]);
	return(NULL);
}

simInt _simGetDummyLinkType_internal(const simVoid* dummy,simInt* linkedDummyID)
{
	C_API_FUNCTION_DEBUG;
	int dType=((CDummy*)dummy)->getLinkType();
	if (linkedDummyID!=NULL)
		linkedDummyID[0]=((CDummy*)dummy)->getLinkedDummyID();
	return(dType);
}

simInt _simGetJointMode_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getJointMode());
}

simBool _simIsJointInHybridOperation_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getHybridFunctionality());
}

simVoid _simDisableDynamicTreeForManipulation_internal(const simVoid* object,simBool disableFlag)
{
	C_API_FUNCTION_DEBUG;
	((C3DObject*)object)->disableDynamicTreeForManipulation(disableFlag!=0);
}

simVoid _simSetJointPosition_internal(const simVoid* joint,simFloat pos)
{
	C_API_FUNCTION_DEBUG;
	((CJoint*)joint)->setPosition(pos,false);
}

simFloat _simGetJointPosition_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getPosition());
}

simVoid _simSetDynamicMotorPositionControlTargetPosition_internal(const simVoid* joint,simFloat pos)
{
	C_API_FUNCTION_DEBUG;
	((CJoint*)joint)->setDynamicMotorPositionControlTargetPosition(pos);
}

simVoid _simGetGravity_internal(simFloat* gravity)
{
	C_API_FUNCTION_DEBUG;
	App::ct->dynamicsContainer->getGravity().getInternalData(gravity);
}

simInt _simGetTimeDiffInMs_internal(simInt previousTime)
{
	C_API_FUNCTION_DEBUG;
	return(VDateTime::getTimeDiffInMs(previousTime));
}

simBool _simDoEntitiesCollide_internal(simInt entity1ID,simInt entity2ID,simInt* cacheBuffer,simBool overrideCollidableFlagIfShape1,simBool overrideCollidableFlagIfShape2,simBool pathOrMotionPlanningRoutineCalling)
{
	C_API_FUNCTION_DEBUG;
	return(CCollisionRoutine::doEntitiesCollide(entity1ID,entity2ID,NULL,cacheBuffer,overrideCollidableFlagIfShape1!=0,overrideCollidableFlagIfShape2!=0,pathOrMotionPlanningRoutineCalling!=0,false));
}

simBool _simGetDistanceBetweenEntitiesIfSmaller_internal(simInt entity1ID,simInt entity2ID,simFloat* distance,simFloat* ray,simInt* cacheBuffer,simBool overrideMeasurableFlagIfNonCollection1,simBool overrideMeasurableFlagIfNonCollection2,simBool pathPlanningRoutineCalling)
{
	C_API_FUNCTION_DEBUG;
	return(CDistanceRoutine::getDistanceBetweenEntitiesIfSmaller(entity1ID,entity2ID,distance[0],ray,cacheBuffer,overrideMeasurableFlagIfNonCollection1!=0,overrideMeasurableFlagIfNonCollection2!=0,pathPlanningRoutineCalling!=0,false));
}

simInt _simHandleJointControl_internal(const simVoid* joint,simInt auxV,const simInt* inputValuesInt,const simFloat* inputValuesFloat,simFloat* outputValues)
{
	C_API_FUNCTION_DEBUG;
	float outVelocity=0.0f;
	float outForce=0.0f;
	((CJoint*)joint)->handleDynJointControl((auxV&1)!=0,inputValuesInt[0],inputValuesInt[1],inputValuesFloat[0],inputValuesFloat[1],inputValuesFloat[2],inputValuesFloat[3],outVelocity,outForce);
	outputValues[0]=outVelocity;
	outputValues[1]=outForce;
	return(2);
}

simInt _simHandleCustomContact_internal(simInt objHandle1,simInt objHandle2,simInt engine,simInt* dataInt,simFloat* dataFloat)
{
	C_API_FUNCTION_DEBUG;
	if (App::ct->environment->getEnableCustomContactHandlingViaScript())
	{
		CLuaScriptObject* script=App::ct->luaScriptContainer->getCustomContactHandlingScript_callback();
		if (script)
		{
			int inDataInt[3]={objHandle1,objHandle2,engine};
			int retVal=script->runContactCallback(inDataInt,dataInt,dataFloat);
			if (retVal==0)
				return(0); // no collision
			if (retVal>0)
				return(1);
		}
	}

	// If we arrive here this means that the custom lua callback didn't exist or didn't handle the contact.

	// We check if a plugin wants to handle the contact:	
	int callbackCount=CIloIlo::allContactCallbacks.size();
	if (callbackCount!=0)
	{
		for (int i=0;i<callbackCount;i++)
		{
			int res=((contactCallback)CIloIlo::allContactCallbacks[i])(objHandle1,objHandle2,engine,dataInt,dataFloat);
			if (res==0)
				return(0); // override... we don't wanna collide
			if (res>0)
				return(1); // override... we want the custom values
		}
	}
	return(-1); // we let V-REP handle the contact
}

simFloat _simGetPureHollowScaling_internal(const simVoid* geometric)
{
	C_API_FUNCTION_DEBUG;
	return(((CGeometric*)geometric)->getPurePrimitiveInsideScaling());
}

simInt _simGetJointCallbackCallOrder_internal(const simVoid* joint)
{
	C_API_FUNCTION_DEBUG;
	return(((CJoint*)joint)->getJointCallbackCallOrder());
}


simVoid _simGetVortexParameters_internal(const simVoid* object,simInt version,simFloat* floatParams,simInt* intParams)
{ // if object is NULL, we return general engine settings, if object is a shape, we return shape settings, otherwise joint settings
	// Version allows to adjust for future extensions.
	C_API_FUNCTION_DEBUG;
	std::vector<float> fparams;
	std::vector<int> iparams;
	int icnt=0;
	int fcnt=0;
	if (object==NULL)
	{
		App::ct->dynamicsContainer->getVortexFloatParams(fparams);
		App::ct->dynamicsContainer->getVortexIntParams(iparams);
		if (version==0)
		{
			fcnt=VORTEX_FLOAT_PARAM_CNT_0_OLD;
			icnt=VORTEX_INT_PARAM_CNT_0_OLD;
		}
		if (version==1)
		{
			fcnt=VORTEX_FLOAT_PARAM_CNT_1_OLD;
			icnt=VORTEX_INT_PARAM_CNT_1_OLD;
		}
		if (version>=2)
		{ // when the dynamics plugin is the same version as V-REP, or newer!
			fcnt=VORTEX_FLOAT_PARAM_CNT_2;
			icnt=VORTEX_INT_PARAM_CNT_2;
		}
	}
	else
	{
		C3DObject* obj=(C3DObject*)object;
		if (obj->getObjectType()==sim_object_shape_type)
		{
			CShape* shape=(CShape*)object;
			CDynMaterialObject* mat=shape->geomData->geomInfo->getDynMaterialObject();
			mat->getVortexFloatParams(fparams);
			mat->getVortexIntParams(iparams);
			if (version==0)
			{
				fcnt=VORTEX_SHAPE_FLOAT_PARAM_CNT_0_OLD;
				icnt=VORTEX_SHAPE_INT_PARAM_CNT_0_OLD;
			}
			if (version==1)
			{
				fcnt=VORTEX_SHAPE_FLOAT_PARAM_CNT_1_OLD;
				icnt=VORTEX_SHAPE_INT_PARAM_CNT_1_OLD;
			}
			if (version>=2)
			{ // when the dynamics plugin is the same version as V-REP, or newer!
				fcnt=VORTEX_SHAPE_FLOAT_PARAM_CNT_2;
				icnt=VORTEX_SHAPE_INT_PARAM_CNT_2;
			}
		}
		if (obj->getObjectType()==sim_object_joint_type)
		{
			CJoint* joint=(CJoint*)object;
			joint->getVortexFloatParams(fparams);
			joint->getVortexIntParams(iparams);
			if (version==0)
			{
				fcnt=VORTEX_JOINT_FLOAT_PARAM_CNT_0_OLD;
				icnt=VORTEX_JOINT_INT_PARAM_CNT_0_OLD;
			}
			if (version==1)
			{
				fcnt=VORTEX_JOINT_FLOAT_PARAM_CNT_1_OLD;
				icnt=VORTEX_JOINT_INT_PARAM_CNT_1_OLD;
			}
			if (version>=2)
			{ // when the dynamics plugin is the same version as V-REP, or newer!
				fcnt=VORTEX_JOINT_FLOAT_PARAM_CNT_2;
				icnt=VORTEX_JOINT_INT_PARAM_CNT_2;
			}
		}
	}
	for (int i=0;i<fcnt;i++)
		floatParams[i]=fparams[i];
	for (int i=0;i<icnt;i++)
		intParams[i]=iparams[i];
}

//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************
