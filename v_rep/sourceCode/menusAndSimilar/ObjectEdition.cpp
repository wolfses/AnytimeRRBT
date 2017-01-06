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
#include "ObjectEdition.h"
#include "OGL.h"
#include "Simulation.h"

#include "IloIlo.h"
#include "geometric.h"
#include "App.h"
#include "MeshManip.h"
#include "Tt.h"
#include "v_repStrings.h"
#include "MeshRoutines.h"
#include <boost/lexical_cast.hpp>
#include "qdlgconvexdecomposition.h"

const char OPERATION_DISABLED_WITH_INDIVIDUAL_PATH_POINTS_SELECTED[]="Operation disabled while individual path points are selected.";

void CObjectEdition::addRootObjectChildrenToSelection(std::vector<int>& selection)
{
	for (int i=0;i<int(selection.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(selection[i]);
		if ( (it!=NULL)&&it->getModelBase() )
		{
			std::vector<C3DObject*> newObjs;
			it->getAllObjectsRecursive(&newObjs,false,true);
			for (int j=0;j<int(newObjs.size());j++)
			{
				if (!App::ct->objCont->isObjectInSelection(newObjs[j]->getID(),&selection))
					selection.push_back(newObjs[j]->getID());
			}
		}
	}
}

void CObjectEdition::copyObjects(std::vector<int>* selection)
{
	// We first copy the selection:
	std::vector<int> sel(*selection);
	addRootObjectChildrenToSelection(sel);
	App::ct->copyBuffer->copyCurrentSelection(&sel,App::ct->environment->getSceneLocked());
	App::ct->objCont->deselectObjects(); // We clear selection
}

void CObjectEdition::pasteCopyBuffer()
{
	FUNCTION_DEBUG;
	if (App::ct->copyBuffer->pasteBuffer(App::ct->environment->getSceneLocked())==-1)
	{ // Error: trying to copy locked buffer into unlocked scene!
		App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Paste"),strTranslate(IDS_SCENE_IS_LOCKED_CANNOT_PASTE_WARNING),VMessageBox::OKELI);
		return;
	}
	App::ct->objCont->removeFromSelectionAllExceptModelBase(true);
}

void CObjectEdition::cutObjects(std::vector<int>* selection)
{
	FUNCTION_DEBUG;
	addRootObjectChildrenToSelection(*selection);
	copyObjects(selection);
	deleteObjects(selection);
	App::ct->objCont->deselectObjects(); // We clear selection
}

void CObjectEdition::deleteObjects(std::vector<int>* selection)
{ // There are a few other spots where objects get deleted (e.g. the C-interface)
	FUNCTION_DEBUG;
	addRootObjectChildrenToSelection(*selection);
	std::vector<int> sel(*selection);
	for (int i=0;i<int(sel.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(sel[i]);
		if (it!=NULL)
			App::ct->objCont->eraseObject(it);
	}
	App::ct->objCont->deselectObjects(); // We clear selection
}

void CObjectEdition::addMenu(VMenu* menu)
{
	int selItems=App::ct->objCont->getSelSize();
	int selDummies=App::ct->objCont->getDummyNumberInSelection();
	int shapeNumber=App::ct->objCont->getShapeNumberInSelection();
	int pathNumber=App::ct->objCont->getPathNumberInSelection();
//	int simpleShapeNumber=App::ct->objCont->getSimpleShapeNumberInSelection();
	bool noSim=App::ct->simulation->isSimulationStopped();
	bool lastSelIsShape=App::ct->objCont->isLastSelectionAShape();
	bool lastSelIsPath=App::ct->objCont->isLastSelectionAPath();
	std::vector<int> rootSel;
	for (int i=0;i<App::ct->objCont->getSelSize();i++)
		rootSel.push_back(App::ct->objCont->getSelID(i));
	CObjectEdition::addRootObjectChildrenToSelection(rootSel);
	int shapesInRootSel=App::ct->objCont->getShapeNumberInSelection(&rootSel);
	if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
	{
		VMenubar::appendMenuItem(menu,App::ct->undoBufferContainer->canUndo(),false,UNDO_CMD,IDSN_UNDO);
		VMenubar::appendMenuItem(menu,App::ct->undoBufferContainer->canRedo(),false,REDO_CMD,IDSN_REDO);
		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,selItems>1,false,MAKE_PARENT_CMD,IDS_MAKE_LAST_SELECTED_OBJECTS_PARENT_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selItems>0,false,MAKE_ORPHANS_CMD,IDS_MAKE_SELECTED_OBJECT_S__ORPHAN_MENU_ITEM);
		VMenubar::appendMenuItem(menu,(selItems>1)&&noSim,false,REPLACE_OBJECTS_CMD,IDS_REPLACE_OBJECTS_MENU_ITEM);
		VMenubar::appendMenuItem(menu,(shapesInRootSel>0)&&noSim,false,MORPH_INTO_CONVEX_CMD,IDS_CONVEX_TRANSFORMATION_MENU_ITEM);
		
		if ((selItems==2)&&(selDummies==2))
		{ // we have 2 selected dummies we might want to link/unlink:
			CDummy* dumA=App::ct->objCont->getDummy(App::ct->objCont->getSelID(0));
			CDummy* dumB=App::ct->objCont->getDummy(App::ct->objCont->getSelID(1));
			if ((dumA!=NULL)&&(dumB!=NULL))
			{
				if (dumA->getLinkedDummyID()==dumB->getID())
					VMenubar::appendMenuItem(menu,true,false,UNLINK_DUMMIES_CMD,IDS_UNLINK_SELECTED_DUMMIES_MENU_ITEM);
				else
				{
					VMenu* dummyLinking=VMenubar::createPopupMenu();
					VMenubar::appendMenuItem(dummyLinking,true,false,LINK_DUMMIES_IK_TIP_TARGET,IDS_DUMMY_LINK_TYPE_IK_TIP_TARGET);
					VMenubar::appendMenuItem(dummyLinking,true,false,LINK_DUMMIES_GCS_LOOP_CLOSURE,IDS_DUMMY_LINK_TYPE_GCS_LOOP_CLOSURE);
					VMenubar::appendMenuItem(dummyLinking,true,false,LINK_DUMMIES_GCS_TIP,IDS_DUMMY_LINK_TYPE_GCS_TIP);
					VMenubar::appendMenuItem(dummyLinking,true,false,LINK_DUMMIES_GCS_TARGET,IDS_DUMMY_LINK_TYPE_GCS_TARGET);
					VMenubar::appendMenuItem(dummyLinking,true,false,LINK_DUMMIES_DYNAMICS_LOOP_CLOSURE,IDS_DUMMY_LINK_TYPE_DYNAMICS_LOOP_CLOSURE);
					VMenubar::appendMenuAndDetach(menu,dummyLinking,true,IDS_LINK_SELECTED_DUMMIES_LINK_TYPE_MENU_ITEM);
				}
			}
		}
		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,selItems>0,false,OBJECT_FULL_COPY_CMD,IDS_COPY_SELECTED_OBJECTS_MENU_ITEM);
		VMenubar::appendMenuItem(menu,!App::ct->copyBuffer->isBufferEmpty(),false,PASTE_OBJECTS_CMD,IDS_PASTE_BUFFER_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selItems>0,false,DELETE_OBJECTS_CMD,IDS_DELETE_SELECTED_OBJECTS_MENU_ITEM);
		VMenubar::appendMenuItem(menu,selItems>0,false,OBJECT_FULL_CUT_CMD,IDS_CUT_SELECTED_OBJECTS_MENU_ITEM);
		VMenubar::appendMenuSeparator(menu);
		VMenubar::appendMenuItem(menu,true,false,SELECT_ALL_OBJECTS_CMD,IDSN_SELECT_ALL_MENU_ITEM);
		VMenubar::appendMenuSeparator(menu);

		if (App::ct->objCont->isLastSelectionAPath())
		{
			VMenubar::appendMenuItem(menu,(selItems==1)&&noSim,false,EMPTY_PATH_CMD,IDS_EMPTY_LAST_SELECTED_PATH_MENU_ITEM);
			VMenubar::appendMenuItem(menu,(selItems==1)&&noSim,false,ROLL_PATH_POINTS_FORWARD_CMD,IDS_ROLL_PATH_POINTS_FORWARD_MENU_ITEM);
			VMenubar::appendMenuItem(menu,(selItems==1)&&noSim,false,ROLL_PATH_POINTS_BACKWARD_CMD,IDS_ROLL_PATH_POINTS_BACKWARD_MENU_ITEM);
			VMenubar::appendMenuSeparator(menu);
		}

		VMenu* grouping=VMenubar::createPopupMenu();
		VMenubar::appendMenuItem(grouping,(shapeNumber==selItems)&&(selItems>1)&&noSim,false,GROUP_SHAPES_CMD,IDS_GROUP_SELECTED_SHAPES_MENU_ITEM);
		VMenubar::appendMenuItem(grouping,(shapeNumber==selItems)&&(selItems>0)&&noSim,false,UNGROUP_SHAPES_CMD,IDS_UNGROUP_SELECTED_SHAPES_MENU_ITEM);
		VMenubar::appendMenuSeparator(grouping);
		VMenubar::appendMenuItem(grouping,(shapeNumber==selItems)&&(selItems>1)&&noSim,false,MERGE_SHAPES_CMD,IDS_MERGE_SELECTED_SHAPES_MENU_ITEM);
		VMenubar::appendMenuItem(grouping,(shapeNumber==selItems)&&(selItems>0)&&noSim,false,DIVIDE_SHAPES_CMD,IDS_DIVIDE_SELECTED_SHAPES_MENU_ITEM);
		VMenubar::appendMenuSeparator(grouping);
		VMenubar::appendMenuItem(grouping,(pathNumber==selItems)&&(selItems>1)&&noSim,false,MERGE_PATHS_CMD,IDS_MERGE_SELECTED_PATHS_MENU_ITEM);
		VMenubar::appendMenuAndDetach(menu,grouping,true,IDS_GROUPING_MERGING_MENU_ITEM);

		VMenu* align=VMenubar::createPopupMenu();
		VMenubar::appendMenuItem(align,(shapeNumber==selItems)&&(selItems!=0)&&noSim,false,ALIGN_BOUNDING_BOX_WITH_WORLD_CMD,IDS_ALIGN_SELECTED_SHAPE_WORLD_MENU_ITEM);
		VMenubar::appendMenuItem(align,(shapeNumber==selItems)&&(selItems!=0)&&noSim,false,ALIGN_BOUNDING_BOX_WITH_MAIN_AXIS_CMD,IDS_ALIGN_SELECTED_SHAPE_MAIN_AXIS_MENU_ITEM);
		VMenubar::appendMenuItem(align,(shapeNumber==selItems)&&(selItems!=0)&&noSim,false,ALIGN_BOUNDING_BOX_WITH_TUBE_MAIN_AXIS_CMD,IDSN_ALIGN_BB_WITH_TUBE);
		VMenubar::appendMenuItem(align,(shapeNumber==selItems)&&(selItems!=0)&&noSim,false,ALIGN_BOUNDING_BOX_WITH_CUBOID_MAIN_AXIS_CMD,IDSN_ALIGN_BB_WITH_CUBOID);
		VMenubar::appendMenuAndDetach(menu,align,true,IDS_BOUNDING_BOX_ALIGNMENT_MENU_ITEM);

		VMenu* edModes=VMenubar::createPopupMenu();
		VMenubar::appendMenuItem(edModes,lastSelIsShape&&App::ct->simulation->isSimulationStopped(),false,ENTER_SHAPE_EDIT_MODE_CMD,IDSN_SHAPE_EDIT_MODE_START_MENU_ITEM);

		VMenubar::appendMenuItem(edModes,lastSelIsPath&&App::ct->simulation->isSimulationStopped(),false,ENTER_PATH_EDIT_MODE_CMD,IDS_PATH_EDIT_MODE_START_MENU_ITEM);
		VMenubar::appendMenuItem(edModes,App::ct->simulation->isSimulationStopped(),false,ENTER_BUTTON_EDIT_MODE_CMD,IDS_ENTER_2D_ELEMENT_EDIT_MODE_MENU_ITEM);
		VMenubar::appendMenuAndDetach(menu,edModes,true,IDS_EDIT_MODES_MENU_ITEM);
	}
}

bool CObjectEdition::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to object edition menu and was executed

	if (commandID==EMPTY_PATH_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("EMPTY_PATH_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if (sel.size()>0)
			{
				CPath* path=App::ct->objCont->getPath(sel[sel.size()-1]);
				if (path!=NULL)
				{
					path->pathContainer->removeAllSimplePathPoints();
					App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
					App::uiThread->addStatusLine(IDSNS_PATH_WAS_EMPTIED);
				}
			}
		}
		return(true);
	}
	if ((commandID==ROLL_PATH_POINTS_FORWARD_CMD)||(commandID==ROLL_PATH_POINTS_BACKWARD_CMD))
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ROLL_PATH_POINTS...CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if (sel.size()>0)
			{
				CPath* path=App::ct->objCont->getPath(sel[sel.size()-1]);
				if (path!=NULL)
				{
					path->pathContainer->rollPathPoints(commandID==ROLL_PATH_POINTS_FORWARD_CMD);
					App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
					if (commandID==ROLL_PATH_POINTS_FORWARD_CMD)
						App::uiThread->addStatusLine(IDSNS_FORWARD_ROLL_PERFORMED);
					else
						App::uiThread->addStatusLine(IDSNS_BACKWARD_ROLL_PERFORMED);
				}
			}
		}
		return(true);
	}
	
	if (commandID==ASSEMBLE_CMD)
	{
		// There is another such routine!! XXBFVGA
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ASSEMBLE_CMD")
		{
			bool assembleEnabled=false;
			bool disassembleEnabled=false;
			int selS=App::ct->objCont->getSelSize();
			if (selS==1)
			{ // here we can only have disassembly
				C3DObject* it=App::ct->objCont->getLastSelection();
				disassembleEnabled=(it->getParent()!=NULL);
				if (disassembleEnabled)
				{
					App::uiThread->addStatusLine(IDSN_DISASSEMBLING_OBJECT);
					App::ct->objCont->makeObjectChildOf(it,NULL);
				}
			}
			else if (selS==2)
			{ // here we can have assembly or disassembly
				C3DObject* it1=App::ct->objCont->getLastSelection();
				C3DObject* it2=App::ct->objCont->getObject(App::ct->objCont->getSelID(0));
				if ((it1->getParent()==it2)||(it2->getParent()==it1))
				{
					App::uiThread->addStatusLine(IDSN_DISASSEMBLING_OBJECT);
					disassembleEnabled=true; // disassembly
					if (it1->getParent()==it2)
					{
						App::ct->objCont->makeObjectChildOf(it1,NULL);
						App::ct->objCont->deselectObjects();
						App::ct->objCont->addObjectToSelection(it1->getID());
						App::ct->objCont->addObjectToSelection(it2->getID());
					}
					else
						App::ct->objCont->makeObjectChildOf(it2,NULL);
				}
				else
				{ // assembly
					if ( it1->getAssemblyRole(false)&&it2->getAssemblyRole(true)&&(it1->getAssemblyMatchValue(false)==it2->getAssemblyMatchValue(true)) )
					{
						App::uiThread->addStatusLine(IDSN_ASSEMBLING_2_OBJECTS);
						assembleEnabled=true;
						App::ct->objCont->makeObjectChildOf(it2,it1);
						// if (it2->getUseSpecialLocalTransformationWhenAssembling())
							it2->setLocalTransformation(it2->getAssemblingLocalTransformation());
					}
					else
					{ // here we might have the opposite of what we usually do to assemble (i.e. last selection should always be parent, but not here)
						// we assemble anyways if the roles are unequivoque:
						if ( it2->getAssemblyRole(false)&&it1->getAssemblyRole(true)&&(it2->getAssemblyMatchValue(false)==it1->getAssemblyMatchValue(true)) )
						{
							App::uiThread->addStatusLine(IDSN_ASSEMBLING_2_OBJECTS);
							assembleEnabled=true;
							App::ct->objCont->makeObjectChildOf(it1,it2);
							// if (it1->getUseSpecialLocalTransformationWhenAssembling())
								it1->setLocalTransformation(it1->getAssemblingLocalTransformation());
						}
					}
				}
			}

			if (assembleEnabled||disassembleEnabled)
			{
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}

	if (commandID==TRANSFER_DNA_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("TRANSFER_DNA_CMD")
		{
			int selS=App::ct->objCont->getSelSize();
			if (selS==1)
			{
				C3DObject* it=App::ct->objCont->getLastSelection();
				if (it->getObjectProperty()&sim_objectproperty_canupdatedna)
				{
					bool model=it->getModelBase();
					std::vector<C3DObject*> siblings;
					// Retrieve the sibling in the scene:
					for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
					{
						C3DObject* it2=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
						if ( (it2!=it)&&(it2->getObjectProperty()&sim_objectproperty_canupdatedna)&&(it2->getUniqueUpdatableString().compare(it->getUniqueUpdatableString())==0) )
						{
							if (!model)
								siblings.push_back(it2);
							else
							{ // Here we also have to check that the sibling model is not located in the same hierarchy as this one:
								bool sameHierarchy=false;
								C3DObject* ite=it2;
								while (true)
								{
									ite=ite->getParent();
									if (ite==NULL)
										break;
									if (ite==it)
									{
										sameHierarchy=true;
										break;
									}
								}
								if (!sameHierarchy)
								{ // Here we also need to check if the sibling model is not located in the hierarchy of another sibling:
									sameHierarchy=false;
									ite=it2;
									while (true)
									{
										ite=ite->getParent();
										if (ite==NULL)
											break;
										if ( (ite->getObjectProperty()&sim_objectproperty_canupdatedna)&&(ite->getUniqueUpdatableString().compare(it->getUniqueUpdatableString())==0) )
										{
											sameHierarchy=true;
											break;
										}
									}
									if (!sameHierarchy)
										siblings.push_back(it2);
								}
							}
						}
					}
					std::vector<int> newSelection;
					if (siblings.size()>0)
					{
						App::uiThread->addStatusLine(IDSN_TRANSFERING_DNA_TO_SIBLINGS);
						App::ct->copyBuffer->memorizeBuffer();

						std::vector<int> sel;
						sel.push_back(it->getID());
						if (model)
							CObjectEdition::addRootObjectChildrenToSelection(sel);
						std::string masterName(it->getName());
						App::ct->copyBuffer->copyCurrentSelection(&sel,App::ct->environment->getSceneLocked());
						App::ct->objCont->deselectObjects();
						for (int i=0;i<int(siblings.size());i++)
						{
							if (!model)
							{
								std::string name(siblings[i]->getName());
								C3DObject* parent(siblings[i]->getParent());
								C7Vector tr(siblings[i]->getLocalTransformationPart1());
								std::vector<C3DObject*> children;
								std::vector<C7Vector> childrenTr;
								for (int j=0;j<int(siblings[i]->childList.size());j++)
								{
									children.push_back(siblings[i]->childList[j]);
									childrenTr.push_back(siblings[i]->childList[j]->getLocalTransformationPart1());
								}
								App::ct->objCont->eraseObject(siblings[i]);
								App::ct->copyBuffer->pasteBuffer(App::ct->environment->getSceneLocked());
								C3DObject* newObj=App::ct->objCont->getLastSelection();
								App::ct->objCont->deselectObjects();
								if (newObj!=NULL)
								{
									newSelection.push_back(newObj->getID());
									App::ct->objCont->makeObjectChildOf(newObj,parent);
									newObj->setLocalTransformation(tr);
									for (int j=0;j<int(children.size());j++)
									{
										children[j]->setParent(newObj);
										children[j]->setLocalTransformation(childrenTr[j]);
									}
									if (App::ct->luaScriptContainer->getScriptFromObjectAttachedTo(newObj->getID())!=NULL)
									{ // we just try to keep the name suffix
										int oldSuffix=tt::getNameSuffixNumber(name.c_str(),true);
										std::string newName(tt::generateNewName_dash(tt::getNameWithoutSuffixNumber(masterName.c_str(),true),oldSuffix));
										while (App::ct->objCont->getObject(newName)!=NULL)
											newName=tt::generateNewName_dash(newName);
										newObj->setName(newName);
									}
									else
										newObj->setName(name); // we keep the old name
								}
							}
							else
							{
								std::vector<int> objs;
								objs.push_back(siblings[i]->getID());
								CObjectEdition::addRootObjectChildrenToSelection(objs);
								C7Vector tr(siblings[i]->getLocalTransformation());
								C3DObject* parent(siblings[i]->getParent());
								for (int j=0;j<int(objs.size());j++)
									App::ct->objCont->eraseObject(App::ct->objCont->getObject(objs[j]));
								App::ct->copyBuffer->pasteBuffer(App::ct->environment->getSceneLocked());
								App::ct->objCont->removeFromSelectionAllExceptModelBase(false);
								C3DObject* newObj=App::ct->objCont->getLastSelection();
								App::ct->objCont->deselectObjects();
								if (newObj!=NULL)
								{
									newSelection.push_back(newObj->getID());
									App::ct->objCont->makeObjectChildOf(newObj,parent);
									newObj->setLocalTransformation(tr);
								}
							}
						}
						App::ct->copyBuffer->restoreBuffer();
						App::ct->copyBuffer->clearMemorizedBuffer();
						App::uiThread->addStatusLine(IDSNS_DONE);
						std::string txt;
						txt+=boost::lexical_cast<std::string>(siblings.size())+IDSN_X_SIBLINGS_WERE_UPDATED;
						App::uiThread->addStatusLine(txt.c_str());

						for (int i=0;i<int(newSelection.size());i++)
							App::ct->objCont->addObjectToSelection(newSelection[i]);

						App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
					}
				}
			}
		}
		return(true);
	}

	if (commandID==UNDO_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("UNDO_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_EXECUTING_UNDO);
			App::ct->undoBufferContainer->undo();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}

	if (commandID==REDO_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("UNDO_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_EXECUTING_REDO);
			App::ct->undoBufferContainer->redo();
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}


	if (commandID==MAKE_PARENT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("MAKE_PARENT_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if (sel.size()>1)
			{
				C3DObject* last=App::ct->objCont->getObject(sel[sel.size()-1]);
				for (int i=0;i<int(sel.size())-1;i++)
				{
					C3DObject* it=App::ct->objCont->getObject(sel[i]);
					App::ct->objCont->makeObjectChildOf(it,last);
				}
				App::ct->objCont->selectObject(last->getID()); // We select the parent

				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				std::string txt(IDSNS_ATTACHING_OBJECTS_TO);
				txt+=last->getName()+"'...";
				App::uiThread->addStatusLine(txt.c_str());
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}

	if (commandID==MAKE_ORPHANS_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("MAKE_ORPHANS_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			App::uiThread->addStatusLine(IDSNS_MAKING_ORPHANS);
			for (int i=0;i<int(sel.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(sel[i]);
				App::ct->objCont->makeObjectChildOf(it,NULL);
			}
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::ct->objCont->deselectObjects(); // We clear selection
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==REPLACE_OBJECTS_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("REPLACE_OBJECTS_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if (sel.size()>1)
			{
				App::uiThread->addStatusLine(IDSNS_REPLACING_OBJECTS);
				C3DObject* last=App::ct->objCont->getObject(sel[sel.size()-1]);
				for (int i=0;i<int(sel.size()-1);i++)
				{
					C3DObject* it=App::ct->objCont->getObject(sel[i]);
					C3DObject* cop=last->copyYourself();
					App::ct->objCont->addObjectToScene(cop,false);
					App::ct->objCont->makeObjectChildOf(cop,it->getParent());
					cop->setLocalTransformation(it->getLocalTransformationPart1());
					std::vector<C3DObject*> children(it->childList);
					for (int j=0;j<int(children.size());j++)
						App::ct->objCont->makeObjectChildOf(children[j],cop);
					std::string name(it->getName());
					App::ct->objCont->eraseObject(it);
					cop->setName(name);

					CLuaScriptObject* luaScript=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo(last->getID());
					if (luaScript!=NULL)
					{
						App::ct->luaScriptContainer->insertScript(luaScript->copyYourself());
						luaScript->setObjectIDThatScriptIsAttachedTo(cop->getID());
					}
				}
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::ct->objCont->deselectObjects(); // We clear selection
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	
	if (commandID==MORPH_INTO_CONVEX_CMD)
	{
		IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG_CMD("MORPH_INTO_CONVEX_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			CObjectEdition::addRootObjectChildrenToSelection(sel);

			bool addExtraDistPoints=true;
			bool addFacesPoints=true;
			int nClusters=1;
			int maxHullVertices=100;
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
			App::uiThread->addStatusLine(IDSNS_MORPHING_INTO_CONVEX);
			for (int obji=0;obji<int(sel.size());obji++)
			{
				CShape* it=App::ct->objCont->getShape(sel[obji]);
				if (it!=NULL)
				{
					int newShapeHandle=generateConvexDecomposed(sel[obji],nClusters,maxConcavity,addExtraDistPoints,addFacesPoints,maxConnectDist,maxTrianglesInDecimatedMesh,maxHullVertices,smallClusterThreshold,individuallyConsiderMultishapeComponents,randomColors,maxIterations);
					if (newShapeHandle!=-1)
					{
						// Get the mass and inertia info from the old shape:
						C7Vector absCOM(it->getCumulativeTransformation());
						absCOM=absCOM*it->geomData->geomInfo->getLocalInertiaFrame();
						float mass=it->geomData->geomInfo->getMass();
						C7Vector absCOMNoShift(absCOM);
						absCOMNoShift.X.clear(); // we just wanna get the orientation of the inertia matrix, no shift info!
						C3X3Matrix tensor(CGeomWrap::getNewTensor(it->geomData->geomInfo->getPrincipalMomentsOfInertia(),absCOMNoShift));

						// Set-up the new shape:
						CShape* newShape=App::ct->objCont->getShape(newShapeHandle);
						C7Vector newLocal(it->getParentCumulativeTransformation().getInverse()*newShape->getCumulativeTransformation());
						C7Vector oldLocal(it->getLocalTransformation());
						delete it->geomData;
						it->geomData=newShape->geomData; // we exchange the geomData object
						it->setLocalTransformation(newLocal); // The shape's frame was changed!
						it->incrementMemorizedConfigurationValidCounter(); // so if we are running in a simulation, the shape doesn't get reset at its initial config
						newShape->geomData=NULL;
						App::ct->objCont->eraseObject(newShape);

						// Transfer the mass and inertia info to the new shape:
						it->geomData->geomInfo->setMass(mass);
						C4Vector rot;
						C3Vector pmoi;
						CGeomWrap::findPrincipalMomentOfInertia(tensor,rot,pmoi);
						it->geomData->geomInfo->setPrincipalMomentsOfInertia(pmoi);
						absCOM.Q=rot;
						C7Vector relCOM(it->getCumulativeTransformation().getInverse()*absCOM);
						it->geomData->geomInfo->setLocalInertiaFrame(relCOM);



						// We need to correct all its children for this change of frame:
						for (int i=0;i<int(it->childList.size());i++)
						{
							it->childList[i]->setLocalTransformation(newLocal.getInverse()*oldLocal*it->childList[i]->getLocalTransformationPart1());
							it->childList[i]->incrementMemorizedConfigurationValidCounter(); // so if we are running in a simulation, the shape doesn't get reset at its initial config
						}
					}
				}
			}
			App::ct->objCont->deselectObjects();
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}

	if ((commandID==UNLINK_DUMMIES_CMD)||(commandID==LINK_DUMMIES_IK_TIP_TARGET)||(commandID==LINK_DUMMIES_GCS_LOOP_CLOSURE)||
		(commandID==LINK_DUMMIES_GCS_TIP)||(commandID==LINK_DUMMIES_GCS_TARGET)||(commandID==LINK_DUMMIES_DYNAMICS_LOOP_CLOSURE))
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("LINK_DUMMIES...")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if (sel.size()==2)
			{
				CDummy* a=App::ct->objCont->getDummy(sel[0]);
				CDummy* b=App::ct->objCont->getDummy(sel[1]);
				if ((a!=NULL)&&(b!=NULL))
				{
					if (commandID==UNLINK_DUMMIES_CMD)
					{
						a->setLinkedDummyID(-1,false);
						App::uiThread->addStatusLine("Unlinking selected dummies... Done.");
					}
					else
					{
						a->setLinkedDummyID(b->getID(),false);
						if (commandID==LINK_DUMMIES_IK_TIP_TARGET)
							b->setLinkType(sim_dummy_linktype_ik_tip_target,false);
						if (commandID==LINK_DUMMIES_GCS_LOOP_CLOSURE)
							b->setLinkType(sim_dummy_linktype_gcs_loop_closure,false);
						if (commandID==LINK_DUMMIES_GCS_TIP)
							b->setLinkType(sim_dummy_linktype_gcs_tip,false);
						if (commandID==LINK_DUMMIES_GCS_TARGET)
							b->setLinkType(sim_dummy_linktype_gcs_target,false);
						if (commandID==LINK_DUMMIES_DYNAMICS_LOOP_CLOSURE)
							b->setLinkType(sim_dummy_linktype_dynamics_loop_closure,false);
						App::uiThread->addStatusLine("Linking selected dummies... Done.");
					}
				}
			}
		}
		return(true);
	}
	if (commandID==SELECT_ALL_OBJECTS_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("SELECT_ALL_OBJECTS_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_SELECTING_ALL_OBJECTS);
			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
				App::ct->objCont->addObjectToSelection(App::ct->objCont->objectList[i]);
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==OBJECT_FULL_COPY_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("OBJECT_FULL_COPY_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if ( (sel.size()==1)&&(App::ct->objCont->getPath(sel[0])!=NULL)&&(App::ct->objCont->getPointerToSelectedPathPointIndices_nonEditMode()->size()!=0) )
			{ // We wanna copy individual path points!
				App::uiThread->addStatusLine(OPERATION_DISABLED_WITH_INDIVIDUAL_PATH_POINTS_SELECTED);
				// Not supported for now
			}
			else
			{ // regular object copy
				App::uiThread->addStatusLine(IDSNS_COPYING_SELECTION);
				copyObjects(&sel);
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	if (commandID==OBJECT_FULL_CUT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("OBJECT_FULL_CUT_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if ( (sel.size()==1)&&(App::ct->objCont->getPath(sel[0])!=NULL)&&(App::ct->objCont->getPointerToSelectedPathPointIndices_nonEditMode()->size()!=0) )
			{ // We wanna cut individual path points!
				App::uiThread->addStatusLine(OPERATION_DISABLED_WITH_INDIVIDUAL_PATH_POINTS_SELECTED);
				// Not supported for now
			}
			else
			{ // regular object cutting
				App::uiThread->addStatusLine(IDSNS_CUTTING_SELECTION);
				cutObjects(&sel);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	if (commandID==PASTE_OBJECTS_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("PASTE_OBJECTS_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if ( (sel.size()==1)&&(App::ct->objCont->getPath(sel[0])!=NULL)&&(App::ct->objCont->getPointerToSelectedPathPointIndices_nonEditMode()->size()!=0) )
			{ // We wanna paste individual path points!
				App::uiThread->addStatusLine(OPERATION_DISABLED_WITH_INDIVIDUAL_PATH_POINTS_SELECTED);
				// Not supported for now
			}
			else
			{ // regular object pasting
				App::uiThread->addStatusLine(IDSNS_PASTING_BUFFER);
				pasteCopyBuffer();
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	if (commandID==EMPTY_OBJECT_COPY_BUFFER_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("EMPTY_OBJECT_COPY_BUFFER_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if ( (sel.size()==1)&&(App::ct->objCont->getPath(sel[0])!=NULL)&&(App::ct->objCont->getPointerToSelectedPathPointIndices_nonEditMode()->size()!=0) )
			{ // We wanna empty the individual path point buffer!
				App::uiThread->addStatusLine(OPERATION_DISABLED_WITH_INDIVIDUAL_PATH_POINTS_SELECTED);
				// Not supported for now
			}
			else
			{ // Emptying regular object buffer
				App::uiThread->addStatusLine(IDSNS_CLEARING_BUFFER);
				App::ct->copyBuffer->clearBuffer();
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	if (commandID==DELETE_OBJECTS_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("DELETE_OBJECTS_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if ( (sel.size()==1)&&(App::ct->objCont->getPath(sel[0])!=NULL)&&(App::ct->objCont->getPointerToSelectedPathPointIndices_nonEditMode()->size()!=0) )
			{ // We wanna destroy individual path points!
				App::uiThread->addStatusLine(IDSNS_DELETING_SELECTED_PATH_POINTS);
				CPath* path=App::ct->objCont->getPath(sel[0]);
				CPathCont* pc=path->pathContainer;
				std::vector<int>* p=App::ct->objCont->getPointerToSelectedPathPointIndices_nonEditMode();
				std::vector<bool> toDelete(pc->getSimplePathPointCount(),false);
				for (int i=0;i<int(p->size());i++)
					toDelete[p->at(i)]=true;
				for (int i=int(toDelete.size())-1;i>=0;i--)
				{
					if (toDelete[i])
						pc->removeSimplePathPoint(i);
				}
				App::ct->objCont->clearPathPointIndices_nonEditMode();
			}
			else
			{ // regular object destruction
				App::uiThread->addStatusLine(IDSNS_DELETING_SELECTION);
				deleteObjects(&sel);
			}
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if ((commandID==ALIGN_BOUNDING_BOX_WITH_MAIN_AXIS_CMD)||(commandID==ALIGN_BOUNDING_BOX_WITH_WORLD_CMD)||(commandID==ALIGN_BOUNDING_BOX_WITH_TUBE_MAIN_AXIS_CMD)||(commandID==ALIGN_BOUNDING_BOX_WITH_CUBOID_MAIN_AXIS_CMD))
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ALIGN_BOUNDING_BOX...CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			bool tubeFail=false;
			bool cuboidFail=false;
			if (commandID==ALIGN_BOUNDING_BOX_WITH_MAIN_AXIS_CMD)
				App::uiThread->addStatusLine(IDSNS_ALIGNING_BOUNDING_BOXES_WITH_MAIN_AXIS);
			if (commandID==ALIGN_BOUNDING_BOX_WITH_WORLD_CMD)
				App::uiThread->addStatusLine(IDSNS_ALIGNING_BOUNDING_BOXES_WITH_WORLD);
			if (commandID==ALIGN_BOUNDING_BOX_WITH_TUBE_MAIN_AXIS_CMD)
				App::uiThread->addStatusLine(IDSNS_ALIGNING_BOUNDING_BOXES_WITH_TUBES);
			if (commandID==ALIGN_BOUNDING_BOX_WITH_CUBOID_MAIN_AXIS_CMD)
				App::uiThread->addStatusLine(IDSNS_ALIGNING_BOUNDING_BOXES_WITH_CUBOIDS);
			if (App::ct->objCont->getShapeNumberInSelection(&sel)==int(sel.size()))
			{
				std::vector<void*> processedGeoms;
				processedGeoms.reserve(sel.size());
				bool informThatPurePrimitivesWereNotChanged=false;
				for (int i=0;i<int(sel.size());i++)
				{
					CShape* theShape=App::ct->objCont->getShape(sel[i]);
					if (theShape!=NULL)
					{
						// Did we already process this geometric resource?
						bool found=false;
						for (int j=0;j<int(processedGeoms.size());j++)
						{
							if (processedGeoms[j]==(void*)theShape->geomData)
								found=true;
						}
						if (!found)
						{
							processedGeoms.push_back(theShape->geomData);
							if ( (!theShape->geomData->geomInfo->isPure())||(theShape->isGrouping()) )
							{ // We can reorient all shapes, except for pure simple shapes (i.e. pure non-grouped shapes)
								if (commandID==ALIGN_BOUNDING_BOX_WITH_WORLD_CMD)
									theShape->alignBoundingBoxWithWorld();
								if (commandID==ALIGN_BOUNDING_BOX_WITH_MAIN_AXIS_CMD)
									theShape->alignBoundingBoxWithMainAxis();
								if (commandID==ALIGN_BOUNDING_BOX_WITH_TUBE_MAIN_AXIS_CMD)
								{
									if (!theShape->alignTubeBoundingBoxWithMainAxis())
										tubeFail=true;
								}
								if (commandID==ALIGN_BOUNDING_BOX_WITH_CUBOID_MAIN_AXIS_CMD)
								{
									if (!theShape->alignCuboidBoundingBoxWithMainAxis())
										cuboidFail=true;
								}
								App::ct->undoBufferContainer->announceChangeStart(); // ************************** UNDO thingy **************************
							}
							else
								informThatPurePrimitivesWereNotChanged=true;
						}
					}
				}
				App::ct->undoBufferContainer->announceChangeEnd(); // ************************** UNDO thingy **************************
				if (informThatPurePrimitivesWereNotChanged)
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Alignment"),strTranslate(IDS_INFORM_PURE_PRIMITIVES_COULD_NOT_BE_REORIENTED),VMessageBox::OKELI);
				if (tubeFail)
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Alignment"),strTranslate(IDSN_INFORM_SHAPE_COULD_NOT_BE_REORIENTED_ALONG_TUBE),VMessageBox::OKELI);
				if (cuboidFail)
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Alignment"),strTranslate(IDSN_INFORM_SHAPE_COULD_NOT_BE_REORIENTED_ALONG_CUBOID),VMessageBox::OKELI);
			}
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==ENTER_SHAPE_EDIT_MODE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ENTER_SHAPE_EDIT_MODE_CMD")
		{
			App::mainWindow->dlgCont->evaluateMenuSelection(TOGGLE_SHAPE_EDITION_DLG_CMD);
		}
		return(true);
	}
	if (commandID==ENTER_PATH_EDIT_MODE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ENTER_PATH_EDIT_MODE_CMD")
		{
			App::mainWindow->dlgCont->evaluateMenuSelection(TOGGLE_PATH_EDITION_DLG_CMD);
		}
		return(true);
	}
	if (commandID==ENTER_BUTTON_EDIT_MODE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("ENTER_BUTTON_EDIT_MODE_CMD")
		{
			App::mainWindow->dlgCont->evaluateMenuSelection(TOGGLE_CUSTOM_UI_DLG_CMD);
		}
		return(true);
	}
	if (commandID==GROUP_SHAPES_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("GROUP_SHAPES_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			App::uiThread->addStatusLine(IDSNS_GROUPING_SELECTED_SHAPES);
			if (groupSelection(&sel,true)!=-1)
			{
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
			else
				App::uiThread->addStatusLine(IDSNS_ABORTED);
		}
		return(true);
	}
	if (commandID==UNGROUP_SHAPES_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("UNGROUP_SHAPES_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			App::uiThread->addStatusLine(IDSNS_UNGROUPING_SELECTED_SHAPES);
			ungroupSelection(&sel);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==MERGE_SHAPES_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("MERGE_SHAPES_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			App::uiThread->addStatusLine(IDSNS_MERGING_SELECTED_SHAPES);
			if (mergeSelection(&sel,true))
			{
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
			else
				App::uiThread->addStatusLine(IDSNS_ABORTED);
		}
		return(true);
	}
	if (commandID==MERGE_PATHS_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("MERGE_PATHS_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			App::uiThread->addStatusLine(IDSNS_MERGING_SELECTED_PATHS);
			mergePathSelection(&sel);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==DIVIDE_SHAPES_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("DIVIDE_SHAPES_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			App::uiThread->addStatusLine(IDSNS_DIVIDING_SELECTED_SHAPES);
			divideSelection(&sel);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	return(false);
}

int CObjectEdition::groupSelection(std::vector<int>* selection,bool showWarningMessages)
{
	if (App::ct->objCont->getShapeNumberInSelection(selection)!=int(selection->size()))
		return(-1);
	if (selection->size()<2)
		return(-1);

	// Check if some shapes are pure primitives, convex, and check if we have a heightfield:
	int pureCount=0;
	int convexCount=0;
	bool includesHeightfields=false;
	for (int i=0;i<int(selection->size());i++)
	{
		CShape* it=App::ct->objCont->getShape(selection->at(i));
		if (it!=NULL)
		{
			if (it->geomData->geomInfo->isPure())
			{
				pureCount++;
				if ( (it->geomData->geomInfo->isGeometric())&&(((CGeometric*)it->geomData->geomInfo)->getPurePrimitiveType()==sim_pure_primitive_heightfield) )
					includesHeightfields=true;
			}
			if (it->geomData->geomInfo->isConvex())
				convexCount++;
		}
	}

	bool onlyPureShapes=false;
	if ( (pureCount!=0)&&(pureCount!=int(selection->size())) )
	{ // we are mixing pure and non-pure shapes. Ask what to do
		if (showWarningMessages)
		{
			if (VMessageBox::REPLY_YES!=App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_GROUPING),strTranslate(IDS_GROUPING_PURE_AND_NON_PURE_SHAPES_PROCEED_INFO_MESSAGE),VMessageBox::YES_NO))
				return(-1); // we abort
		}
	}
	else
	{
		onlyPureShapes=true;
		if (includesHeightfields&&showWarningMessages)
		{
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDS_GROUPING_MERGING_MENU_ITEM),strTranslate(IDS_GROUPING_HEIGHTFIELDS_ERROR_MESSAGE),VMessageBox::OKELI);
			return(-1); // we abort, heightfields cannot be grouped
		}
	}

	if (!onlyPureShapes)
	{ // we have only non-pure shapes, or a mix of pure and non-pure. Make sure we have all non-pure now, and that all shapes have no attached drawing objects:
		for (int i=0;i<int(selection->size());i++)
		{
			CShape* it=App::ct->objCont->getShape(selection->at(i));
			if (it!=NULL)
			{
				it->geomData->geomInfo->setPurePrimitiveType(sim_pure_primitive_none,1.0f,1.0f,1.0f); // this will be propagated to all geometrics!
				// we have to remove all attached drawing objects (we cannot correct for that or it would be very difficult!!)
				App::ct->drawingCont->announceObjectWillBeErased(it->getID());
				App::ct->pointCloudCont->announceObjectWillBeErased(it->getID());
				App::ct->bannerCont->announceObjectWillBeErased(it->getID());
			}
		}
	}

	bool allConvex=(convexCount==int(selection->size()));

	App::ct->objCont->deselectObjects();

	CShape* lastSel=(CShape*)App::ct->objCont->getObject(selection->at(selection->size()-1));
	int materialID=lastSel->geomData->geomInfo->getDynMaterialId();

	// Let's first compute the composed mass and center of mass:
	C3Vector newCenterOfMass; // absolute
	newCenterOfMass.clear();
	float cumulMass=0.0f;
	for (int i=0;i<int(selection->size());i++)
	{
		CShape* it=App::ct->objCont->getShape(selection->at(i));
		if (it!=NULL)
		{
			newCenterOfMass+=it->getCumulativeTransformation()*it->geomData->geomInfo->getLocalInertiaFrame().X*it->geomData->geomInfo->getMass();
			cumulMass+=it->geomData->geomInfo->getMass();
		}
	}
	newCenterOfMass/=cumulMass;
	C3X3Matrix composedInertia; // relative to world
	C7Vector newInertiaFrame(C4Vector::identityRotation,newCenterOfMass);
	composedInertia.clear();

	// Now the recipient and the first item:
	CGeomWrap* theWrap=new CGeomWrap();
	theWrap->setConvex(allConvex);
	theWrap->setDynMaterialId(materialID);
	theWrap->childList.push_back(lastSel->geomData->geomInfo);
	lastSel->geomData->geomInfo->setTransformationsSinceGrouping(C7Vector::identityTransformation); // so that we can properly (i.e. like it was before) reorient the shape after ungrouping
	lastSel->geomData->geomInfo->setName(lastSel->getName());
	C7Vector tmp(newInertiaFrame.getInverse()*lastSel->getCumulativeTransformation()*lastSel->geomData->geomInfo->getLocalInertiaFrame());
	composedInertia+=CGeomWrap::getNewTensor(lastSel->geomData->geomInfo->getPrincipalMomentsOfInertia(),tmp)*lastSel->geomData->geomInfo->getMass();

//	lastSel->geomData->geomInfo->copyEnginePropertiesTo(theWrap);
	theWrap->setDynMaterialId(lastSel->geomData->geomInfo->getDynMaterialId());

	// now the other items:
	std::vector<CShape*> objectsToErase;
	for (int i=0;i<int(selection->size()-1);i++)
	{
		CShape* it=App::ct->objCont->getShape(selection->at(i));
		if (it!=NULL)
		{
			it->geomData->geomInfo->setTransformationsSinceGrouping(C7Vector::identityTransformation); // so that we can properly (i.e. like it was before) reorient the shape after ungrouping
			it->geomData->geomInfo->setName(it->getName());
			it->geomData->geomInfo->setDynMaterialId(materialID);

			tmp=newInertiaFrame.getInverse()*it->getCumulativeTransformation()*it->geomData->geomInfo->getLocalInertiaFrame();
			composedInertia+=CGeomWrap::getNewTensor(it->geomData->geomInfo->getPrincipalMomentsOfInertia(),tmp)*it->geomData->geomInfo->getMass();

			C7Vector correctionTr=lastSel->getCumulativeTransformation().getInverse()*it->getCumulativeTransformation();
			it->geomData->geomInfo->preMultiplyAllVerticeLocalFrames(correctionTr);
			theWrap->childList.push_back(it->geomData->geomInfo);
			it->geomData->geomInfo=NULL;
			objectsToErase.push_back(it); // erase it later (if we do it now, texture dependencies might get mixed up)
		}
	}
	lastSel->geomData->geomInfo=NULL;
	delete lastSel->geomData;
	CGeomProxy* proxy=new CGeomProxy(lastSel->getCumulativeTransformation(),theWrap);
	theWrap->setMass(cumulMass);
	C7Vector oldTrLocal(lastSel->getLocalTransformation());
	C7Vector newTrLocal(lastSel->getParentCumulativeTransformation().getInverse()*proxy->getCreationTransformation());
	lastSel->setLocalTransformation(newTrLocal);
	proxy->setCreationTransformation(C7Vector::identityTransformation);
	lastSel->geomData=proxy;

	// Set the composed inertia:
	composedInertia/=cumulMass; // remember, we only normally work with massless inertias!
	C7Vector newAbsOfComposedInertia;
	newAbsOfComposedInertia.X=newCenterOfMass;
	C3Vector principalMoments;
	CGeomWrap::findPrincipalMomentOfInertia(composedInertia,newAbsOfComposedInertia.Q,principalMoments);
	lastSel->geomData->geomInfo->setLocalInertiaFrame(lastSel->getCumulativeTransformation().getInverse()*newAbsOfComposedInertia);
	lastSel->geomData->geomInfo->setPrincipalMomentsOfInertia(principalMoments);

	// correct the pos/orient. of all children of the 'lastSel' shape:
	for (int i=0;i<int(lastSel->childList.size());i++)
		lastSel->childList[i]->setLocalTransformation(newTrLocal.getInverse()*oldTrLocal*lastSel->childList[i]->getLocalTransformationPart1());

	lastSel->actualizeContainsTransparentComponent();

	App::ct->textureCont->updateAllDependencies();

	for (int i=0;i<int(objectsToErase.size());i++)
		App::ct->objCont->eraseObject(objectsToErase[i]);

	App::ct->objCont->selectObject(lastSel->getID());
	return(lastSel->getID());

}

void CObjectEdition::ungroupSelection(std::vector<int>* selection)
{
	App::ct->objCont->deselectObjects();
	std::vector<int> finalSel;
	for (int i=0;i<int(selection->size());i++)
	{
		CShape* it=App::ct->objCont->getShape(selection->at(i));
		if (it!=NULL)
		{
			if (it->isGrouping())
			{
				// Following 2 lines not needed, but added because a previous bug might have done something wrong! So here we make sure that all elements of the multishape are non-pure!!!
				if (!it->geomData->geomInfo->isPure())
					it->geomData->geomInfo->setPurePrimitiveType(sim_pure_primitive_none,1.0f,1.0f,1.0f);

				// we have to remove all attached drawing objects (we cannot correct for that or it would be very difficult!!)
				App::ct->drawingCont->announceObjectWillBeErased(it->getID());
				App::ct->pointCloudCont->announceObjectWillBeErased(it->getID());
				App::ct->bannerCont->announceObjectWillBeErased(it->getID());
			
				CGeomWrap* oldGeomInfo=it->geomData->geomInfo;
				int materialID=oldGeomInfo->getDynMaterialId();
				it->geomData->geomInfo=NULL;
				C7Vector itCumulTransf(it->getCumulativeTransformation());
				for (int i=int(oldGeomInfo->childList.size())-1;i>=0;i--)
				{
					if (i==0)
					{ // the first element in the list keeps its original shape
						C7Vector itOldLocal=it->getLocalTransformation();
						CGeomProxy* newGeomProxy=new CGeomProxy(itCumulTransf,oldGeomInfo->childList[i]);
						delete it->geomData;
						C7Vector itNewLocal(it->getParentCumulativeTransformation().getInverse()*newGeomProxy->getCreationTransformation());
						it->setLocalTransformation(itNewLocal);
						newGeomProxy->setCreationTransformation(C7Vector::identityTransformation);
						it->geomData=newGeomProxy;
						it->geomData->geomInfo->setDynMaterialId(materialID);
						finalSel.push_back(it->getID());
						it->actualizeContainsTransparentComponent();
						// Now correct for all attached chil objects:
						for (int j=0;j<int(it->childList.size());j++)
						{
							C3DObject* aChild=it->childList[j];
							C7Vector oldChild=aChild->getLocalTransformationPart1();
							aChild->setLocalTransformation(itNewLocal.getInverse()*itOldLocal*oldChild);
						}

						// Correctly reorient the shape to what we had before grouping (important for inertia frames that are relative to the shape's frame):
						C7Vector tr(oldGeomInfo->childList[i]->getTransformationsSinceGrouping());
						C7Vector currentLocal=it->getLocalTransformation();
						C7Vector tempLocal=it->getParentCumulativeTransformation().getInverse()*tr.getInverse();
						it->setLocalTransformation(tempLocal);
						it->alignBoundingBoxWithWorld();
						it->setLocalTransformation(currentLocal*tempLocal.getInverse()*it->getLocalTransformation());
					}
					else
					{ // the other elements in the list will receive a new shape
						CGeomProxy* newGeomProxy=new CGeomProxy(itCumulTransf,oldGeomInfo->childList[i]);
						CShape* newIt=new CShape();
						newIt->setLocalTransformation(newGeomProxy->getCreationTransformation());
						newGeomProxy->setCreationTransformation(C7Vector::identityTransformation);
						newIt->geomData=newGeomProxy;
						newIt->setName(oldGeomInfo->childList[i]->getName());
						newIt->geomData->geomInfo->setDynMaterialId(materialID);
						App::ct->objCont->addObjectToScene(newIt,false);
						// Now a few properties/things we want to be same for the new shape:
						App::ct->objCont->makeObjectChildOf(newIt,it->getParent());
						newIt->setSizeFactor(it->getSizeFactor());
						newIt->setObjectProperty(it->getObjectProperty());
						newIt->setMainPropertyLocal(it->getMainPropertyLocal());
						newIt->layer=it->layer;
						newIt->setShapeIsDynamicallyStatic(it->getShapeIsDynamicallyStatic());
						newIt->setRespondable(it->getRespondable());
						newIt->setDynamicCollisionMask(it->getDynamicCollisionMask());
						finalSel.push_back(newIt->getID());
						newIt->actualizeContainsTransparentComponent();

						// Correctly reorient the shape to what we had before grouping (important for inertia frames that are relative to the shape's frame):
						C7Vector tr(oldGeomInfo->childList[i]->getTransformationsSinceGrouping());
						C7Vector currentLocal=newIt->getLocalTransformation();
						C7Vector tempLocal=newIt->getParentCumulativeTransformation().getInverse()*tr.getInverse();

						newIt->setLocalTransformation(tempLocal);
						newIt->alignBoundingBoxWithWorld();
						newIt->setLocalTransformation(currentLocal*tempLocal.getInverse()*newIt->getLocalTransformation());
					}
				}
				oldGeomInfo->childList.clear();
				delete oldGeomInfo;
			}
		}
	}

	App::ct->textureCont->updateAllDependencies();

	selection->clear();
	for (int i=0;i<int(finalSel.size());i++)
	{
		App::ct->objCont->addObjectToSelection(finalSel[i]);
		selection->push_back(finalSel[i]);
	}
}

bool CObjectEdition::mergeSelection(std::vector<int>* selection,bool showWarningMessages)
{
	if (selection->size()<2)
		return(false);

	// Check if some shapes are pure primitives:
	if (showWarningMessages)
	{
		for (int i=0;i<int(selection->size());i++)
		{
			CShape* it=App::ct->objCont->getShape(selection->at(i));
			if (it!=NULL)
			{
				if (it->geomData->geomInfo->isPure())
				{
					if (VMessageBox::REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_MERGING),strTranslate(IDS_MERGING_SOME_PURE_SHAPES_PROCEED_INFO_MESSAGE),VMessageBox::YES_NO))
						break;
					return(false); // we abort
				}
			}
		}
	}

	// Check if some shapes contain textures:
	bool textureWarningOutput=false;
	for (int i=0;i<int(selection->size());i++)
	{
		CShape* it=App::ct->objCont->getShape(selection->at(i));
		if (it!=NULL)
		{
			if (it->geomData->geomInfo->getTextureCount()!=0)
			{
				if (showWarningMessages)
				{
					if ( (!textureWarningOutput)&&(VMessageBox::REPLY_NO==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_MERGING),strTranslate(IDS_MERGING_OR_DIVIDING_REMOVES_TEXTURES_PROCEED_INFO_MESSAGE),VMessageBox::YES_NO)) )
						return(false); // we abort
				}
				textureWarningOutput=true;
				// Now remove the textures:
				App::ct->textureCont->announceGeneralObjectWillBeErased(it->getID(),-1);
				it->geomData->geomInfo->removeAllTextures();
			}
		}
	}

	// We have to decompose completely all groups first
	// First isolate the simple shape that should hold all other shapes:
	CShape* lastSel=App::ct->objCont->getShape(selection->at(selection->size()-1));
	selection->pop_back();
	std::vector<int> augmentedSelection(*selection);
	while (lastSel->isGrouping())
	{
		std::vector<int> sel;
		sel.push_back(lastSel->getID());
		ungroupSelection(&sel);
		for (int j=0;j<int(sel.size()-1);j++)
			augmentedSelection.push_back(sel[j]);
		lastSel=App::ct->objCont->getShape(sel[sel.size()-1]);
	}
	lastSel->geomData->geomInfo->setPurePrimitiveType(sim_pure_primitive_none,1.0f,1.0f,1.0f);

	// Now decompose all other shapes:
	std::vector<CShape*> simpleShapes;
	for (int i=0;i<int(augmentedSelection.size());i++)
	{
		CShape* it=App::ct->objCont->getShape(augmentedSelection[i]);
		if (it->isGrouping())
		{ // We have to decompose this group:
			std::vector<int> sel;
			sel.push_back(it->getID());
			ungroupSelection(&sel);
			for (int j=0;j<int(sel.size());j++)
				augmentedSelection.push_back(sel[j]);
		}
		else
		{
			it->geomData->geomInfo->setPurePrimitiveType(sim_pure_primitive_none,1.0f,1.0f,1.0f);
			simpleShapes.push_back(it);
		}
	}
	App::ct->objCont->deselectObjects();

	if (simpleShapes.size()>0)
	{
		// we have to remove all attached drawing objects (we cannot correct for that or it would be very difficult!!)
		App::ct->drawingCont->announceObjectWillBeErased(lastSel->getID());
		App::ct->pointCloudCont->announceObjectWillBeErased(lastSel->getID());
		App::ct->bannerCont->announceObjectWillBeErased(lastSel->getID());

		std::vector<float> wvert;
		std::vector<int> wind;
		lastSel->geomData->geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
		float cumulMass=lastSel->geomData->geomInfo->getMass();
		C7Vector tr(lastSel->getCumulativeTransformation());
		for (int i=0;i<int(wvert.size())/3;i++)
		{
			C3Vector v(&wvert[3*i+0]);
			v*=tr;
			wvert[3*i+0]=v(0);
			wvert[3*i+1]=v(1);
			wvert[3*i+2]=v(2);
		}
		int voff=wvert.size();
		for (int j=0;j<int(simpleShapes.size());j++)
		{
			CShape* aShape=simpleShapes[j];
			aShape->geomData->geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
			cumulMass+=aShape->geomData->geomInfo->getMass();
			tr=aShape->getCumulativeTransformation();
			for (int i=voff/3;i<int(wvert.size())/3;i++)
			{
				C3Vector v(&wvert[3*i+0]);
				v*=tr;
				wvert[3*i+0]=v(0);
				wvert[3*i+1]=v(1);
				wvert[3*i+2]=v(2);
			}
			App::ct->objCont->eraseObject(aShape);
			voff=wvert.size();
		}
		// We now have in wvert and wind all the vertices and indices (absolute vertices)

		CGeomProxy* proxy=new CGeomProxy(NULL,wvert,wind,NULL,NULL);
		C7Vector lastSelPreviousLocal(lastSel->getLocalTransformation());
		C7Vector lastSelCurrentLocal(lastSel->getParentCumulativeTransformation().getInverse()*proxy->getCreationTransformation());
		lastSel->setLocalTransformation(lastSelCurrentLocal);
		proxy->setCreationTransformation(C7Vector::identityTransformation);
		// Copy the CGeometric properties (not all):

		((CGeometric*)lastSel->geomData->geomInfo)->copyVisualAttributesTo(((CGeometric*)proxy->geomInfo));
		((CGeometric*)proxy->geomInfo)->actualizeGouraudShadingAndVisibleEdges(); // since 21/3/2014

		// Copy the CGeomWrap properties (not all):
		proxy->geomInfo->setConvex(false); // not really needed here, already set to false normally
		proxy->geomInfo->setMass(cumulMass); // we do not copy here!

//		lastSel->geomData->geomInfo->copyEnginePropertiesTo(proxy->geomInfo);
		proxy->geomInfo->setDynMaterialId(lastSel->geomData->geomInfo->getDynMaterialId());

		// Do not copy following:
		//		proxy->geomInfo->setPrincipalMomentsOfInertia(lastSel->geomData->geomInfo->getPrincipalMomentsOfInertia());
		proxy->geomInfo->setLocalInertiaFrame(C7Vector::identityTransformation); // to have the inertia frame centered in the geometric middle of the mesh!

		// Delete the old structure and connect the new one:
		delete lastSel->geomData;
		lastSel->geomData=proxy;

		// Adjust the transformation of all its children:
		for (int i=0;i<int(lastSel->childList.size());i++)
			lastSel->childList[i]->setLocalTransformation(lastSelCurrentLocal.getInverse()*lastSelPreviousLocal*lastSel->childList[i]->getLocalTransformation());

		// Finally select the merged object:
		App::ct->objCont->selectObject(lastSel->getID());
	}
	return(true);
}

void CObjectEdition::divideSelection(std::vector<int>* selection)
{
	if (selection->size()<1)
		return;

	// Check if some shapes contain textures:
	bool textureWarningOutput=false;
	for (int i=0;i<int(selection->size());i++)
	{
		CShape* it=App::ct->objCont->getShape(selection->at(i));
		if (it!=NULL)
		{
			if (it->geomData->geomInfo->getTextureCount()!=0)
			{
				if ( (!textureWarningOutput)&&(VMessageBox::REPLY_NO==App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Dividing"),strTranslate(IDS_MERGING_OR_DIVIDING_REMOVES_TEXTURES_PROCEED_INFO_MESSAGE),VMessageBox::YES_NO)) )
					return; // we abort
				textureWarningOutput=true;
				// Now remove the textures:
				App::ct->textureCont->announceGeneralObjectWillBeErased(it->getID(),-1);
				it->geomData->geomInfo->removeAllTextures();
			}
		}
	}

	// We have to decompose completely all groups first
	std::vector<int> augmentedSelection(*selection);
	std::vector<CShape*> simpleShapes;
	for (int i=0;i<int(augmentedSelection.size());i++)
	{
		CShape* it=App::ct->objCont->getShape(augmentedSelection[i]);
		if (it->isGrouping())
		{ // We have to decompose this group:
			std::vector<int> sel;
			sel.push_back(it->getID());
			ungroupSelection(&sel);
			for (int j=0;j<int(sel.size());j++)
				augmentedSelection.push_back(sel[j]);
		}
		else
		{
			if (!it->geomData->geomInfo->isPure())
				simpleShapes.push_back(it); // pure simple shapes can anyway not be divided!
		}
	}
	App::ct->objCont->deselectObjects();
	selection->clear();

	for (int i=0;i<int(simpleShapes.size());i++)
	{
		CShape* it=simpleShapes[i];

		std::vector<float> wvert;
		std::vector<int> wind;
		it->geomData->geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
		int extractedCount=0;
		while (true)
		{
			std::vector<float> subvert;
			std::vector<int> subind;
			if (CMeshManip::extractOneShape(&wvert,&wind,&subvert,&subind))
			{ // There are more parts to extract
				extractedCount++;
				C7Vector tmpTr(it->getCumulativeTransformation());
				CGeomProxy* newGeomProxy=new CGeomProxy(&tmpTr,subvert,subind,NULL,NULL);
				CShape* newIt=new CShape();
				newIt->setLocalTransformation(newGeomProxy->getCreationTransformation());
				newGeomProxy->setCreationTransformation(C7Vector::identityTransformation);
				newIt->geomData=newGeomProxy;

				std::string name(it->getName()+"_sub");
				tt::removeIllegalCharacters(name,false);
				while (App::ct->objCont->getObject(name.c_str())!=NULL)
					name=tt::generateNewName_noDash(name);
				newIt->setName(name);
				App::ct->objCont->addObjectToScene(newIt,false);
				// Now a few properties/things we want to be same for the new shape:
				App::ct->objCont->makeObjectChildOf(newIt,it->getParent());
				newIt->setSizeFactor(it->getSizeFactor());
				newIt->setObjectProperty(it->getObjectProperty());
				newIt->setMainPropertyLocal(it->getMainPropertyLocal());
				newIt->layer=it->layer;
				// Do not copy following:
				//				newIt->setShapeIsDynamicallyStatic(it->getShapeIsDynamicallyStatic());
				//				newIt->setRespondable(it->getRespondable());
				//				newIt->setDynamicCollisionMask(it->getDynamicCollisionMask());

				// Copy the CGeometric properties (not all):
				((CGeometric*)it->geomData->geomInfo)->copyVisualAttributesTo(((CGeometric*)newIt->geomData->geomInfo));
				((CGeometric*)newIt->geomData->geomInfo)->actualizeGouraudShadingAndVisibleEdges(); // since 21/3/2014

				// Copy the CGeomWrap properties (not all):
//				it->geomData->geomInfo->copyEnginePropertiesTo(newIt->geomData->geomInfo);
				newIt->geomData->geomInfo->setDynMaterialId(it->geomData->geomInfo->getDynMaterialId());

				// Do not copy following:
				//		newIt->geomData->geomInfo->setPrincipalMomentsOfInertia(it->geomData->geomInfo->getPrincipalMomentsOfInertia());
				newIt->geomData->geomInfo->setLocalInertiaFrame(C7Vector::identityTransformation); // to have the inertia frame centered in the geometric middle of the mesh!

				newIt->actualizeContainsTransparentComponent();
				selection->push_back(newIt->getID());
			}
			else
			{ // This was the last part
				if (extractedCount==0)
					break; // we couldn't extract anything!
				C7Vector tmpTr(it->getCumulativeTransformation());
				CGeomProxy* newGeomProxy=new CGeomProxy(&tmpTr,subvert,subind,NULL,NULL);
				C7Vector itLocalOld(it->getLocalTransformation());
//	Was			C7Vector itLocalNew(newGeomProxy->getCreationTransformation()); and corrected on 18/4/2013 to:
				C7Vector itLocalNew(it->getParentCumulativeTransformation().getInverse()*newGeomProxy->getCreationTransformation());
				it->setLocalTransformation(itLocalNew);
				newGeomProxy->setCreationTransformation(C7Vector::identityTransformation);

				// Copy the CGeometric properties (not all):
				((CGeometric*)it->geomData->geomInfo)->copyVisualAttributesTo(((CGeometric*)newGeomProxy->geomInfo));
				((CGeometric*)newGeomProxy->geomInfo)->actualizeGouraudShadingAndVisibleEdges(); // since 21/3/2014

				// Copy the CGeomWrap properties (not all):
//				it->geomData->geomInfo->copyEnginePropertiesTo(newGeomProxy->geomInfo);
				newGeomProxy->geomInfo->setDynMaterialId(it->geomData->geomInfo->getDynMaterialId());

				// Do not copy following:
				//		newIt->geomData->geomInfo->setPrincipalMomentsOfInertia(it->geomData->geomInfo->getPrincipalMomentsOfInertia());
				newGeomProxy->geomInfo->setLocalInertiaFrame(C7Vector::identityTransformation); // to have the inertia frame centered in the geometric middle of the mesh!
				//		newIt->geomData->geomInfo->setLocalInertiaFrame(bla);

				delete it->geomData;
				it->geomData=newGeomProxy;
				it->actualizeContainsTransparentComponent();
				
				// Now we have to adjust all the children:
				for (int j=0;j<int(it->childList.size());j++)
					it->childList[j]->setLocalTransformation(itLocalNew.getInverse()*itLocalOld*it->childList[j]->getLocalTransformationPart1());

				selection->push_back(it->getID());
				break;
			}
		}
	}

	// We select extracted shapes:
	for (int i=0;i<int(selection->size());i++)
		App::ct->objCont->addObjectToSelection(selection->at(i));
}

void CObjectEdition::mergePathSelection(std::vector<int>* selection)
{
	App::ct->objCont->deselectObjects();
	if (selection->size()<2)
		return;
	C3DObject* lastObj=App::ct->objCont->getLastSelection(selection);
	if (lastObj->getObjectType()==sim_object_path_type)
	{
		CPath* last=(CPath*)lastObj;
		C7Vector lastTrInv(last->getCumulativeTransformation().getInverse());
		last->pathContainer->enableActualization(false);
		for (int i=0;i<int(selection->size())-1;i++)
		{
			C3DObject* obj=App::ct->objCont->getObject((*selection)[i]);
			if (obj->getObjectType()==sim_object_path_type)
			{
				CPath* it=(CPath*)obj;
				C7Vector itTr(it->getCumulativeTransformation());
				for (int j=0;j<it->pathContainer->getSimplePathPointCount();j++)
				{
					CSimplePathPoint* itCopy(it->pathContainer->getSimplePathPoint(j)->copyYourself());
					C7Vector confRel(itCopy->getTransformation());
					last->pathContainer->addSimplePathPoint(itCopy);
					itCopy->setTransformation(lastTrInv*itTr*confRel,it->pathContainer->getAttributes());
				}
				// Now remove that path:
				App::ct->objCont->eraseObject(obj);
			}		
		}
		last->pathContainer->enableActualization(true);
		last->pathContainer->actualizePath();
	}
}


void CObjectEdition::scaleObjects(const std::vector<int>& selection,float scalingFactor,bool scalePositionsToo)
{
	std::vector<int> sel(selection);
	CObjectEdition::addRootObjectChildrenToSelection(sel);
	for (int i=0;i<int(sel.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(sel[i]);
		if (scalePositionsToo)
			it->scalePosition(scalingFactor);
		else
		{ // If one parent is a root object (model base) and in this selection, then we also scale the position here!! (2009/06/10)
			C3DObject* itp=it->getParent();
			while (itp!=NULL)
			{
				if (itp->getModelBase())
				{ // We found a parent that is a root! Is it in the selection?
					bool f=false;
					for (int j=0;j<int(sel.size());j++)
					{
						if (sel[j]==itp->getID())
						{ // YEs!
							f=true;
							break;
						}
					}
					if (f)
					{ // We also scale the pos here!!
						it->scalePosition(scalingFactor);
						break;
					}
				}
				itp=itp->getParent();
			}
		}
		it->scaleObject(scalingFactor);
	}

	// Now we might have to scale a few ikElements/ikGroups:
	for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
	{
		CikGroup* ikGroup=App::ct->ikGroups->ikGroups[i];
		bool scaleIkGroup=true;
		// Go through all ikElement lists:
		for (int j=0;j<int(ikGroup->ikElements.size());j++)
		{
			CikEl* ikEl=ikGroup->ikElements[j];
			CDummy* tip=App::ct->objCont->getDummy(ikEl->getTooltip());
			bool scaleElement=false;
			if (tip!=NULL)
			{ // was this tip scaled?
				bool tipFound=false;
				for (int k=0;k<int(sel.size());k++)
				{
					if (sel[k]==tip->getID())
					{
						tipFound=true;
						break;
					}
				}
				if (tipFound)
				{ // yes, tip was found!
					scaleElement=true;
				}
			}
			if (scaleElement)
				ikEl->setMinLinearPrecision(ikEl->getMinLinearPrecision()*scalingFactor); // we scale that ikElement!
			else
				scaleIkGroup=false; // not all ik elements are scaled --> we do not scale the ik goup!
		}
		if (scaleIkGroup)
		{ // we scale that ikGroup!
			ikGroup->setJointTreshholdLinear(ikGroup->getJointTreshholdLinear()*scalingFactor);
			ikGroup->setAvoidanceThreshold(ikGroup->getAvoidanceThreshold()*scalingFactor);
		}
	}

	App::ct->objCont->setFullDialogRefreshFlag();
}

int CObjectEdition::generateConvexDecomposed(int shapeHandle,size_t nClusters,double maxConcavity,bool addExtraDistPoints,bool addFacesPoints,double maxConnectDist,size_t maxTrianglesInDecimatedMesh,size_t maxHullVertices,double smallClusterThreshold,bool individuallyConsiderMultishapeComponents,bool randomColors,int maxIterations)
{
	FUNCTION_DEBUG;
	std::vector<float> vert;
	std::vector<int> ind;
	CShape* it=App::ct->objCont->getShape(shapeHandle);
	if (it!=NULL)
	{
		C7Vector tr(it->getCumulativeTransformation());
		std::vector<int> generatedShapeHandles;
		if (individuallyConsiderMultishapeComponents&&(!it->geomData->geomInfo->isGeometric()))
		{
			std::vector<CGeometric*> shapeComponents;
			it->geomData->geomInfo->getAllShapeComponentsCumulative(shapeComponents);
			for (int comp=0;comp<int(shapeComponents.size());comp++)
			{
				CGeometric* geom=shapeComponents[comp];
				vert.clear();
				ind.clear();
				geom->getCumulativeMeshes(vert,&ind,NULL);
				for (int j=0;j<int(vert.size()/3);j++)
				{
					C3Vector v(&vert[3*j+0]);
					v=tr*v;
					vert[3*j+0]=v(0);
					vert[3*j+1]=v(1);
					vert[3*j+2]=v(2);
				}
				std::vector<std::vector<float>*> outputVert;
				std::vector<std::vector<int>*> outputInd;
				int addClusters=0;
				for (int tryNumber=0;tryNumber<maxIterations;tryNumber++)
				{ // the convex decomposition routine sometimes fails producing good convectivity (i.e. there are slightly non-convex items that V-REP doesn't want to recognize as convex)
					// For those situations, we try several times to convex decompose:
					outputVert.clear();
					outputInd.clear();
					std::vector<int> _tempHandles;
					CMeshRoutines::convexDecompose(&vert[0],vert.size(),&ind[0],ind.size(),outputVert,outputInd,nClusters,maxConcavity,addExtraDistPoints,addFacesPoints,maxConnectDist,maxTrianglesInDecimatedMesh,maxHullVertices,smallClusterThreshold);
					int convexRecognizedCount=0;
					for (int i=0;i<int(outputVert.size());i++)
					{
						int handle=simCreateMeshShape_internal(2,20.0f*piValue/180.0f,&outputVert[i]->at(0),outputVert[i]->size(),&outputInd[i]->at(0),outputInd[i]->size(),NULL);
						CShape* shape=App::ct->objCont->getShape(handle);
						if (shape!=NULL)
						{
							// Following flag is automatically set upon shape creation. Also, it seems that the convex decomposition algo sometimes failes..
							// ((CGeometric*)shape->geomData->geomInfo)->setConvex(true);
							if (((CGeometric*)shape->geomData->geomInfo)->isConvex())
								convexRecognizedCount++; // V-REP convex test is more strict than what the convex decomp. algo does
							_tempHandles.push_back(handle);
							if (!randomColors)
								geom->copyVisualAttributesTo(((CGeometric*)shape->geomData->geomInfo));
						}
						delete outputVert[i];
						delete outputInd[i];
					}
					// we check if all shapes are recognized as convex shapes by V-REP
					if ( (convexRecognizedCount==int(outputVert.size())) || (tryNumber>=maxIterations-1) )
					{
						for (int i=0;i<int(_tempHandles.size());i++)
							generatedShapeHandles.push_back(_tempHandles[i]);
						break;
					}
					else
					{ // No! Some shapes have a too large non-convexity. We take all generated shapes, and use them to generate new convex shapes:
						vert.clear();
						ind.clear();
						for (int i=0;i<int(_tempHandles.size());i++)
						{
							CShape* as=App::ct->objCont->getShape(_tempHandles[i]);
							if (as!=NULL)
							{
								C7Vector tr2(as->getCumulativeTransformation());
								CGeometric* geom=(CGeometric*)as->geomData->geomInfo;
								int offset=vert.size()/3;
								geom->getCumulativeMeshes(vert,&ind,NULL);
								for (int j=offset;j<int(vert.size()/3);j++)
								{
									C3Vector v(&vert[3*j+0]);
									v=tr2*v;
									vert[3*j+0]=v(0);
									vert[3*j+1]=v(1);
									vert[3*j+2]=v(2);
								}
							}
							simRemoveObject_internal(_tempHandles[i]);
						}
						// We adjust some parameters a bit, in order to obtain a better convexity for all shapes:
						addClusters+=2;
						nClusters=addClusters+int(_tempHandles.size());
					}
				}
			}
		}
		else
		{
			it->geomData->geomInfo->getCumulativeMeshes(vert,&ind,NULL);
			for (int j=0;j<int(vert.size()/3);j++)
			{
				C3Vector v(&vert[3*j+0]);
				v=tr*v;
				vert[3*j+0]=v(0);
				vert[3*j+1]=v(1);
				vert[3*j+2]=v(2);
			}
			std::vector<std::vector<float>*> outputVert;
			std::vector<std::vector<int>*> outputInd;

			int addClusters=0;
			for (int tryNumber=0;tryNumber<maxIterations;tryNumber++)
			{ // the convex decomposition routine sometimes fails producing good convectivity (i.e. there are slightly non-convex items that V-REP doesn't want to recognize as convex)
				// For those situations, we try several times to convex decompose:
				outputVert.clear();
				outputInd.clear();
				std::vector<int> _tempHandles;
				CMeshRoutines::convexDecompose(&vert[0],vert.size(),&ind[0],ind.size(),outputVert,outputInd,nClusters,maxConcavity,addExtraDistPoints,addFacesPoints,maxConnectDist,maxTrianglesInDecimatedMesh,maxHullVertices,smallClusterThreshold);
				int convexRecognizedCount=0;
				for (int i=0;i<int(outputVert.size());i++)
				{
					int handle=simCreateMeshShape_internal(2,20.0f*piValue/180.0f,&outputVert[i]->at(0),outputVert[i]->size(),&outputInd[i]->at(0),outputInd[i]->size(),NULL);
					CShape* shape=App::ct->objCont->getShape(handle);
					if (shape!=NULL)
					{
						// Following flag is automatically set upon shape creation. Also, it seems that the convex decomposition algo sometimes failes..
						// ((CGeometric*)shape->geomData->geomInfo)->setConvex(true);
						if (((CGeometric*)shape->geomData->geomInfo)->isConvex())
							convexRecognizedCount++; // V-REP convex test is more strict than what the convex decomp. algo does
						_tempHandles.push_back(handle);
						if (it->geomData->geomInfo->isGeometric()&&(!randomColors))
						{ // We do this only if the original shape isn't grouped
							((CGeometric*)it->geomData->geomInfo)->copyVisualAttributesTo(((CGeometric*)shape->geomData->geomInfo));
						}
					}
					delete outputVert[i];
					delete outputInd[i];
				}
				// we check if all shapes are recognized as convex shapes by V-REP
				if ( (convexRecognizedCount==int(outputVert.size())) || (tryNumber>=maxIterations-1) )
				{
					for (int i=0;i<int(_tempHandles.size());i++)
						generatedShapeHandles.push_back(_tempHandles[i]);
					break;
				}
				else
				{ // No! Some shapes have a too large non-convexity. We take all generated shapes, and use them to generate new convex shapes:
					vert.clear();
					ind.clear();
					for (int i=0;i<int(_tempHandles.size());i++)
					{
						CShape* as=App::ct->objCont->getShape(_tempHandles[i]);
						if (as!=NULL)
						{
							C7Vector tr2(as->getCumulativeTransformation());
							CGeometric* geom=(CGeometric*)as->geomData->geomInfo;
							int offset=vert.size()/3;
							geom->getCumulativeMeshes(vert,&ind,NULL);
							for (int j=offset;j<int(vert.size()/3);j++)
							{
								C3Vector v(&vert[3*j+0]);
								v=tr2*v;
								vert[3*j+0]=v(0);
								vert[3*j+1]=v(1);
								vert[3*j+2]=v(2);
							}
						}
						simRemoveObject_internal(_tempHandles[i]);
					}
					// We adjust some parameters a bit, in order to obtain a better convexity for all shapes:
					addClusters+=2;
					nClusters=addClusters+int(_tempHandles.size());
				}
			}
		}


		int newShapeHandle=-1;
		if (generatedShapeHandles.size()==1)
			newShapeHandle=generatedShapeHandles[0];
		if (generatedShapeHandles.size()>1)
			newShapeHandle=simGroupShapes_internal(&generatedShapeHandles[0],generatedShapeHandles.size()); // we have to group them first
		return(newShapeHandle);
	}
	return(-1);
}
