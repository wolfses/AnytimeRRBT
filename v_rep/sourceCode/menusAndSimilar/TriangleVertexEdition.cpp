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
#include "TriangleVertexEdition.h"
#include "OGL.h"
#include "Simulation.h"

#include "IloIlo.h"
#include "AddMenu.h"
#include "App.h"
#include "MeshManip.h"
#include "geometric.h"
#include "v_repStrings.h"

void CTriangleVertexEdition::copySelection(std::vector<int>* selection)
{
	if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
	{ // We have to copy the selected triangles with their vertices
		copySelectedFaces(selection,&App::ct->objCont->editBufferVerticesCopy,
							&App::ct->objCont->editBufferIndicesCopy,
							&App::ct->objCont->editBufferNormalsCopy,
							&App::ct->objCont->editBufferTextureCoordsCopy);
		App::ct->objCont->deselectEditModeBuffer();
	}
	if (App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)
	{ // We copy only the vertices
		App::ct->objCont->editBufferVerticesCopy.reserve(selection->size()*3);
		App::ct->objCont->editBufferVerticesCopy.clear();
		App::ct->objCont->editBufferIndicesCopy.clear();
		App::ct->objCont->editBufferNormalsCopy.clear();
		App::ct->objCont->editBufferTextureCoordsCopy.clear();
		for (int i=0;i<int(selection->size());i++)
		{
			int index=selection->at(i);
			App::ct->objCont->editBufferVerticesCopy.push_back(App::ct->objCont->_editionVertices[3*index+0]);
			App::ct->objCont->editBufferVerticesCopy.push_back(App::ct->objCont->_editionVertices[3*index+1]);
			App::ct->objCont->editBufferVerticesCopy.push_back(App::ct->objCont->_editionVertices[3*index+2]);
		}
		App::ct->objCont->deselectEditModeBuffer();
	}
	App::ct->objCont->setLightDialogRefreshFlag();
}
void CTriangleVertexEdition::paste()
{
	if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
	{
		App::ct->objCont->deselectEditModeBuffer();
		int startInd=App::ct->objCont->_editionVertices.size()/3;
		for (int i=0;i<int(App::ct->objCont->editBufferVerticesCopy.size());i++)
			App::ct->objCont->_editionVertices.push_back(App::ct->objCont->editBufferVerticesCopy[i]);
		int orSize=App::ct->objCont->_editionIndices.size()/3;
		for (int i=0;i<int(App::ct->objCont->editBufferIndicesCopy.size())/3;i++)
		{
			int ind[3];
			ind[0]=App::ct->objCont->editBufferIndicesCopy[3*i+0]+startInd;
			ind[1]=App::ct->objCont->editBufferIndicesCopy[3*i+1]+startInd;
			ind[2]=App::ct->objCont->editBufferIndicesCopy[3*i+2]+startInd;
			App::ct->objCont->_editionIndices.push_back(ind[0]);
			App::ct->objCont->_editionIndices.push_back(ind[1]);
			App::ct->objCont->_editionIndices.push_back(ind[2]);
			int index=App::ct->objCont->_edgeCont.getEdgeIndex(ind[0],ind[1]);
			if (index==-1)
			{ // edge doesn't yet exist
				index=App::ct->objCont->_edgeCont.addEdge(ind[0],ind[1]);
			}
			App::ct->objCont->_editionIndicesToEdgesIndex.push_back(index);
			index=App::ct->objCont->_edgeCont.getEdgeIndex(ind[0],ind[2]);
			if (index==-1)
			{ // edge doesn't yet exist
				index=App::ct->objCont->_edgeCont.addEdge(ind[0],ind[2]);
			}
			App::ct->objCont->_editionIndicesToEdgesIndex.push_back(index);
			index=App::ct->objCont->_edgeCont.getEdgeIndex(ind[2],ind[1]);
			if (index==-1)
			{ // edge doesn't yet exist
				index=App::ct->objCont->_edgeCont.addEdge(ind[2],ind[1]);
			}
			App::ct->objCont->_editionIndicesToEdgesIndex.push_back(index);
			App::ct->objCont->addItemToEditModeBuffer(orSize+i);
		}
		for (int i=0;i<int(App::ct->objCont->editBufferNormalsCopy.size());i++)
			App::ct->objCont->_editionNormals.push_back(App::ct->objCont->editBufferNormalsCopy[i]);
		for (int i=0;i<int(App::ct->objCont->editBufferTextureCoordsCopy.size());i++)
			App::ct->objCont->_editionTextureCoords.push_back(App::ct->objCont->editBufferTextureCoordsCopy[i]);
	}
	if (App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)
	{
		App::ct->objCont->deselectEditModeBuffer();
		int startInd=App::ct->objCont->_editionVertices.size()/3;
		for (int i=0;i<int(App::ct->objCont->editBufferVerticesCopy.size())/3;i++)
		{
			App::ct->objCont->_editionVertices.push_back(App::ct->objCont->editBufferVerticesCopy[3*i+0]);	
			App::ct->objCont->_editionVertices.push_back(App::ct->objCont->editBufferVerticesCopy[3*i+1]);	
			App::ct->objCont->_editionVertices.push_back(App::ct->objCont->editBufferVerticesCopy[3*i+2]);
			App::ct->objCont->addItemToEditModeBuffer(startInd);
			startInd++;
		}
	}
	App::ct->objCont->setFullDialogRefreshFlag();
}

void CTriangleVertexEdition::cutSelection(std::vector<int>* selection)
{
	copySelection(selection);
	deleteSelection(selection);
}
void CTriangleVertexEdition::deleteSelection(std::vector<int>* selection)
{
	if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
	{
		while (selection->size()!=0)
		{ // We have to remove the triangles from the highest index to the lowest:
			int max=0;
			int maxIndex=0;
			for (int i=0;i<int(selection->size());i++)
			{
				if (selection->at(i)>max)
				{
					max=selection->at(i);
					maxIndex=i;
				}
			}
			App::ct->objCont->_editionIndices.erase(App::ct->objCont->_editionIndices.begin()+3*max,App::ct->objCont->_editionIndices.begin()+3*max+3);
			App::ct->objCont->_editionNormals.erase(App::ct->objCont->_editionNormals.begin()+9*max,App::ct->objCont->_editionNormals.begin()+9*max+9);
			App::ct->objCont->_editionTextureCoords.erase(App::ct->objCont->_editionTextureCoords.begin()+6*max,App::ct->objCont->_editionTextureCoords.begin()+6*max+6);
			selection->erase(selection->begin()+maxIndex);
		}
		App::ct->objCont->actualizeEditModeEditionEdges();
		App::ct->objCont->deselectEditModeBuffer();
	}
	if (App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)
	{
		while (selection->size()!=0)
		{ // We have to remove the selected vertices. We don't remove them now,
			// we just set the x value to > 1'000'000. We then don't display vertices
			// > than that value. 
			int max=0;
			int maxIndex=0;
			for (int i=0;i<int(selection->size());i++)
			{
				if (selection->at(i)>max)
				{
					max=selection->at(i);
					maxIndex=i;
				}
			}
			App::ct->objCont->_editionVertices.erase(App::ct->objCont->_editionVertices.begin()+3*max,App::ct->objCont->_editionVertices.begin()+3*max+3);
			// We have to decrease all value higher than max in the indices.
			// Indices pointing to max are set to -1:
			for (int i=0;i<int(App::ct->objCont->_editionIndices.size());i++)
			{
				if (App::ct->objCont->_editionIndices[i]==max)
					App::ct->objCont->_editionIndices[i]=-1;
				if (App::ct->objCont->_editionIndices[i]>max)
					App::ct->objCont->_editionIndices[i]--;
			}
			selection->erase(selection->begin()+maxIndex);
		}
		// Now we remove all indices/normals where one index is -1:
		int i=0;
		while (i<int(App::ct->objCont->_editionIndices.size())/3)
		{
			if ( (App::ct->objCont->_editionIndices[3*i+0]==-1)||
				(App::ct->objCont->_editionIndices[3*i+1]==-1) ||
				(App::ct->objCont->_editionIndices[3*i+2]==-1) )
			{
				App::ct->objCont->_editionIndices.erase(App::ct->objCont->_editionIndices.begin()+3*i,App::ct->objCont->_editionIndices.begin()+3*i+3);
				App::ct->objCont->_editionNormals.erase(App::ct->objCont->_editionNormals.begin()+9*i,App::ct->objCont->_editionNormals.begin()+9*i+9);
				App::ct->objCont->_editionTextureCoords.erase(App::ct->objCont->_editionTextureCoords.begin()+6*i,App::ct->objCont->_editionTextureCoords.begin()+6*i+6);
			}
			else
				i++;
		}
		App::ct->objCont->actualizeEditModeEditionEdges();
		App::ct->objCont->deselectEditModeBuffer();
	}
	if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
	{
		// We prepare an array indicating which edge needs to be removed:
		std::vector<bool> edgesWhichNeedToBeRemoved(App::ct->objCont->_edgeCont.allEdges.size(),false);
		for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
			edgesWhichNeedToBeRemoved[App::ct->objCont->editModeBuffer[i]]=true;
		// We now go through all triangles (from highest to lowest) and remove the ones that have a listed edge:
		for (int i=int((App::ct->objCont->_editionIndices.size())/3)-1;i>=0;i--)
		{
			int ind[3];
			ind[0]=App::ct->objCont->_editionIndicesToEdgesIndex[3*i+0];
			ind[1]=App::ct->objCont->_editionIndicesToEdgesIndex[3*i+1];
			ind[2]=App::ct->objCont->_editionIndicesToEdgesIndex[3*i+2];
			if (edgesWhichNeedToBeRemoved[ind[0]] ||
				edgesWhichNeedToBeRemoved[ind[1]] ||
				edgesWhichNeedToBeRemoved[ind[2]] )
			{ // needs to be removed
				App::ct->objCont->_editionIndices.erase(App::ct->objCont->_editionIndices.begin()+3*i,App::ct->objCont->_editionIndices.begin()+3*i+3);
				App::ct->objCont->_editionNormals.erase(App::ct->objCont->_editionNormals.begin()+9*i,App::ct->objCont->_editionNormals.begin()+9*i+9);
				App::ct->objCont->_editionTextureCoords.erase(App::ct->objCont->_editionTextureCoords.begin()+6*i,App::ct->objCont->_editionTextureCoords.begin()+6*i+6);
			}
		}
		App::ct->objCont->actualizeEditModeEditionEdges();
		App::ct->objCont->deselectEditModeBuffer();
	}
	App::ct->objCont->setFullDialogRefreshFlag();
}

void CTriangleVertexEdition::addMenu(VMenu* menu)
{
	int selSize=App::ct->objCont->editModeBuffer.size();
	int buffSize=App::ct->objCont->editBufferVerticesCopy.size();
	bool triangleEditMode=((App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)!=0);
	bool vertexEditMode=((App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)!=0);
	bool edgeEditMode=((App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)!=0);
	VMenubar::appendMenuItem(menu,true,triangleEditMode,SWAP_TO_TRIANGLE_EDIT_MODE_CMD,IDS_TRIANGLE_EDIT_MODE_MENU_ITEM,true);
	VMenubar::appendMenuItem(menu,true,vertexEditMode,SWAP_TO_VERTEX_EDIT_MODE_CMD,IDS_VERTEX_EDIT_MODE_MENU_ITEM,true);
	VMenubar::appendMenuItem(menu,true,edgeEditMode,SWAP_TO_EDGE_EDIT_MODE_CMD,IDS_EDGE_EDIT_MODE_MENU_ITEM,true);
	VMenubar::appendMenuSeparator(menu);
	if (App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)
	{
		VMenubar::appendMenuItem(menu,true,App::ct->objCont->showHiddenVerticeAndEdges,TOGGLE_EDIT_MODE_HIDDEN_VERTICE_EDGE_SHOW_CMD,IDS_SHOW_HIDDEN_VERTICES_MENU_ITEM,true);
		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,selSize>0,false,TRIANGLE_VERTEX_COPY_CMD,IDS_COPY_SELECTED_VERTICES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,buffSize>0,false,PASTE_TRIANGLE_VERTEX_CMD,IDS_PASTE_VERTICES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selSize>0,false,DELETE_TRIANGLE_VERTEX_EDGE_CMD,IDS_DELETE_SELECTED_VERTICES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selSize>0,false,TRIANGLE_VERTEX_CUT_CMD,IDS_CUT_SELECTED_VERTICES_MENU_ITEM);
		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,selSize>=3,false,INSERT_TRIANGLES_CMD,IDS_INSERT_TRIANGLES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selSize>=3,false,INSERT_TRIANGLE_FAN_CMD,IDS_INSERT_TRIANGLE_FAN_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selSize>0,false,MAKE_DUMMIES_WITH_SELECTED_VERTICES_CMD,IDS_MAKE_DUMMIES_WITH_SELECTED_VERTICES_MENU_ITEM);
		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,true,false,SELECT_ALL_SHAPE_EDIT_MODE_ITEMS_CMD,IDSN_SELECT_ALL_MENU_ITEM);
	}
	else if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
	{
		VMenubar::appendMenuItem(menu,selSize>0,false,TRIANGLE_VERTEX_COPY_CMD,IDS_COPY_SELECTED_TRIANGLES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,buffSize>0,false,PASTE_TRIANGLE_VERTEX_CMD,IDS_PASTE_TRIANGLES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selSize>0,false,DELETE_TRIANGLE_VERTEX_EDGE_CMD,IDS_DELETE_SELECTED_TRIANGLES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selSize>0,false,TRIANGLE_VERTEX_CUT_CMD,IDS_CUT_SELECTED_TRIANGLES_MENU_ITEM);
		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,selSize>0,false,FLIP_TRIANGLES_SIDE_CMD,IDS_FLIP_SELECTED_TRIANGLES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selSize>0,false,MAKE_SHAPE_WITH_SELECTED_TRIANGLES_CMD,IDS_MAKE_SHAPE_WITH_SELECTED_TRIANGLES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selSize>0,false,MAKE_RECTANGLE_WITH_SELECTED_TRIANGLES_CMD,IDS_MAKE_RECTANGLE_WITH_SELECTED_TRIANGLES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selSize>0,false,MAKE_SPHERE_WITH_SELECTED_TRIANGLES_CMD,IDS_MAKE_SPHERE_WITH_SELECTED_TRIANGLES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selSize>0,false,MAKE_CYLINDER_WITH_SELECTED_TRIANGLES_CMD,IDS_MAKE_CYLINDER_WITH_SELECTED_TRIANGLES_MENU_ITEM);
		VMenubar::appendMenuItem(menu,true,false,DIVIDE_LARGEST_TRIANGLES_CMD,IDS_REDUCE_TRIANGLE_SIZE_MENU_ITEM);
		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,true,false,SELECT_ALL_SHAPE_EDIT_MODE_ITEMS_CMD,IDSN_SELECT_ALL_MENU_ITEM);
	}
	else if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
	{
		VMenubar::appendMenuItem(menu,true,App::ct->objCont->showHiddenVerticeAndEdges,TOGGLE_EDIT_MODE_HIDDEN_VERTICE_EDGE_SHOW_CMD,IDS_SHOW_HIDDEN_EDGES_MENU_ITEM,true);
		VMenubar::appendMenuItem(menu,true,App::ct->objCont->automaticallyFollowEdges,TOGGLE_EDIT_MODE_AUTOMATIC_EDGE_FOLLOWING_CMD,IDS_AUTOMATIC_EDGE_FOLLOWING_MENU_ITEM,true);
		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,selSize>0,false,DELETE_TRIANGLE_VERTEX_EDGE_CMD,IDS_DELETE_SELECTED_EDGES_MENU_ITEM);
		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,selSize>0,false,MAKE_PATH_WITH_SELECTED_EDGES_CMD,IDS_MAKE_PATH_WITH_SELECTED_EDGES_MENU_ITEM);
		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,true,false,SELECT_ALL_SHAPE_EDIT_MODE_ITEMS_CMD,IDSN_SELECT_ALL_MENU_ITEM);
	}
	VMenubar::appendMenuSeparator(menu);
	VMenubar::appendMenuItem(menu,true,false,LEAVE_EDIT_MODE_CANCEL_CHANGES_CMD,IDS_LEAVE_EDIT_MODE_AND_CANCEL_CHANGES_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,LEAVE_EDIT_MODE_CMD,IDS_LEAVE_EDIT_MODE_AND_APPLY_CHANGES_MENU_ITEM);
}

bool CTriangleVertexEdition::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to object edition menu and was executed
	if (commandID==SWAP_TO_VERTEX_EDIT_MODE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("SWAP_TO_VERTEX_EDIT_MODE_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_SWAPPING_TO_VERTEX_EDIT_MODE);
			if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
			{
				App::ct->objCont->selectionFromTriangleToVertexEditMode();
				App::ct->objCont->swapShapeEditModeType(VERTEX_EDIT_MODE);
			}
			if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
			{
				App::ct->objCont->selectionFromEdgeToVertexEditMode();
				App::ct->objCont->swapShapeEditModeType(VERTEX_EDIT_MODE);
			}
			App::ct->objCont->editBufferVerticesCopy.clear();
			App::ct->objCont->editBufferIndicesCopy.clear();
			App::ct->objCont->editBufferNormalsCopy.clear();
			App::ct->objCont->editBufferTextureCoordsCopy.clear();
			App::ct->objCont->setFullDialogRefreshFlag();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==SWAP_TO_TRIANGLE_EDIT_MODE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("SWAP_TO_TRIANGLE_EDIT_MODE_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_SWAPPING_TO_TRIANGLE_EDIT_MODE);
			if (App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)
			{
				App::ct->objCont->selectionFromVertexToTriangleEditMode();
				App::ct->objCont->swapShapeEditModeType(TRIANGLE_EDIT_MODE);
			}
			if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
			{
				App::ct->objCont->selectionFromEdgeToTriangleEditMode();
				App::ct->objCont->swapShapeEditModeType(TRIANGLE_EDIT_MODE);
			}
			App::ct->objCont->editBufferVerticesCopy.clear();
			App::ct->objCont->editBufferIndicesCopy.clear();
			App::ct->objCont->editBufferNormalsCopy.clear();
			App::ct->objCont->editBufferTextureCoordsCopy.clear();
			App::ct->objCont->setFullDialogRefreshFlag();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==SWAP_TO_EDGE_EDIT_MODE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("SWAP_TO_EDGE_EDIT_MODE_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_SWAPPING_TO_EDGE_EDIT_MODE);
			if (App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)
			{
				App::ct->objCont->selectionFromVertexToEdgeEditMode();
				App::ct->objCont->swapShapeEditModeType(EDGE_EDIT_MODE);
			}
			if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
			{
				App::ct->objCont->selectionFromTriangleToEdgeEditMode();
				App::ct->objCont->swapShapeEditModeType(EDGE_EDIT_MODE);
			}
			App::ct->objCont->editBufferVerticesCopy.clear();
			App::ct->objCont->editBufferIndicesCopy.clear();
			App::ct->objCont->editBufferNormalsCopy.clear();
			App::ct->objCont->editBufferTextureCoordsCopy.clear();
			App::ct->objCont->setFullDialogRefreshFlag();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==TRIANGLE_VERTEX_COPY_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("TRIANGLE_VERTEX_COPY_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
				App::uiThread->addStatusLine(IDSNS_EDGES_CANNOT_BE_COPIED_CUR_OR_PASTED);
			else
			{
				App::uiThread->addStatusLine(IDSNS_COPYING_SELECTION);
				copySelection(&sel);
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	if (commandID==PASTE_TRIANGLE_VERTEX_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("PASTE_TRIANGLE_VERTEX_CMD")
		{
			if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
				App::uiThread->addStatusLine(IDSNS_EDGES_CANNOT_BE_COPIED_CUR_OR_PASTED);
			else
			{
				App::uiThread->addStatusLine(IDSNS_PASTING_BUFFER);
				paste();
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	if (commandID==EMPTY_TRIANGLE_VERTEX_COPY_BUFFER_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("EMPTY_TRIANGLE_VERTEX_COPY_BUFFER_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_CLEARING_BUFFER);
			App::ct->objCont->editBufferVerticesCopy.clear();
			App::ct->objCont->editBufferIndicesCopy.clear();
			App::ct->objCont->editBufferNormalsCopy.clear();
			App::ct->objCont->editBufferTextureCoordsCopy.clear();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==DELETE_TRIANGLE_VERTEX_EDGE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("DELETE_TRIANGLE_VERTEX_EDGE_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			App::uiThread->addStatusLine(IDSNS_DELETING_SELECTION);
			deleteSelection(&sel);
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==SELECT_ALL_SHAPE_EDIT_MODE_ITEMS_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("SELECT_ALL_SHAPE_EDIT_MODE_ITEMS_CMD")
		{
			if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
			{
				App::uiThread->addStatusLine(IDSNS_SELECTING_ALL_TRIANGLES);
				for (int i=0;i<int(App::ct->objCont->_editionIndices.size())/3;i++)
					App::ct->objCont->addItemToEditModeBuffer(i);
			}
			if (App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)
			{
				App::uiThread->addStatusLine(IDSNS_SELECTING_ALL_VERTICES);
				for (int i=0;i<int(App::ct->objCont->_editionVertices.size())/3;i++)
					App::ct->objCont->addItemToEditModeBuffer(i);
			}
			if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
			{
				App::uiThread->addStatusLine(IDSNS_SELECTING_ALL_EDGES);
				for (int i=0;i<int(App::ct->objCont->_edgeCont.allEdges.size())/2;i++)
					App::ct->objCont->addItemToEditModeBuffer(i);
			}

			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==TOGGLE_EDIT_MODE_HIDDEN_VERTICE_EDGE_SHOW_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_EDIT_MODE_HIDDEN_VERTICE_EDGE_SHOW_CMD")
		{
			App::ct->objCont->showHiddenVerticeAndEdges=!App::ct->objCont->showHiddenVerticeAndEdges;
			App::uiThread->addStatusLine(IDSNS_TOGGLED_HIDDEN_VERTICES_EDGES_SHOW_STATE);
		}
		return(true);
	}
	if (commandID==TOGGLE_EDIT_MODE_AUTOMATIC_EDGE_FOLLOWING_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_EDIT_MODE_AUTOMATIC_EDGE_FOLLOWING_CMD")
		{
			App::ct->objCont->automaticallyFollowEdges=!App::ct->objCont->automaticallyFollowEdges;
			App::uiThread->addStatusLine(IDSNS_TOGGLED_AUTOMATIC_EDGE_FOLLOWING);
		}
		return(true);
	}
	if (commandID==TRIANGLE_VERTEX_CUT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("TRIANGLE_VERTEX_CUT_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
				App::uiThread->addStatusLine(IDSNS_EDGES_CANNOT_BE_COPIED_CUR_OR_PASTED);
			else
			{
				App::uiThread->addStatusLine(IDSNS_CUTTING_SELECTION);
				cutSelection(&sel);
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	if (commandID==FLIP_TRIANGLES_SIDE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("FLIP_TRIANGLES_SIDE_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			App::uiThread->addStatusLine(IDSNS_FLIPPING_TRIANGLE_SIDES);
			if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
			{
				for (int i=0;i<int(sel.size());i++)
					App::ct->objCont->flipTriangle(sel[i]);
			}
			App::ct->objCont->deselectEditModeBuffer();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==MAKE_SHAPE_WITH_SELECTED_TRIANGLES_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("MAKE_SHAPE_WITH_SELECTED_TRIANGLES_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			App::uiThread->addStatusLine(IDSNS_GENERATING_SHAPE);
			std::vector<float> nVertices;
			std::vector<int> nIndices;
			std::vector<float> nNormals;
			std::vector<float> nTexCoords;
			copySelectedFaces(&sel,&nVertices,&nIndices,&nNormals,&nTexCoords);
			if (nVertices.size()!=0)
			{	// Now we have to transform all vertices with the cumulative transform
				// matrix of the shape beeing edited:
				CShape* it=App::ct->objCont->getShape(App::ct->objCont->getEditModeObjectID());
				if (it!=NULL)
				{
					C7Vector m(it->getCumulativeTransformation());
					C3Vector v;
					for (int i=0;i<int(nVertices.size())/3;i++)
					{
						v(0)=nVertices[3*i+0];
						v(1)=nVertices[3*i+1];
						v(2)=nVertices[3*i+2];
						v*=m;
						nVertices[3*i+0]=v(0);
						nVertices[3*i+1]=v(1);
						nVertices[3*i+2]=v(2);
					}
					CGeomProxy* newGeom;
					if (App::ct->objCont->_editionTextureProperty!=NULL)
						newGeom=new CGeomProxy(NULL,nVertices,nIndices,NULL,&nTexCoords);
					else
						newGeom=new CGeomProxy(NULL,nVertices,nIndices,NULL,NULL);
					CShape* newShape=new CShape();
					newShape->setLocalTransformation(newGeom->getCreationTransformation());
					newGeom->setCreationTransformation(C7Vector::identityTransformation);
					newShape->setVisibleEdges(true);
					((CGeometric*)newGeom->geomInfo)->setGouraudShadingAngle(20.0f*degToRad);
					((CGeometric*)newGeom->geomInfo)->setEdgeThresholdAngle(20.0f*degToRad);
					newShape->setName("Extracted_shape");
					newShape->layer=it->layer;
					newShape->geomData=newGeom;
					App::ct->objCont->addObjectToScene(newShape,false);
					App::ct->objCont->objectsAddedDuringEditMode.push_back(newShape->getID());

					if (App::ct->objCont->_editionTextureProperty!=NULL)
					{
						CTextureObject* to=App::ct->textureCont->getObject(App::ct->objCont->_editionTextureProperty->getTextureObjectID());
						if (to!=NULL)
						{
							to->addDependentObject(newShape->getID(),((CGeometric*)newShape->geomData->geomInfo)->getUniqueID());
							CTextureProperty* tp=new CTextureProperty(to->getObjectID());
							((CGeometric*)newShape->geomData->geomInfo)->setTextureProperty(tp);
							tp->setFixedCoordinates(&((CGeometric*)newShape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized);
							((CGeometric*)newShape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.clear();
						}
					}
				}
			}
			App::ct->objCont->setFullDialogRefreshFlag();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if ( (commandID==MAKE_RECTANGLE_WITH_SELECTED_TRIANGLES_CMD)||
		(commandID==MAKE_SPHERE_WITH_SELECTED_TRIANGLES_CMD)||
		(commandID==MAKE_SPHEROID_WITH_SELECTED_TRIANGLES_CMD)||
		(commandID==MAKE_CYLINDER_WITH_SELECTED_TRIANGLES_CMD) )
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("MAKE...WITH_SELECTED_TRIANGLES_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			App::uiThread->addStatusLine(IDSNS_GENERATING_PURE_SHAPE);
			std::vector<float> nVertices;
			std::vector<int> nIndices;
			std::vector<float> nNormals;
			copySelectedFaces(&sel,&nVertices,&nIndices,&nNormals,NULL);
			CShape* generatedShape=NULL;
			if (nVertices.size()!=0)
			{	// Now we have to transform all vertices with the cumulative transform
				// matrix of the shape beeing edited:
				CShape* it=App::ct->objCont->getShape(App::ct->objCont->getEditModeObjectID());
				if (it!=NULL)
				{
					C7Vector m(it->getCumulativeTransformation());
					C3Vector v;
					for (int i=0;i<int(nVertices.size())/3;i++)
					{
						v(0)=nVertices[3*i+0];
						v(1)=nVertices[3*i+1];
						v(2)=nVertices[3*i+2];
						v*=m;
						nVertices[3*i+0]=v(0);
						nVertices[3*i+1]=v(1);
						nVertices[3*i+2]=v(2);
					}
					CGeomProxy* newGeom=new CGeomProxy(NULL,nVertices,nIndices,NULL,NULL);
					CShape* newShape=new CShape();
					newShape->setLocalTransformation(newGeom->getCreationTransformation());
					newGeom->setCreationTransformation(C7Vector::identityTransformation);
					newShape->geomData=newGeom;
					C3Vector size(newGeom->getBoundingBoxHalfSizes()*2.0f);
					C7Vector conf(newShape->getLocalTransformationPart1());
					delete newShape;
					if (commandID==MAKE_RECTANGLE_WITH_SELECTED_TRIANGLES_CMD)
					{
						generatedShape=CAddMenu::addPrimitive(ADD_PRIMITIVE_RECTANGLE_CMD,&size);
						if (generatedShape!=NULL)
							generatedShape->setLocalTransformation(conf);
					}
					if (commandID==MAKE_SPHERE_WITH_SELECTED_TRIANGLES_CMD)
					{
						float mm=SIM_MAX(SIM_MAX(size(0),size(1)),size(2));
						size(0)=mm;
						size(1)=mm;
						size(2)=mm;
						generatedShape=CAddMenu::addPrimitive(ADD_PRIMITIVE_SPHERE_CMD,&size);
						if (generatedShape!=NULL)
							generatedShape->setLocalTransformation(conf);
					}
					if (commandID==MAKE_SPHEROID_WITH_SELECTED_TRIANGLES_CMD)
					{
						generatedShape=CAddMenu::addPrimitive(ADD_PRIMITIVE_SPHERE_CMD,&size);
						if (generatedShape!=NULL)
							generatedShape->setLocalTransformation(conf);
					}
					if (commandID==MAKE_CYLINDER_WITH_SELECTED_TRIANGLES_CMD)
					{
						C3Vector diff(fabs(size(0)-size(1)),fabs(size(0)-size(2)),fabs(size(1)-size(2)));
						int t=2;
						if (SIM_MIN(SIM_MIN(diff(0),diff(1)),diff(2))==diff(0))
							t=0;
						if (SIM_MIN(SIM_MIN(diff(0),diff(1)),diff(2))==diff(1))
							t=1;
						if (t==0)
						{
							C3Vector s(size);
							size(0)=(s(0)+s(1))*0.5f;
							size(1)=(s(0)+s(1))*0.5f;
							size(2)=s(2);
						}
						if (t==1)
						{
							C3Vector s(size);
							size(0)=(s(0)+s(2))*0.5f;
							size(1)=(s(0)+s(2))*0.5f;
							size(2)=s(1);
						}
						if (t==2)
						{
							C3Vector s(size);
							size(0)=(s(2)+s(1))*0.5f;
							size(1)=(s(2)+s(1))*0.5f;
							size(2)=s(0);
						}
						generatedShape=CAddMenu::addPrimitive(ADD_PRIMITIVE_CYLINDER_CMD,&size);
						if (generatedShape!=NULL)
						{
							C7Vector r;
							r.setIdentity();
							if (t==1)
								r.Q.setEulerAngles(C3Vector(piValD2,0.0f,0.0f));
							if (t==2)
								r.Q.setEulerAngles(C3Vector(0.0f,piValD2,0.0f));
							generatedShape->setLocalTransformation(conf*r);
						}
					}
					if (generatedShape!=NULL)
						App::ct->objCont->objectsAddedDuringEditMode.push_back(generatedShape->getID());

				}
			}
			App::ct->objCont->setFullDialogRefreshFlag();
			if (generatedShape!=NULL)
				App::uiThread->addStatusLine(IDSNS_DONE);
			else
				App::uiThread->addStatusLine(IDSNS_OPERATION_ABORTED);
		}
		return(true);
	}
	if (commandID==MAKE_PATH_WITH_SELECTED_EDGES_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("MAKE_PATH_WITH_SELECTED_EDGES_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			App::uiThread->addStatusLine(IDSNS_GENERATING_PATH);
			CShape* shape=App::ct->objCont->getShape(App::ct->objCont->getEditModeObjectID());
			if ( (shape!=NULL)&&(sel.size()>0) )
			{ // Only consecutive edges will be used! NOOOO! (modified on 2009/03/14)
				CPath* newObject=new CPath();
				newObject->pathContainer->enableActualization(false);
				int verticeInd[2];
				verticeInd[0]=App::ct->objCont->_edgeCont.allEdges[2*sel[0]+0];
				verticeInd[1]=App::ct->objCont->_edgeCont.allEdges[2*sel[0]+1];
				C3Vector v0(&App::ct->objCont->_editionVertices[3*verticeInd[0]+0]);
				C3Vector v1(&App::ct->objCont->_editionVertices[3*verticeInd[1]+0]);
				CSimplePathPoint* it=NULL;
				C7Vector sctm(shape->getCumulativeTransformation());
				C3Vector lastAddedPoint;
				if (sel.size()==1)
				{ // We simply add the two points:
					it=new CSimplePathPoint();
					it->setBezierPointCount(1);
					C7Vector trtmp(it->getTransformation());
					trtmp.X=sctm*v0;
					it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
					newObject->pathContainer->addSimplePathPoint(it);
					it=new CSimplePathPoint();
					it->setBezierPointCount(1);
					trtmp=it->getTransformation();
					lastAddedPoint=v1;
					trtmp.X=sctm*v1;
					it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
					newObject->pathContainer->addSimplePathPoint(it);
				}
				else
				{
					int firstPointIndex=-1;
					for (int i=1;i<int(sel.size());i++)
					{
						int in0=App::ct->objCont->_edgeCont.allEdges[2*sel[i]+0];
						int in1=App::ct->objCont->_edgeCont.allEdges[2*sel[i]+1];
						if (i==1)
						{ // We have to add the two first points:
							if ( (in0==verticeInd[0])||(in1==verticeInd[0]) )
							{
								it=new CSimplePathPoint();
								it->setBezierPointCount(1);
								C7Vector trtmp(it->getTransformation());
								trtmp.X=sctm*v1;
								it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
								newObject->pathContainer->addSimplePathPoint(it);
								it=new CSimplePathPoint();
								it->setBezierPointCount(1);
								trtmp=it->getTransformation();
								lastAddedPoint=v0;
								trtmp.X=sctm*v0;
								it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
								newObject->pathContainer->addSimplePathPoint(it);
								firstPointIndex=verticeInd[1];
							}
							else
							{
								it=new CSimplePathPoint();
								it->setBezierPointCount(1);
								C7Vector trtmp(it->getTransformation());
								trtmp.X=sctm*v0;
								it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
								newObject->pathContainer->addSimplePathPoint(it);
								it=new CSimplePathPoint();
								it->setBezierPointCount(1);
								trtmp=it->getTransformation();
								lastAddedPoint=v1;
								trtmp.X=sctm*v1;
								it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
								newObject->pathContainer->addSimplePathPoint(it);
								firstPointIndex=verticeInd[0];
							}
						}
						// We have to add the vertex that wasn't added yet. If both weren't added yet, we add the closest one first, then the other one
						if ( (in0==verticeInd[0])||(in0==verticeInd[1]) )
						{ // We have to add in1
							if ( (in1==firstPointIndex)&&(i==int(sel.size())-1) )
							{ // we make a closed path only if the is the last edge!
								newObject->pathContainer->setAttributes(newObject->pathContainer->getAttributes()|sim_pathproperty_closed_path);
								break;
							}
							C3Vector v(&App::ct->objCont->_editionVertices[3*in1+0]);
							it=new CSimplePathPoint();
							it->setBezierPointCount(1);
							C7Vector trtmp(it->getTransformation());
							lastAddedPoint=v;
							trtmp.X=sctm*v;
							it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
							newObject->pathContainer->addSimplePathPoint(it);
						}
						else if ( (in1==verticeInd[0])||(in1==verticeInd[1]) )
						{ // We have to add in0
							if ( (in0==firstPointIndex)&&(i==int(sel.size())-1) )
							{ // we make a closed path only if the is the last edge!
								newObject->pathContainer->setAttributes(newObject->pathContainer->getAttributes()|sim_pathproperty_closed_path);
								break;
							}
							C3Vector v(&App::ct->objCont->_editionVertices[3*in0+0]);
							it=new CSimplePathPoint();
							it->setBezierPointCount(1);
							C7Vector trtmp(it->getTransformation());
							lastAddedPoint=v;
							trtmp.X=sctm*v;
							it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
							newObject->pathContainer->addSimplePathPoint(it);
						}
						else
						{ // Following replaces the break command (since 2009/03/14)
							// Here none of the two vertices were added. We first add the closest one, then the other one:
							C3Vector w0(&App::ct->objCont->_editionVertices[3*in0+0]);
							C3Vector w1(&App::ct->objCont->_editionVertices[3*in1+0]);
							float l0=(w0-lastAddedPoint).getLength();
							float l1=(w1-lastAddedPoint).getLength();
							if (l0>l1)
							{ // We swap both
								int in0Cop=in0;
								in0=in1;
								in1=in0Cop;
								C3Vector v(w0);
								w0=w1;
								w1=v;
							}
							// the first one:
							it=new CSimplePathPoint();
							it->setBezierPointCount(1);
							C7Vector trtmp(it->getTransformation());
							lastAddedPoint=w0;
							trtmp.X=sctm*w0;
							it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
							newObject->pathContainer->addSimplePathPoint(it);
							// Now the second one:
							if ( (in1==firstPointIndex)&&(i==int(sel.size())-1) )
							{ // we make a closed path only if the is the last edge!
								newObject->pathContainer->setAttributes(newObject->pathContainer->getAttributes()|sim_pathproperty_closed_path);
								break;
							}
							it=new CSimplePathPoint();
							it->setBezierPointCount(1);
							trtmp=it->getTransformation();
							lastAddedPoint=w1;
							trtmp.X=sctm*w1;
							it->setTransformation(trtmp,newObject->pathContainer->getAttributes());
							newObject->pathContainer->addSimplePathPoint(it);
						}
						verticeInd[0]=in0;
						verticeInd[1]=in1;
					}
				}
				newObject->pathContainer->enableActualization(true);
				newObject->pathContainer->actualizePath();
				newObject->setName("ExtractedPath");
				App::ct->objCont->addObjectToScene(newObject,false);
				App::ct->objCont->objectsAddedDuringEditMode.push_back(newObject->getID());
				App::ct->objCont->editModeBuffer.clear();
			}
			App::ct->objCont->setFullDialogRefreshFlag();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if ( (commandID==MAKE_SELECTED_EDGES_INVISIBLE_CMD)||(commandID==MAKE_SELECTED_EDGES_VISIBLE_CMD) )
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("MAKE_SELECTED_EDGES_INVISIBLE...VISIBLE_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			for (int i=0;i<int(sel.size());i++)
			{
				if (commandID==MAKE_SELECTED_EDGES_INVISIBLE_CMD)
					App::ct->objCont->_edgeCont.allEdgesVisibilityState[sel[i]]=0;
				else
					App::ct->objCont->_edgeCont.allEdgesVisibilityState[sel[i]]=1;
			}
			App::ct->objCont->setFullDialogRefreshFlag();
			App::uiThread->addStatusLine(IDSNS_EDGE_VISIBILITY_TOGGLED);
		}
		return(true);
	}
	if (commandID==MAKE_DUMMIES_WITH_SELECTED_VERTICES_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("MAKE_DUMMIES_WITH_SELECTED_VERTICES_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_GENERATING_DUMMIES);
			CShape* it=App::ct->objCont->getShape(App::ct->objCont->getEditModeObjectID());
			if (it!=NULL)
			{
				C7Vector tr(it->getCumulativeTransformation());
				bool proceed=true;
				if (App::ct->objCont->editModeBuffer.size()>50)
					proceed=(VMessageBox::REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_VERTICES),strTranslate(IDS_LARGE_QUANTITY_OF_OBJECT_WARNING),VMessageBox::YES_NO));
				if (proceed)
				{
					for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
					{
						int ind=App::ct->objCont->editModeBuffer[i];
						C3Vector v(App::ct->objCont->_editionVertices[3*ind+0],
									App::ct->objCont->_editionVertices[3*ind+1],
									App::ct->objCont->_editionVertices[3*ind+2]);
						v=tr*v;
						CDummy* newDummy=new CDummy();
						newDummy->setName("ExtractedDummy");
						App::ct->objCont->addObjectToScene(newDummy,false);
						newDummy->setLocalTransformation(v);
						App::ct->objCont->objectsAddedDuringEditMode.push_back(newDummy->getID());
					}
					App::ct->objCont->editModeBuffer.clear();
				}
			}
			App::ct->objCont->setFullDialogRefreshFlag();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==INSERT_TRIANGLES_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("INSERT_TRIANGLES_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			App::uiThread->addStatusLine(IDSNS_INSERTING_TRIANGLES);
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getEditModeObjectID());
			if ((it!=NULL)&&(App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE))
			{
				if (it->getObjectType()==sim_object_shape_type)
				{
					for (int i=0;i<int(sel.size())/3;i++)
						App::ct->objCont->addTriangle(sel[3*i+0],sel[3*i+1],sel[3*i+2]);
				}
			}
			App::ct->objCont->deselectEditModeBuffer();
			App::ct->objCont->setFullDialogRefreshFlag();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==DIVIDE_LARGEST_TRIANGLES_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("DIVIDE_LARGEST_TRIANGLES_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_REDUCING_TRIANGLE_SIZES);
			App::ct->objCont->deselectEditModeBuffer();
			CMeshManip::reduceTriangleSize(App::ct->objCont->_editionVertices,App::ct->objCont->_editionIndices,&App::ct->objCont->_editionNormals,&App::ct->objCont->_editionTextureCoords,0.0f,0.00001f); // 0.0 simple uses the half of the max edge!
			App::ct->objCont->actualizeEditModeEditionEdges();
			App::ct->objCont->setFullDialogRefreshFlag();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==INSERT_TRIANGLE_FAN_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("INSERT_TRIANGLE_FAN_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			App::uiThread->addStatusLine(IDSNS_INSERTING_TRIANGLE_FAN);
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getEditModeObjectID());
			if ((it!=NULL)&&(App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE))
			{
				if (it->getObjectType()==sim_object_shape_type)
				{
					for (int i=0;i<(int(sel.size())-1)/2;i++)
					{
						App::ct->objCont->addTriangle(sel[2*i+0],sel[2*i+1],sel[sel.size()-1]);
						if (2*i+2<(int(sel.size())-1))
							App::ct->objCont->addTriangle(sel[2*i+1],sel[2*i+2],sel[sel.size()-1]);
					}
				}
			}
			App::ct->objCont->deselectEditModeBuffer();
			App::ct->objCont->setFullDialogRefreshFlag();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==LEAVE_EDIT_MODE_CANCEL_CHANGES_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("LEAVE_EDIT_MODE_CANCEL_CHANGES_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ENDING_EDIT_MODE_AND_CANCELLING_CHANGES);
			App::ct->objCont->endEditMode(true);
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==LEAVE_EDIT_MODE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("LEAVE_EDIT_MODE_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ENDING_EDIT_MODE_AND_APPLYING_CHANGES);
			App::ct->objCont->endEditMode(false);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	return(false);
}

void CTriangleVertexEdition::copySelectedFaces(std::vector<int>* sel,std::vector<float>* vert,std::vector<int>* ind,std::vector<float>* norm,std::vector<float>* tex)
{  // norm or tex can be NULL
	if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
	{
		std::vector<int> vertInd;
		vertInd.reserve(sel->size()*3);// max value
		vertInd.clear();
		// We add only once every vertex:
		for (int i=0;i<int(sel->size());i++)
		{
			for (int j=0;j<3;j++)
			{
				int val=App::ct->objCont->_editionIndices[3*sel->at(i)+j];
				// We check if already present:
				bool present=false;
				for (int k=0;k<int(vertInd.size());k++)
				{
					if (vertInd[k]==val)
					{
						present=true;
						break;
					}
				}
				if (!present)
					vertInd.push_back(val);
			}
		}
		vert->reserve(3*vertInd.size());
		vert->clear();
		ind->reserve(3*sel->size());
		ind->clear();
		if (norm!=NULL)
		{
			norm->reserve(9*sel->size());
			norm->clear();
		}
		if (tex!=NULL)
		{
			tex->reserve(6*sel->size());
			tex->clear();
		}
		// We copy the vertices:
		for (int i=0;i<int(vertInd.size());i++)
		{
			for (int j=0;j<3;j++)
				vert->push_back(App::ct->objCont->_editionVertices[3*vertInd[i]+j]);
		}
		// We copy the indices and normals and tex coords:
		for (int i=0;i<int(sel->size());i++)
		{
			for (int j=0;j<3;j++)
			{
				int oldIndex=App::ct->objCont->_editionIndices[3*sel->at(i)+j];
				// We have to find the new index:
				int newIndex=0;
				for (int k=0;k<int(vertInd.size());k++)
				{
					if (vertInd[k]==oldIndex)
					{
						newIndex=k;
						break;
					}
				}
				ind->push_back(newIndex);
				if (norm!=NULL)
				{
					norm->push_back(App::ct->objCont->_editionNormals[9*sel->at(i)+3*j+0]);
					norm->push_back(App::ct->objCont->_editionNormals[9*sel->at(i)+3*j+1]);
					norm->push_back(App::ct->objCont->_editionNormals[9*sel->at(i)+3*j+2]);
				}
				if (tex!=NULL)
				{
					tex->push_back(App::ct->objCont->_editionTextureCoords[6*sel->at(i)+2*j+0]);
					tex->push_back(App::ct->objCont->_editionTextureCoords[6*sel->at(i)+2*j+1]);
				}
			}
		}
	}
}
