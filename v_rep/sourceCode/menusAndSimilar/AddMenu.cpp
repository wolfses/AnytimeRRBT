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
#include "AddMenu.h"
#include "Tt.h"
#include "OGL.h"
#include "IloIlo.h"

#include "MeshRoutines.h"
#include "ObjectEdition.h"
#include "ShapeComponent.h"
#include "App.h"
#include "MeshManip.h"
#include "geometric.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>
#include "qdlgconvexdecomposition.h"

CAddMenu::CAddMenu()
{

}

CAddMenu::~CAddMenu()
{

}

void CAddMenu::addMenu(VMenu* menu,CSView* subView,bool onlyCamera)
{
	// subView can be null
	bool canAddChildScript=false;
	if (App::ct->objCont->getSelSize()==1)
		canAddChildScript=(App::ct->luaScriptContainer->getScriptFromObjectAttachedTo(App::ct->objCont->getSelID(0))==NULL);

	std::vector<int> rootSel;
	for (int i=0;i<App::ct->objCont->getSelSize();i++)
		rootSel.push_back(App::ct->objCont->getSelID(i));
	CObjectEdition::addRootObjectChildrenToSelection(rootSel);
	int shapesInRootSel=App::ct->objCont->getShapeNumberInSelection(&rootSel);
	int shapesAndDummiesInRootSel=App::ct->objCont->getShapeNumberInSelection(&rootSel)+App::ct->objCont->getDummyNumberInSelection(&rootSel);

	bool linkedObjIsInexistentOrNotGraphNorRenderingSens=true;
	if (subView!=NULL)
		linkedObjIsInexistentOrNotGraphNorRenderingSens=((App::ct->objCont->getGraph(subView->getLinkedObjectID())==NULL)&&(App::ct->objCont->getVisionSensor(subView->getLinkedObjectID())==NULL));
	bool itemsPresent=false;
	if (subView!=NULL)
	{
		VMenubar::appendMenuItem(menu,true,false,ADD_FLOATING_VIEW_CMD,IDS_FLOATING_VIEW_MENU_ITEM);
		itemsPresent=true;
		if (onlyCamera)
			VMenubar::appendMenuItem(menu,linkedObjIsInexistentOrNotGraphNorRenderingSens,false,ADD_CAMERA_CMD,IDS_CAMERA_MENU_ITEM);
	}
	C3DObject* associatedViewable=NULL;
	if (subView!=NULL)
		associatedViewable=App::ct->objCont->getObject(subView->getLinkedObjectID());
	if ( (subView==NULL)||((associatedViewable!=NULL)&&(associatedViewable->getObjectType()==sim_object_camera_type) ) )
	{
	if (!onlyCamera)
	{
		if (itemsPresent)
			VMenubar::appendMenuSeparator(menu);
		VMenu* prim=VMenubar::createPopupMenu();
		VMenubar::appendMenuItem(prim,true,false,ADD_PRIMITIVE_PLANE_CMD,IDS_PLANE_MENU_ITEM);
		VMenubar::appendMenuItem(prim,true,false,ADD_PRIMITIVE_DISC_CMD,IDS_DISC_MENU_ITEM);
		VMenubar::appendMenuItem(prim,true,false,ADD_PRIMITIVE_RECTANGLE_CMD,IDS_RECTANGLE_MENU_ITEM);
		VMenubar::appendMenuItem(prim,true,false,ADD_PRIMITIVE_SPHERE_CMD,IDS_SPHERE_MENU_ITEM);
		VMenubar::appendMenuItem(prim,true,false,ADD_PRIMITIVE_CYLINDER_CMD,IDS_CYLINDER_MENU_ITEM);
		VMenubar::appendMenuAndDetach(menu,prim,true,IDS_PRIMITIVE_SHAPE_MENU_ITEM);
		itemsPresent=true;

		VMenu* joint=VMenubar::createPopupMenu();
		VMenubar::appendMenuItem(joint,true,false,ADD_REVOLUTE_JOINT_CMD,IDS_REVOLUTE_MENU_ITEM);
		VMenubar::appendMenuItem(joint,true,false,ADD_PRISMATIC_JOINT_CMD,IDS_PRISMATIC_MENU_ITEM);
		VMenubar::appendMenuItem(joint,true,false,ADD_SPHERICAL_JOINT_CMD,IDS_SPHERICAL_MENU_ITEM);
		VMenubar::appendMenuAndDetach(menu,joint,true,IDS_JOINT_MENU_ITEM);
		itemsPresent=true;
	}
	else
	{
		if (itemsPresent)
			VMenubar::appendMenuSeparator(menu);
	}

	VMenubar::appendMenuItem(menu,linkedObjIsInexistentOrNotGraphNorRenderingSens,false,ADD_CAMERA_CMD,IDS_CAMERA_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,ADD_MIRROR_CMD,IDS_MIRROR_MENU_ITEM);
	itemsPresent=true;

	if (!onlyCamera)
	{
		VMenu* light=VMenubar::createPopupMenu();
		VMenubar::appendMenuItem(light,linkedObjIsInexistentOrNotGraphNorRenderingSens,false,ADD_OMNI_LIGHT_CMD,IDS_OMNIDIRECTIONAL_MENU_ITEM);
		VMenubar::appendMenuItem(light,linkedObjIsInexistentOrNotGraphNorRenderingSens,false,ADD_SPOT_LIGHT_CMD,IDS_SPOTLIGHT_MENU_ITEM);
		VMenubar::appendMenuItem(light,linkedObjIsInexistentOrNotGraphNorRenderingSens,false,ADD_DIR_LIGHT_CMD,IDS_DIRECTIONAL_MENU_ITEM);
		if (linkedObjIsInexistentOrNotGraphNorRenderingSens)
			VMenubar::appendMenuAndDetach(menu,light,true,IDS_LIGHT_MENU_ITEM);
		else
		{
			VMenubar::appendMenuItem(menu,false,false,0,IDS_LIGHT_MENU_ITEM);
			delete light;
		}

		VMenubar::appendMenuItem(menu,true,false,ADD_DUMMY_CMD,IDS_DUMMY_MENU_ITEM);

		VMenubar::appendMenuItem(menu,true,false,ADD_GRAPH_CMD,IDS_GRAPH_MENU_ITEM);

		VMenu* sens=VMenubar::createPopupMenu();
		VMenubar::appendMenuItem(sens,true,false,ADD_RAY_PROXSENSOR_CMD,IDS_RAY_TYPE_MENU_ITEM);
		VMenubar::appendMenuItem(sens,true,false,ADD_RANDOMIZED_RAY_PROXSENSOR_CMD,IDS_RANDOMIZED_RAY_TYPE_MENU_ITEM);
		VMenubar::appendMenuItem(sens,true,false,ADD_PYRAMID_PROXSENSOR_CMD,IDS_PYRAMID_TYPE_MENU_ITEM);
		VMenubar::appendMenuItem(sens,true,false,ADD_CYLINDER_PROXSENSOR_CMD,IDS_CYLINDER_TYPE_MENU_ITEM);
		VMenubar::appendMenuItem(sens,true,false,ADD_DISC_PROXSENSOR_CMD,IDS_DISC_TYPE_MENU_ITEM);
		VMenubar::appendMenuItem(sens,true,false,ADD_CONE_PROXSENSOR_CMD,IDS_CONE_TYPE_MENU_ITEM);

		VMenubar::appendMenuAndDetach(menu,sens,true,IDS_PROXSENSOR_MENU_ITEM);

		VMenu* visSens=VMenubar::createPopupMenu();
		VMenubar::appendMenuItem(visSens,true,false,ADD_ORTHOGONAL_VISION_SENSOR_CMD,IDS_VISION_SENSOR_ORTHOGONAL_TYPE_MENU_ITEM);
		VMenubar::appendMenuItem(visSens,true,false,ADD_PERSPECTIVE_VISION_SENSOR_CMD,IDS_VISION_SENSOR_PERSPECTIVE_TYPE_MENU_ITEM);
		VMenubar::appendMenuAndDetach(menu,visSens,true,IDSN_VISION_SENSOR);

		VMenubar::appendMenuItem(menu,true,false,ADD_FORCE_SENSOR_CMD,IDSN_FORCE_SENSOR);

		VMenu* mill=VMenubar::createPopupMenu();
		VMenubar::appendMenuItem(mill,true,false,ADD_PYRAMID_MILL_CMD,IDS_PYRAMID_TYPE_MENU_ITEM);
		VMenubar::appendMenuItem(mill,true,false,ADD_CYLINDER_MILL_CMD,IDS_CYLINDER_TYPE_MENU_ITEM);
		VMenubar::appendMenuItem(mill,true,false,ADD_DISC_MILL_CMD,IDS_DISC_TYPE_MENU_ITEM);
		VMenubar::appendMenuItem(mill,true,false,ADD_CONE_MILL_CMD,IDS_CONE_TYPE_MENU_ITEM);
		VMenubar::appendMenuAndDetach(menu,mill,true,IDSN_MILL);

		VMenu* pathM=VMenubar::createPopupMenu();
		VMenubar::appendMenuItem(pathM,true,false,ADD_PATH_SEGMENT_CMD,IDS_SEGMENT_TYPE_MENU_ITEM);
		VMenubar::appendMenuItem(pathM,true,false,ADD_PATH_CIRCLE_CMD,IDS_CIRCLE_TYPE_MENU_ITEM);
		VMenubar::appendMenuAndDetach(menu,pathM,true,IDSN_PATH);

		VMenu* childScript=VMenubar::createPopupMenu();
		VMenubar::appendMenuItem(childScript,true,false,ADD_AND_ASSOCIATE_NON_THREADED_CHILD_SCRIPT_CMD,IDS_NON_THREADED_MENU_ITEM);
		VMenubar::appendMenuItem(childScript,true,false,ADD_AND_ASSOCIATE_THREADED_CHILD_SCRIPT_CMD,IDS_THREADED_MENU_ITEM);

		VMenubar::appendMenuAndDetach(menu,childScript,canAddChildScript,IDS_ASSOCIATED_CHILD_SCRIPT_MENU_ITEM);

		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,shapesAndDummiesInRootSel>0,false,ADD_CONVEX_HULL_CMD,IDS_CONVEX_HULL_OF_SELECTION_MENU_ITEM);
		VMenubar::appendMenuItem(menu,shapesInRootSel>0,false,ADD_CONVEX_DECOMPOSITION_CMD,IDS_CONVEX_DECOMPOSITION_OF_SELECTION_MENU_ITEM);
	}
	}

	if ((associatedViewable!=NULL)&&(associatedViewable->getObjectType()==sim_object_graph_type)&&(subView!=NULL))
	{
		VMenubar::appendMenuSeparator(menu);
		bool itemEnabled=(subView->getTrackedGraphCurveIndex()!=-1);
		VMenubar::appendMenuItem(menu,itemEnabled,false,MAKE_GRAPH_CURVE_STATIC_CMD,IDS_MAKE_CURVE_STATIC_MENU_ITEM);
		VMenubar::appendMenuItem(menu,itemEnabled,false,COPY_GRAPH_CURVE_TO_CLIPBOARD_CMD,IDS_COPY_CURVE_TO_CLIPBOARD);
	}
}

bool CAddMenu::evaluateMenuSelection(int commandID,CSView* subView)
{ // Return value is true if the command belonged to Add menu and was executed
	if ( (commandID==ADD_PRIMITIVE_PLANE_CMD)||(commandID==ADD_PRIMITIVE_DISC_CMD)||
		(commandID==ADD_PRIMITIVE_RECTANGLE_CMD)||(commandID==ADD_PRIMITIVE_SPHERE_CMD)||
		(commandID==ADD_PRIMITIVE_CYLINDER_CMD) )
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_PRIMITIVE_..._CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_PRIMITIVE_SHAPE);
			CShape* it=addPrimitive(commandID,NULL);
			if (it!=NULL)
			{
				App::ct->objCont->deselectObjects();
				App::ct->objCont->selectObject(it->getID());
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
			else
				App::uiThread->addStatusLine(IDSNS_OPERATION_ABORTED);
		}
		return(true);
	}
	if (commandID==MAKE_GRAPH_CURVE_STATIC_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("MAKE_GRAPH_CURVE_STATIC_CMD")
		{
			int lo=-1;
			if (subView!=NULL)
				lo=subView->getLinkedObjectID();
			CGraph* graph=App::ct->objCont->getGraph(lo);
			int val=0;
			if (!subView->getTimeGraph())
				val=1;
			if ((graph!=NULL)&&(subView->getTrackedGraphCurveIndex()!=-1))
			{
				App::uiThread->addStatusLine(IDSNS_ADDING_STATIC_DUPLICATE_OF_CURVE);
				graph->makeCurveStatic(subView->getTrackedGraphCurveIndex(),val);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	if (commandID==COPY_GRAPH_CURVE_TO_CLIPBOARD_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("COPY_GRAPH_CURVE_TO_CLIPBOARD_CMD")
		{
			int lo=-1;
			if (subView!=NULL)
				lo=subView->getLinkedObjectID();
			CGraph* graph=App::ct->objCont->getGraph(lo);
			int val=0;
			if (!subView->getTimeGraph())
				val=1;
			if ((graph!=NULL)&&(subView->getTrackedGraphCurveIndex()!=-1))
			{
				App::uiThread->addStatusLine(IDSNS_CURVE_DATA_COPIED_TO_CLIPBOARD);
				graph->copyCurveToClipboard(subView->getTrackedGraphCurveIndex(),val);
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	if (commandID==ADD_FLOATING_VIEW_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_FLOATING_VIEW_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_FLOATING_VIEW);
			App::ct->oglSurface->pageCont->getPage(App::ct->oglSurface->pageCont->getActivePageIndex())->addFloatingView();
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if ( (commandID==ADD_REVOLUTE_JOINT_CMD)||(commandID==ADD_PRISMATIC_JOINT_CMD)||
		(commandID==ADD_SPHERICAL_JOINT_CMD) )
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_..._JOINT_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_JOINT);
			CJoint* newObject;
			if (commandID==ADD_REVOLUTE_JOINT_CMD)
				newObject=new CJoint(sim_joint_revolute_subtype);
			if (commandID==ADD_PRISMATIC_JOINT_CMD)
				newObject=new CJoint(sim_joint_prismatic_subtype);
			if (commandID==ADD_SPHERICAL_JOINT_CMD)
				newObject=new CJoint(sim_joint_spherical_subtype);
			App::ct->objCont->addObjectToScene(newObject,false);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::ct->objCont->selectObject(newObject->getID());
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if ( (commandID==ADD_CAMERA_CMD)||(commandID==ADD_OMNI_LIGHT_CMD)||
		(commandID==ADD_SPOT_LIGHT_CMD)||(commandID==ADD_DIR_LIGHT_CMD) )
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_CAMERA...LIGHT_CMD")
		{
			int lo=-1;
			if (subView!=NULL)
				lo=subView->getLinkedObjectID();
			CCamera* camera=App::ct->objCont->getCamera(lo);
			CGraph* graph=App::ct->objCont->getGraph(lo);
			if (graph!=NULL)
				return(true);
			CCamera* myNewCamera=NULL;
			CLight* myNewLight=NULL;
			if (commandID==ADD_CAMERA_CMD)
			{
				App::uiThread->addStatusLine(IDSNS_ADDING_A_CAMERA);
				myNewCamera=new CCamera();
				App::ct->objCont->addObjectToScene(myNewCamera,false);
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
			else
			{
				App::uiThread->addStatusLine(IDSNS_ADDING_A_LIGHT);
				int tp;
				if (commandID==ADD_OMNI_LIGHT_CMD)
					tp=sim_light_omnidirectional_subtype;
				if (commandID==ADD_SPOT_LIGHT_CMD)
					tp=sim_light_spot_subtype;
				if (commandID==ADD_DIR_LIGHT_CMD)
					tp=sim_light_directional_subtype;
				myNewLight=new CLight(tp);
				App::ct->objCont->addObjectToScene(myNewLight,false);
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
			C3DObject* addedObject=myNewCamera;
			if (addedObject==NULL)
				addedObject=myNewLight;
			addedObject->setLocalTransformation(C3Vector(0.0f,0.0f,1.0f));
			addedObject->setLocalTransformation(C4Vector(piValue*0.5f,0.0f,0.0f));
			if (camera!=NULL)
			{
				if (myNewCamera!=NULL)
				{
					App::ct->objCont->selectObject(myNewCamera->getID());
					C7Vector m(camera->getCumulativeTransformation());
					myNewCamera->setLocalTransformation(m);
					myNewCamera->scaleObject(camera->getCameraSize()/myNewCamera->getCameraSize());
					C3Vector minV,maxV;
					myNewCamera->getFullBoundingBox(minV,maxV);
					m=myNewCamera->getLocalTransformation();
					maxV-=minV;
					float averageSize=(maxV(0)+maxV(1)+maxV(2))/3.0f;
					float shiftForward=camera->getNearClippingPlane()-minV(2)+3.0f*averageSize;
					m.X+=(m.Q.getAxis(2)*shiftForward);
					myNewCamera->setLocalTransformation(m.X);
				}
			}
			else if (subView!=NULL)
			{	// When we want to add a camera to an empty window
				if (myNewCamera!=NULL)
				{
					C7Vector m;
					m.X=C3Vector(-1.12f,1.9f,1.08f);
					m.Q.setEulerAngles(C3Vector(110.933f*degToRad,28.703f*degToRad,-10.41f*degToRad));
					myNewCamera->setLocalTransformation(m);
					subView->setLinkedObjectID(myNewCamera->getID(),false);
				}
			}
			App::ct->objCont->selectObject(addedObject->getID());
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		return(true);
	}
	if (commandID==ADD_MIRROR_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_MIRROR_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_MIRROR);
			CMirror* newObject=new CMirror();
			App::ct->objCont->addObjectToScene(newObject,false);
			App::ct->objCont->setAbsoluteAngles(newObject->getID(),C3Vector(piValD2,0.0f,piValue));
			App::ct->objCont->setAbsolutePosition(newObject->getID(),newObject->getMirrorHeight()*0.5f,2);
			App::ct->objCont->selectObject(newObject->getID());
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==ADD_DUMMY_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_DUMMY_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_DUMMY);
			CDummy* newObject=new CDummy();
			App::ct->objCont->addObjectToScene(newObject,false);
			App::ct->objCont->selectObject(newObject->getID());
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==ADD_PATH_SEGMENT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_PATH_SEGMENT_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_PATH);
			CPath* newObject=new CPath();
			CSimplePathPoint* it=new CSimplePathPoint();
			C7Vector trtmp(it->getTransformation());
			trtmp.X(0)=-0.25f;
			it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
			newObject->pathContainer->addSimplePathPoint(it);
			it=it->copyYourself();
			trtmp=it->getTransformation();
			trtmp.X(0)=+0.25f;
			it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
			newObject->pathContainer->addSimplePathPoint(it);
			App::ct->objCont->addObjectToScene(newObject,false);
			App::ct->objCont->selectObject(newObject->getID());
			int atr=newObject->pathContainer->getAttributes();
			if (atr&sim_pathproperty_endpoints_at_zero)
				atr-=sim_pathproperty_endpoints_at_zero;
			newObject->pathContainer->setAttributes(atr);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if ((commandID==ADD_AND_ASSOCIATE_NON_THREADED_CHILD_SCRIPT_CMD)||(commandID==ADD_AND_ASSOCIATE_THREADED_CHILD_SCRIPT_CMD))
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_AND_ASSOCIATE...CHILD_SCRIPT_CMD")
		{
			if (App::ct->objCont->getSelSize()==1)
			{
				int scriptID=App::ct->luaScriptContainer->insertDefaultScript_mainAndChildScriptsOnly(sim_scripttype_childscript,commandID==ADD_AND_ASSOCIATE_THREADED_CHILD_SCRIPT_CMD);
				CLuaScriptObject* script=App::ct->luaScriptContainer->getScriptFromID(scriptID);
				if (script!=NULL)
					script->setObjectIDThatScriptIsAttachedTo(App::ct->objCont->getSelID(0));
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::ct->objCont->setFullDialogRefreshFlag();
			}
		}
		return(true);
	}
	if (commandID==ADD_PATH_CIRCLE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_PATH_CIRCLE_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_PATH);
			CPath* newObject=new CPath();
			CSimplePathPoint* it=NULL;
			float a=0.0f;
			float da=piValTimes2/16.0f;
			float r=0.25f/cos(360.0f*degToRad/32);
			for (int i=0;i<16;i++)
			{
				it=new CSimplePathPoint();
				it->setBezierFactors(0.95f,0.95f);
				C7Vector trtmp(it->getTransformation());
				trtmp.X(0)=r*cos(a);
				trtmp.X(1)=r*sin(a);
				it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
				newObject->pathContainer->addSimplePathPoint(it);
				a+=da;
			}
			newObject->pathContainer->setAttributes(newObject->pathContainer->getAttributes()|sim_pathproperty_closed_path);
			App::ct->objCont->addObjectToScene(newObject,false);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::ct->objCont->selectObject(newObject->getID());
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==ADD_GRAPH_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_GRAPH_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_GRAPH);
			CGraph* newObject=new CGraph();
			App::ct->objCont->addObjectToScene(newObject,false);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::ct->objCont->selectObject(newObject->getID());
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==ADD_ORTHOGONAL_VISION_SENSOR_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_ORTHOGONAL_VISION_SENSOR_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_VISION_SENSOR);
			CVisionSensor* newObject=new CVisionSensor();
			App::ct->objCont->addObjectToScene(newObject,false);
			newObject->setLocalTransformation(C3Vector(0.0f,0.0f,newObject->getSize()(2)));
			newObject->setPerspectiveOperation(false);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::ct->objCont->selectObject(newObject->getID());
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==ADD_PERSPECTIVE_VISION_SENSOR_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_PERSPECTIVE_VISION_SENSOR_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_VISION_SENSOR);
			CVisionSensor* newObject=new CVisionSensor();
			App::ct->objCont->addObjectToScene(newObject,false);
			newObject->setLocalTransformation(C3Vector(0.0f,0.0f,newObject->getSize()(2)));
			newObject->setPerspectiveOperation(true);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::ct->objCont->selectObject(newObject->getID());
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==ADD_FORCE_SENSOR_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_FORCE_SENSOR_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_FORCE_SENSOR);
			CForceSensor* newObject=new CForceSensor();
			App::ct->objCont->addObjectToScene(newObject,false);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::ct->objCont->selectObject(newObject->getID());
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if ( (commandID==ADD_RAY_PROXSENSOR_CMD)||(commandID==ADD_PYRAMID_PROXSENSOR_CMD)||(commandID==ADD_CYLINDER_PROXSENSOR_CMD)||
		(commandID==ADD_DISC_PROXSENSOR_CMD)||(commandID==ADD_CONE_PROXSENSOR_CMD)||(commandID==ADD_RANDOMIZED_RAY_PROXSENSOR_CMD) )
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD...PROXSENSOR_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_PROXIMITY_SENSOR);
			CProxSensor* newObject=new CProxSensor();
			int type=sim_proximitysensor_pyramid_subtype;
			if (commandID==ADD_CYLINDER_PROXSENSOR_CMD)
				type=sim_proximitysensor_cylinder_subtype;
			if (commandID==ADD_DISC_PROXSENSOR_CMD)
				type=sim_proximitysensor_disc_subtype;
			if (commandID==ADD_CONE_PROXSENSOR_CMD)
				type=sim_proximitysensor_cone_subtype;
			if (commandID==ADD_RAY_PROXSENSOR_CMD)
				type=sim_proximitysensor_ray_subtype;
			if (commandID==ADD_RANDOMIZED_RAY_PROXSENSOR_CMD)
				type=sim_proximitysensor_ray_subtype;
			newObject->setSensorType(type);
			if (commandID==ADD_RANDOMIZED_RAY_PROXSENSOR_CMD)
				newObject->setRandomizedDetection(true);
			App::ct->objCont->addObjectToScene(newObject,false);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::ct->objCont->selectObject(newObject->getID());
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if ( (commandID==ADD_PYRAMID_MILL_CMD)||(commandID==ADD_CYLINDER_MILL_CMD)||
		(commandID==ADD_DISC_MILL_CMD)||(commandID==ADD_CONE_MILL_CMD) )
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD...MILL_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ADDING_A_MILL);
			CMill* newObject=new CMill();
			int type=sim_mill_pyramid_subtype;
			if (commandID==ADD_CYLINDER_MILL_CMD)
				type=sim_mill_cylinder_subtype;
			if (commandID==ADD_DISC_MILL_CMD)
				type=sim_mill_disc_subtype;
			if (commandID==ADD_CONE_MILL_CMD)
				type=sim_mill_cone_subtype;
			newObject->setMillType(type);
			App::ct->objCont->addObjectToScene(newObject,false);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::ct->objCont->selectObject(newObject->getID());
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==ADD_CONVEX_DECOMPOSITION_CMD)
	{
		IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG_CMD("ADD_CONVEX_DECOMPOSITION_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			CObjectEdition::addRootObjectChildrenToSelection(sel);

			std::vector<int> newSelection;

			bool addExtraDistPoints=true;
			bool addFacesPoints=true;
			int nClusters=1;
			int maxHullVertices=200; // from 100 to 200 on 5/2/2014
			float maxConcavity=100.0f;
			float smallClusterThreshold=0.25f;
			int maxTrianglesInDecimatedMesh=500;
			float maxConnectDist=30.0f;
			bool individuallyConsiderMultishapeComponents=false;
			bool randomColors=false;
			int maxIterations=4;
			CQDlgConvexDecomposition theDialog(App::mainWindow);
			theDialog.refresh();
			if (theDialog.makeDialogModal()==VDIALOG_MODAL_RETURN_CANCEL)
				return(false);
			addExtraDistPoints=theDialog.addExtraDistPoints;
			addFacesPoints=theDialog.addFacesPoints;
			nClusters=theDialog.nClusters;
			maxHullVertices=theDialog.maxHullVertices;
			maxConcavity=theDialog.maxConcavity;
			smallClusterThreshold=theDialog.smallClusterThreshold;
			maxTrianglesInDecimatedMesh=theDialog.maxTrianglesInDecimatedMesh;
			maxConnectDist=theDialog.maxConnectDist;
			individuallyConsiderMultishapeComponents=theDialog.individuallyConsiderMultishapeComponents;
			randomColors=theDialog.randomColors;
			maxIterations=theDialog.maxIterations;
			App::uiThread->addStatusLine(IDSNS_ADDING_CONVEX_DECOMPOSITION);
			for (int obji=0;obji<int(sel.size());obji++)
			{
				CShape* oldShape=App::ct->objCont->getShape(sel[obji]);
				if (oldShape!=NULL)
				{
					int newShapeHandle=CObjectEdition::generateConvexDecomposed(sel[obji],nClusters,maxConcavity,addExtraDistPoints,addFacesPoints,maxConnectDist,maxTrianglesInDecimatedMesh,maxHullVertices,smallClusterThreshold,individuallyConsiderMultishapeComponents,randomColors,maxIterations);
					if (newShapeHandle!=-1)
					{
						// Get the mass and inertia info from the old shape:
						C7Vector absCOM(oldShape->getCumulativeTransformation());
						absCOM=absCOM*oldShape->geomData->geomInfo->getLocalInertiaFrame();
						float mass=oldShape->geomData->geomInfo->getMass();
						C7Vector absCOMNoShift(absCOM);
						absCOMNoShift.X.clear(); // we just wanna get the orientation of the inertia matrix, no shift info!
						C3X3Matrix tensor(CGeomWrap::getNewTensor(oldShape->geomData->geomInfo->getPrincipalMomentsOfInertia(),absCOMNoShift));

						// Fix the name and add the new shape to the new selection vector:
						CShape* newShape=App::ct->objCont->getShape(newShapeHandle);
						std::string baseN("generated_part");
						std::string n(baseN);
						int suff=0;
						while (App::ct->objCont->getObject(n)!=NULL)
							n=baseN+boost::lexical_cast<std::string>(suff++);
						newShape->setName(n);
						newSelection.push_back(newShapeHandle);

						// Transfer the mass and inertia info to the new shape:
						newShape->geomData->geomInfo->setMass(mass);
						C4Vector rot;
						C3Vector pmoi;
						CGeomWrap::findPrincipalMomentOfInertia(tensor,rot,pmoi);
						newShape->geomData->geomInfo->setPrincipalMomentsOfInertia(pmoi);
						absCOM.Q=rot;
						C7Vector relCOM(newShape->getCumulativeTransformation().getInverse()*absCOM);
						newShape->geomData->geomInfo->setLocalInertiaFrame(relCOM);
					}
				}
			}
			App::ct->objCont->deselectObjects();
			for (int i=0;i<int(newSelection.size());i++)
				App::ct->objCont->addObjectToSelection(newSelection[i]);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}

	if (commandID==ADD_CONVEX_HULL_CMD)
	{ 
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ADD_CONVEX_HULL_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			std::vector<int> newSelection;
			std::vector<int> tmp;
			for (int i=0;i<int(sel.size());i++)
				tmp.push_back(sel[i]);
			CObjectEdition::addRootObjectChildrenToSelection(tmp);

			// Now keep only visible objects:
			std::vector<int> rootSel;
			for (int i=0;i<int(tmp.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(tmp[i]);
				if ( (it!=NULL)&&(!it->isObjectPartOfInvisibleModel())&&(App::ct->mainSettings->activeLayers&it->layer) )
					rootSel.push_back(tmp[i]);
			}


			std::vector<float> allHullVertices;
			allHullVertices.reserve(40000*3);
			std::vector<bool> selectedDummies(App::ct->objCont->getHighestObjectID()+1,false);
			App::uiThread->addStatusLine(IDSNS_ADDING_CONVEX_HULL);
			for (int i=0;i<int(rootSel.size());i++)
			{
				CShape* it=App::ct->objCont->getShape(rootSel[i]);
				if (it!=NULL)
				{
					C7Vector transf(it->getCumulativeTransformation());
					std::vector<float> vert;
					std::vector<float> vertD;
					std::vector<int> ind;
					it->geomData->geomInfo->getCumulativeMeshes(vertD,&ind,NULL);
					for (int j=0;j<int(vertD.size())/3;j++)
					{
						C3Vector v(&vertD[3*j+0]);
						v=transf*v;
						allHullVertices.push_back(v(0));
						allHullVertices.push_back(v(1));
						allHullVertices.push_back(v(2));
					}
				}
				// here we take care of dummies
				CDummy* dum=App::ct->objCont->getDummy(rootSel[i]);
				if (dum!=NULL)
				{
					if (!selectedDummies[dum->getID()])
					{
						C3Vector v(dum->getCumulativeTransformation().X);
						allHullVertices.push_back(v(0));
						allHullVertices.push_back(v(1));
						allHullVertices.push_back(v(2));
						selectedDummies[dum->getID()]=true;
					}
				}
			}
			App::ct->objCont->deselectObjects();
			if (allHullVertices.size()!=0)
			{
				std::vector<float> hull;
				std::vector<int> indices;
				std::vector<float> normals;
				CMeshRoutines::getConvexHull(&allHullVertices,NULL,&hull,&indices);
				CGeomProxy* geom=new CGeomProxy(NULL,hull,indices,NULL,NULL);
				CShape* it=new CShape();
				it->setLocalTransformation(geom->getCreationTransformation());
				geom->setCreationTransformation(C7Vector::identityTransformation);
				it->geomData=geom;
				geom->geomInfo->setGouraudShadingAngle(0.0f);
				geom->geomInfo->setEdgeThresholdAngle(0.0f);
				((CGeometric*)geom->geomInfo)->setVisibleEdges(true);
				geom->geomInfo->setLocalInertiaFrame(C7Vector::identityTransformation);
				App::ct->objCont->addObjectToScene(it,false);
				App::ct->objCont->addObjectToSelection(it->getID());
			}
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}

	return(false);
}

CShape* CAddMenu::addPrimitive(int command,C3Vector* sizes)
{ // if sizes is not NULL, then sizes are locked in the dialog
	CQDlgPrimitives theDialog(App::mainWindow);
	if (command==ADD_PRIMITIVE_PLANE_CMD)
	{
		theDialog.initialize(0,sizes);
		if (theDialog.makeDialogModal()==VDIALOG_MODAL_RETURN_CANCEL)
			return(NULL);
	}
	if (command==ADD_PRIMITIVE_RECTANGLE_CMD)
	{
		theDialog.initialize(1,sizes);
		if (theDialog.makeDialogModal()==VDIALOG_MODAL_RETURN_CANCEL)
			return(NULL);
	}
	if (command==ADD_PRIMITIVE_SPHERE_CMD)
	{
		theDialog.initialize(2,sizes);
		if (theDialog.makeDialogModal()==VDIALOG_MODAL_RETURN_CANCEL)
			return(NULL);
	}
	if (command==ADD_PRIMITIVE_CYLINDER_CMD)
	{
		theDialog.initialize(3,sizes);
		if (theDialog.makeDialogModal()==VDIALOG_MODAL_RETURN_CANCEL)
			return(NULL);
	}	
	if (command==ADD_PRIMITIVE_DISC_CMD)
	{
		theDialog.initialize(4,sizes);
		if (theDialog.makeDialogModal()==VDIALOG_MODAL_RETURN_CANCEL)
			return(NULL);
	}	
	CShape* it=addPrimitiveWithoutDialog(command,theDialog);
	return(it);
}		

CShape* CAddMenu::addPrimitiveWithoutDialog(int command,CQDlgPrimitives& theDialog)
{
	if (command==ADD_PRIMITIVE_PLANE_CMD)
	{
		int divX=theDialog.subdivX+1;
		int divY=theDialog.subdivY+1;
		float xhSize=theDialog.xSize/2.0f;
		float yhSize=theDialog.ySize/2.0f;
		float xs=theDialog.xSize/((float)divX);
		float ys=theDialog.ySize/((float)divY);
		std::vector<float> vertices;
		std::vector<int> indices;
		std::vector<float> normals;
		vertices.reserve((divX+1)*(divY+1)*3);
		indices.reserve(divX*divY*6);
		normals.reserve(divX*divY*6*3);
		// We first create the vertices:
		for (int i=0;i<(divY+1);i++)
		{ // along y
			for (int j=0;j<(divX+1);j++)
			{ // along x
				tt::addToFloatArray(&vertices,-xhSize+j*xs,-yhSize+i*ys,0.0f);	
			}
		}
		// Now we create the indices:
		for (int i=0;i<divY;i++)
		{ // along y
			for (int j=0;j<divX;j++)
			{ // along x
				tt::addToIntArray(&indices,i*(divX+1)+j,i*(divX+1)+j+1,(i+1)*(divX+1)+j+1);
				tt::addToIntArray(&indices,i*(divX+1)+j,(i+1)*(divX+1)+j+1,(i+1)*(divX+1)+j);
			}
		}
		// And now the normals:
		for (int i=0;i<divX*divY*6;i++)
			tt::addToFloatArray(&normals,0.0f,0.0f,1.0f);

		CGeomProxy* geom=new CGeomProxy(NULL,vertices,indices,NULL,NULL);
		CShape* it=new CShape();
		it->setLocalTransformation(geom->getCreationTransformation());
		((CGeometric*)geom->geomInfo)->color.setDefaultValues();
		((CGeometric*)geom->geomInfo)->color.setColor(0.75f,0.75f,0.75f,AMBIENT_MODE);
		((CGeometric*)geom->geomInfo)->setVisibleEdges(true);
		geom->setCreationTransformation(C7Vector::identityTransformation);
		((CGeometric*)geom->geomInfo)->setGouraudShadingAngle(20.0f*degToRad);
		((CGeometric*)geom->geomInfo)->setEdgeThresholdAngle(20.0f*degToRad);
		it->geomData=geom;
		it->setName(IDSOGL_PLANE);
		App::ct->objCont->addObjectToScene(it,false);
		it->setLocalTransformation(C3Vector(0.0f,0.0f,0.002f)); // we shift the plane so that it is above the floor
		it->alignBoundingBoxWithWorld();
		if (theDialog.pure)
		{
			((CGeometric*)geom->geomInfo)->setPurePrimitiveType(sim_pure_primitive_plane,theDialog.xSize,theDialog.ySize,0.0f);
			int propToRemove=sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable|sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_renderable|sim_objectspecialproperty_cuttable;
			it->setMainPropertyLocal((it->getMainPropertyLocal()|propToRemove)-propToRemove);
			it->setRespondable(true);
			it->setShapeIsDynamicallyStatic(false);
			((CGeometric*)it->geomData->geomInfo)->color.setColor(0.645f,0.66f,0.855f,AMBIENT_MODE);
		}
		geom->geomInfo->setMass(theDialog.xSize*theDialog.ySize*theDialog.density*0.001f); // we assume 1mm thickness
		geom->geomInfo->setPrincipalMomentsOfInertia(C3Vector(theDialog.ySize*theDialog.ySize/12.0f,theDialog.xSize*theDialog.xSize/12.0f,(theDialog.xSize*theDialog.xSize+theDialog.ySize*theDialog.ySize)/12.0f));

		// make sure that object rests, stacks and can be easily grasped:
		CDynMaterialObject* mat=App::ct->dynMaterialCont->getRestStackGraspMaterial();
		if (mat!=NULL)
			geom->geomInfo->setDynMaterialId(mat->getObjectID());

		return(it);
	}
	if (command==ADD_PRIMITIVE_RECTANGLE_CMD)
	{
		int divX=theDialog.subdivX+1;
		int divY=theDialog.subdivY+1;
		int divZ=theDialog.subdivZ+1;
		float xhSize=theDialog.xSize/2.0f;
		float yhSize=theDialog.ySize/2.0f;
		float zhSize=theDialog.zSize/2.0f;
		std::vector<float> vertices;
		std::vector<int> indices;
		vertices.reserve(((divX+1)*(2*divY+2*divZ)+2*(divY-1)*(divZ-1))*3);
		indices.reserve(divX*divY*12+divX*divZ*12+divY*divZ*12);
		int theDiv[3]={divX,divY,divZ};

		float sca=theDialog.negativeVolumeScaling;
		CMeshRoutines::createCube(vertices,indices,C3Vector(xhSize*2.0f,yhSize*2.0f,zhSize*2.0f),theDiv);
		if (theDialog.negativeVolume)
		{
			std::vector<float> auxVert;
			std::vector<int> auxInd;
			auxVert.reserve(((divX+1)*(2*divY+2*divZ)+2*(divY-1)*(divZ-1))*3);
			auxInd.reserve(divX*divY*12+divX*divZ*12+divY*divZ*12);
			CMeshRoutines::createCube(auxVert,auxInd,C3Vector(xhSize*2.0f*sca,yhSize*2.0f*sca,zhSize*2.0f*sca),theDiv);
			CMeshManip::mergeWith(&vertices,&indices,NULL,&auxVert,&auxInd,NULL);
		}

		CGeomProxy* geom=new CGeomProxy(NULL,vertices,indices,NULL,NULL);
		CShape* it=new CShape();
		it->setLocalTransformation(geom->getCreationTransformation());
		((CGeometric*)geom->geomInfo)->color.setDefaultValues();
		((CGeometric*)geom->geomInfo)->color.setColor(0.75f,0.75f,0.75f,AMBIENT_MODE);
		((CGeometric*)geom->geomInfo)->setVisibleEdges(true);
		geom->setCreationTransformation(C7Vector::identityTransformation);
		((CGeometric*)geom->geomInfo)->setGouraudShadingAngle(20.0f*degToRad);
		((CGeometric*)geom->geomInfo)->setEdgeThresholdAngle(20.0f*degToRad);
		it->geomData=geom;
		it->setName(IDSOGL_RECTANGLE);
		App::ct->objCont->addObjectToScene(it,false);
		it->setLocalTransformation(C3Vector(0.0f,0.0f,zhSize)); // we shift the rectangle so that it sits on the floor
		it->alignBoundingBoxWithWorld();
		if (theDialog.pure)
		{
			((CGeometric*)geom->geomInfo)->setPurePrimitiveType(sim_pure_primitive_cuboid,theDialog.xSize,theDialog.ySize,theDialog.zSize);
			int propToRemove=sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable|sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_renderable|sim_objectspecialproperty_cuttable;
			it->setMainPropertyLocal((it->getMainPropertyLocal()|propToRemove)-propToRemove);
			it->setRespondable(true);
			it->setShapeIsDynamicallyStatic(false);
			((CGeometric*)it->geomData->geomInfo)->color.setColor(0.645f,0.66f,0.855f,AMBIENT_MODE);
		}

		if (theDialog.negativeVolume)
		{
			// For now, keep a mass and moment of inertia as if the shape was not hollow!
			//float sca3=sca*sca*sca;
			//float sca2=sca*sca;
			//geom->geomInfo->setMass(theDialog.xSize*theDialog.ySize*theDialog.zSize*theDialog.density*(1.0f-sca3));
			//geom->geomInfo->setPrincipalMomentsOfInertia(C3Vector(((theDialog.ySize*theDialog.ySize+theDialog.zSize*theDialog.zSize)/12.0f)*(1.0f-sca2),((theDialog.xSize*theDialog.xSize+theDialog.zSize*theDialog.zSize)/12.0f)*(1.0f-sca2),((theDialog.xSize*theDialog.xSize+theDialog.ySize*theDialog.ySize)/12.0f)*(1.0f-sca2)));
			((CGeometric*)it->geomData->geomInfo)->setCulling(true);
			((CGeometric*)it->geomData->geomInfo)->flipFaces();
			((CGeometric*)it->geomData->geomInfo)->setPurePrimitiveInsideScaling(theDialog.negativeVolumeScaling);
		}

		geom->geomInfo->setMass(theDialog.xSize*theDialog.ySize*theDialog.zSize*theDialog.density);
		geom->geomInfo->setPrincipalMomentsOfInertia(C3Vector((theDialog.ySize*theDialog.ySize+theDialog.zSize*theDialog.zSize)/12.0f,(theDialog.xSize*theDialog.xSize+theDialog.zSize*theDialog.zSize)/12.0f,(theDialog.xSize*theDialog.xSize+theDialog.ySize*theDialog.ySize)/12.0f));

		// make sure that object rests, stacks and can be easily grasped:
		CDynMaterialObject* mat=App::ct->dynMaterialCont->getRestStackGraspMaterial();
		if (mat!=NULL)
			geom->geomInfo->setDynMaterialId(mat->getObjectID());

		return(it);
	}
	if (command==ADD_PRIMITIVE_SPHERE_CMD)
	{
		int sides=theDialog.sides;
		int faces=theDialog.faces;
		float xhSize=theDialog.xSize/2.0f;
		float yhSize=theDialog.ySize/2.0f;
		float zhSize=theDialog.zSize/2.0f;
		std::vector<float> vertices;
		std::vector<int> indices;
		vertices.reserve(((faces-1)*sides+2)*3);
		indices.reserve((sides*2+2*(faces-2)*sides)*3);
		bool smooth=theDialog.smooth;
		if (sides<3) sides=3;
		if (faces<2) faces=2;


		float sca=theDialog.negativeVolumeScaling;
		CMeshRoutines::createSphere(vertices,indices,C3Vector(xhSize*2.0f,yhSize*2.0f,zhSize*2.0f),sides,faces);
		if (theDialog.negativeVolume)
		{
			std::vector<float> auxVert;
			std::vector<int> auxInd;
			auxVert.reserve(((faces-1)*sides+2)*3);
			auxInd.reserve((sides*2+2*(faces-2)*sides)*3);
			CMeshRoutines::createSphere(auxVert,auxInd,C3Vector(xhSize*2.0f*sca,yhSize*2.0f*sca,zhSize*2.0f*sca),sides,faces);
			CMeshManip::mergeWith(&vertices,&indices,NULL,&auxVert,&auxInd,NULL);
		}

		CGeomProxy* geom=new CGeomProxy(NULL,vertices,indices,NULL,NULL);
		CShape* it=new CShape();
		it->setLocalTransformation(geom->getCreationTransformation());
		((CGeometric*)geom->geomInfo)->color.setDefaultValues();
		((CGeometric*)geom->geomInfo)->color.setColor(0.75f,0.75f,0.75f,AMBIENT_MODE);
		geom->setCreationTransformation(C7Vector::identityTransformation);
		if (smooth)
		{
			((CGeometric*)geom->geomInfo)->setGouraudShadingAngle(20.0f*degToRad);
			((CGeometric*)geom->geomInfo)->setEdgeThresholdAngle(20.0f*degToRad);
		}
		it->geomData=geom;
		it->setName(IDSOGL_SPHERE);
		App::ct->objCont->addObjectToScene(it,false);
		it->setLocalTransformation(C3Vector(0.0f,0.0f,zhSize)); // we shift the sphere so that it sits on the floor
		it->alignBoundingBoxWithWorld();
		if (theDialog.pure)
		{
			((CGeometric*)geom->geomInfo)->setPurePrimitiveType(sim_pure_primitive_spheroid,theDialog.xSize,theDialog.ySize,theDialog.zSize);
			int propToRemove=sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable|sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_renderable|sim_objectspecialproperty_cuttable;
			it->setMainPropertyLocal((it->getMainPropertyLocal()|propToRemove)-propToRemove);
			it->setRespondable(true);
			it->setShapeIsDynamicallyStatic(false);
			((CGeometric*)it->geomData->geomInfo)->color.setColor(0.645f,0.66f,0.855f,AMBIENT_MODE);
		}
		float avR=(theDialog.xSize+theDialog.ySize+theDialog.zSize)/6.0f;

		if (theDialog.negativeVolume)
		{
			// For now, keep a mass and moment of inertia as if the shape was not hollow!
			//float sca3=sca*sca*sca;
			//float sca5=sca3*sca*sca;
			//geom->geomInfo->setMass((4.0f*piValue/3.0f)*avR*avR*avR*theDialog.density*(1.0f-sca3));
			//geom->geomInfo->setPrincipalMomentsOfInertia(C3Vector((2.0f*avR*avR/5.0f)*((1.0f-sca5)/(1.0f-sca3)),(2.0f*avR*avR/5.0f)*((1.0f-sca5)/(1.0f-sca3)),(2.0f*avR*avR/5.0f)*((1.0f-sca5)/(1.0f-sca3))));
			//float avr2=avR*2.0f;
			//geom->geomInfo->scaleMassAndInertia(theDialog.xSize/avr2,theDialog.ySize/avr2,theDialog.zSize/avr2);

			((CGeometric*)it->geomData->geomInfo)->setCulling(true);
			((CGeometric*)it->geomData->geomInfo)->flipFaces();
			((CGeometric*)it->geomData->geomInfo)->setPurePrimitiveInsideScaling(theDialog.negativeVolumeScaling);
		}

		geom->geomInfo->setMass((4.0f*piValue/3.0f)*avR*avR*avR*theDialog.density);
		geom->geomInfo->setPrincipalMomentsOfInertia(C3Vector(2.0f*avR*avR/5.0f,2.0f*avR*avR/5.0f,2.0f*avR*avR/5.0f));
		float avr2=avR*2.0f;
		geom->geomInfo->scaleMassAndInertia(theDialog.xSize/avr2,theDialog.ySize/avr2,theDialog.zSize/avr2);

		// make sure that object rests, stacks and can be easily grasped:
		CDynMaterialObject* mat=App::ct->dynMaterialCont->getRestStackGraspMaterial();
		if (mat!=NULL)
			geom->geomInfo->setDynMaterialId(mat->getObjectID());

		return(it);
	}
	if (command==ADD_PRIMITIVE_CYLINDER_CMD)
	{
		int sides=theDialog.sides;
		int faces=theDialog.faces+1;
		float xhSize=theDialog.xSize/2.0f;
		float yhSize=theDialog.ySize/2.0f;
		float zhSize=theDialog.zSize/2.0f;
		int discDiv=theDialog.discSubdiv+1;
		std::vector<float> vertices;
		std::vector<int> indices;
		vertices.reserve((sides*(1+faces)+2+(discDiv-1)*sides*2)*3);
		indices.reserve((sides*2+sides*faces*2+(discDiv-1)*4*sides)*3);
		bool smooth=theDialog.smooth;
		bool open=theDialog.open;
		bool cone=theDialog.cone;
		if (sides<3)
			sides=3;

		float sca=theDialog.negativeVolumeScaling;
		if (theDialog.negativeVolume)
		{
			CMeshRoutines::createCylinder(vertices,indices,C3Vector(xhSize*2.0f,yhSize*2.0f,zhSize*2.0f),sides,faces,discDiv,true,false);
			std::vector<float> auxVert;
			std::vector<int> auxInd;
			auxVert.reserve((sides*(1+faces)+2+(discDiv-1)*sides*2)*3);
			auxInd.reserve((sides*2+sides*faces*2+(discDiv-1)*4*sides)*3);
			CMeshRoutines::createCylinder(auxVert,auxInd,C3Vector(xhSize*2.0f*sca,yhSize*2.0f*sca,zhSize*2.0f),sides,faces,discDiv,true,false);
			CMeshManip::mergeWith(&vertices,&indices,NULL,&auxVert,&auxInd,NULL);
			CMeshRoutines::createAnnulus(auxVert,auxInd,xhSize*2.0f,xhSize*2.0f*sca,-zhSize,sides,true);
			CMeshManip::mergeWith(&vertices,&indices,NULL,&auxVert,&auxInd,NULL);
			CMeshRoutines::createAnnulus(auxVert,auxInd,xhSize*2.0f,xhSize*2.0f*sca,+zhSize,sides,false);
			CMeshManip::mergeWith(&vertices,&indices,NULL,&auxVert,&auxInd,NULL);
		}
		else
			CMeshRoutines::createCylinder(vertices,indices,C3Vector(xhSize*2.0f,yhSize*2.0f,zhSize*2.0f),sides,faces,discDiv,open,cone);

		CGeomProxy* geom=new CGeomProxy(NULL,vertices,indices,NULL,NULL);
		CShape* it=new CShape();
		it->setLocalTransformation(geom->getCreationTransformation());
		((CGeometric*)geom->geomInfo)->color.setDefaultValues();
		((CGeometric*)geom->geomInfo)->color.setColor(0.75f,0.75f,0.75f,AMBIENT_MODE);
		((CGeometric*)geom->geomInfo)->setVisibleEdges(true);
		geom->setCreationTransformation(C7Vector::identityTransformation);
		if (smooth)
		{
			((CGeometric*)geom->geomInfo)->setGouraudShadingAngle(20.0f*degToRad);
			((CGeometric*)geom->geomInfo)->setEdgeThresholdAngle(20.0f*degToRad);
		}
		it->geomData=geom;
		it->setName(IDSOGL_CYLINDER);
		App::ct->objCont->addObjectToScene(it,false);
		it->alignBoundingBoxWithWorld();
		it->setLocalTransformation(C3Vector(0.0f,0.0f,zhSize)); // Now we shift the cylinder so it sits on the floor
		if (theDialog.pure)
		{
			if (theDialog.cone)
				((CGeometric*)geom->geomInfo)->setPurePrimitiveType(sim_pure_primitive_cone,theDialog.xSize,theDialog.ySize,theDialog.zSize);
			else
				((CGeometric*)geom->geomInfo)->setPurePrimitiveType(sim_pure_primitive_cylinder,theDialog.xSize,theDialog.ySize,theDialog.zSize);
			int propToRemove=sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable|sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_renderable|sim_objectspecialproperty_cuttable;
			it->setMainPropertyLocal((it->getMainPropertyLocal()|propToRemove)-propToRemove);
			it->setRespondable(true);
			it->setShapeIsDynamicallyStatic(false);
			((CGeometric*)it->geomData->geomInfo)->color.setColor(0.645f,0.66f,0.855f,AMBIENT_MODE);
		}
		float avR=(theDialog.xSize+theDialog.ySize)/4.0f;
		float divider=1.0f;
		if (theDialog.cone)
			divider=3.0f;


		if (theDialog.negativeVolume)
		{
			// For now, keep a mass and moment of inertia as if the shape was not hollow!
			//float sca3=sca*sca*sca;
			//float sca2=sca*sca;
			//geom->geomInfo->setMass(piValue*avR*avR*divider*theDialog.zSize*theDialog.density*(1.0f-sca2));
			//geom->geomInfo->setPrincipalMomentsOfInertia(C3Vector((3.0f*avR*avR*(1.0f-sca2)+theDialog.zSize*theDialog.zSize)/12.0f,(3.0f*avR*avR*(1.0f-sca2)+theDialog.zSize*theDialog.zSize)/12.0f,(avR*avR/2.0f)*(1.0f-sca2)));
			//float avR2=avR*2.0f;
			//geom->geomInfo->scaleMassAndInertia(theDialog.xSize/avR2,theDialog.ySize/avR2,1.0f);

			((CGeometric*)it->geomData->geomInfo)->setCulling(true);
			((CGeometric*)it->geomData->geomInfo)->flipFaces();
			((CGeometric*)it->geomData->geomInfo)->setPurePrimitiveInsideScaling(theDialog.negativeVolumeScaling);
		}
		geom->geomInfo->setMass(piValue*avR*avR*divider*theDialog.zSize*theDialog.density);
		if (theDialog.cone)
			geom->geomInfo->setPrincipalMomentsOfInertia(C3Vector(3.0f*(0.25f*avR*avR+theDialog.zSize*theDialog.zSize)/5.0f,3.0f*(0.25f*avR*avR+theDialog.zSize*theDialog.zSize)/5.0f,3.0f*avR*avR/10.0f));
		else
			geom->geomInfo->setPrincipalMomentsOfInertia(C3Vector((3.0f*avR*avR+theDialog.zSize*theDialog.zSize)/12.0f,(3.0f*avR*avR+theDialog.zSize*theDialog.zSize)/12.0f,avR*avR/2.0f));
		float avR2=avR*2.0f;
		geom->geomInfo->scaleMassAndInertia(theDialog.xSize/avR2,theDialog.ySize/avR2,1.0f);

		// make sure that object rests, stacks and can be easily grasped:
		CDynMaterialObject* mat=App::ct->dynMaterialCont->getRestStackGraspMaterial();
		if (mat!=NULL)
			geom->geomInfo->setDynMaterialId(mat->getObjectID());

		return(it);
	}
	if (command==ADD_PRIMITIVE_DISC_CMD)
	{
		int sides=theDialog.sides;
		float xhSize=theDialog.xSize/2.0f;
		float yhSize=theDialog.ySize/2.0f;
		int discDiv=theDialog.discSubdiv+1;
		std::vector<float> vertices;
		std::vector<int> indices;
		vertices.reserve((discDiv*sides*2+2)*3);
		indices.reserve(((discDiv-1)*2*sides)*3);
		float dd=1.0f/((float)discDiv);
		if (sides<3) sides=3;
		float sa=2.0f*piValue/((float)sides);
		// The two middle vertices:
		int sideStart=1;
		tt::addToFloatArray(&vertices,0.0f,0.0f,0.0f);	
		for (int i=0;i<sides;i++)
		{ // The side vertices:
			tt::addToFloatArray(&vertices,(float)cos(sa*i),(float)sin(sa*i),0.0f);	
		}
		int dstStart=vertices.size()/3;
		// The disc subdivision vertices:
		for (int i=1;i<discDiv;i++)
			for (int j=0;j<sides;j++)
				tt::addToFloatArray(&vertices,(1.0f-dd*i)*(float)cos(sa*j),(1.0f-dd*i)*(float)sin(sa*j),0.0f);	
		// We set up the indices:
		for (int i=0;i<sides-1;i++)
		{
			if (discDiv==1)
				tt::addToIntArray(&indices,0,i+sideStart,(i+1)+sideStart);
			else
			{
				tt::addToIntArray(&indices,0,dstStart+i+sides*(discDiv-2),dstStart+i+sides*(discDiv-2)+1);
				for (int j=0;j<discDiv-2;j++)
				{
					tt::addToIntArray(&indices,dstStart+j*sides+i,dstStart+j*sides+i+1,dstStart+(j+1)*sides+i);	
					tt::addToIntArray(&indices,dstStart+j*sides+i+1,dstStart+(j+1)*sides+i+1,dstStart+(j+1)*sides+i);	
				}
				tt::addToIntArray(&indices,sideStart+i,sideStart+(i+1),dstStart+i);
				tt::addToIntArray(&indices,sideStart+(i+1),dstStart+i+1,dstStart+i);
			}
		}

		// We have to close the cylinder here:
		// First top and bottom part:
		if (discDiv==1)
			tt::addToIntArray(&indices,0,(sides-1)+sideStart,sideStart);
		else
		{
			tt::addToIntArray(&indices,0,dstStart+sides*(discDiv-1)-1,dstStart+sides*(discDiv-2));
			for (int j=0;j<discDiv-2;j++)
			{
				tt::addToIntArray(&indices,dstStart+j*sides+sides-1,dstStart+j*sides,dstStart+(j+2)*sides-1);	
				tt::addToIntArray(&indices,dstStart+j*sides,dstStart+(j+1)*sides,dstStart+(j+2)*sides-1);	
			}
			tt::addToIntArray(&indices,sideStart+(sides-1),sideStart,dstStart+sides-1);
			tt::addToIntArray(&indices,sideStart,dstStart,dstStart+sides-1);
		}
		// Now we scale the disc:
		for (int i=0;i<int(vertices.size())/3;i++)
		{
			C3Vector p(vertices[3*i+0],vertices[3*i+1],0.0f);
			p(0)=p(0)*xhSize;
			p(1)=p(1)*yhSize;
			vertices[3*i+0]=p(0);
			vertices[3*i+1]=p(1);
		}

		CGeomProxy* geom=new CGeomProxy(NULL,vertices,indices,NULL,NULL);
		CShape* it=new CShape();
		it->setLocalTransformation(geom->getCreationTransformation());
		((CGeometric*)geom->geomInfo)->color.setDefaultValues();
		((CGeometric*)geom->geomInfo)->color.setColor(0.75f,0.75f,0.75f,AMBIENT_MODE);
		((CGeometric*)geom->geomInfo)->setVisibleEdges(true);
		geom->setCreationTransformation(C7Vector::identityTransformation);
		((CGeometric*)geom->geomInfo)->setGouraudShadingAngle(20.0f*degToRad);
		((CGeometric*)geom->geomInfo)->setEdgeThresholdAngle(20.0f*degToRad);
		it->geomData=geom;
		it->setName(IDSOGL_DISC);
		App::ct->objCont->addObjectToScene(it,false);
		it->setLocalTransformation(C3Vector(0.0f,0.0f,0.002f)); // Now we shift the disc so it sits just above the floor
		it->alignBoundingBoxWithWorld();
		if (theDialog.pure)
		{
			((CGeometric*)geom->geomInfo)->setPurePrimitiveType(sim_pure_primitive_disc,theDialog.xSize,theDialog.ySize,0.0f);
			int propToRemove=sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable|sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_renderable|sim_objectspecialproperty_cuttable;
			it->setMainPropertyLocal((it->getMainPropertyLocal()|propToRemove)-propToRemove);
			it->setRespondable(true);
			it->setShapeIsDynamicallyStatic(false);
			((CGeometric*)it->geomData->geomInfo)->color.setColor(0.645f,0.66f,0.855f,AMBIENT_MODE);
		}
		float avR=(theDialog.xSize+theDialog.ySize)/4.0f;
		geom->geomInfo->setMass(piValue*avR*avR*theDialog.density*0.001f); // we assume 1mm thickness
		geom->geomInfo->setPrincipalMomentsOfInertia(C3Vector(3.0f*(avR*avR)/12.0f,3.0f*(avR*avR)/12.0f,avR*avR/2.0f));
		float avR2=avR*2.0f;
		geom->geomInfo->scaleMassAndInertia(theDialog.xSize/avR2,theDialog.ySize/avR2,1.0f);

		// make sure that object rests, stacks and can be easily grasped:
		CDynMaterialObject* mat=App::ct->dynMaterialCont->getRestStackGraspMaterial();
		if (mat!=NULL)
			geom->geomInfo->setDynMaterialId(mat->getObjectID());

		return(it);
	}
	return(NULL);
}
