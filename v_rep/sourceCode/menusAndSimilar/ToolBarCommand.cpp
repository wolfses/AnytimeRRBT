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
#include "ToolBarCommand.h"
#include "Simulation.h"
#include "OglSurface.h"
#include "IloIlo.h"

#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>
#include "App.h"

bool CToolBarCommand::evaluateCommand(int commandID)
{ // Return value is true if the command belonged to toolbar menu and was executed
	if (commandID==CAMERA_SHIFT_TO_FRAME_SELECTION_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("CAMERA_SHIFT_TO_FRAME_SELECTION_CMD")
		{
			if (App::ct->oglSurface!=NULL)
			{
				int pageIndex=App::ct->oglSurface->pageCont->getActivePageIndex();
				CSPage* page=App::ct->oglSurface->pageCont->getPage(pageIndex);
				if (page!=NULL)
				{
					int ind=page->getLastMouseDownViewIndex();
					if (ind==-1)
						ind=0;
					CSView* view=page->getView(ind);
					if (view!=NULL)
					{
						CCamera* cam=App::ct->objCont->getCamera(view->getLinkedObjectID());
						if ( (cam!=NULL) )
						{
							int viewSize[2];
							view->getViewSize(viewSize);

							cam->frameSceneOrSelectedObjects(float(viewSize[0])/float(viewSize[1]),view->getPerspectiveDisplay(),NULL,true,true,1.0f);
							App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
							cam->setFogTimer(4.0f);
						}
					}
				}
			}
		}
		return(true);
	}
	if (commandID==CAMERA_SHIFT_NAVIGATION_CMD)
	{
		App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_camerashift);
		return(true);
	}
	if (commandID==CAMERA_ROTATE_NAVIGATION_CMD)
	{
		App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_camerarotate);
		return(true);
	}
	if (commandID==CAMERA_ZOOM_NAVIGATION_CMD)
	{
		App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_camerazoom);
		return(true);
	}
	if (commandID==CAMERA_TILT_NAVIGATION_CMD)
	{
		App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_cameratilt);
		return(true);
	}
	if (commandID==CAMERA_ANGLE_NAVIGATION_CMD)
	{
		App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_cameraangle);
		return(true);
	}
	if (commandID==CAMERA_FLY_NAVIGATION_CMD)
	{
		App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_camerafly);
		return(true);
	}
	if (commandID==OBJECT_SHIFT_NAVIGATION_CMD)
	{
		App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_objectshift);
		return(true);
	}
	if (commandID==OBJECT_ROTATE_NAVIGATION_CMD)
	{
		App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_objectrotate);
		return(true);
	}
	
	if (commandID==PAGE_SELECTOR_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("XXXXXXXXXXXXXXXXXXXXXXXXXXXX")
		{
			if (App::ct->oglSurface->isPageSelectionActive())
				App::ct->oglSurface->setPageSelectionActive(false);
			else
				App::ct->oglSurface->setPageSelectionActive(true);
		}
		return(true);
	}
	if (commandID==SCENE_SELECTOR_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("XXXXXXXXXXXXXXXXXXXXXXXXXXXX")
		{
			if (App::ct->oglSurface->isSceneSelectionActive())
				App::ct->oglSurface->setSceneSelectionActive(false);
			else
				App::ct->oglSurface->setSceneSelectionActive(true);
		}
		return(true);
	}
	if (commandID==OBJECT_SELECTION_SELECTION_CMD)
	{
		if ( (App::ct->getMouseMode()&0x0300)&sim_navigation_clickselection )
			App::ct->setMouseMode(App::ct->getMouseMode()&0xfcff);
		else
		{
			App::ct->setMouseMode((App::ct->getMouseMode()&0xfcff)|sim_navigation_clickselection);
		}
		return(true);
	}
	if (commandID==PATH_POINT_CREATION_MODE_CMD)
	{
		if ( (App::ct->getMouseMode()&0x0300)&sim_navigation_createpathpoint )
			App::ct->setMouseMode(App::ct->getMouseMode()&0xfcff);
		else
		{
			App::ct->setMouseMode((App::ct->getMouseMode()&0xfcff)|sim_navigation_createpathpoint);
		}
		return(true);
	}

	if (commandID==CLEAR_SELECTION_SELECTION_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("CLEAR_SELECTION_SELECTION_CMD")
		{
			App::ct->objCont->deselectObjects();
			App::ct->objCont->deselectEditModeBuffer();
		}
		return(true);
	}
	if (commandID==START_RESUME_SIMULATION_REQUEST_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("START_RESUME_SIMULATION_REQUEST_CMD")
		{
			App::ct->simulatorMessageQueue->addCommand(sim_message_simulation_start_resume_request,0,0,0,0,NULL,0);
		}
		return(true);
	}
	if (commandID==PAUSE_SIMULATION_REQUEST_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("PAUSE_SIMULATION_REQUEST_CMD")
		{
			App::ct->simulatorMessageQueue->addCommand(sim_message_simulation_pause_request,0,0,0,0,NULL,0);
		}
		return(true);
	}
	if (commandID==STOP_SIMULATION_REQUEST_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("STOP_SIMULATION_REQUEST_CMD")
		{
			App::ct->simulatorMessageQueue->addCommand(sim_message_simulation_stop_request,0,0,0,0,NULL,0);
			App::ct->simulation->incrementStopRequestCounter();
		}
		return(true);
	}
	if ( (commandID>=VIEW_1_CMD)&&(commandID<=VIEW_8_CMD) )
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("VIEW...CMD")
		{
			if (App::ct->oglSurface->pageCont->getActivePageIndex()!=(commandID-VIEW_1_CMD))
			{
				App::ct->oglSurface->pageCont->setActivePage(commandID-VIEW_1_CMD);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				std::string str(IDSNS_SWAPPED_TO_PAGE);
				str+=" ";
				str+=boost::lexical_cast<std::string>(commandID-VIEW_1_CMD+1)+".";
				App::uiThread->addStatusLine(str.c_str());
			}
		}
		return(true);
	}
	if ( (commandID>=SWITCH_TO_INSTANCE_INDEX0_CMD)&&(commandID<=SWITCH_TO_INSTANCE_INDEX39_CMD) )
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("SWITCH_TO_INSTANCE_INDEX...CMD")
		{
			App::ct->evaluateMenuSelection(commandID);
		}
		return(true);
	}
	return(false);
}
