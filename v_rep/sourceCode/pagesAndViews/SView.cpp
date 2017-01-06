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

// This file requires some serious refactoring!

#include "vrepPrecompiledHeader.h"
#include "v_rep_internal.h"
#include "SView.h"
#include "Tt.h"
#include "OGL.h"
#include "OglSurface.h"
#include "IloIlo.h"
#include "Simulation.h"
#include "ObjectEdition.h"
#include "Simulation.h"
#include "AddMenu.h"
#include "TriangleVertexEdition.h"

#include "PathEdition.h"
#include "v_repStrings.h"
#include "VDateTime.h"
#include "proxSensorRoutine.h"
#include "pluginContainer.h"
#include "App.h"

CSView::CSView(int theLinkedObjectID)
{
	setDefaultValues();
	linkedObjectID=theLinkedObjectID;
	static int nextUniqueID=10000;
	_uniqueID=nextUniqueID++;
	_initialValuesInitialized=false;
}

CSView::~CSView()
{

}
void CSView::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		_initialPerspectiveDisplay=perspectiveDisplay;
		_initialShowEdges=_showEdges;
		_initialThickEdges=_thickEdges;
		_initialRenderingMode=_renderingMode;
		_initialGraphIsTimeGraph=graphIsTimeGraph;
	}
}

bool CSView::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
	{
		perspectiveDisplay=_initialPerspectiveDisplay;
		_showEdges=_initialShowEdges;
		_thickEdges=_initialThickEdges;
		_renderingMode=_initialRenderingMode;
		graphIsTimeGraph=_initialGraphIsTimeGraph;
	}
	_initialValuesInitialized=false;
	return(_removeFloatingViewAtSimulationEnd);
}

int CSView::getUniqueID()
{
	return(_uniqueID);
}

void CSView::setDefaultValues()
{
	dontActivatePopup=false;
	linkedObjectID=-1;
	perspectiveDisplay=true;
	_showEdges=true;
	_thickEdges=false;
	_removeFloatingViewAtSimulationEnd=false;
	_renderingMode=RENDERING_MODE_SOLID;
	_xyGraphInAutoModeDuringSimulation=true;
	_timeGraphXInAutoModeDuringSimulation=true;
	_timeGraphYInAutoModeDuringSimulation=true;
	_xyGraphIsOneOneProportional=false;
	_canSwapViewWithMainView=true;
	_canBeClosed=true;
	_canBeShifted=true;
	_canBeResized=true;

	_fitSceneToView=false;
	_fitSelectionToView=false;

	graphPosition[0]=-0.5f;
	graphPosition[1]=-0.5f;
	graphSize[0]=1.0f;
	graphSize[1]=1.0f;
	_relativeViewPositionOffset=0.0f;
	_trackedCurveIndex=-1;

	rightButtonIsCaught=false;
	mouseIsDown=false;
	mouseJustWentDownWasProcessed=true;
	mouseJustWentDownFlag=false;
	mouseJustWentUpFlag=false;
	selectionStatus=NOSELECTION;
	_caughtElements=0;	
	dontActivatePopup=false;
	linkedObjectID=-1;
	graphIsTimeGraph=true;
	graphPosition[0]=-0.5f;
	graphPosition[1]=-0.5f;
	graphSize[0]=1.0f;
	graphSize[1]=1.0f;
	_relativeViewPositionOffset=0.0f;

	rightButtonIsCaught=false;
	mouseIsDown=false;
	mouseJustWentDownWasProcessed=true;
	mouseJustWentDownFlag=false;
	mouseJustWentUpFlag=false;
	selectionStatus=NOSELECTION;
	_caughtElements=0;
}

int CSView::getCaughtElements()
{
	return(_caughtElements);
}

void CSView::clearCaughtElements(int keepMask)
{
	_caughtElements&=keepMask;
}

void CSView::setCenterPosition(float pos[3])
{
	centerPosition[0]=pos[0];
	centerPosition[1]=pos[1];
	centerPosition[2]=pos[2];
}
void CSView::getCenterPosition(float pos[3])
{
	pos[0]=centerPosition[0];
	pos[1]=centerPosition[1];
	pos[2]=centerPosition[2];
}
void CSView::setMousePositionDepth(float depth)
{
	mousePositionDepth=depth;
}
float CSView::getMousePositionDepth()
{
	return(mousePositionDepth);
}

void CSView::clearMouseJustWentDownAndUpFlag()
{
	if (mouseJustWentUpFlag)
		mouseIsDown=false;
	mouseJustWentDownFlag=false;
	mouseJustWentUpFlag=false;
}

void CSView::setViewSizeAndPosition(int sizeX,int sizeY,int posX,int posY)
{
	_viewSize[0]=sizeX;
	_viewSize[1]=sizeY;
	_viewPosition[0]=posX;
	_viewPosition[1]=posY;
}

void CSView::getViewSize(int size[2])
{
	size[0]=_viewSize[0];
	size[1]=_viewSize[1];
}

void CSView::getViewPosition(int pos[2])
{
	pos[0]=_viewPosition[0];
	pos[1]=_viewPosition[1];
}

int CSView::getLinkedObjectID()
{
	return(linkedObjectID);
}
void CSView::setLinkedObjectID(int theNewLinkedObjectID,bool noDefaultValues)
{
	if (!noDefaultValues)
		setDefaultValues();
	linkedObjectID=theNewLinkedObjectID;
}

void CSView::setAlternativeViewName(const char* name)
{
	_alternativeViewName=name;
}

std::string CSView::getAlternativeViewName()
{
	return(_alternativeViewName);
}

void CSView::setPerspectiveDisplay(bool perspective)
{
	perspectiveDisplay=perspective;
}

bool CSView::getPerspectiveDisplay()
{
	return(perspectiveDisplay);
}

void CSView::setShowEdges(bool edges)
{
	_showEdges=edges;
}

bool CSView::getShowEdges()
{
	return(_showEdges);
}

void CSView::setThickEdges(bool thickEdges)
{
	_thickEdges=thickEdges;
}

bool CSView::getThickEdges()
{
	return(_thickEdges);
}


void CSView::setRenderingMode(int mode)
{
	_renderingMode=mode;
}

void CSView::setRemoveFloatingViewAtSimulationEnd(bool remove)
{

	_removeFloatingViewAtSimulationEnd=remove;
}

bool CSView::getRemoveFloatingViewAtSimulationEnd()
{
	return(_removeFloatingViewAtSimulationEnd);
}

int CSView::getRenderingMode()
{
	return(_renderingMode);
}

void CSView::setXYGraphAutoModeDuringSimulation(bool autoMode)
{
	_xyGraphInAutoModeDuringSimulation=autoMode;
}

bool CSView::getXYGraphAutoModeDuringSimulation()
{
	return(_xyGraphInAutoModeDuringSimulation);
}

void CSView::setTimeGraphXAutoModeDuringSimulation(bool autoMode)
{
	_timeGraphXInAutoModeDuringSimulation=autoMode;
}

bool CSView::getTimeGraphXAutoModeDuringSimulation()
{
	return(_timeGraphXInAutoModeDuringSimulation);
}

void CSView::setTimeGraphYAutoModeDuringSimulation(bool autoMode)
{
	_timeGraphYInAutoModeDuringSimulation=autoMode;
}

bool CSView::getTimeGraphYAutoModeDuringSimulation()
{
	return(_timeGraphYInAutoModeDuringSimulation);
}

void CSView::setXYGraphIsOneOneProportional(bool autoMode)
{
	_xyGraphIsOneOneProportional=autoMode;
}

bool CSView::getXYGraphIsOneOneProportional()
{
	return(_xyGraphIsOneOneProportional);
}

void CSView::setTimeGraph(bool timeGraph)
{
	graphIsTimeGraph=timeGraph;
}

bool CSView::getTimeGraph()
{
	return(graphIsTimeGraph);
}

void CSView::setCanSwapViewWithMainView(bool canDoIt)
{
	_canSwapViewWithMainView=canDoIt;
}

bool CSView::getCanSwapViewWithMainView()
{
	return(_canSwapViewWithMainView);
}

void CSView::setCanBeClosed(bool canDoIt)
{
	_canBeClosed=canDoIt;
}

bool CSView::getCanBeClosed()
{
	return(_canBeClosed);
}

void CSView::setCanBeShifted(bool canDoIt)
{
	_canBeShifted=canDoIt;
}

bool CSView::getCanBeShifted()
{
	return(_canBeShifted);
}

void CSView::setCanBeResized(bool canDoIt)
{
	_canBeResized=canDoIt;
}

bool CSView::getCanBeResized()
{
	return(_canBeResized);
}

void CSView::setGraphPosition(float x,float y)
{
	graphPosition[0]=x;
	graphPosition[1]=y;
}

void CSView::getGraphPosition(float position[2])
{
	position[0]=graphPosition[0];
	position[1]=graphPosition[1];
}

void CSView::setGraphSize(float x,float y)
{
	graphSize[0]=x;
	graphSize[1]=y;
}

void CSView::getGraphSize(float size[2])
{
	size[0]=graphSize[0];
	size[1]=graphSize[1];
}

bool CSView::announceObjectWillBeErased(int objectID)
{ // Never called from copy buffer!
	if (objectID==linkedObjectID)
	{
		linkedObjectID=-1;
		return(true);
	}
	return(false);
}

void CSView::performObjectLoadingMapping(std::vector<int>* map)
{
	linkedObjectID=App::ct->objCont->getLoadingMapping(map,linkedObjectID);
}
bool CSView::mouseWheel(int deltaZ,int x,int y)
{
	if (x<0)
		return(false);
	if (y<0)
		return(false);
	if (x>_viewSize[0])
		return(false);
	if (y>_viewSize[1])
		return(false);
	C3DObject* it=App::ct->objCont->getObject(linkedObjectID);
	if (it!=NULL)
	{
		deltaZ=int(float(deltaZ)*App::userSettings->mouseWheelZoomFactor*1.001f);
		if (it->getObjectType()==sim_object_camera_type)
		{
			C3DObject* cameraParentProxy=NULL;
			if (((CCamera*)it)->getUseParentObjectAsManipulationProxy())
				cameraParentProxy=((CCamera*)it)->getParent();
			if (!perspectiveDisplay)
			{
				((CCamera*)it)->setOrthoViewSize(((CCamera*)it)->getOrthoViewSize()*(1.0f+float(deltaZ)/1920.0f));
				if (cameraParentProxy!=NULL)
				{ // We report the same camera opening to all cameras attached to cameraParentProxy
					for (int i=0;i<int(cameraParentProxy->childList.size());i++)
					{
						if (cameraParentProxy->childList[i]->getObjectType()==sim_object_camera_type)
						{
							((CCamera*)cameraParentProxy->childList[i])->setOrthoViewSize(((CCamera*)it)->getOrthoViewSize());
							App::ct->undoBufferContainer->announceChangeGradual(); // **************** UNDO THINGY ****************
						}
					}
				}
			}
			else
			{
				int ct=VDateTime::getTimeInMs();
				static int lastTime=ct;
				int timeDiffInMs=VDateTime::getTimeDiffInMs(lastTime,ct);
				if (timeDiffInMs<100)
					timeDiffInMs=100;
				float rl=float(deltaZ/120);
				float vel=rl/timeDiffInMs;
				static float previousRl=rl;
				static float fact=1.0f;
				if (previousRl*rl<0)
				{
					fact=1.0f;
				}
				fact+=fabs(vel)/0.02f;
				if (fact>50.0f)
					fact=50.0f;
				if (timeDiffInMs>800)
				{
					fact=1.0f;
				}
				//printf("vel: %f, fact: %f\n",vel,fact);
				previousRl=rl;
				lastTime=ct;

				/*
				int ct=VDateTime::getTimeInMs();
				static int lastTime=ct;
				static float fact=1.0f;
				static int cnt=0;
				static float previousDir=float(deltaZ);
				cnt+=deltaZ/120;
				if (previousDir*deltaZ<0.0f)
					cnt=0;
				previousDir=float(deltaZ);
				if (VDateTime::getTimeDiffInMs(lastTime)<350) // was 500 before 2010/08/05
				{
					if (abs(cnt)>=3) // was 4 before 2010/08/05
					{
						if (fact<=30.0f) // was 15.0 before 2010/08/05
							fact*=2.0f; // was 2.0 before 2010/08/05
						cnt=0;
					}
				}
				else
				{
					fact=1.0f;
					cnt=0;
				}
				lastTime=ct;
				*/
				C4X4Matrix local(((CCamera*)it)->getLocalTransformation().getMatrix());
				C4X4Matrix localNew(local);
				localNew.X-=localNew.M.axis[2]*0.01f*(((CCamera*)it)->getNearClippingPlane()/0.05f)*fact*float(deltaZ)/120.0f; // Added *(((CCamera*)it)->getNearClippingPlane()/0.05f) on 23/02/2011 to make smaller displacements when near clip. plane is closer
				((CCamera*)it)->shiftCameraInCameraManipulationMode(localNew.X);
				if (cameraParentProxy!=NULL)
				{ // We manipulate the parent object instead:
					C7Vector local1(((CCamera*)it)->getLocalTransformation());
					((CCamera*)it)->setLocalTransformation(local.getTransformation()); // we reset to initial
					cameraParentProxy->setLocalTransformation(cameraParentProxy->getLocalTransformation()*local1*(local.getInverse()).getTransformation());
				}
				App::ct->undoBufferContainer->announceChangeGradual(); // **************** UNDO THINGY ****************
			}
		}
		if ( (it->getObjectType()==sim_object_graph_type)&&(!App::ct->simulation->isSimulationRunning()) )
		{
			float zoomFact=float(deltaZ/120)*0.1f;
			float centerPos[2]={graphPosition[0]+graphSize[0]/2.0f,graphPosition[1]+graphSize[1]/2.0f};
			graphSize[0]=graphSize[0]*(1.0f+zoomFact);
			graphSize[1]=graphSize[1]*(1.0f+zoomFact);
			graphPosition[0]=centerPos[0]-graphSize[0]/2.0f;
			graphPosition[1]=centerPos[1]-graphSize[1]/2.0f;
			((CGraph*)it)->validateViewValues(_viewSize,graphPosition,graphSize,graphIsTimeGraph,false,false,false);
			App::ct->undoBufferContainer->announceChangeGradual(); // **************** UNDO THINGY ****************
		}
	}
	return(true); // This event was caught
}

bool CSView::getMouseRelPosObjectAndViewSize(int x,int y,int relPos[2],int& objType,int& objID,int vSize[2],bool& viewIsPerspective)
{ // NOT FULLY IMPLEMENTED! objType=-1 --> not supported, 0 --> hierarchy, 1 --> 3DViewable
	if ( (x<0)||(y<0)||(x>_viewSize[0])||(y>_viewSize[1]) )
		return(false);
	// The position is in this subview
	if (linkedObjectID==-1)
	{
		objType=-1;
		return(true);
	}
	relPos[0]=x;
	relPos[1]=y;
	vSize[0]=_viewSize[0];
	vSize[1]=_viewSize[1];
	objType=1; // 3D object
	objID=linkedObjectID;
	viewIsPerspective=perspectiveDisplay;
	return(true);
}

void CSView::_handleClickRayIntersection(int x,int y,bool mouseDown)
{
	if (App::mainWindow->ctrlKeyDown||App::mainWindow->shiftKeyDown)
		return; // doesn't generate any message when the ctrl or shift key is pressed
	CCamera* cam=App::ct->objCont->getCamera(linkedObjectID);
	if (cam==NULL)
		return;
	if (!App::ct->simulation->isSimulationRunning())
		return; // for now only while simulation is running
	bool allShapesAlsoNonDetectable=false;
	if (mouseDown)
	{
		if (CIloIlo::proxSensorClickSelectDown==0)
			return;
		if ((App::mainWindow!=NULL)&&(!App::mainWindow->leftMouseDown))
			return; // happens when right-clicking for rotation
		allShapesAlsoNonDetectable=(CIloIlo::proxSensorClickSelectDown<0);
	}
	else
	{
		if (CIloIlo::proxSensorClickSelectUp==0)
			return;
		allShapesAlsoNonDetectable=(CIloIlo::proxSensorClickSelectUp<0);
	}
	std::vector<int> currentSelectionState;
	App::ct->objCont->getSelectedObjects(currentSelectionState);

	int intParams[8]={0,0,0,0,0,0,0,0};
	float floatParams[15]={cam->getNearClippingPlane(),999999.9f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.01f,0.0f,0.0f};
	int psh=simCreateProximitySensor_internal(sim_proximitysensor_ray_subtype,sim_objectspecialproperty_detectable_all,0,intParams,floatParams,NULL);
	float pos[3]={0.0f,0.0f,0.0f};
	simSetObjectPosition_internal(psh,linkedObjectID,pos);
	simSetObjectOrientation_internal(psh,linkedObjectID,pos);
	simSetObjectParent_internal(psh,linkedObjectID,true);
	float t[2]={(1.0f-float(x)/float(_viewSize[0]))-0.5f,(1.0f-float(y)/float(_viewSize[1]))-0.5f};
	float ratio=(float)(_viewSize[0]/(float)_viewSize[1]);
	if (perspectiveDisplay)
	{
		float va[2];
		if (ratio>1.0f)
		{
			va[0]=cam->getViewAngle();
			va[1]=2.0f*(float)atan(tan(cam->getViewAngle()/2.0f)/ratio);
		}
		else
		{
			va[0]=2.0f*(float)atan(tan(cam->getViewAngle()/2.0f)*ratio);
			va[1]=cam->getViewAngle();
		}
		float a0=atan(2.0f*t[0]*tan(va[0]*0.5f));
		float a1=atan(2.0f*t[1]*tan(va[1]*0.5f));

		C4Vector q(C3Vector(0.0f,0.0f,1.0f),C3Vector(tan(a0),-tan(a1),1.0f));
		C3Vector euler(q.getEulerAngles());

		simSetObjectOrientation_internal(psh,sim_handle_parent,euler.data);
	}
	else
	{
		float va[2];
		if (ratio>1.0f)
		{
			va[0]=cam->getOrthoViewSize();
			va[1]=cam->getOrthoViewSize()/ratio;
		}
		else
		{
			va[0]=cam->getOrthoViewSize()*ratio;
			va[1]=cam->getOrthoViewSize();
		}
		float a0=va[0]*t[0];
		float a1=va[1]*(-t[1]);
		float pos[3]={a0,a1,0.0f};
		simSetObjectPosition_internal(psh,sim_handle_parent,pos);
	}

	int displayAttrib=sim_displayattribute_renderpass;
	if (App::ct->simulation->getDynamicContentVisualizationOnly())
		displayAttrib|=sim_displayattribute_dynamiccontentonly;
	CProxSensor* prox=App::ct->objCont->getProximitySensor(psh);
	C3Vector pt,triNormal;
	int obj=-1;
	float dist=SIM_MAX_FLOAT;
	bool ptValid=false;
	for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
	{
		int shapeHandle=App::ct->objCont->shapeList[i];
		CShape* shape=App::ct->objCont->getShape(shapeHandle);
		if (shape->getShouldObjectBeDisplayed(displayAttrib))
		{
			int theObj;
			bool valid=CProxSensorRoutine::detectEntity(psh,shapeHandle,true,false,0.0f,pt,dist,true,true,theObj,0.0f,triNormal,allShapesAlsoNonDetectable,false,false);
			ptValid|=valid;
			if (valid)
				obj=theObj;
		}
	}

	// In abs coords now:
	C7Vector sensTr(prox->getCumulativeTransformation());
	pt*=sensTr;
	triNormal=sensTr.Q*triNormal;

	// Now generate a plugin callback:
	float ptdata[6]={pt(0),pt(1),pt(2),triNormal(0),triNormal(1),triNormal(2)};
	int msg;
	if (mouseDown)
		msg=sim_message_eventcallback_proxsensorselectdown;
	else
		msg=sim_message_eventcallback_proxsensorselectup;
	int data[4]={obj,0,0,0};
	void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(msg,data,ptdata,NULL);
	delete[] (char*)retVal;
	// Now generate a script message:
	if (mouseDown)
		msg=sim_message_prox_sensor_select_down;
	else
		msg=sim_message_prox_sensor_select_up;
	App::ct->outsideCommandQueue->addCommand(msg,obj,0,0,0,ptdata,6);
//	int dummyHandle=simCreateDummy(0.03f,NULL);
//	simSetObjectPosition(dummyHandle,psh,pt.data);
	simRemoveObject_internal(psh);

	App::ct->objCont->setSelectedObjects(currentSelectionState);
}

bool CSView::leftMouseButtonDown(int x,int y,int selStatus)
{
	_caughtElements&=0xffff-sim_left_button;
	_mouseMovedWhileDownFlag=false;
	if ( (x<0)||(y<0)||(x>_viewSize[0])||(y>_viewSize[1]) )
		return(false);
	// Maybe generate an intersection coordinate+object ID with scene objects:
	_handleClickRayIntersection(x,y,true);

	// The mouse went down in this subview
	_caughtElements|=sim_left_button;
	// Was a previous mouse action processed (needs rendering pass)?
	if (mouseJustWentDownFlag||mouseJustWentUpFlag||mouseIsDown)
		return(true); // the mouse is caught but we don't process the event
	selectionStatus=selStatus;
	mouseDownRelativePosition[0]=x;
	mouseDownRelativePosition[1]=y;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	mousePreviousRelativePosition[0]=x;
	mousePreviousRelativePosition[1]=y;
	mouseIsDown=true;
	mouseJustWentDownFlag=true;
	mouseDownTimings=VDateTime::getTimeInMs();
	mouseJustWentDownWasProcessed=false;
	mouseJustWentUpFlag=false;
	// Clear all manip mode overlay grid flags:
	for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
		it->clearManipulationModeOverlayGridFlag();
	}

	return(true); // This event was caught
}
void CSView::leftMouseButtonUp(int x,int y)
{
	// Maybe generate an intersection coordinate+object ID with scene objects:
	if (!_mouseMovedWhileDownFlag)
		_handleClickRayIntersection(x,y,false);


	// Was a previous mouse action processed (needs rendering pass)?
	if ( (!mouseIsDown)||mouseJustWentUpFlag) 
		return;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	mouseJustWentUpFlag=true;
}

void CSView::mouseMove(int x,int y,bool passiveAndFocused)
{
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	if (mouseIsDown&&(mouseDownRelativePosition[0]!=x)&&(mouseDownRelativePosition[1]!=y))
		_mouseMovedWhileDownFlag=true;
	if (!passiveAndFocused)
	{
		if (_caughtElements&sim_right_button)
		{
			if ( ((abs(rightMouseDownRelativePosition[1]-mouseRelativePosition[1])>1)||
				(abs(rightMouseDownRelativePosition[0]-mouseRelativePosition[0])>1) ) &&(!dontActivatePopup)&&CMainWindow::rightMouseDown)
			{ // we are zooming with the right button!
				dontActivatePopup=true;
				if (!subviewIsPassive) // we do that only on an active view
					leftMouseButtonDown(_rightMouseButtonDownCoord[0],_rightMouseButtonDownCoord[1],NOSELECTION); // pretend we are using the left button
			}
		}
		else
		{
		}
	}
}

bool CSView::rightMouseButtonDown(int x,int y,bool subViewIsPassive)
{
	subviewIsPassive=subViewIsPassive;
	if ( (x<0)||(y<0)||(x>_viewSize[0])||(y>_viewSize[1]) )
		return(false);
	_rightMouseButtonDownCoord[0]=x;
	_rightMouseButtonDownCoord[1]=y;
	_caughtElements|=sim_right_button;
	// The mouse went down in this subview
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	rightMouseDownRelativePosition[0]=x;
	rightMouseDownRelativePosition[1]=y;
	dontActivatePopup=false;
	rightButtonIsCaught=true;
	return(true); // The right button was caught
}
bool CSView::rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow,int subViewIndex)
{ // true indicates that this view has to be removed (command "Remove view")
	bool caughtSave=rightButtonIsCaught;
	rightButtonIsCaught=false;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	mouseDownRelativePosition[0]=x;
	mouseDownRelativePosition[1]=y;
	rightMouseDownRelativePosition[0]=x;
	rightMouseDownRelativePosition[1]=y;


	if (dontActivatePopup)
		leftMouseButtonUp(x,y); // pretend we are using the left button (we were in right-button zoom mode)

	if (x<0)
		return(false);
	if (y<0)
		return(false);
	if (x>_viewSize[0])
		return(false);
	if (y>_viewSize[1])
		return(false);
	// The mouse went up in this subview
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	C3DObject* it=App::ct->objCont->getObject(linkedObjectID);
	int linkedObj=-1;
	if (it!=NULL)
	{
		if (it->getObjectType()==sim_object_camera_type)
			linkedObj=0;
		if (it->getObjectType()==sim_object_graph_type)
			linkedObj=1;
		if (it->getObjectType()==sim_object_visionsensor_type)
			linkedObj=2;
	}
	
	if (caughtSave&&(!dontActivatePopup))
	{ // We have to activate a popup-menu:
		if (App::operationalUIParts&sim_gui_popups)
		{ // Default popups
			VMenu* mainMenu=VMenubar::createPopupMenu();

			VMenubar::appendMenuItem(mainMenu,App::ct->objCont->getEditModeType()!=BUTTON_EDIT_MODE,false,REMOVE_VIEW_CMD,IDS_REMOVE_PAGE_MENU_ITEM);
			VMenu* viewMenu=VMenubar::createPopupMenu();
			addMenu(viewMenu);
			VMenubar::appendMenuAndDetach(mainMenu,viewMenu,App::ct->objCont->getEditModeType()!=BUTTON_EDIT_MODE,IDS_VIEW_MENU_ITEM);
			if ( (App::ct->objCont->getEditModeType()==NO_EDIT_MODE) )
			{
				if (linkedObj==0)
				{
					VMenu* objectEditMenu=VMenubar::createPopupMenu();
					CObjectEdition::addMenu(objectEditMenu);
					VMenubar::appendMenuAndDetach(mainMenu,objectEditMenu,true,IDS_EDIT_MENU_ITEM);
				}

				VMenu* addMenu=VMenubar::createPopupMenu();
				CAddMenu::addMenu(addMenu,this,linkedObj==-1);
				VMenubar::appendMenuAndDetach(mainMenu,addMenu,true,IDS_ADD_MENU_ITEM);
			}
			else
			{
				int t=App::ct->objCont->getEditModeType();
				if (t&SHAPE_EDIT_MODE)
				{
					VMenu* triangleVertexEditMenu=VMenubar::createPopupMenu();
					CTriangleVertexEdition::addMenu(triangleVertexEditMenu);
					VMenubar::appendMenuAndDetach(mainMenu,triangleVertexEditMenu,true,IDS_EDIT_MENU_ITEM);
				}
				if (t&PATH_EDIT_MODE)
				{
					VMenu* pathEditMenu=VMenubar::createPopupMenu();
					CPathEdition::addMenu(pathEditMenu,App::ct->objCont->getObject(linkedObjectID));
					VMenubar::appendMenuAndDetach(mainMenu,pathEditMenu,true,IDS_EDIT_MENU_ITEM);
				}
				if (t&BUTTON_EDIT_MODE)
				{
					VMenu* buttonEditMenu=VMenubar::createPopupMenu();
					App::ct->buttonBlockContainer->addMenu(buttonEditMenu);
					VMenubar::appendMenuAndDetach(mainMenu,buttonEditMenu,true,IDS_EDIT_MENU_ITEM);
				}
			}

			if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
			{
				if (linkedObj!=-1)
				{
					VMenu* simulationMenu=VMenubar::createPopupMenu();
					App::ct->simulation->addMenu(simulationMenu);
					VMenubar::appendMenuAndDetach(mainMenu,simulationMenu,true,IDS_SIMULATION_MENU_ITEM);
				}
			}
			int command=VMenubar::trackPopupMenu(mainMenu,absX,absY,mainWindow);
			delete mainMenu;
			if (command==REMOVE_VIEW_CMD)
			{
				App::uiThread->addStatusLine(IDSNS_REMOVED_VIEW);
				return(true);
			}
			bool processed=false;
			if (!processed)
				processed=evaluateMenuSelection(command,subViewIndex);
			if (!processed)
				processed=CAddMenu::evaluateMenuSelection(command,this);
			if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
			{
				if (!processed)
					processed=CObjectEdition::evaluateMenuSelection(command);
			}
			if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
			{
				if (!processed)
					processed=CTriangleVertexEdition::evaluateMenuSelection(command);
			}
			if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
			{
				if (!processed)
					processed=CPathEdition::evaluateMenuSelection(command,App::ct->objCont->getObject(linkedObjectID));
			}
			if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
			{
				if (!processed)
					processed=App::ct->buttonBlockContainer->evaluateMenuSelection(command);
			}
			if (!processed)
				processed=App::ct->simulation->evaluateMenuSelection(command);
		}
	}
	return(false);
}
bool CSView::leftMouseButtonDoubleClick(int x,int y,int selStatus)
{
	if (x<0)
		return(false);
	if (y<0)
		return(false);
	if (x>_viewSize[0])
		return(false);
	if (y>_viewSize[1])
		return(false);
	// The mouse went down in this subview
	selectionStatus=selStatus;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	return(false); // Not yet processed
}

void CSView::getMouseDownRelativePosition(int p[2])
{
	p[0]=mouseDownRelativePosition[0];
	p[1]=mouseDownRelativePosition[1];
}
void CSView::getMouseRelativePosition(int p[2])
{
	p[0]=mouseRelativePosition[0];
	p[1]=mouseRelativePosition[1];
}
void CSView::getPreviousMouseRelativePosition(int p[2])
{
	p[0]=mousePreviousRelativePosition[0];
	p[1]=mousePreviousRelativePosition[1];
}
bool CSView::isMouseDown()
{
	return(mouseIsDown);
}
bool CSView::didMouseJustGoDown()
{
	return(mouseJustWentDownFlag);
}
bool CSView::wasMouseJustWentDownEventProcessed()
{
	return(mouseJustWentDownWasProcessed);
}
bool CSView::didMouseJustGoUp()
{
	return(mouseJustWentUpFlag);
}

bool CSView::didMouseMoveWhileDown()
{
	return(_mouseMovedWhileDownFlag);
}

void CSView::addMenu(VMenu* menu)
{
	CCamera* camera=App::ct->objCont->getCamera(linkedObjectID);
	CGraph* graph=App::ct->objCont->getGraph(linkedObjectID);
	CVisionSensor* sensor=App::ct->objCont->getVisionSensor(linkedObjectID);
	int selSize=App::ct->objCont->getSelSize();
	bool lastSelIsCamera=false;
	bool lastSelIsGraph=false;
	bool lastSelIsRendSens=false;
	if (selSize>0)
	{
		if (App::ct->objCont->getLastSelection()->getObjectType()==sim_object_camera_type)
			lastSelIsCamera=true;
		if (App::ct->objCont->getLastSelection()->getObjectType()==sim_object_graph_type)
			lastSelIsGraph=true;
		if (App::ct->objCont->getLastSelection()->getObjectType()==sim_object_visionsensor_type)
			lastSelIsRendSens=true;
	}
	if (camera!=NULL)
	{ // The linked object is a camera:
		VMenubar::appendMenuItem(menu,true,_renderingMode==RENDERING_MODE_SOLID,TOGGLE_SOLID_DISPLAY_CMD,IDS_SOLID_RENDERING_MENU_ITEM,true);
		VMenubar::appendMenuItem(menu,true,perspectiveDisplay,PERSPECTIVE_DISPLAY_CMD,IDS_PERSPECTIVE_PROJECTION_MENU_ITEM,true);
		VMenubar::appendMenuItem(menu,true,_showEdges,SHOW_EDGES_CMD,IDS_SHOW_EDGES_IN_VIEW_MENU_ITEM,true);
		VMenubar::appendMenuItem(menu,_showEdges,_thickEdges,THICK_EDGES_CMD,IDS_THICK_EDGES_IN_VIEW_MENU_ITEM,true);
		VMenubar::appendMenuItem(menu,true,App::ct->environment->getShapeTexturesEnabled(),TEXTURED_DISPLAY_CMD,IDSN_SHAPE_TEXTURES_ENABLED,true);
		VMenubar::appendMenuItem(menu,true,App::ct->environment->getShowingInertias(),SHOW_INERTIAS_CMD,IDSN_SHOW_INERTIAS,true);
		VMenubar::appendMenuSeparator(menu);

		if ( (selSize==1)&&lastSelIsGraph )
			VMenubar::appendMenuItem(menu,true,false,LOOK_AT_GRAPH_CMD,IDS_LOOK_AT_SELECTED_GRAPH_MENU_ITEM);
		else if ( (selSize==1)&&lastSelIsRendSens )
			VMenubar::appendMenuItem(menu,true,false,LOOK_AT_VISION_SENSOR_CMD,IDS_LOOK_AT_SELECTED_VISION_SENSOR_MENU_ITEM);
		else
			VMenubar::appendMenuItem(menu,lastSelIsCamera&&(selSize==1),false,LOOK_THROUGH_CAMERA_CMD,IDS_LOOK_THROUGH_SELECTED_CAMERA_MENU_ITEM);

		VMenubar::appendMenuItem(menu,true,false,VIEW_SELECTOR_ANY_CMD,IDSN_SELECT_VIEWABLE_OBJECT);

		C3DObject* trkObj=App::ct->objCont->getObject(camera->getTrackedObjectID());
		if (trkObj!=NULL)
		{
			std::string tmp(IDS_DONT_TRACK_OBJECT__MENU_ITEM);
			tmp+=trkObj->getName()+"'";
			VMenubar::appendMenuItem(menu,true,false,DONT_TRACK_OBJECT_CMD,tmp);
		}
		else
		{
			bool illegalLoop=(selSize==1)&&(App::ct->objCont->getLastSelection()==camera);
			VMenubar::appendMenuItem(menu,(selSize==1)&&(!illegalLoop),false,TRACK_OBJECT_CMD,IDS_TRACK_SELECTED_OBJECT_MENU_ITEM);
		}
	}
	if (graph!=NULL)
	{ // The linked object is a graph:
		if (graphIsTimeGraph)
		{
			VMenubar::appendMenuItem(menu,true,false,XY_GRAPH_DISPLAY_CMD,IDS_DISPLAY_X_Y_GRAPH_MENU_ITEM);
			VMenubar::appendMenuItem(menu,true,_timeGraphXInAutoModeDuringSimulation,TIME_GRAPH_X_AUTO_MODE_DURING_SIMULATION_CMD,IDS_TIME_GRAPH_X_AUTO_MODE_DURING_SIMULATION_MENU_ITEM,true);
			VMenubar::appendMenuItem(menu,true,_timeGraphYInAutoModeDuringSimulation,TIME_GRAPH_Y_AUTO_MODE_DURING_SIMULATION_CMD,IDS_TIME_GRAPH_Y_AUTO_MODE_DURING_SIMULATION_MENU_ITEM,true);
		}
		else
		{
			VMenubar::appendMenuItem(menu,true,false,XY_GRAPH_DISPLAY_CMD,IDS_DISPLAY_TIME_GRAPH_MENU_ITEM);
			VMenubar::appendMenuItem(menu,true,_xyGraphInAutoModeDuringSimulation,XY_GRAPH_AUTO_MODE_DURING_SIMULATION_CMD,IDS_XY_GRAPH_AUTO_MODE_DURING_SIMULATION_MENU_ITEM,true);
			VMenubar::appendMenuItem(menu,true,_xyGraphIsOneOneProportional,XY_GRAPH_KEEP_PROPORTIONS_AT_ONE_ONE_CMD,IDS_XY_GRAPH_ONE_ONE_PROPORTION_MENU_ITEM,true);
		}
		VMenubar::appendMenuSeparator(menu);

		if ( (selSize==1)&&lastSelIsCamera )
			VMenubar::appendMenuItem(menu,true,false,LOOK_THROUGH_CAMERA_CMD,IDS_LOOK_THROUGH_SELECTED_CAMERA_MENU_ITEM);
		else if ( (selSize==1)&&lastSelIsRendSens )
			VMenubar::appendMenuItem(menu,true,false,LOOK_AT_VISION_SENSOR_CMD,IDS_LOOK_AT_SELECTED_VISION_SENSOR_MENU_ITEM);
		else
			VMenubar::appendMenuItem(menu,lastSelIsGraph&&(selSize==1),false,LOOK_AT_GRAPH_CMD,IDS_LOOK_AT_SELECTED_GRAPH_MENU_ITEM);

		VMenubar::appendMenuItem(menu,true,false,VIEW_SELECTOR_ANY_CMD,IDSN_SELECT_VIEWABLE_OBJECT);
	}
	if (sensor!=NULL)
	{ // The linked object is a vision sensor:
		if ( (selSize==1)&&lastSelIsGraph )
			VMenubar::appendMenuItem(menu,true,false,LOOK_AT_GRAPH_CMD,IDS_LOOK_AT_SELECTED_GRAPH_MENU_ITEM);
		else if ( (selSize==1)&&lastSelIsCamera )
			VMenubar::appendMenuItem(menu,true,false,LOOK_THROUGH_CAMERA_CMD,IDS_LOOK_THROUGH_SELECTED_CAMERA_MENU_ITEM);
		else
			VMenubar::appendMenuItem(menu,lastSelIsRendSens&&(selSize==1),false,LOOK_AT_VISION_SENSOR_CMD,IDS_LOOK_AT_SELECTED_VISION_SENSOR_MENU_ITEM);

		VMenubar::appendMenuItem(menu,true,false,VIEW_SELECTOR_ANY_CMD,IDSN_SELECT_VIEWABLE_OBJECT);
	}
	if ( (camera==NULL)&&(graph==NULL)&&(sensor==NULL) )
	{
		if ( (selSize==1)&&lastSelIsGraph )
			VMenubar::appendMenuItem(menu,true,false,LOOK_AT_GRAPH_CMD,IDS_LOOK_AT_SELECTED_GRAPH_MENU_ITEM);
		else if ( (selSize==1)&&lastSelIsRendSens )
			VMenubar::appendMenuItem(menu,true,false,LOOK_AT_VISION_SENSOR_CMD,IDS_LOOK_AT_SELECTED_VISION_SENSOR_MENU_ITEM);
		else
			VMenubar::appendMenuItem(menu,lastSelIsCamera&&(selSize==1),false,LOOK_THROUGH_CAMERA_CMD,IDS_LOOK_THROUGH_SELECTED_CAMERA_MENU_ITEM);
		VMenubar::appendMenuItem(menu,true,false,VIEW_SELECTOR_ANY_CMD,IDSN_SELECT_VIEWABLE_OBJECT);
	}
}

void CSView::setTrackedGraphCurveIndex(int index)
{
	_trackedCurveIndex=index;
}

int CSView::getTrackedGraphCurveIndex()
{
	return(_trackedCurveIndex);
}

void CSView::setFitViewToScene(bool doIt)
{
	_fitSceneToView=doIt;
	if (doIt)
		_fitSelectionToView=false;
}

bool CSView::getFitViewToScene()
{
	return(_fitSceneToView);
}

void CSView::setFitViewToSelection(bool doIt)
{
	_fitSelectionToView=doIt;
	if (doIt)
		_fitSceneToView=false;
}

bool CSView::getFitViewToSelection()
{
	return(_fitSelectionToView);
}


bool CSView::evaluateMenuSelection(int commandID,int subViewIndex)
{ // Return value is true if the command belonged to hierarchy menu and was executed
	if (commandID==TOGGLE_SOLID_DISPLAY_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_SOLID_DISPLAY_CMD")
		{
			if (_renderingMode==RENDERING_MODE_SOLID)
			{
				_renderingMode=RENDERING_MODE_WIREFRAME_TRIANGLES;
				App::uiThread->addStatusLine(IDSNS_NOW_IN_WIREFRAME_RENDERING_MODE);
			}
			else
			{
				_renderingMode=RENDERING_MODE_SOLID;
				App::uiThread->addStatusLine(IDSNS_NOW_IN_SOLID_RENDERING_MODE);
			}
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		return(true);
	}

	if (commandID==PERSPECTIVE_DISPLAY_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("PERSPECTIVE_DISPLAY_CMD")
		{
			perspectiveDisplay=!perspectiveDisplay;
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			if (perspectiveDisplay)
				App::uiThread->addStatusLine(IDSNS_NOW_IN_PERSPECTIVE_PROJECTION_MODE);
			else
				App::uiThread->addStatusLine(IDSNS_NOW_IN_ORTHOGRAPHIC_PROJECTION_MODE);
		}
		return(true);
	}
	if (commandID==SHOW_EDGES_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("SHOW_EDGES_CMD")
		{
			_showEdges=!_showEdges;
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			if (_showEdges)
				App::uiThread->addStatusLine(IDSNS_NOW_SHOWING_EDGES);
			else
				App::uiThread->addStatusLine(IDSNS_NOW_HIDING_EDGES);
		}
		return(true);
	}
	if (commandID==THICK_EDGES_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("THICK_EDGES_CMD")
		{
			_thickEdges=!_thickEdges;
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			if (_thickEdges)
				App::uiThread->addStatusLine(IDSNS_EDGES_ARE_NOW_THICK);
			else
				App::uiThread->addStatusLine(IDSNS_EDGES_ARE_NOW_THIN);
		}
		return(true);
	}
	if (commandID==TEXTURED_DISPLAY_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TEXTURED_DISPLAY_CMD")
		{
			App::ct->environment->setShapeTexturesEnabled(!App::ct->environment->getShapeTexturesEnabled());
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			if (App::ct->environment->getShapeTexturesEnabled())
				App::uiThread->addStatusLine(IDSNS_SHAPE_TEXTURES_ENABLED);
			else
				App::uiThread->addStatusLine(IDSNS_SHAPE_TEXTURES_DISABLED);
			App::ct->objCont->setFullDialogRefreshFlag(); // so that env. dlg gets refreshed
		}
		return(true);
	}

	if (commandID==SHOW_INERTIAS_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("SHOW_INERTIAS_CMD")
		{
			App::ct->environment->setShowingInertias(!App::ct->environment->getShowingInertias());
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			if (App::ct->environment->getShowingInertias())
				App::uiThread->addStatusLine(IDSNS_SHOWING_INERTIAS);
			else
				App::uiThread->addStatusLine(IDSNS_NOT_SHOWING_INERTIAS);
			App::ct->objCont->setFullDialogRefreshFlag(); // so that env. dlg gets refreshed
		}
		return(true);
	}


	if (commandID==XY_GRAPH_AUTO_MODE_DURING_SIMULATION_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("XY_GRAPH_AUTO_MODE_DURING_SIMULATION_CMD")
		{
			_xyGraphInAutoModeDuringSimulation=!_xyGraphInAutoModeDuringSimulation;
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			if (_xyGraphInAutoModeDuringSimulation)
				App::uiThread->addStatusLine(IDSNS_NOW_IN_AUTO_MODE);
			else
				App::uiThread->addStatusLine(IDSNS_AUTO_MODE_DISABLED);
		}
		return(true);
	}
	if (commandID==TIME_GRAPH_X_AUTO_MODE_DURING_SIMULATION_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TIME_GRAPH_X_AUTO_MODE_DURING_SIMULATION_CMD")
		{
			_timeGraphXInAutoModeDuringSimulation=!_timeGraphXInAutoModeDuringSimulation;
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			if (_timeGraphXInAutoModeDuringSimulation)
				App::uiThread->addStatusLine(IDSNS_NOW_IN_AUTO_MODE);
			else
				App::uiThread->addStatusLine(IDSNS_AUTO_MODE_DISABLED);
		}
		return(true);
	}
	if (commandID==TIME_GRAPH_Y_AUTO_MODE_DURING_SIMULATION_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TIME_GRAPH_Y_AUTO_MODE_DURING_SIMULATION_CMD")
		{
			_timeGraphYInAutoModeDuringSimulation=!_timeGraphYInAutoModeDuringSimulation;
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			if (_timeGraphYInAutoModeDuringSimulation)
				App::uiThread->addStatusLine(IDSNS_NOW_IN_AUTO_MODE);
			else
				App::uiThread->addStatusLine(IDSNS_AUTO_MODE_DISABLED);
		}
		return(true);
	}
	if (commandID==XY_GRAPH_KEEP_PROPORTIONS_AT_ONE_ONE_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("XY_GRAPH_KEEP_PROPORTIONS_AT_ONE_ONE_CMD")
		{
			_xyGraphIsOneOneProportional=!_xyGraphIsOneOneProportional;
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			if (_xyGraphIsOneOneProportional)
				App::uiThread->addStatusLine(IDSNS_KEEPING_PROPORTIONS_AT_1_1);
			else
				App::uiThread->addStatusLine(IDSNS_PROPORTIONS_NOT_CONSTRAINED_ANYMORE);
		}
		return(true);
	}
	if (commandID==XY_GRAPH_DISPLAY_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("XY_GRAPH_DISPLAY_CMD")
		{
			graphIsTimeGraph=!graphIsTimeGraph;
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			if (graphIsTimeGraph)
				App::uiThread->addStatusLine(IDSNS_NOW_IN_TIME_GRAPH_MODE);
			else
				App::uiThread->addStatusLine(IDSNS_NOW_IN_XY_GRAPH_MODE);
		}
		return(true);
	}

	if (commandID==LOOK_THROUGH_CAMERA_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("LOOK_THROUGH_CAMERA_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if ((sel.size()==1)&&(App::ct->objCont->getCamera(sel[0])!=NULL))
			{
				setDefaultValues();
				linkedObjectID=sel[0];
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_NOW_LOOKING_THROUGH_SELECTED_CAMERA);
			}
		}
		return(true);
	}
	if (commandID==LOOK_AT_GRAPH_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("LOOK_AT_GRAPH_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if ((sel.size()==1)&&(App::ct->objCont->getGraph(sel[0])!=NULL))
			{
				setDefaultValues();
				linkedObjectID=sel[0];
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_NOW_LOOKING_AT_SELECTED_GRAPH);
			}
		}
		return(true);
	}
	if (commandID==LOOK_AT_VISION_SENSOR_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("LOOK_AT_VISION_SENSOR_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if ((sel.size()==1)&&(App::ct->objCont->getVisionSensor(sel[0])!=NULL))
			{
				setDefaultValues();
				linkedObjectID=sel[0];
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_NOW_LOOKING_AT_SELECTED_VISION_SENSOR);
			}
		}
		return(true);
	}
	if (commandID==TRACK_OBJECT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("TRACK_OBJECT_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			CCamera* camera=App::ct->objCont->getCamera(linkedObjectID);
			if ((camera!=NULL)&&(sel.size()==1))
			{
				camera->setTrackedObjectID(sel[0]);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_CAMERA_NOW_TRACKING_SELECTED_OBJECT);
			}
		}
		return(true);
	}
	if (commandID==DONT_TRACK_OBJECT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("DONT_TRACK_OBJECT_CMD")
		{
			CCamera* camera=App::ct->objCont->getCamera(linkedObjectID);
			if (camera!=NULL)
			{
				camera->setTrackedObjectID(-1);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_CAMERA_NOW_NOT_TRACKING_ANY_OBJECT);
			}
		}
		return(true);
	}

	return(App::ct->oglSurface->viewSelector->evaluateMenuSelection(commandID,subViewIndex));
}

int CSView::getSelectionStatus()
{
	return(selectionStatus);
}

CSView* CSView::copyYourself()
{
	CSView* newView=new CSView(linkedObjectID);
	newView->setDefaultValues();
	newView->linkedObjectID=linkedObjectID;
	newView->perspectiveDisplay=perspectiveDisplay;

	newView->_showEdges=_showEdges;
	newView->_thickEdges=_thickEdges;

	newView->_fitSceneToView=_fitSceneToView;
	newView->_fitSelectionToView=_fitSelectionToView;

	newView->_renderingMode=_renderingMode;
	newView->_xyGraphInAutoModeDuringSimulation=_xyGraphInAutoModeDuringSimulation;
	newView->_timeGraphXInAutoModeDuringSimulation=_timeGraphXInAutoModeDuringSimulation;
	newView->_timeGraphYInAutoModeDuringSimulation=_timeGraphYInAutoModeDuringSimulation;
	newView->_xyGraphIsOneOneProportional=_xyGraphIsOneOneProportional;
	newView->graphIsTimeGraph=graphIsTimeGraph;
	newView->_canSwapViewWithMainView=_canSwapViewWithMainView;

	newView->_canBeClosed=_canBeClosed;
	newView->_canBeShifted=_canBeShifted;
	newView->_canBeResized=_canBeResized;

	newView->graphPosition[0]=graphPosition[0];
	newView->graphPosition[1]=graphPosition[1];
	newView->graphSize[0]=graphSize[0];
	newView->graphSize[1]=graphSize[1];

	newView->_relativeViewPosition[0]=_relativeViewPosition[0]+_relativeViewPositionOffset;
	newView->_relativeViewPosition[1]=_relativeViewPosition[1]+_relativeViewPositionOffset;
	if (newView->_relativeViewPosition[0]>1.0f)
		newView->_relativeViewPosition[0]=1.0f;
	if (newView->_relativeViewPosition[1]>1.0f)
		newView->_relativeViewPosition[1]=0.0f;
	newView->_relativeViewSize[0]=_relativeViewSize[0];
	newView->_relativeViewSize[1]=_relativeViewSize[1];
	newView->_relativeViewPositionOffset=_relativeViewPositionOffset;
	_relativeViewPositionOffset+=0.02f;

	return(newView);
}

void CSView::handleCameraOrGraphMotion()
{
	if ( (mouseJustWentDownFlag||mouseIsDown||mouseJustWentUpFlag) )
	{
		if (selectionStatus==NOSELECTION)
			cameraAndObjectMotion();

		graphMotion();
		// Important to reset the old mouse position to the new mouse position:
		mousePreviousRelativePosition[0]=mouseRelativePosition[0];
		mousePreviousRelativePosition[1]=mouseRelativePosition[1];
	}
	if (mouseJustWentUpFlag)
	{
		mouseIsDown=false;
		selectionStatus=NOSELECTION;
		// Following added on 2010/02/09 in order to be able to manipulate dynamic objects too
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			it->disableDynamicTreeForManipulation(false);
		}
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	}
	mouseJustWentDownFlag=false;
	mouseJustWentUpFlag=false;
}

void CSView::render(int mainWindowXPos,bool clipWithMainWindowXPos,bool drawText,bool passiveSubView)
{
	FUNCTION_DEBUG;
	C3DObject* it=App::ct->objCont->getObject(linkedObjectID);
	if (it!=NULL)
	{
		glEnable(GL_SCISSOR_TEST);	
		glViewport(_viewPosition[0],_viewPosition[1],_viewSize[0],_viewSize[1]);
		int xMax=_viewPosition[0];
		int xSize=_viewSize[0];
		if ( (xMax<mainWindowXPos)&&clipWithMainWindowXPos )
		{
			xSize=xSize-(mainWindowXPos-xMax);
			xMax=mainWindowXPos;
		}
		glScissor(xMax,_viewPosition[1],xSize,_viewSize[1]);
		if (xSize>0) // to avoid strange effects!
		{
			if (it->getObjectType()==sim_object_camera_type)
				((CCamera*)it)->lookIn(NULL,this,drawText,passiveSubView);
			if (it->getObjectType()==sim_object_graph_type)
				((CGraph*)it)->lookAt(NULL,this,graphIsTimeGraph,drawText,passiveSubView,true);
			if (it->getObjectType()==sim_object_visionsensor_type) 
				((CVisionSensor*)it)->lookAt(this);
		}
		glDisable(GL_SCISSOR_TEST);
	}
	else
	{ // We draw a white sub-view:
		glEnable(GL_SCISSOR_TEST);	
		glViewport(_viewPosition[0],_viewPosition[1],_viewSize[0],_viewSize[1]);
		int xMax=_viewPosition[0];
		if ( (xMax<mainWindowXPos)&&clipWithMainWindowXPos )
			xMax=mainWindowXPos;
		glScissor(xMax,_viewPosition[1],_viewSize[0],_viewSize[1]);
		glClearColor(0.85f,0.85f,0.85f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
	}
}

void CSView::setViewIndex(int ind)
{
	_viewIndex=ind;
}

int CSView::getViewIndex()
{
	return(_viewIndex);
}

void CSView::graphMotion()
{ 
	CGraph* graph=App::ct->objCont->getGraph(linkedObjectID);
	if (graph==NULL)
		return;

	int const zoomTrigger=30;
	int posDifference=abs(mouseDownRelativePosition[1]-mouseRelativePosition[1])+
						abs(mouseDownRelativePosition[0]-mouseRelativePosition[0]);
	if (posDifference<zoomTrigger) 
		posDifference=0;
	else 
		posDifference=posDifference-zoomTrigger;
	if ((selectionStatus!=SHIFTSELECTION)&&(selectionStatus!=CTRLSELECTION))
	{
		float xShift=(mousePreviousRelativePosition[0]-mouseRelativePosition[0])*graphSize[0]/(float)_viewSize[0];
		float yShift=(mousePreviousRelativePosition[1]-mouseRelativePosition[1])*graphSize[1]/(float)_viewSize[1];
		graphPosition[0]=graphPosition[0]+xShift;
		graphPosition[1]=graphPosition[1]+yShift;
		graph->validateViewValues(_viewSize,graphPosition,graphSize,graphIsTimeGraph,true,false,false);
	}

	if (selectionStatus==SHIFTSELECTION)
	{
		float x1=graphPosition[0]+((float)mouseDownRelativePosition[0]/(float)_viewSize[0])*graphSize[0];
		float y1=graphPosition[1]+((float)mouseDownRelativePosition[1]/(float)_viewSize[1])*graphSize[1];
		float x2=graphPosition[0]+((float)mouseRelativePosition[0]/(float)_viewSize[0])*graphSize[0];
		float y2=graphPosition[1]+((float)mouseRelativePosition[1]/(float)_viewSize[1])*graphSize[1];
		tt::limitValue(graphPosition[0],graphPosition[0]+graphSize[0],x1);
		tt::limitValue(graphPosition[1],graphPosition[1]+graphSize[1],y1);
		tt::limitValue(graphPosition[0],graphPosition[0]+graphSize[0],x2);
		tt::limitValue(graphPosition[1],graphPosition[1]+graphSize[1],y2);
		if ((x1>x2)&&(y2>y1))
		{ // Gradually zooming out
			float centerPos[2]={graphPosition[0]+graphSize[0]/2.0f,graphPosition[1]+graphSize[1]/2.0f};
			graphSize[0]=graphSize[0]*(1.0f+0.00005f*posDifference);
			graphSize[1]=graphSize[1]*(1.0f+0.00005f*posDifference);
			graphPosition[0]=centerPos[0]-graphSize[0]/2.0f;
			graphPosition[1]=centerPos[1]-graphSize[1]/2.0f;
			graph->validateViewValues(_viewSize,graphPosition,graphSize,graphIsTimeGraph,false,false,false);
		}
		if ((x1>x2)&&(y2<y1))
		{ // Gradually zooming in
			float centerPos[2]={graphPosition[0]+graphSize[0]/2.0f,graphPosition[1]+graphSize[1]/2.0f};
			graphSize[0]=graphSize[0]*(1.0f-0.00005f*posDifference);
			graphSize[1]=graphSize[1]*(1.0f-0.00005f*posDifference);
			graphPosition[0]=centerPos[0]-graphSize[0]/2.0f;
			graphPosition[1]=centerPos[1]-graphSize[1]/2.0f;
			graph->validateViewValues(_viewSize,graphPosition,graphSize,graphIsTimeGraph,false,false,false);
		}
		if ((x2>x1)&&(y1>y2)&&mouseJustWentUpFlag)
		{ // Zooming in the selected square
			graphPosition[0]=x1;
			graphPosition[1]=y2;
			graphSize[0]=x2-x1;
			graphSize[1]=y1-y2;
			graph->validateViewValues(_viewSize,graphPosition,graphSize,graphIsTimeGraph,false,false,false);
		}
		else if ((x2>x1)&&(y2>y1)&&mouseJustWentUpFlag)
		{// Setting proportions to 1:1
			graph->validateViewValues(_viewSize,graphPosition,graphSize,graphIsTimeGraph,false,true,false);
		}
	}
}


void CSView::cameraAndObjectMotion()
{
	static int eventID=0;
	static int mouseDownInitialPage=0;
	static int mouseDownInitialInstance=0;
	if (mouseJustWentDownFlag)
	{
		eventID++;
		mouseDownInitialPage=App::ct->oglSurface->pageCont->getActivePageIndex();
		mouseDownInitialInstance=App::ct->getCurrentInstanceIndex();
	}
	else
	{
		if (mouseDownInitialPage!=App::ct->oglSurface->pageCont->getActivePageIndex())
			eventID++;
		if (mouseDownInitialInstance!=App::ct->getCurrentInstanceIndex())
			eventID++;
	}
	CCamera* camera=App::ct->objCont->getCamera(linkedObjectID);
	if (camera==NULL)
		return;
	C3DObject* cameraParentProxy=NULL;
	if (camera->getUseParentObjectAsManipulationProxy())
		cameraParentProxy=camera->getParent();
	VPoint activeWinSize(_viewSize[0],_viewSize[1]);
	VPoint dummy;
	bool perspective=perspectiveDisplay;
	VPoint mousePosition(mouseRelativePosition[0],mouseRelativePosition[1]);
	VPoint mouseDownPosition(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
	VPoint previousMousePosition(mousePreviousRelativePosition[0],mousePreviousRelativePosition[1]);
	int navigationMode=App::ct->getMouseMode()&0x00ff;
	float zoomSensitivity=0.000005f;

	// Needed later...
	int yPosDifference=mouseDownPosition.y-mousePosition.y;
	static int restY=0;
	static int restX=0;
	const int treshhold=20;
	const int sleep=10;
//	const float linJointResolution=0.0001f;
//	const float angJointResolution=0.001f*degToRad;
	int yDiff=mousePosition.y-mouseDownPosition.y;
	int xDiff=mousePosition.x-mouseDownPosition.x;
	if ( (yDiff>-treshhold)&&(yDiff<treshhold) )
		restY=restY+yDiff;
	if ( (restY<=-treshhold*sleep)||(restY>=treshhold*sleep) )
	{
		yDiff=restY/sleep;
		restY=0;
	}
	if ( (xDiff>-treshhold)&&(xDiff<treshhold) )
		restX=restX+xDiff;
	if ( (restX<=-treshhold*sleep)||(restX>=treshhold*sleep) )
	{
		xDiff=restX/sleep;
		restX=0;
	}
	std::vector<int> vertexSel;
	if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
	{
		if (App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)
		{ // We simply copy the selection buffer
			vertexSel.reserve(App::ct->objCont->editModeBuffer.size());
			vertexSel.clear();
			vertexSel.insert(vertexSel.begin(),App::ct->objCont->editModeBuffer.begin(),App::ct->objCont->editModeBuffer.end());
		}
		if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
			App::ct->objCont->selectionFromTriangleToVertexEditMode(&vertexSel);
		if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
			App::ct->objCont->selectionFromEdgeToVertexEditMode(&vertexSel);
	}

	// Camera/object/vertice/path point 2D rotation
	// ****************************************************************************
	if (navigationMode==sim_navigation_cameratilt)//||
	{
		// Camera 2D rotation
		//-------------------
			C7Vector camOld(camera->getLocalTransformationPart1());
			camera->tiltCameraInCameraManipulationMode(-float(previousMousePosition.x-mousePosition.x)*0.5f*degToRad);

			if (cameraParentProxy!=NULL)
			{ // We manipulate the parent object instead:
				C7Vector camNew(camera->getLocalTransformationPart1());
				camera->setLocalTransformation(camOld); // we reset to initial
				cameraParentProxy->setLocalTransformation(cameraParentProxy->getLocalTransformation()*camNew*camOld.getInverse());
			}
	}
	// ****************************************************************************

	// Camera/object 3D rotation..	
	// ****************************************************************************
	if ( (navigationMode==sim_navigation_camerarotate)||
		(navigationMode==sim_navigation_objectrotate) )
	{
		float aroundX=(previousMousePosition.y-mousePosition.y)*0.005f;
		float aroundY=(previousMousePosition.x-mousePosition.x)*0.005f;
		C3Vector cp(centerPosition[0],centerPosition[1],centerPosition[2]);
		C7Vector cameraCTM=camera->getCumulativeTransformationPart1();
		if ( (navigationMode==sim_navigation_camerarotate)&&(mousePositionDepth>0.0f) )
		{
			C7Vector parentCTMI(camera->getParentCumulativeTransformation().getInverse());
			{ // We have to keep head up
				C3X3Matrix cameraCumulTr=cameraCTM.Q.getMatrix();
				bool headUp=cameraCumulTr(2,1)>=0.0f;
				C3Vector euler=cameraCumulTr.getEulerAngles();
				euler(2)=atan2(cos(euler(0))*sin(euler(1)),sin(euler(0)));
				if (!headUp)
					euler(2)=euler(2)+piValue;
				cameraCTM.Q.setEulerAngles(euler);
				if (!headUp)
					aroundY=-aroundY;
				C7Vector rot1(aroundY,cp,C3Vector(0.0f,0.0f,1.0f));
				cameraCTM=rot1*cameraCTM;
				C7Vector rot2(aroundX,cp,cameraCTM.getAxis(0));
				cameraCTM=rot2*cameraCTM;
			}
			C7Vector local(camera->getLocalTransformation());
			camera->rotateCameraInCameraManipulationMode(parentCTMI*cameraCTM);

			if (cameraParentProxy!=NULL)
			{ // We manipulate the parent object instead:
				C7Vector local1(camera->getLocalTransformation());
				camera->setLocalTransformation(local); // we reset to initial
				if ((cameraParentProxy->getObjectManipulationModePermissions()&0x1f)==0x1f) 
					cameraParentProxy->setLocalTransformation(cameraParentProxy->getLocalTransformation()*local1*local.getInverse());
			}
		}
		if (navigationMode==sim_navigation_objectrotate)
		{
			float dX=-float(previousMousePosition.x-mousePosition.x)*degToRad;

			aroundX=-aroundX;
			aroundY=-aroundY;
			if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
			{ // We have object rotation here:
				std::vector<C3DObject*> allSelObjects;
				allSelObjects.reserve(App::ct->objCont->getSelSize());
				allSelObjects.clear();
				for (int i=0;i<App::ct->objCont->getSelSize();i++)
				{
					C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getSelID(i));
					allSelObjects.push_back(it);
				}
				for (int i=0;i<int(allSelObjects.size());i++)
				{
					C3DObject* it=allSelObjects[i];
					if (it->getFirstParentInSelection(&allSelObjects)==NULL)
					{
						if (it->setLocalTransformationFromObjectRotationMode(camera->getCumulativeTransformation().getMatrix(),dX*0.5f,perspective,eventID))
							it->disableDynamicTreeForManipulation(true); // so that we can also manipulate dynamic objects
					}
				}
			}
		}
	}
	// ****************************************************************************

	// Camera/object/vertice shifting/zooming...
	// ****************************************************************************
	if ( ((navigationMode==sim_navigation_camerashift)||
		(navigationMode==sim_navigation_objectshift)||
		(navigationMode==sim_navigation_camerazoom) )&&(!mouseJustWentUpFlag) )
	{
		C7Vector cameraLTM(camera->getLocalTransformationPart1());
		C7Vector cameraCTM(camera->getCumulativeTransformationPart1());
		float ratio=(float)(activeWinSize.x/(float)activeWinSize.y);
		float scaleFactor=2*mousePositionDepth*(float)tan((camera->getViewAngle()*180.0f/piValTimes2)
			*degToRad)/(float)activeWinSize.y;
		if (!perspective)
			scaleFactor=camera->getOrthoViewSize()/(float)activeWinSize.y;
		if (ratio>1.0f)
			scaleFactor=scaleFactor/ratio;
		int ct=VDateTime::getTimeInMs();
		float zoomFactor=(float)(yPosDifference*yPosDifference*zoomSensitivity)*(float(VDateTime::getTimeDiffInMs(mouseDownTimings))/50.0f);
		mouseDownTimings=ct;
		if (yPosDifference>0) 
			zoomFactor=-zoomFactor;
		if ( (navigationMode==sim_navigation_camerashift)||(navigationMode==sim_navigation_objectshift) )
			zoomFactor=0.0f;
		if (navigationMode==sim_navigation_camerazoom)
			scaleFactor=0.0f;

		float deltaX=-(previousMousePosition.x-mousePosition.x)*scaleFactor;
		float deltaY=(previousMousePosition.y-mousePosition.y)*scaleFactor;
		float deltaZ=-zoomFactor;

		C3Vector relativeTransl(cameraLTM.getAxis(0)*deltaX+cameraLTM.getAxis(1)*deltaY+cameraLTM.getAxis(2)*deltaZ);
		C3Vector absoluteTransl(cameraCTM.getAxis(0)*deltaX+cameraCTM.getAxis(1)*deltaY+cameraCTM.getAxis(2)*deltaZ);

		if ( (mousePositionDepth<=0.0f)&&( (navigationMode==sim_navigation_camerashift)||
			(navigationMode==sim_navigation_objectshift) ) )
		{ // In case we didn't click an object
			relativeTransl.clear();
			absoluteTransl.clear();
		}
	
		if ( ((navigationMode==sim_navigation_camerashift)||(navigationMode==sim_navigation_camerazoom)) )
		{ // Camera shifting/zooming
			C7Vector local(camera->getLocalTransformation());
			camera->shiftCameraInCameraManipulationMode(cameraLTM.X+relativeTransl);
			if (cameraParentProxy!=NULL)
			{ // We manipulate the parent object instead:
				C7Vector local1(camera->getLocalTransformation());
				camera->setLocalTransformation(local); // we reset to initial
				cameraParentProxy->setLocalTransformation(cameraParentProxy->getLocalTransformation()*local1*local.getInverse());
			}
		}
		if (navigationMode==sim_navigation_objectshift)
		{ // Object/vertice shifting/zooming
			// We have to invert the translation vector:
			absoluteTransl*=-1.0f;
			if (navigationMode==sim_navigation_objectshift)
			{ // We have to shift the green ball
				centerPosition[0]+=absoluteTransl(0);
				centerPosition[1]+=absoluteTransl(1);
				centerPosition[2]+=absoluteTransl(2);
			}
			if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
			{ // Object shifting/zooming
				std::vector<C3DObject*> allSelObjects;
				allSelObjects.reserve(App::ct->objCont->getSelSize());
				allSelObjects.clear();
				for (int i=0;i<App::ct->objCont->getSelSize();i++)
				{
					C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getSelID(i));
					allSelObjects.push_back(it);
				}
				for (int i=0;i<int(allSelObjects.size());i++)
				{
					C3DObject* it=allSelObjects[i];
					if (it->getFirstParentInSelection(&allSelObjects)==NULL)
					{
						float prevPos[2]={float(previousMousePosition.x),float(previousMousePosition.y)};
						float pos[2]={float(mousePosition.x),float(mousePosition.y)};
						float screenHalfSizes[2]={float(activeWinSize.x)/2.0f,float(activeWinSize.y)/2.0f};
						float halfSizes[2];
						float ratio=screenHalfSizes[0]/screenHalfSizes[1];
						if (perspective)
						{
							if (ratio>1.0f)
							{
								float a=2.0f*(float)atan(tan(camera->getViewAngle()/2.0f)/ratio);
								halfSizes[0]=camera->getViewAngle()/2.0f;
								halfSizes[1]=a/2.0f;
							}
							else
							{
								float a=2.0f*(float)atan(tan(camera->getViewAngle()/2.0f)*ratio);
								halfSizes[0]=a/2.0f;
								halfSizes[1]=camera->getViewAngle()/2.0f;
							}
							prevPos[0]-=screenHalfSizes[0];
							prevPos[1]-=screenHalfSizes[1];
							pos[0]-=screenHalfSizes[0];
							pos[1]-=screenHalfSizes[1];
							prevPos[0]=atan((prevPos[0]/screenHalfSizes[0])*tan(halfSizes[0]));
							prevPos[1]=atan((prevPos[1]/screenHalfSizes[1])*tan(halfSizes[1]));
							pos[0]=atan((pos[0]/screenHalfSizes[0])*tan(halfSizes[0]));
							pos[1]=atan((pos[1]/screenHalfSizes[1])*tan(halfSizes[1]));
							screenHalfSizes[0]=halfSizes[0];
							screenHalfSizes[1]=halfSizes[1];
						}
						else
						{
							if (ratio>1.0f)
							{
								halfSizes[0]=camera->getOrthoViewSize()*0.5f;
								halfSizes[1]=camera->getOrthoViewSize()*0.5f/ratio;
							}
							else
							{
								halfSizes[1]=camera->getOrthoViewSize()*0.5f;
								halfSizes[0]=camera->getOrthoViewSize()*0.5f*ratio;
							}
							prevPos[0]-=screenHalfSizes[0];
							prevPos[1]-=screenHalfSizes[1];
							pos[0]-=screenHalfSizes[0];
							pos[1]-=screenHalfSizes[1];
						}
						C3Vector centerPos(centerPosition);
						if ( (it->getObjectType()!=sim_object_path_type)||(allSelObjects.size()!=1)||(App::ct->objCont->getPointerToSelectedPathPointIndices_nonEditMode()->size()==0) )
						{ // normal object shifting:
							if (it->setLocalTransformationFromObjectTranslationMode(camera->getCumulativeTransformation().getMatrix(),centerPos,prevPos,pos,screenHalfSizes,halfSizes,perspective,eventID))
								it->disableDynamicTreeForManipulation(true); // so that we can also manipulate dynamic objects
						}
						else
						{ // path point shifting (non-edit mode!):
							((CPath*)it)->transformSelectedPathPoints(camera->getCumulativeTransformation().getMatrix(),centerPos,prevPos,pos,screenHalfSizes,halfSizes,perspective,eventID);
						}
					}
				}
			}
			if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
			{ // Vertice shifting/zooming
				CShape* shape=(CShape*)App::ct->objCont->getObject(App::ct->objCont->getEditModeObjectID());
				if (shape!=NULL)
				{
					C7Vector objCTM(shape->getCumulativeTransformationPart1());
					C7Vector objCTMI(objCTM.getInverse());
					objCTM.X+=absoluteTransl;
					objCTM=objCTMI*objCTM;
					for (int i=0;i<int(vertexSel.size());i++)
					{
						C3Vector v(App::ct->objCont->getRelativeVertexPosition(vertexSel[i]));
						App::ct->objCont->setRelativeVertexPosition(vertexSel[i],objCTM*v);
					}
				}
			}
			if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
			{ // Path point shifting
				float prevPos[2]={float(previousMousePosition.x),float(previousMousePosition.y)};
				float pos[2]={float(mousePosition.x),float(mousePosition.y)};
				float screenHalfSizes[2]={float(activeWinSize.x)/2.0f,float(activeWinSize.y)/2.0f};
				float halfSizes[2];
				float ratio=screenHalfSizes[0]/screenHalfSizes[1];
				if (perspective)
				{
					if (ratio>1.0f)
					{
						float a=2.0f*(float)atan(tan(camera->getViewAngle()/2.0f)/ratio);
						halfSizes[0]=camera->getViewAngle()/2.0f;
						halfSizes[1]=a/2.0f;
					}
					else
					{
						float a=2.0f*(float)atan(tan(camera->getViewAngle()/2.0f)*ratio);
						halfSizes[0]=a/2.0f;
						halfSizes[1]=camera->getViewAngle()/2.0f;
					}
					prevPos[0]-=screenHalfSizes[0];
					prevPos[1]-=screenHalfSizes[1];
					pos[0]-=screenHalfSizes[0];
					pos[1]-=screenHalfSizes[1];
					prevPos[0]=atan((prevPos[0]/screenHalfSizes[0])*tan(halfSizes[0]));
					prevPos[1]=atan((prevPos[1]/screenHalfSizes[1])*tan(halfSizes[1]));
					pos[0]=atan((pos[0]/screenHalfSizes[0])*tan(halfSizes[0]));
					pos[1]=atan((pos[1]/screenHalfSizes[1])*tan(halfSizes[1]));
					screenHalfSizes[0]=halfSizes[0];
					screenHalfSizes[1]=halfSizes[1];
				}
				else
				{
					if (ratio>1.0f)
					{
						halfSizes[0]=camera->getOrthoViewSize()*0.5f;
						halfSizes[1]=camera->getOrthoViewSize()*0.5f/ratio;
					}
					else
					{
						halfSizes[1]=camera->getOrthoViewSize()*0.5f;
						halfSizes[0]=camera->getOrthoViewSize()*0.5f*ratio;
					}
					prevPos[0]-=screenHalfSizes[0];
					prevPos[1]-=screenHalfSizes[1];
					pos[0]-=screenHalfSizes[0];
					pos[1]-=screenHalfSizes[1];
				}
				C3Vector centerPos(centerPosition);

				CPath* path=(CPath*)App::ct->objCont->getObject(App::ct->objCont->getEditModeObjectID());
				if (path!=NULL)
					path->transformSelectedPathPoints(camera->getCumulativeTransformation().getMatrix(),centerPos,prevPos,pos,screenHalfSizes,halfSizes,perspective,eventID);
			}
		}
	}
	// ****************************************************************************

	// Camera opening angle...
	// ****************************************************************************
	if (navigationMode==sim_navigation_cameraangle)
	{
		float zoomFactor=(float)(previousMousePosition.y-mousePosition.y)*0.005f;
		if (perspective)
		{
			float newViewAngle=camera->getViewAngle()+zoomFactor;
			tt::limitValue(10.0f*degToRad,135.0f*degToRad,newViewAngle); // with 90 degrees, objects disappear!! Really??? Changed to 135 on 2010/11/12
			camera->setViewAngle(newViewAngle);
			if (cameraParentProxy!=NULL)
			{ // We report the same camera opening to all cameras attached to cameraPrentProxy
				for (int i=0;i<int(cameraParentProxy->childList.size());i++)
				{
					if (cameraParentProxy->childList[i]->getObjectType()==sim_object_camera_type)
						((CCamera*)cameraParentProxy->childList[i])->setViewAngle(newViewAngle);
				}
			}
		}
		else
		{
			camera->setOrthoViewSize(camera->getOrthoViewSize()*(1.0f+zoomFactor));
			if (cameraParentProxy!=NULL)
			{ // We report the same camera opening to all cameras attached to cameraPrentProxy
				for (int i=0;i<int(cameraParentProxy->childList.size());i++)
				{
					if (cameraParentProxy->childList[i]->getObjectType()==sim_object_camera_type)
						((CCamera*)cameraParentProxy->childList[i])->setOrthoViewSize(camera->getOrthoViewSize()*(1.0f+zoomFactor));
				}
			}
		}
	}
	// ****************************************************************************

	// Camera fly...
	// ****************************************************************************
	if ((navigationMode==sim_navigation_camerafly)&&(camera->getCameraManipulationModePermissions()&0x008))
	{
		int ct=VDateTime::getTimeInMs();
		static int currentEventID=-4;
		static int lastTime;
		static float velocity;
		static float rotX;
		static float rotY;
		static float rotXVel;
		static float rotYVel;
		static int downX,downY;
		static C4X4Matrix travelDir;
		static bool previousTranslateMode=false;
		if (eventID!=currentEventID)
		{
			currentEventID=eventID;
			lastTime=VDateTime::getTimeInMs();
			velocity=0.15f;
			rotX=0.0f;
			rotY=0.0f;
			rotXVel=0.0f;
			rotYVel=0.0f;
			downX=mouseDownPosition.x;
			downY=mouseDownPosition.y;
			travelDir=camera->getCumulativeTransformationPart1();
			CIloIlo::flyModeCameraHandle=camera->getID();
			CIloIlo::flyModeCameraHandleInstance=App::ct->getCurrentInstanceIndex();
		}
		bool translateMode=((App::mainWindow!=NULL)&&App::mainWindow->ctrlKeyDown&&App::mainWindow->shiftKeyDown);
		if (translateMode!=previousTranslateMode)
		{
			downX=mousePosition.x;
			downY=mousePosition.y;
		}
		previousTranslateMode=translateMode;

		float dt=float(VDateTime::getTimeDiffInMs(lastTime))/1000.0f;
		lastTime=ct;

		if (fabs(dt)>0.0001f)
		{
			float dx=-0.08f*float(mousePosition.x-downX)/float(activeWinSize.x);
			float dy=0.08f*float(mousePosition.y-downY)/float(activeWinSize.y);
			C4X4Matrix m(travelDir);
			C3Vector xDir((C3Vector::unitZVector^m.M.axis[2]).getNormalized());
			C3Vector yDir((m.M.axis[2]^xDir));
			float accelX=0.0f;
			float accelY=0.0f;
			if ((App::mainWindow!=NULL)&&App::mainWindow->leftKeyDown)
				accelX=0.5f;
			if ((App::mainWindow!=NULL)&&App::mainWindow->rightKeyDown)
				accelX=-0.5f;
			if ((App::mainWindow!=NULL)&&App::mainWindow->upKeyDown)
				accelY=-0.5f;
			if ((App::mainWindow!=NULL)&&App::mainWindow->downKeyDown)
				accelY=0.5f;
			if ((accelX==0.0f)&&(rotXVel!=0.0f))
				accelX=-SIM_MIN(0.5f,fabs(rotXVel)/dt)*rotXVel/fabs(rotXVel);
			if ((accelY==0.0f)&&(rotYVel!=0.0f))
				accelY=-SIM_MIN(0.5f,fabs(rotYVel)/dt)*rotYVel/fabs(rotYVel);
			rotXVel+=dt*accelX;
			rotYVel+=dt*accelY;
			if (rotXVel!=0.0f)
				rotXVel=SIM_MIN(fabs(rotXVel),0.55f)*rotXVel/fabs(rotXVel);
			if (rotYVel!=0.0f)
				rotYVel=SIM_MIN(fabs(rotYVel),0.55f)*rotYVel/fabs(rotYVel);
			rotX+=dt*rotXVel;
			rotY+=dt*rotYVel;
			if (translateMode)
			{
				m.X+=(m.M.axis[2]*velocity*dt)+(xDir*velocity*dt*dx*50.0f)+(yDir*velocity*dt*dy*50.0f);
			}
			else
			{
				if ((App::mainWindow!=NULL)&&App::mainWindow->ctrlKeyDown)
					velocity-=dt*0.5f;
				if ((App::mainWindow!=NULL)&&App::mainWindow->shiftKeyDown)
					velocity+=dt*0.5f;
				if ((velocity<0.0001f)&&(velocity>=0.0f))
					velocity=0.0001f;
				if ((velocity>-0.0001f)&&(velocity<0.0f))
					velocity=-0.0001f;
				m.X+=m.M.axis[2]*velocity*dt;
				C3Vector v(((m.M.axis[2]*fabs(velocity)*dt)+(xDir*fabs(velocity)*dt*dx)+(yDir*fabs(velocity)*dt*dy)).getNormalized());
				C3Vector w(((m.M.axis[2]*fabs(velocity)*dt)+(xDir*fabs(velocity)*dt*dx)).getNormalized());
				if (fabs(v*C3Vector::unitZVector)>0.99f)
					v=w;
				m.M.axis[2]=v;
				m.M.axis[0]=(C3Vector::unitZVector^m.M.axis[2]).getNormalized();
				m.M.axis[1]=m.M.axis[2]^m.M.axis[0];
			}
			C4X4Matrix newCameraPos(m);
			C3X3Matrix rot;
			rot.buildXRotation(-rotY);
			newCameraPos.M*=rot;
			rot.buildZRotation(rotX);
			newCameraPos.M=rot*newCameraPos.M;

			C7Vector local(camera->getLocalTransformationPart1());
			C7Vector local1(camera->getParentCumulativeTransformation().getInverse()*newCameraPos.getTransformation());
			camera->setLocalTransformation(local1);
			if (cameraParentProxy!=NULL)
			{ // We manipulate the parent object instead:
				C7Vector local1(camera->getLocalTransformation());
				camera->setLocalTransformation(local); // we reset to initial
				cameraParentProxy->setLocalTransformation(cameraParentProxy->getLocalTransformation()*local1*local.getInverse());
			}
			travelDir=m;
		}

	}
	// ****************************************************************************

}

void CSView::serialize(CSer& ar)
{
	if (ar.isStoring())
	{
		ar.storeDataName("Oid");
		ar << linkedObjectID;
		ar.flush();

		ar.storeDataName("Gps");
		ar << graphPosition[0] << graphPosition[1];
		ar << graphSize[0] << graphSize[1];
		ar.flush();

		ar.storeDataName("Rem");
		ar << _renderingMode;
		ar.flush();

		ar.storeDataName("Va2");
		BYTE dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,perspectiveDisplay);
		SIM_SET_CLEAR_BIT(dummy,1,!_canSwapViewWithMainView);
		SIM_SET_CLEAR_BIT(dummy,2,graphIsTimeGraph);
		SIM_SET_CLEAR_BIT(dummy,3,!_canBeClosed);
		SIM_SET_CLEAR_BIT(dummy,4,_xyGraphInAutoModeDuringSimulation);
		SIM_SET_CLEAR_BIT(dummy,5,_xyGraphIsOneOneProportional);
		SIM_SET_CLEAR_BIT(dummy,6,!_canBeShifted);
		SIM_SET_CLEAR_BIT(dummy,7,!_canBeResized);
		ar << dummy;
		ar.flush();


		ar.storeDataName("Va3");
		dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,_showEdges);
		SIM_SET_CLEAR_BIT(dummy,1,_thickEdges);
		SIM_SET_CLEAR_BIT(dummy,2,_fitSceneToView);
		SIM_SET_CLEAR_BIT(dummy,3,_fitSelectionToView);
		SIM_SET_CLEAR_BIT(dummy,4,!_timeGraphXInAutoModeDuringSimulation);
		SIM_SET_CLEAR_BIT(dummy,5,!_timeGraphYInAutoModeDuringSimulation);
		ar << dummy;
		ar.flush();


		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Oid")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> linkedObjectID;
				}
				if (theName.compare("Gps")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> graphPosition[0] >> graphPosition[1];
					ar >> graphSize[0] >> graphSize[1];
				}
				if (theName.compare("Rem")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _renderingMode;
				}
				if (theName.compare("Va2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					perspectiveDisplay=SIM_IS_BIT_SET(dummy,0);
					_canSwapViewWithMainView=!SIM_IS_BIT_SET(dummy,1);
					graphIsTimeGraph=SIM_IS_BIT_SET(dummy,2);
					_canBeClosed=!SIM_IS_BIT_SET(dummy,3);
					_xyGraphInAutoModeDuringSimulation=SIM_IS_BIT_SET(dummy,4);
					_xyGraphIsOneOneProportional=SIM_IS_BIT_SET(dummy,5);
					_canBeShifted=!SIM_IS_BIT_SET(dummy,6);
					_canBeResized=!SIM_IS_BIT_SET(dummy,7);
				}
				if (theName.compare("Va3")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					_showEdges=SIM_IS_BIT_SET(dummy,0);
					_thickEdges=SIM_IS_BIT_SET(dummy,1);
					_fitSceneToView=SIM_IS_BIT_SET(dummy,2);
					_fitSelectionToView=SIM_IS_BIT_SET(dummy,3);
					_timeGraphXInAutoModeDuringSimulation=!SIM_IS_BIT_SET(dummy,4);
					_timeGraphYInAutoModeDuringSimulation=!SIM_IS_BIT_SET(dummy,4);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
