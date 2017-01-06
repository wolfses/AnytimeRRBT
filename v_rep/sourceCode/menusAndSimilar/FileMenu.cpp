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
#include "FileMenu.h"
#include "OGL.h"
#include "Simulation.h"
#include "OglSurface.h"
#include "IloIlo.h"
#include "Tt.h"
#include "DxfFile.h"
#include "ObjFile.h"
#include "StlFile.h"
#include "persistentDataContainer.h"
#include "ObjectEdition.h"
#include "algos.h"
#include "App.h"
#include "pluginContainer.h"
#include "MeshManip.h"
#include "geometric.h"
#include "v_repStrings.h"
#include "VFileDialog.h"
#include <boost/lexical_cast.hpp>
#include "imgLoader.h"
#include "VVarious.h"
#include "VDateTime.h"
#include "ttUtil.h"
#include "qdlgmodelthumbnailvisu.h"
#include "qdlgmodelthumbnail.h"
#include "qdlgheightfielddimension.h"
#include "qdlgimportoptions.h"
#include "qdlgmessageandcheckbox.h"
#include "qdlgtextureloadoptions.h"
#include "v_repLib.h"


void CFileMenu::addMenu(VMenu* menu)
{
	bool fileOpOk=(App::ct->simulation->isSimulationStopped())&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE);
	bool fileOpOkAlsoDuringSimulation=(App::ct->objCont->getEditModeType()==NO_EDIT_MODE);
	int selItems=App::ct->objCont->getSelSize();
	bool justModelSelected=false;
	if (selItems==1)
	{
		C3DObject* obj=App::ct->objCont->getObject(App::ct->objCont->getSelID(0));
		justModelSelected=(obj!=NULL)&&(obj->getModelBase());
	}
	std::vector<int> sel;
	sel.reserve(App::ct->objCont->getSelSize());
	for (int i=0;i<App::ct->objCont->getSelSize();i++)
		sel.push_back(App::ct->objCont->getSelID(i));
	CObjectEdition::addRootObjectChildrenToSelection(sel);
	int shapeNumber=App::ct->objCont->getShapeNumberInSelection(&sel);
	int pathNumber=App::ct->objCont->getPathNumberInSelection(&sel);
	int graphNumber=App::ct->objCont->getGraphNumberInSelection(&sel);

	bool uiSaveOk=false;
	for (int i=0;i<int(App::ct->buttonBlockContainer->allBlocks.size());i++)
	{
		if ((App::ct->buttonBlockContainer->allBlocks[i]->getAttributes()&sim_ui_property_systemblock)==0)
		{
			uiSaveOk=true;
			break;
		}
	}

	VMenubar::appendMenuItem(menu,fileOpOk,false,NEW_SCENE_CMD,IDS_NEW_SCENE_MENU_ITEM);
	VMenubar::appendMenuItem(menu,fileOpOk,false,OPEN_SCENE_CMD,IDS_OPEN_SCENE___MENU_ITEM);

	// recent scene files:
	CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
	std::string recentScenes[10];
	int recentScenesCnt=0;
	for (int i=0;i<10;i++)
	{
		std::string tmp("SIMSETTINGS_RECENTSCENE0");
		tmp[23]=48+i;
		cont.readData(tmp.c_str(),recentScenes[i]);
		if (recentScenes[i].length()>3)
			recentScenesCnt++;
	}
	VMenu* recentSceneMenu=VMenubar::createPopupMenu();
	for (int i=0;i<10;i++)
	{
		if (recentScenes[i].length()>3)
			VMenubar::appendMenuItem(recentSceneMenu,fileOpOk,false,OPEN_RECENT_SCENE0_CMD+i,VVarious::splitPath_fileBaseAndExtension(recentScenes[i]).c_str());
	}
	VMenubar::appendMenuAndDetach(menu,recentSceneMenu,(recentScenesCnt>0)&&fileOpOk,IDS_OPEN_RECENT_SCENE_MENU_ITEM);

	VMenubar::appendMenuItem(menu,fileOpOkAlsoDuringSimulation,false,LOAD_MODEL_CMD,IDS_LOAD_MODEL___MENU_ITEM);
	VMenubar::appendMenuItem(menu,fileOpOk,false,LOAD_UI_CMD,IDS_LOAD_UI___MENU_ITEM);
	VMenubar::appendMenuSeparator(menu);

	VMenubar::appendMenuItem(menu,fileOpOk,false,CLOSE_SCENE_CMD,IDS_CLOSE_SCENE_MENU_ITEM);
	VMenubar::appendMenuSeparator(menu);

	VMenubar::appendMenuItem(menu,fileOpOk,false,SAVE_SCENE_CMD,IDS_SAVE_SCENE_MENU_ITEM);
	VMenubar::appendMenuItem(menu,fileOpOk,false,SAVE_SCENE_AS_CMD,IDS_SAVE_SCENE_AS___MENU_ITEM);
	VMenubar::appendMenuItem(menu,fileOpOk&&justModelSelected,false,SAVE_MODEL_CMD,IDS_SAVE_MODEL_AS___MENU_ITEM);
	VMenubar::appendMenuItem(menu,fileOpOk&&uiSaveOk,false,SAVE_UI_CMD,IDS_SAVE_UI_AS___MENU_ITEM);
	VMenubar::appendMenuSeparator(menu);


	VMenu* impMenu=VMenubar::createPopupMenu();
	VMenubar::appendMenuItem(impMenu,fileOpOk,false,IMPORT_MESH_CMD,IDS_IMPORT_MESH___MENU_ITEM);
	VMenubar::appendMenuItem(impMenu,fileOpOk,false,IMPORT_PATH_CMD,IDS_IMPORT_PATH___MENU_ITEM);
	VMenubar::appendMenuItem(impMenu,fileOpOk,false,IMPORT_HEIGHTFIELD_CMD,std::string(IDSN_IMPORT_HEIGHTFIELD)+"...");
	VMenubar::appendMenuAndDetach(menu,impMenu,true,IDSN_IMPORT_MENU_ITEM);

	VMenu* expMenu=VMenubar::createPopupMenu();
	VMenubar::appendMenuItem(expMenu,fileOpOk&&(shapeNumber>0),false,EXPORT_SHAPE_CMD,IDS_EXPORT_SHAPE_MENU_ITEM);
	VMenubar::appendMenuItem(expMenu,fileOpOk&&(graphNumber!=0),false,EXPORT_GRAPHS_CMD,IDS_EXPORT_SELECTED_GRAPHS_MENU_ITEM);
	VMenubar::appendMenuItem(expMenu,fileOpOk&&(pathNumber==1)&&(selItems==1),false,EXPORT_PATH_SIMPLE_POINTS_CMD,IDS_EXPORT_SELECTED_PATH_MENU_ITEM);
	VMenubar::appendMenuItem(expMenu,fileOpOk&&(pathNumber==1)&&(selItems==1),false,EXPORT_PATH_BEZIER_POINTS_CMD,IDS_EXPORT_SELECTED_PATH_BEZIER_CURVE_MENU_ITEM);
	VMenubar::appendMenuItem(expMenu,fileOpOk,false,EXPORT_IK_CONTENT_CMD,IDS_EXPORT_IK_CONTENT_MENU_ITEM);
	bool canExportDynamicContent=CDynInterface::isDynamicContentAvailable()!=0;
	VMenubar::appendMenuItem(expMenu,canExportDynamicContent,false,EXPORT_DYNAMIC_CONTENT_CMD,IDSN_EXPORT_DYNAMIC_CONTENT);
	VMenubar::appendMenuAndDetach(menu,expMenu,true,IDSN_EXPORT_MENU_ITEM);



	VMenubar::appendMenuSeparator(menu);

	VMenubar::appendMenuSeparator(menu);
	VMenubar::appendMenuItem(menu,true,false,EXIT_SIMULATOR_CMD,IDS_EXIT_MENU_ITEM);
}

bool CFileMenu::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to file menu and was executed
	if (commandID==NEW_SCENE_CMD)
	{ // Cannot undo this command
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("NEW_SCENE_CMD")
		{
			CFileMenu::createNewScene(true,false);
		}
		return(true);
	}
	if (commandID==CLOSE_SCENE_CMD)
	{ // Cannot undo this command
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("CLOSE_SCENE_CMD")
		{
			CFileMenu::closeScene(true);
		}
		return(true);
	}
	if (commandID==OPEN_SCENE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("OPEN_SCENE_CMD")
		{
			App::uiThread->addStatusLine(tt::decorateString("",IDSNS_LOADING_SCENE,"...").c_str());

			std::string tst(App::directories->sceneDirectory);
			std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDSN_LOADING_SCENE),tst,"",false,"V-REP Scene Files","ttt");

			if (filenameAndPath.length()!=0)
			{
				App::ct->hierarchy->setRebuildHierarchyFlag();
				bool useNewInstance=(App::ct->undoBufferContainer->isSceneSaveMaybeNeededFlagSet()||(App::ct->mainSettings->scenePathAndName!=""))&&(!App::ct->environment->getSceneCanBeDiscardedWhenNewSceneOpened());
				App::ct->setDefaultMouseMode();
				if (useNewInstance)
				{
					App::ct->createNewInstance(true);
					createNewScene(false,false);
				}
				else
				{
					App::ct->scintillaEditorContainer->closeAllEditors();
					App::ct->simulation->stopSimulation();
					App::ct->objCont->endEditMode(true);
					App::ct->closeAllNonToolDialogs();
					App::ct->emptyScene(true);
				}
				if (loadScene(filenameAndPath.c_str(),true,true))
					_addToRecentlyOpenedScenes(filenameAndPath);
				else
					_removeFromRecentlyOpenedScenes(filenameAndPath);
			}
			else
				App::uiThread->addStatusLine(IDSNS_ABORTED);
			App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
		}
		return(true);
	}

	if ((commandID>=OPEN_RECENT_SCENE0_CMD)&&(commandID<=OPEN_RECENT_SCENE9_CMD))
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("OPEN_RECENT_SCENE...CMD")
		{
			CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
			std::string filenameAndPath;
			int recentScenesCnt=0;
			for (int i=0;i<10;i++)
			{
				std::string tmp("SIMSETTINGS_RECENTSCENE0");
				tmp[23]=48+i;
				cont.readData(tmp.c_str(),filenameAndPath);
				if (filenameAndPath.length()>3)
					recentScenesCnt++;
				if (recentScenesCnt==commandID-OPEN_RECENT_SCENE0_CMD+1)
					break;
			}

			bool useNewInstance=(App::ct->undoBufferContainer->isSceneSaveMaybeNeededFlagSet()||(App::ct->mainSettings->scenePathAndName!=""))&&(!App::ct->environment->getSceneCanBeDiscardedWhenNewSceneOpened());
			App::ct->setDefaultMouseMode();
			if (useNewInstance)
			{
				App::ct->createNewInstance(true);
				CFileMenu::createNewScene(false,false);
			}
			else
			{
				App::ct->scintillaEditorContainer->closeAllEditors();
				App::ct->simulation->stopSimulation();
				App::ct->objCont->endEditMode(true);
				App::ct->closeAllNonToolDialogs();
				App::ct->emptyScene(true);
			}

			if (loadScene(filenameAndPath.c_str(),true,true))
				_addToRecentlyOpenedScenes(filenameAndPath);
			else
				_removeFromRecentlyOpenedScenes(filenameAndPath);
			App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
		}
		return(true);
	}

	if (commandID==LOAD_MODEL_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("LOAD_MODEL_CMD")
		{
			std::string tst(App::directories->modelDirectory);
			std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDSN_LOADING_MODEL),tst,"",false,"V-REP Model Files","ttm");
			if (filenameAndPath.length()!=0)
				loadModel(filenameAndPath.c_str(),true,true,false,NULL,true); // Undo things is in here.
			else
				App::uiThread->addStatusLine(IDSNS_ABORTED);
		}
		return(true);
	}
	if (commandID==LOAD_UI_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("LOAD_UI_CMD")
		{
			std::string tst(App::directories->uiDirectory);
			std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDSN_LOADING_UI),tst,"",false,"V-REP Custom UI Files","ttb");
			if (filenameAndPath.length()!=0)
				loadUserInterfaces(filenameAndPath.c_str(),true,true,NULL,true); // Undo thing done here too
			else
				App::uiThread->addStatusLine(IDSNS_ABORTED);
		}
		return(true);
	}

	if (commandID==SAVE_SCENE_CMD)
	{
		bool done=false;
		for (int i=0;i<5;i++)
		{ // if following condition fails, we have a problem!
			IF_UI_EVENT_CAN_WRITE_DATA_CMD("SAVE_SCENE_CMD")
			{
				_saveSceneWithDialogAndEverything(); // will call save as if needed!
				App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
				done=true;
			}
			if (done)
				break;
		}
		if (!done) // should normally never happen
			App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SCENE),strTranslate(IDSN_SCENE_SAVE_FAILED_WARNING),VMessageBox::OKELI);

		return(true);
	}
	if (commandID==EXPORT_IK_CONTENT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("EXPORT_IK_CONTENT_CMD")
		{
			if (!App::ct->environment->getSceneLocked())
			{
				App::uiThread->addStatusLine(IDSNS_EXPORTING_IK_CONTENT);
				std::string tst(App::directories->otherFilesDirectory);
				std::string filenameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,strTranslate(IDS_EXPORTING_IK_CONTENT___),tst,"",false,"V-REP IK Content Files","ik");
				if (filenameAndPath.length()!=0)
				{
					App::directories->otherFilesDirectory=App::directories->getPathFromFull(filenameAndPath);
					CExtIkSer ar;
					App::ct->objCont->exportIkContent(ar);

					VFile myFile(filenameAndPath.c_str(),VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
					myFile.setLength(0);
					VArchive arch(&myFile,VArchive::STORE);

					int dataLength;
					unsigned char* data=ar.getBuffer(dataLength);
					for (int i=0;i<dataLength;i++)
						arch << data[i];
					arch.close();
					myFile.close();

					App::uiThread->addStatusLine(IDSNS_DONE);
				}
				else
					App::uiThread->addStatusLine(IDSNS_ABORTED);
			}
			else
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXPORT),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMessageBox::OKELI);
		}
		return(true);
	}
	if (commandID==SAVE_SCENE_AS_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("SAVE_SCENE_AS_CMD")
		{
			_saveSceneAsWithDialogAndEverything();
			App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
		}
		return(true);
	}
	if (commandID==SAVE_MODEL_CMD)
	{
		IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG_CMD("SAVE_MODEL_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if (!App::ct->environment->getSceneLocked())
			{
				std::string infoM(IDSNS_SAVING_MODEL);
				infoM+="...";
				App::uiThread->addStatusLine(infoM.c_str());
				if (sel.size()!=0)
				{
// Display a warning if needed
//******************************************
					CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
					std::string val;
					cont.readData("SIMSETTINGS_MODEL_SAVE_OFFSET_WARNING",val);
					int intVal=0;
					tt::getValidInt(val,intVal);
					if (intVal<1)
					{
						CQDlgMessageAndCheckbox dlg(App::mainWindow);
						dlg.title=strTranslate(IDSN_MODEL);
						dlg.text=strTranslate(IDSN_MODEL_SAVE_POSITION_OFFSET_INFO);
						dlg.checkbox=strTranslate(IDSN_DO_NOT_SHOW_THIS_MESSAGE_AGAIN);
						dlg.refresh();
						dlg.makeDialogModal();
						if (dlg.checkboxState)
						{
							intVal++;
							val=tt::FNb(intVal);
							cont.writeData("SIMSETTINGS_MODEL_SAVE_OFFSET_WARNING",val,!App::userSettings->doNotWritePersistentData);
						}
					}
//******************************************
					bool keepCurrentThumbnail=false;
					bool operationCancelled=false;
					int modelBase=App::ct->objCont->getLastSelectionID();
					while (true)
					{
						if (App::ct->environment->modelThumbnail_notSerializedHere.hasImage())
						{ // we already have a thumbnail!
							CQDlgModelThumbnailVisu dlg;
							dlg.applyThumbnail(&App::ct->environment->modelThumbnail_notSerializedHere);
							keepCurrentThumbnail=(dlg.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL);
						}
						if (!keepCurrentThumbnail)
						{
							CQDlgModelThumbnail dlg;
							dlg.modelBaseDummyID=modelBase;
							dlg.initialize();
							dlg.actualizeBitmap();
							if (dlg.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
							{
								App::ct->environment->modelThumbnail_notSerializedHere.copyFrom(&dlg.thumbnail);
								if (!dlg.thumbnailIsFromFile)
									break;
							}
							else
							{
								operationCancelled=true;
								break;
							}
						}
						else
							break;
					}
					if (!operationCancelled)
					{
						std::string tst(App::directories->modelDirectory);
						std::string filenameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,strTranslate(IDS_SAVING_MODEL___),tst,"",false,"V-REP Model Files","ttm");
						if (filenameAndPath.length()!=0)
							saveModel(modelBase,filenameAndPath.c_str(),true,true);
						else
							App::uiThread->addStatusLine(IDSNS_ABORTED);
					}
					else
						App::uiThread->addStatusLine(IDSNS_ABORTED);
				}
				else
					App::uiThread->addStatusLine(IDSNS_CANNOT_PROCEED_SELECTION_IS_EMPTY);
			}
			else
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_MODEL),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMessageBox::OKELI);
		}
		return(true);
	}
	if (commandID==SAVE_UI_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("SAVE_UI_CMD")
		{
			if (!App::ct->environment->getSceneLocked())
			{
				App::uiThread->addStatusLine(IDSNS_SAVING_CUSTOM_USER_INTERFACES);
				std::string filenameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,strTranslate(IDSNS_SAVING_CUSTOM_USER_INTERFACES),"","",false,"V-REP User Interface Files","ttb");
				if (filenameAndPath.length()!=0)
					CFileMenu::saveUserInterfaces(filenameAndPath.c_str(),true,true,NULL);
				else
					App::uiThread->addStatusLine(IDSNS_ABORTED);
			}
			else
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMessageBox::OKELI);
		}
		return(true);
	}
	if (commandID==IMPORT_MESH_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("IMPORT_MESH_CMD")
		{
			App::uiThread->addStatusLine(IDS_IMPORTING_MESH___);
			App::ct->objCont->deselectObjects();
			std::string tst(App::directories->cadFormatDirectory);

			float scalingFactor=1.0f;
			bool showDlg=true;

			std::vector<std::string> filenamesAndPaths;

			VFileDialog::getOpenFileNames(filenamesAndPaths,App::mainWindow,0,IDS_IMPORTING_MESH___,tst,"",false,"Mesh files","obj","dxf","stl");

			for (int i=0;i<int(filenamesAndPaths.size());i++)
			{
				std::string filenameAndPath=filenamesAndPaths[i];
				App::uiThread->addStatusLine((std::string("--> ")+filenameAndPath).c_str());
				if (VFile::doesFileExist(filenameAndPath))
				{
					std::string ext(CTTUtil::getLowerCaseString(VVarious::splitPath_fileExtension(filenameAndPath).c_str()));
					int fileFormat=FILE_FORMAT_OBJ;
					if (ext.compare("dxf")==0)
						fileFormat=FILE_FORMAT_DXF;
					if (ext.compare("3ds")==0)
						fileFormat=FILE_FORMAT_3DS;
					if (ext.compare("stl")==0)
						fileFormat=FILE_FORMAT_ANY_STL;
					if (commonImportRoutine(fileFormat,filenameAndPath,showDlg,scalingFactor,-1))
						App::uiThread->addStatusLine(IDSNS_DONE);
					else
						App::uiThread->addStatusLine(IDSNS_AN_ERROR_OCCURRED_DURING_THE_IMPORT_OPERATION);
					showDlg=false;
				}
				else
					App::uiThread->addStatusLine(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
			}
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		return(true);
	}
	if (commandID==IMPORT_PATH_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("IMPORT_PATH_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_IMPORTING_PATH_FROM_CSV_FILE);
			App::ct->objCont->deselectObjects();
			std::string tst(App::directories->cadFormatDirectory);
			std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDS_IMPORTING_PATH_FROM_CSV_FILE),tst,"",false,"CSV Files","csv");
			if (filenameAndPath.length()!=0)
			{
				if (VFile::doesFileExist(filenameAndPath))
				{
					if (pathImportRoutine(filenameAndPath))
						App::uiThread->addStatusLine(IDSNS_DONE);
					else
						App::uiThread->addStatusLine(IDSNS_AN_ERROR_OCCURRED_DURING_THE_IMPORT_OPERATION);
					App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				}
				else
					App::uiThread->addStatusLine(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
			}
			else
				App::uiThread->addStatusLine(IDSNS_ABORTED);
		}
		return(true);
	}

	if (commandID==IMPORT_HEIGHTFIELD_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("IMPORT_HEIGHTFIELD_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_IMPORTING_HEIGHTFIELD_SHAPE);
			App::ct->objCont->deselectObjects();
			std::string tst(App::directories->cadFormatDirectory);
			std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDS_IMPORTING_HEIGHTFIELD___),tst,"",true,"Image, CSV and TXT files","tga","jpg","jpeg","png","gif","bmp","tiff","csv","txt");

			if (filenameAndPath.length()!=0)
			{
				if (VFile::doesFileExist(filenameAndPath))
				{
					if (heightfieldImportRoutine(filenameAndPath))
						App::uiThread->addStatusLine(IDSNS_DONE);
					else
						App::uiThread->addStatusLine(IDSNS_AN_ERROR_OCCURRED_DURING_THE_IMPORT_OPERATION);
					App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				}
				else
					App::uiThread->addStatusLine(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
			}
			else
				App::uiThread->addStatusLine(IDSNS_ABORTED);
		}
		return(true);
	}

	if (commandID==EXPORT_SHAPE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("EXPORT_SHAPE_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if (!App::ct->environment->getSceneLocked())
			{
				App::uiThread->addStatusLine(IDSNS_EXPORTING_SHAPES);
				CObjectEdition::addRootObjectChildrenToSelection(sel);
				if (0==App::ct->objCont->getShapeNumberInSelection(&sel))
					return(true); // Selection contains nothing that can be exported!
				std::string tst(App::directories->cadFormatDirectory);
				std::string filenameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,strTranslate(IDSNS_EXPORTING_SHAPES),tst,"",false,"Mesh files","obj","dxf","stl");
				if (filenameAndPath.length()!=0)
				{
					App::directories->cadFormatDirectory=App::directories->getPathFromFull(filenameAndPath);

					std::string ext(CTTUtil::getLowerCaseString(VVarious::splitPath_fileExtension(filenameAndPath).c_str()));
					bool error=true;
					if (ext.compare("obj")==0)
					{
						CObjFile exporter;
						error=!exporter.exportFunc(filenameAndPath,false,&sel);
					}
					if (ext.compare("dxf")==0)
					{
						CDxfFile dxf;
						error=!dxf.exportFunc(filenameAndPath,true,&sel);
					}
					if (ext.compare("stl")==0)
					{
						CStlFile exporter(true);
						error=!exporter.exportFunc(filenameAndPath,false,&sel);
					}
					App::ct->objCont->deselectObjects();
					if (error)
						App::uiThread->addStatusLine(IDSNS_AN_ERROR_OCCURRED_DURING_THE_EXPORT_OPERATION);
					else
						App::uiThread->addStatusLine(IDSNS_DONE);
				}
				else
					App::uiThread->addStatusLine(IDSNS_ABORTED);
			}
			else
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXPORT),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMessageBox::OKELI);
		}
		return(true);
	}
	if (commandID==EXPORT_GRAPHS_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("EXPORT_GRAPHS_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			App::uiThread->addStatusLine(IDSNS_EXPORTING_GRAPH_DATA);
			App::ct->simulation->stopSimulation();
			if (App::ct->objCont->getGraphNumberInSelection(&sel)!=0)
			{
				std::string tst(App::directories->otherFilesDirectory);
				std::string filenameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,strTranslate(IDS_SAVING_GRAPHS___),tst,"",false,"CSV Files","csv");
				if (filenameAndPath.length()!=0)
				{
					VFile myFile(filenameAndPath.c_str(),VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
					myFile.setLength(0);
					VArchive ar(&myFile,VArchive::STORE);
					App::directories->otherFilesDirectory=App::directories->getPathFromFull(filenameAndPath);
					for (int i=0;i<int(sel.size());i++)
					{
						CGraph* it=App::ct->objCont->getGraph(sel[i]);
						if (it!=NULL)
							it->exportGraphData(ar);
					}
					ar.close();
					myFile.close();
					App::uiThread->addStatusLine(IDSNS_DONE);
				}
				else
					App::uiThread->addStatusLine(IDSNS_ABORTED);
			}
		}
		return(true);
	}
	
	if ( (commandID==EXPORT_PATH_SIMPLE_POINTS_CMD)||(commandID==EXPORT_PATH_BEZIER_POINTS_CMD) )
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("EXPORT_PATH...POINTS_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				sel.push_back(App::ct->objCont->getSelID(i));
			if (App::ct->objCont->isLastSelectionAPath(&sel))
			{
				CPath* it=(CPath*)App::ct->objCont->getLastSelection(&sel);
				if (it->pathContainer->getSimplePathPointCount()!=0)
				{
					if (commandID==EXPORT_PATH_SIMPLE_POINTS_CMD)
						App::uiThread->addStatusLine(IDSNS_EXPORTING_PATH);
					else
						App::uiThread->addStatusLine(IDSNS_EXPORTING_PATHS_BEZIER_CURVE);
					App::ct->simulation->stopSimulation();
					std::string titleString;
					if (commandID==EXPORT_PATH_SIMPLE_POINTS_CMD)
						titleString=strTranslate(IDS_EXPORTING_PATH___);
					else
						titleString=strTranslate(IDS_EXPORTING_PATH_BEZIER_CURVE___);
					std::string filenameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,titleString,App::directories->executableDirectory,"",false,"CSV Files","csv");
					if (filenameAndPath.length()!=0)
					{
						pathExportPoints(filenameAndPath,it->getID(),commandID==EXPORT_PATH_BEZIER_POINTS_CMD);
						App::uiThread->addStatusLine(IDSNS_DONE);
					}
					else
						App::uiThread->addStatusLine(IDSNS_ABORTED);
				}
				else
					App::uiThread->addStatusLine(IDSNS_CANNOT_EXPORT_AN_EMPTY_PATH);
			}
			else
				App::uiThread->addStatusLine(IDSNS_LAST_SELECTION_IS_NOT_A_PATH);
			App::ct->objCont->deselectObjects();
		}
		return(true);
	}
	if (commandID==EXPORT_DYNAMIC_CONTENT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("EXPORT_DYNAMIC_CONTENT_CMD")
		{
			if (!App::ct->environment->getSceneLocked())
			{
				App::uiThread->addStatusLine(IDSNS_EXPORTING_DYNAMIC_CONTENT);
				if (CDynInterface::isDynamicContentAvailable()!=0)
				{
					if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_ode)
					{
						std::string tst(App::directories->otherFilesDirectory);
						std::string filenameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,strTranslate(IDS_EXPORTING_DYNAMIC_CONTENT___),tst,"",false,"ODE Dynamics World Files","ode");
						if (filenameAndPath.length()!=0)
						{
							App::directories->otherFilesDirectory=App::directories->getPathFromFull(filenameAndPath);
							CDynInterface::serializeDynamicContent(filenameAndPath.c_str(),0);
							App::uiThread->addStatusLine(IDSNS_DONE);
						}
						else
							App::uiThread->addStatusLine(IDSNS_ABORTED);
					}
					if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_bullet)
					{
						std::string tst(App::directories->otherFilesDirectory);
						std::string filenameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,strTranslate(IDS_EXPORTING_DYNAMIC_CONTENT___),tst,"",false,"Bullet Dynamics World Files","bullet");
						if (filenameAndPath.length()!=0)
						{
							App::directories->otherFilesDirectory=App::directories->getPathFromFull(filenameAndPath);
							CDynInterface::serializeDynamicContent(filenameAndPath.c_str(),App::userSettings->bulletSerializationBuffer);
							App::uiThread->addStatusLine(IDSNS_DONE);
						}
						else
							App::uiThread->addStatusLine(IDSNS_ABORTED);
					}
					if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_vortex)
					{ // TODO_VORTEX
						std::string tst(App::directories->otherFilesDirectory);
						std::string filenameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,strTranslate(IDS_EXPORTING_DYNAMIC_CONTENT___),tst,"",false,"Vortex Dynamics World Files","vortex");
						if (filenameAndPath.length()!=0)
						{
							App::directories->otherFilesDirectory=App::directories->getPathFromFull(filenameAndPath);
							CDynInterface::serializeDynamicContent(filenameAndPath.c_str(),App::userSettings->bulletSerializationBuffer);
							App::uiThread->addStatusLine(IDSNS_DONE);
						}
						else
							App::uiThread->addStatusLine(IDSNS_ABORTED);
					}
				}
				else
					App::uiThread->addStatusLine(IDSNS_CANNOT_PROCEED_NO_DYNAMIC_CONTENT_AVAILABLE);
			}
			else
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXPORT),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMessageBox::OKELI);
		}
		return(true);
	}
	if (commandID==EXIT_SIMULATOR_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("EXIT_SIMULATOR_CMD")
		{
			App::mainWindow->simulationRecorder->stopRecording(false);

			int si=-1;
			int ei=App::ct->getInstanceIndexOfAnEditMode();
			int ci=App::ct->getInstanceIndexOfASceneNotYetSaved();
			if (!App::ct->simulation->isSimulationStopped())
				si=App::ct->getCurrentInstanceIndex();
			if (App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)
				ei=App::ct->getCurrentInstanceIndex();
			if (App::ct->undoBufferContainer->isSceneSaveMaybeNeededFlagSet())
				ci=App::ct->getCurrentInstanceIndex();
			bool displayed=false;
			if ((ei==App::ct->getCurrentInstanceIndex())&&(!displayed))
			{
				if (VMessageBox::REPLY_OK==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXIT),strTranslate(IDS_INSTANCE_STILL_IN_EDIT_MODE_MESSAGE),VMessageBox::OK_CANCEL))
				{
					App::ct->objCont->endEditMode(true);
					ei=App::ct->getInstanceIndexOfAnEditMode();
				}
				else
					displayed=true;
			}
			if ((si==App::ct->getCurrentInstanceIndex())&&(!displayed))
			{
				if (VMessageBox::REPLY_OK==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXIT),strTranslate(IDS_SIMULATION_STILL_RUNNING_MESSAGE),VMessageBox::OK_CANCEL))
					App::ct->simulatorMessageQueue->addCommand(sim_message_simulation_stop_request,0,0,0,0,NULL,0);
				displayed=true;
			}
			if ((ci==App::ct->getCurrentInstanceIndex())&&(!displayed))
			{
				WORD action=VMessageBox::REPLY_NO;
				action=App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_WANNA_SAVE_THE_SCENE_WARNING),VMessageBox::YES_NO_CANCEL);
				if (action==VMessageBox::REPLY_YES)
				{
					if (_saveSceneWithDialogAndEverything()) // will call save as if needed!
						action=VMessageBox::REPLY_NO;
				}
				if (action==VMessageBox::REPLY_NO)
				{
					App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
					ci=App::ct->getInstanceIndexOfASceneNotYetSaved();
				}
				else
					displayed=true;
			}
			if ((ei!=-1)&&(!displayed))
			{
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXIT),strTranslate(IDS_ANOTHER_INSTANCE_STILL_IN_EDIT_MODE_MESSAGE),VMessageBox::OKELI);
				App::ct->evaluateMenuSelection(SWITCH_TO_INSTANCE_INDEX0_CMD+ei);
				displayed=true;
			}
			if ((si!=-1)&&(!displayed))
			{
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_EXIT),strTranslate(IDS_ANOTHER_SIMULATION_STILL_RUNNING_MESSAGE),VMessageBox::OKELI);
				App::ct->evaluateMenuSelection(SWITCH_TO_INSTANCE_INDEX0_CMD+si);
				displayed=true;
			}
			if ((ci!=-1)&&(!displayed))
			{
				if (VMessageBox::REPLY_CANCEL==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_ANOTHER_INSTANCE_STILL_NOT_SAVED_WANNA_LEAVE_ANYWAY_MESSAGE),VMessageBox::OK_CANCEL))
				{
					App::ct->evaluateMenuSelection(SWITCH_TO_INSTANCE_INDEX0_CMD+ci);
					displayed=true;
				}
			}
			if (!displayed)
			{
				App::uiThread->addStatusLine(IDSNS_LEAVING);
				App::setExitRequest(true);
			}
		}
		return(true);
	}
	return(false);
}


void CFileMenu::createNewScene(bool displayMessages,bool forceForNewInstance)
{
	FUNCTION_DEBUG;
	bool useNewInstance=(App::ct->undoBufferContainer->isSceneSaveMaybeNeededFlagSet()||(App::ct->mainSettings->scenePathAndName!=""))&&(!App::ct->environment->getSceneCanBeDiscardedWhenNewSceneOpened());
	if (forceForNewInstance)
		useNewInstance=true;
	App::ct->setDefaultMouseMode();
	if (useNewInstance)
		App::ct->createNewInstance(true);
	else
	{
		if (App::mainWindow!=NULL)
			App::ct->scintillaEditorContainer->closeAllEditors();
		App::ct->simulation->stopSimulation();
		App::ct->objCont->endEditMode(true);
		if (App::mainWindow!=NULL)
			App::ct->closeAllNonToolDialogs();
	}
	App::ct->emptyScene(true);
	std::string fullPathAndFilename=App::directories->systemDirectory+VREP_SLASH;
	fullPathAndFilename.append("dfltscn.ttt");
	CFileMenu::loadScene(fullPathAndFilename.c_str(),false,false);
	App::ct->mainSettings->scenePathAndName="";//savedLoc;
	App::uiThread->addStatusLine(IDSNS_DEFAULT_SCENE_WAS_SET_UP);
	App::ct->undoBufferContainer->memorizeState(); // so that we can come back to the initial state!
	App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
	App::ct->objCont->setDefaultSceneID(App::ct->objCont->computeSceneID());
}

void CFileMenu::closeScene(bool displayMessages)
{
	FUNCTION_DEBUG;
	WORD action=VMessageBox::REPLY_NO;
	if (displayMessages&&(!App::ct->environment->getSceneCanBeDiscardedWhenNewSceneOpened()))
	{
		if (App::ct->undoBufferContainer->isSceneSaveMaybeNeededFlagSet())
		{
			action=App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_WANNA_SAVE_THE_SCENE_WARNING),VMessageBox::YES_NO_CANCEL);
			if (action==VMessageBox::REPLY_YES)
			{
				if (_saveSceneWithDialogAndEverything()) // will call save as if needed!
					action=VMessageBox::REPLY_NO;
			}
		}
	}
	if (action==VMessageBox::REPLY_NO)
	{
		App::ct->setDefaultMouseMode();
		App::ct->scintillaEditorContainer->closeAllEditors();
		App::ct->simulation->stopSimulation();
		App::ct->objCont->endEditMode(true);
		App::ct->closeAllNonToolDialogs();
		App::ct->emptyScene(true);
		if (App::ct->getInstanceNumber()>1)
		{ // remove this instance:
			App::ct->destroyCurrentInstance();
		}
		else
		{ // simply set-up an empty (default) scene
			std::string savedLoc=App::ct->mainSettings->scenePathAndName;
			std::string fullPathAndFilename=App::directories->systemDirectory+VREP_SLASH;
			fullPathAndFilename.append("dfltscn.ttt");
			CFileMenu::loadScene(fullPathAndFilename.c_str(),false,false);
			App::ct->mainSettings->scenePathAndName=savedLoc;
			App::uiThread->addStatusLine(IDSNS_DEFAULT_SCENE_WAS_SET_UP);
			App::ct->undoBufferContainer->memorizeState(); // so that we can come back to the initial state!
			App::ct->undoBufferContainer->clearSceneSaveMaybeNeededFlag();
			App::ct->objCont->setDefaultSceneID(App::ct->objCont->computeSceneID());
		}
	}
	App::ct->hierarchy->setRebuildHierarchyFlag();
}

bool CFileMenu::commonImportRoutine(int importType,const std::string& pathName,bool showDlg,float& sf,int options)
{ // options==-1 --> default
	// bit0 set --> keep identical vertices
	// bit1 set --> keep identival triangles
	// bit2 set --> not used
	// bit3 set --> not used
	// bit4 set --> not used
	// bit5 set --> not used
	FUNCTION_DEBUG;

	bool keepIdenticalVertices=0;
	bool keepIdenticalTriangles=0;
	if (options==-1)
	{
		keepIdenticalVertices=App::userSettings->identicalVerticesCheck;
		keepIdenticalTriangles=App::userSettings->identicalTrianglesCheck;
	}
	else
	{
		keepIdenticalVertices=((options&1)!=0);
		keepIdenticalTriangles=((options&2)!=0);
	}

	CImportExport* importer=NULL;
	App::directories->cadFormatDirectory=App::directories->getPathFromFull(pathName);
	if (importType==FILE_FORMAT_DXF)
		importer=new CDxfFile();
	if (importType==FILE_FORMAT_OBJ)
		importer=new CObjFile();
	if ((importType==FILE_FORMAT_ASCII_STL)||(importType==FILE_FORMAT_BINARY_STL)||(importType==FILE_FORMAT_ANY_STL))
		importer=new CStlFile(importType);

	std::vector<std::vector<float>*> aVertices;
	std::vector<std::vector<int>*> aIndices;
	std::vector<std::vector<float>*> aTextureCoordinates;
	std::vector<std::vector<float>*> aNormals;
	std::vector<SObjMaterial*> aMaterials;
	std::vector<std::string> groupNames;
	bool error=!importer->importFunc(pathName);

	for (int i=0;i<importer->getGroupCount();i++)
	{
		if ((importer->getVertexCount(i)>=3)&&(importer->getTriangleCount(i)>=1))
		{ 
			std::vector<float>* vert=new std::vector<float>;
			std::vector<int>* ind=new std::vector<int>;
			std::vector<float>* tex=NULL;
			std::vector<float>* norm=NULL;
			SObjMaterial* mat=NULL;
			for (int j=0;j<importer->getVertexCount(i);j++)
			{
				float vertex[3];
				importer->getVertex(i,j,vertex);
				vert->push_back(vertex[0]);
				vert->push_back(vertex[1]);
				vert->push_back(vertex[2]);
			}
			for (int j=0;j<importer->getTriangleCount(i);j++)
			{
				int triangle[3];
				importer->getTriangle(i,j,triangle);
				ind->push_back(triangle[0]);
				ind->push_back(triangle[1]);
				ind->push_back(triangle[2]);
			}
			if (importer->getTextureCoordinateCount(i)==importer->getTriangleCount(i)*3)
			{
				tex=new std::vector<float>;
				for (int j=0;j<importer->getTextureCoordinateCount(i);j++)
				{
					float coord[2];
					importer->getTextureCoordinate(i,j,coord);
					tex->push_back(coord[0]);
					tex->push_back(coord[1]);
				}
			}
			if (importer->getNormalsCount(i)==importer->getTriangleCount(i)*3)
			{
				norm=new std::vector<float>;
				for (int j=0;j<importer->getNormalsCount(i);j++)
				{
					float n[3];
					importer->getNormal(i,j,n);
					norm->push_back(n[0]);
					norm->push_back(n[1]);
					norm->push_back(n[2]);
				}
			}
			mat=importer->getAndClearMaterial(i);

			CMeshManip::checkVerticesIndicesNormalsTexCoords(*vert,*ind,norm,tex,keepIdenticalVertices,App::userSettings->identicalVerticesTolerance/sf,keepIdenticalTriangles);
			if ((norm!=NULL)&&(norm->size()==0))
			{
				delete norm;
				norm=NULL;
			}
			if ((tex!=NULL)&&(tex->size()==0))
			{
				delete tex;
				tex=NULL;
			}

			if ((vert->size()>=9)&&(ind->size()>=3))
			{
				aVertices.push_back(vert);
				aIndices.push_back(ind);
				aNormals.push_back(norm);
				aTextureCoordinates.push_back(tex);
				std::string tmpStr(importer->getName(i));
				tt::removeIllegalCharacters(tmpStr,false);
				groupNames.push_back(tmpStr);
				aMaterials.push_back(mat);
			}
			else
			{
				delete vert;
				delete ind;
				delete norm;
				delete tex;
				delete norm;
				delete mat;
			}
		}
	}
	delete importer;

	C3Vector minV,maxV;
	C3Vector minVRot,maxVRot;
	float underZCounter=0;
	float underZCounter2=0;
	C4X4Matrix tr;
	tr.setIdentity();
	tr.M.buildXRotation(piValD2);
	for (int i=0;i<int(aVertices.size());i++)
	{
		for (int j=0;j<int(aVertices[i]->size())/3;j++)
		{
			C3Vector v(&aVertices[i]->at(3*j));
			if ((i==0)&&(j==0))
			{
				minV=v;
				maxV=v;
				v*=tr;
				minVRot=v;
				maxVRot=v;
			}
			else
			{
				minV.keepMin(v);
				maxV.keepMax(v);
				float vz=v(2);
				v*=tr;
				minVRot.keepMin(v);
				maxVRot.keepMax(v);
				if (v(2)<vz)
					underZCounter2++;
				else
					underZCounter++;
			}
		}
	}

	bool zIsUp=true;
	if (underZCounter>underZCounter2)
	{ // the imported scene was probably using a coord. system with Y up!
		minV=minVRot;
		maxV=maxVRot;
		zIsUp=false;
	}
	else
		tr.setIdentity();

	C3Vector dim(maxV-minV);
	float maxDim=SIM_MAX(SIM_MAX(dim(0),dim(1)),dim(2));
	float performedScaling=1.0f;
	int invScaling=1;
	for (int i=0;i<3;i++)
	{
		if (maxDim>2.0f)
		{
			maxDim*=0.1f;
			performedScaling/=10.0f;
			invScaling*=10;
		}
	}
	if (invScaling==10)
	{ // no one uses a unit of 10cm
		invScaling=1;
		performedScaling=1.0f; 
	}

	if (!showDlg)
	{
		performedScaling=sf;
		tr.setIdentity();
	}

	for (int i=0;i<int(aVertices.size());i++)
	{
		for (int j=0;j<int(aVertices[i]->size())/3;j++)
		{
			C3Vector v(&aVertices[i]->at(3*j));
			v*=performedScaling;
			v*=tr;
			aVertices[i]->at(3*j+0)=v(0);
			aVertices[i]->at(3*j+1)=v(1);
			aVertices[i]->at(3*j+2)=v(2);
		}
	}

	std::vector<int> newSelection;
	App::ct->objCont->enableObjectActualization(false);
	bool groupImport=false;
	if (aVertices.size()>2000)
	{ // We have too many lose elements... we merge them
		printf("Import operation with more than 2000 separate items (%i).\n",int(aVertices.size()));
		for (int i=0;i<int(aVertices.size());i++)
		{
			if (aMaterials[i]!=NULL)
				groupImport=true;
		}
		if (groupImport)
			printf("Shapes will be grouped.\n");
		else
		{
			printf("Shapes will be merged.\n");
			for (int i=1;i<int(aVertices.size());i++)
			{
				int indOffset=aVertices[0]->size()/3;
				aVertices[0]->insert(aVertices[0]->end(),aVertices[i]->begin(),aVertices[i]->end());
				for (int j=0;j<int(aIndices[i]->size());j++)
					aIndices[0]->push_back(aIndices[i]->at(j)+indOffset);
				delete aVertices[i];
				delete aIndices[i];
				delete aNormals[i];
				delete aTextureCoordinates[i];
				delete aMaterials[i];
			}
			aVertices.resize(1);
			aIndices.resize(1);
			aNormals.resize(1);
			groupNames.resize(1);
			// we have to kill the texures:
			aTextureCoordinates.resize(1);
			delete aTextureCoordinates[0];
			aTextureCoordinates[0]=NULL;
			aMaterials.resize(1);
			delete aMaterials[0];
			aMaterials[0]=NULL;

			// Important to check the meshe here again (we might have merged individual triangles of a complexe shape here)
			CMeshManip::checkVerticesIndicesNormalsTexCoords(aVertices[0][0],aIndices[0][0],aNormals[0],aTextureCoordinates[0],keepIdenticalVertices,App::userSettings->identicalVerticesTolerance,keepIdenticalTriangles);
			if ((aNormals[0]!=NULL)&&(aNormals[0]->size()==0))
			{
				delete aNormals[0];
				aNormals[0]=NULL;
			}
			if ((aTextureCoordinates[0]!=NULL)&&(aTextureCoordinates[0]->size()==0))
			{
				delete aTextureCoordinates[0];
				aTextureCoordinates[0]=NULL;
			}
		}
		if (showDlg)
			App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_IMPORT),strTranslate(IDS_TOO_MANY_MESH_ELEMENTS_DURING_IMPORT),VMessageBox::OKELI);
	}

	int scaleTexturesTo=-1;

	std::vector<std::string> _allTextureNames;
	std::vector<unsigned char*> _allTextureData;
	std::vector<int> _allTextureProp;

	for (int i=0;i<int(aVertices.size());i++)
	{
		printf("Generating a shape (%i)... ",i);
		CGeomProxy* geom;
		geom=new CGeomProxy(NULL,*aVertices[i],*aIndices[i],aNormals[i],aTextureCoordinates[i]);
		CShape* shape=new CShape();
		shape->setLocalTransformation(geom->getCreationTransformation());
		geom->setCreationTransformation(C7Vector::identityTransformation);
		shape->geomData=geom;
		App::ct->objCont->addObjectToScene(shape,false);

		// Handle colors and textures:
		if (aMaterials[i]!=NULL)
		{
			// Color:
			if ((aMaterials[i]->textureFile.length()!=0)&&(aTextureCoordinates[i]!=NULL))
			{ // because some formats have zero here, and then the texture seems not to be present!
				for (int j=0;j<3;j++)
				{
					((CGeometric*)geom->geomInfo)->color.colors[j]=0.5f;
					((CGeometric*)geom->geomInfo)->color.colors[3+j]=0.25f;
				}
			}
			else
			{
				for (int j=0;j<3;j++)
				{
					((CGeometric*)geom->geomInfo)->color.colors[j]=aMaterials[i]->ambient[j];
					((CGeometric*)geom->geomInfo)->color.colors[3+j]=aMaterials[i]->diffuse[j];
				}
			}
			((CGeometric*)geom->geomInfo)->color.colors[6]=aMaterials[i]->specular[0];
			((CGeometric*)geom->geomInfo)->color.colors[7]=aMaterials[i]->specular[1];
			((CGeometric*)geom->geomInfo)->color.colors[8]=aMaterials[i]->specular[2];

			if ((aMaterials[i]->textureFile.length()!=0)&&(aTextureCoordinates[i]!=NULL))
			{ // Texture:
				bool textureLoadFail=true;

				if (VVarious::isAbsolutePath(aMaterials[i]->textureFile))
					aMaterials[i]->textureFile=VVarious::splitPath_fileBaseAndExtension(aMaterials[i]->textureFile);
				std::string tmp=VVarious::splitPath_path(pathName)+VREP_SLASH+aMaterials[i]->textureFile;
				if (VFile::doesFileExist(tmp))
				{
					if (scaleTexturesTo==-1)
					{
						if (showDlg)
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
										cam->setFogTimer(4.0f);
									}
								}
							}

							if (VThread::isCurrentThreadTheUiThread())
								App::mainWindow->uiThread_renderScene(true); // otherwise we won't see the new scene content correctly

							CQDlgTextureLoadOptions dlg(App::mainWindow);;
							dlg.refresh();
							dlg.makeDialogModal();
							scaleTexturesTo=0;
							if (dlg.scale)
								scaleTexturesTo=dlg.scaleTo;
						}
						else
							scaleTexturesTo=0;//256;
					}
					int resX,resY,n;

					int theIndex=-1;
					for (int j=0;j<int(_allTextureNames.size());j++)
					{
						if (_allTextureNames[j].compare(tmp)==0)
						{
							theIndex=j;
							break;
						}
					}
					unsigned char* data=NULL;
					if (theIndex!=-1)
					{ // that texture was already loaded
						data=_allTextureData[theIndex];
						resX=_allTextureProp[3*theIndex+0];
						resY=_allTextureProp[3*theIndex+1];
						n=_allTextureProp[3*theIndex+2];
					}
					else
					{ // that texture was not yet loaded
						data=CImageLoader::load(tmp.c_str(),&resX,&resY,&n,0,scaleTexturesTo);
						if (n<3)
						{
							delete[] data;
							data=NULL;
						}
						if (data!=NULL)
						{
							_allTextureNames.push_back(tmp);
							_allTextureData.push_back(data);
							_allTextureProp.push_back(resX);
							_allTextureProp.push_back(resY);
							_allTextureProp.push_back(n);
						}
					}
					bool rgba=(n==4);
					if (data!=NULL)
					{
						textureLoadFail=false;
						CTextureObject* textureObj=new CTextureObject(resX,resY);
						textureObj->setImage(rgba,false,false,data); // keep false,false
						textureObj->setObjectName(App::directories->getNameFromFull(aMaterials[i]->textureFile).c_str());
// destroyed later						delete[] data;
						textureObj->addDependentObject(shape->getID(),((CGeometric*)shape->geomData->geomInfo)->getUniqueID());
						int textureID=App::ct->textureCont->addObject(textureObj,false); // might erase the textureObj and return a similar object already present!!
						CTextureProperty* tp=new CTextureProperty(textureID);
						((CGeometric*)shape->geomData->geomInfo)->setTextureProperty(tp);

						std::vector<float> wvert;
						std::vector<int> wind;
						((CGeometric*)shape->geomData->geomInfo)->getCumulativeMeshes(wvert,&wind,NULL);
						if (((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.size()/2==wind.size())
						{ // we have texture coordinate data attached to the shape's geometry (was added during shape import)
							tp->setFixedCoordinates(&((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized);
							((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.clear();
						}
					}
				}
				if (textureLoadFail)
				{
					std::string txtTmp(IDS_ERROR);
					txtTmp+=": ";
					txtTmp+=IDS_A_TEXTURE_FILE_COULD_NOT_BE_LOADED;
					txtTmp+=": ";
					txtTmp+=tmp;
					App::uiThread->addStatusLine(txtTmp.c_str());
				}
			}
		}


		shape->setVisibleEdges(aTextureCoordinates[i]==NULL); // Visible edges only if no texture coords (otherwise it looks like a bug sometimes)
		if (aNormals[i]==NULL)
			((CGeometric*)shape->geomData->geomInfo)->setGouraudShadingAngle(30.0f*degToRad); 
		((CGeometric*)shape->geomData->geomInfo)->setEdgeThresholdAngle(30.0f*degToRad); 
		shape->geomData->geomInfo->setLocalInertiaFrame(C7Vector::identityTransformation); // 11/2/2013
		newSelection.push_back(shape->getID());

		std::string tempName("imported_part_0");
		if (groupNames[i]!="")
			tempName=groupNames[i];

		std::string tempName2=App::ct->objCont->getSimilarNameWithHighestSuffix(tempName,false);
		if (tempName2.length()!=0)
			tempName=tempName2;

		while (App::ct->objCont->getObject(tempName)!=NULL)
			tempName=tt::generateNewName_noDash(tempName);
		shape->setName(tempName);

		delete aVertices[i];
		delete aIndices[i];
		delete aNormals[i];
		delete aTextureCoordinates[i];
		delete aMaterials[i];
		printf("done.\n");
	}

	for (int i=0;i<int(_allTextureData.size());i++)
		delete[] _allTextureData[i];

	App::ct->objCont->enableObjectActualization(true);
	App::ct->objCont->actualizeObjectInformation();
	groupNames.clear();
	aVertices.clear();
	aIndices.clear();
	aNormals.clear();
	aTextureCoordinates.clear();

	if (groupImport)
		App::ct->objCont->addObjectToSelection(CObjectEdition::groupSelection(&newSelection,false));
	else
	{
		for (int i=0;i<int(newSelection.size());i++)
			App::ct->objCont->addObjectToSelection(newSelection[i]);
	}

	if (showDlg)
	{
		CQDlgImportOptions theDialog(App::mainWindow);
		theDialog.sizeIndex=0;
		if (invScaling==100)
			theDialog.sizeIndex=4;
		if (invScaling==1000)
			theDialog.sizeIndex=5;
		theDialog.zIsUp=zIsUp;
		theDialog.refresh();
		theDialog.makeDialogModal();
	}

	return(!error);
}

bool CFileMenu::apiImportRoutine(int importType,const std::string& pathName,int options,float identicalVerticesTolerance,float sizeFactor,std::vector<std::vector<float>*>& listOfVertices,std::vector<std::vector<int>*>& listOfIndices,std::vector<std::string>& groupNames)
{ // options: bit 0: keep identical vertices, bit 1: keep identical triangles, bit 2: no winding check
	CImportExport* importer=NULL;
	if (importType==FILE_FORMAT_DXF)
		importer=new CDxfFile();
	if (importType==FILE_FORMAT_OBJ)
		importer=new CObjFile();
	if ((importType==FILE_FORMAT_ASCII_STL)||(importType==FILE_FORMAT_BINARY_STL)||(importType==FILE_FORMAT_ANY_STL))
		importer=new CStlFile(importType);
	importer->importFunc(pathName);

	for (int i=0;i<importer->getGroupCount();i++)
	{
		if ((importer->getVertexCount(i)>=3)&&(importer->getTriangleCount(i)>=1))
		{ 
			std::vector<float>* vert=new std::vector<float>;
			std::vector<int>* ind=new std::vector<int>;
			for (int j=0;j<importer->getVertexCount(i);j++)
			{
				float vertex[3];
				importer->getVertex(i,j,vertex);
				vert->push_back(vertex[0]);
				vert->push_back(vertex[1]);
				vert->push_back(vertex[2]);
			}
			for (int j=0;j<importer->getTriangleCount(i);j++)
			{
				int triangle[3];
				importer->getTriangle(i,j,triangle);
				ind->push_back(triangle[0]);
				ind->push_back(triangle[1]);
				ind->push_back(triangle[2]);
			}
			CMeshManip::checkVerticesIndicesNormalsTexCoords(*vert,*ind,NULL,NULL,(options&1)==0,identicalVerticesTolerance,(options&2)==0);
			if ((vert->size()>=9)&&(ind->size()>=3))
			{
				listOfVertices.push_back(vert);
				listOfIndices.push_back(ind);
				// Here we don't need to remove illegal chars! (API importer)
				groupNames.push_back(importer->getName(i));
			}
			else
			{
				delete vert;
				delete ind;
			}
		}
	}
	delete importer;


	for (int i=0;i<int(listOfVertices.size());i++)
		CMeshManip::centerAndScale(listOfVertices[i],0.0f,0.0f,0.0f,sizeFactor);

	return(listOfVertices.size()!=0);
}



bool CFileMenu::pathImportRoutine(const std::string& pathName)
{
	if (VFile::doesFileExist(pathName))
	{
		try
		{
			VFile file(pathName.c_str(),VFile::READ|VFile::SHARE_DENY_NONE);
			VArchive archive(&file,VArchive::LOAD);
			DWORD currentPos=0;
			std::string line;
			std::vector<float> readData;
			bool automaticOrientation=true;
			const int ds=16;
			// We read:
			// (x,y,z), (a,b,g), velocityRatio, bezierPtCount,beforeRatio,afterRatio,virtualDistance,auxFlags,auxChannel1,auxChannel2,auxChannel3,auxChannel4
			while (archive.readLine(currentPos,line,false))
			{
				float data[ds];
				int cnt=0;
				std::string word;
				float val;
				while (tt::extractCommaSeparatedWord(line,word))
				{
					if (tt::getValidFloat(word,val))
						data[cnt]=val;
					else
						break;
					cnt++;
					if (cnt>=ds)
						break;
				}
				if (cnt>=3)
				{ // We have a point!
					int dataOff=readData.size();
					readData.push_back(0.0f); // x
					readData.push_back(0.0f); // y
					readData.push_back(0.0f); // z
					readData.push_back(0.0f); // Euler 0
					readData.push_back(0.0f); // Euler 1
					readData.push_back(0.0f); // Euler 2
					readData.push_back(1.0f); // Velocity ratio
					readData.push_back(1.0f); // Bezier count
					readData.push_back(0.5f); // Bezier 0
					readData.push_back(0.5f); // Bezier 1
					readData.push_back(0.0f); // virtual distance
					readData.push_back(0.0f); // aux flags
					readData.push_back(0.0f); // aux channel1
					readData.push_back(0.0f); // aux channel2
					readData.push_back(0.0f); // aux channel3
					readData.push_back(0.0f); // aux channel4

					readData[dataOff+0]=data[0]; // x
					readData[dataOff+1]=data[1]; // y
					readData[dataOff+2]=data[2]; // z
					if (cnt>=6)
					{
						automaticOrientation=false;
						readData[dataOff+3]=data[3]*degToRad; // Euler 0
						readData[dataOff+4]=data[4]*degToRad; // Euler 1
						readData[dataOff+5]=data[5]*degToRad; // Euler 2
					}
					if (cnt>=7)
						readData[dataOff+6]=data[6]; // velocity ratio
					if (cnt>=8)
						readData[dataOff+7]=data[7]; // Bezier count
					if (cnt>=10)
					{
						readData[dataOff+8]=data[8]; // Bezier 0
						readData[dataOff+9]=data[9]; // Bezier 1
					}
					if (cnt>=11)
						readData[dataOff+10]=data[10]; // Virtual distance
					if (cnt>=12)
						readData[dataOff+11]=data[11]; // aux flags
					if (cnt>=13)
						readData[dataOff+12]=data[12]; // aux channel1
					if (cnt>=14)
						readData[dataOff+13]=data[13]; // aux channel2
					if (cnt>=15)
						readData[dataOff+14]=data[14]; // aux channel3
					if (cnt>=16)
						readData[dataOff+15]=data[15]; // aux channel4
				}
			}
			if (readData.size()!=0)
			{
				CPath* newObject=new CPath();
				newObject->pathContainer->enableActualization(false);
				int attr=newObject->pathContainer->getAttributes()|sim_pathproperty_automatic_orientation;
				if (!automaticOrientation)
					attr-=sim_pathproperty_automatic_orientation;
				newObject->pathContainer->setAttributes(attr);
				for (int i=0;i<int(readData.size())/ds;i++)
				{
					CSimplePathPoint* it=new CSimplePathPoint();
					C7Vector tr;
					tr.X(0)=readData[ds*i+0];
					tr.X(1)=readData[ds*i+1];
					tr.X(2)=readData[ds*i+2];
					tr.Q.setEulerAngles(readData[ds*i+3],readData[ds*i+4],readData[ds*i+5]);
					it->setTransformation(tr,newObject->pathContainer->getAttributes());
					it->setMaxRelAbsVelocity(readData[ds*i+6]);
					it->setBezierPointCount(int(readData[ds*i+7]+0.5f));
					it->setBezierFactors(readData[ds*i+8],readData[ds*i+9]);
					it->setOnSpotDistance(readData[ds*i+10]);
					it->setAuxFlags(int(readData[ds*i+11]+0.5f));
					it->setAuxChannels(&readData[ds*i+12]);
					newObject->pathContainer->addSimplePathPoint(it);
				}
				newObject->pathContainer->enableActualization(true);
				newObject->pathContainer->actualizePath();
				newObject->setName(IDSOGL_IMPORTEDPATH);
				App::ct->objCont->addObjectToScene(newObject,false);
				App::ct->objCont->selectObject(newObject->getID());
			}
			archive.close();
			file.close();
			return(readData.size()!=0);
		}
		catch(VFILE_EXCEPTION_TYPE e)
		{
			VFile::reportAndHandleFileExceptionError(e);
			return(false);
		}
	}
	return(false);
}

bool CFileMenu::pathExportPoints(const std::string& pathName,int pathID,bool bezierPoints)
{
	CPath* pathObject=App::ct->objCont->getPath(pathID);
	if (pathObject==NULL)
		return(false);
	try
	{
		VFile myFile(pathName.c_str(),VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
		myFile.setLength(0);
		VArchive ar(&myFile,VArchive::STORE);

		CPathCont* it=pathObject->pathContainer;
		C7Vector pathTr(pathObject->getCumulativeTransformation());
		if (bezierPoints)
		{
			for (int i=0;i<it->getBezierPathPointCount();i++)
			{
				CBezierPathPoint* bp=it->getBezierPathPoint(i);
				C7Vector tr(pathTr*bp->getTransformation());
				C3Vector euler(tr.Q.getEulerAngles());
				std::string line(tt::FNb(tr.X(0))+',');
				line+=tt::FNb(tr.X(1))+',';
				line+=tt::FNb(tr.X(2))+',';
				line+=tt::FNb(euler(0)*radToDeg)+',';
				line+=tt::FNb(euler(1)*radToDeg)+',';
				line+=tt::FNb(euler(2)*radToDeg)+',';
				line+=tt::FNb(bp->getMaxRelAbsVelocity())+',';
				line+=tt::FNb(bp->getOnSpotDistance())+',';
				line+=tt::FNb(bp->getAuxFlags())+',';
				float auxChannels[4];
				bp->getAuxChannels(auxChannels);
				line+=tt::FNb(auxChannels[0])+',';
				line+=tt::FNb(auxChannels[1])+',';
				line+=tt::FNb(auxChannels[2])+',';
				line+=tt::FNb(auxChannels[3]);
				ar.writeLine(line);
			}
		}
		else
		{
			for (int i=0;i<it->getSimplePathPointCount();i++)
			{
				CSimplePathPoint* bp=it->getSimplePathPoint(i);
				C7Vector tr(pathTr*bp->getTransformation());
				C3Vector euler(tr.Q.getEulerAngles());
				float f0,f1;
				bp->getBezierFactors(f0,f1);
				std::string line(tt::FNb(tr.X(0))+',');
				line+=tt::FNb(tr.X(1))+',';
				line+=tt::FNb(tr.X(2))+',';
				line+=tt::FNb(euler(0)*radToDeg)+',';
				line+=tt::FNb(euler(1)*radToDeg)+',';
				line+=tt::FNb(euler(2)*radToDeg)+',';
				line+=tt::FNb(bp->getMaxRelAbsVelocity())+',';
				line+=tt::FNb(bp->getBezierPointCount())+',';
				line+=tt::FNb(f0)+',';
				line+=tt::FNb(f1)+',';
				line+=tt::FNb(bp->getOnSpotDistance())+',';
				line+=tt::FNb(bp->getAuxFlags())+',';
				float auxChannels[4];
				bp->getAuxChannels(auxChannels);
				line+=tt::FNb(auxChannels[0])+',';
				line+=tt::FNb(auxChannels[1])+',';
				line+=tt::FNb(auxChannels[2])+',';
				line+=tt::FNb(auxChannels[3]);
				ar.writeLine(line);
			}
		}
		ar.close();
		myFile.close();
		return(true);
	}
	catch(VFILE_EXCEPTION_TYPE e)
	{
		VFile::reportAndHandleFileExceptionError(e);
		return(false);
	}
	return(false);
}

bool CFileMenu::loadScene(const char* pathAndFilename,bool displayMessages,bool setCurrentDir)
{
	FUNCTION_DEBUG;
	if (App::isFullScreen())
		displayMessages=false;

	if (strlen(pathAndFilename)==0)
	{
		createNewScene(displayMessages,true);
		return(true);
	}

	App::ct->setDefaultMouseMode();
	int result=-3;
	if (App::mainWindow!=NULL)
		App::ct->scintillaEditorContainer->closeAllEditors();
	App::ct->objCont->deselectObjects();
	App::ct->simulation->stopSimulation(); // should be anyway stopped!
	if (VFile::doesFileExist(pathAndFilename))
	{
		App::ct->emptyScene(true);
		VFile file(pathAndFilename,VFile::READ|VFile::SHARE_DENY_NONE);
		App::ct->mainSettings->scenePathAndName=pathAndFilename;
		if (setCurrentDir)
			App::directories->sceneDirectory=App::directories->getPathFromFull(App::ct->mainSettings->scenePathAndName.c_str());
		VArchive archive(&file,VArchive::LOAD);
		CSer serObj(archive);

		int serializationVersion;
		WORD vrepVersionThatWroteThis;
		int licenseTypeThatWroteThis;
		result=serObj.readOpen(serializationVersion,vrepVersionThatWroteThis,licenseTypeThatWroteThis);
		std::string infoPrintOut(tt::decorateString("",IDSNS_LOADING_SCENE," ("));
		infoPrintOut+=std::string(pathAndFilename)+"). ";
		if ((result==-3)&&displayMessages&&(App::mainWindow!=NULL))
		{
			App::uiThread->addStatusLine(IDS_NOT_VALID_V_REP_FILE);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_NOT_VALID_V_REP_FILE),VMessageBox::OKELI);
		}
		if ((result!=-3)&&displayMessages&&(App::mainWindow!=NULL))
		{
			infoPrintOut+=" ";
			infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
			infoPrintOut+=" ";
			infoPrintOut+=boost::lexical_cast<std::string>(serializationVersion)+".";
			App::uiThread->addStatusLine(infoPrintOut.c_str());
			infoPrintOut=_getStringOfVersionAndLicenseThatTheFileWasWrittenWith(vrepVersionThatWroteThis,licenseTypeThatWroteThis);
			if (infoPrintOut!="")
				App::uiThread->addStatusLine(infoPrintOut.c_str());
		}
		if ((result==-2)&&displayMessages&&(App::mainWindow!=NULL))
		{
			App::uiThread->addStatusLine(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE),VMessageBox::OKELI);
		}
		if ((result==-1)&&displayMessages&&(App::mainWindow!=NULL))
		{
			App::uiThread->addStatusLine(IDS_SERIALIZATION_VERSION_TOO_RECENT);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_TOO_RECENT),VMessageBox::OKELI);
		}
		if ((result==0)&&displayMessages&&(App::mainWindow!=NULL))
		{
			App::uiThread->addStatusLine(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED),VMessageBox::OKELI);
		}
		if (result==1)
		{
			App::ct->objCont->loadScene(serObj,false);
			serObj.readClose();
			if (App::mainWindow!=NULL)
				App::mainWindow->refreshDimensions(); // this is important so that the new pages and views are set to the correct dimensions
			if (displayMessages)
			{
				App::uiThread->addStatusLine(IDSNS_SCENE_OPENED);
				if (vrepVersionThatWroteThis>VREP_PROGRAM_VERSION_NB)
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SCENE),strTranslate(IDS_SAVED_WITH_MORE_RECENT_VERSION_WARNING),VMessageBox::OKELI);
			}
			std::string acknowledgement(App::ct->environment->getAcknowledgement());
			std::string tmp(acknowledgement);
			tt::removeSpacesAtBeginningAndEnd(tmp);
			if (displayMessages&&(App::mainWindow!=NULL))
			{
				if ( (tmp.length()!=0)&&(!CIloIlo::simulationShowEnabled) )
				{
					if (VThread::isCurrentThreadTheUiThread())
						App::mainWindow->uiThread_renderScene(true); // otherwise we won't see the new scene before the acknowledgment message
					App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDS_SCENE_CONTENT_ACKNOWLEDGMENTS),acknowledgement,VMessageBox::OKELI);
				}
			}
		}
		else
		{
			if (displayMessages)
				App::uiThread->addStatusLine(IDSNS_SCENE_COULD_NOT_BE_OPENED);
		}
		archive.close();
		file.close();
		App::ct->undoBufferContainer->memorizeState(); // so that we can come back to the initial state!
	}
	else
	{
		if (displayMessages)
			App::uiThread->addStatusLine(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
	}
	App::ct->hierarchy->setRebuildHierarchyFlag();
	return(result==1);
}

bool CFileMenu::loadModel(const char* pathAndFilename,bool displayMessages,bool setCurrentDir,bool noScreenRefresh,std::string* acknowledgmentPointerInReturn,bool doUndoThingInHere)
{ // if acknowledgment is NULL, then acknowledgments are directly displayed here!
	// This function, if called from the UI thread, will temporarily halt the simulation thread
	FUNCTION_DEBUG;
	if (App::isFullScreen())
		displayMessages=false;
	int result=-3;
	if (VFile::doesFileExist(pathAndFilename))
	{
		std::string theAcknowledgement;
		App::ct->objCont->deselectObjects();
		CIloIlo::setModelAcknowledgementOLD_20100714("");
		VFile file(pathAndFilename,VFile::READ|VFile::SHARE_DENY_NONE);
		VArchive archive(&file,VArchive::LOAD);
		if (setCurrentDir)
			App::directories->modelDirectory=App::directories->getPathFromFull(pathAndFilename);
		CSer serObj(archive);

		int serializationVersion;
		WORD vrepVersionThatWroteThis;
		int licenseTypeThatWroteThis;
		result=serObj.readOpen(serializationVersion,vrepVersionThatWroteThis,licenseTypeThatWroteThis);
		std::string infoPrintOut(tt::decorateString("",IDSNS_LOADING_MODEL," ("));
		infoPrintOut+=std::string(pathAndFilename)+"). ";
		if ((result==-3)&&displayMessages)
		{
			App::uiThread->addStatusLine(IDS_NOT_VALID_V_REP_FILE);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_NOT_VALID_V_REP_FILE),VMessageBox::OKELI);
		}
		if ((result!=-3)&&displayMessages)
		{
			infoPrintOut+=" ";
			infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
			infoPrintOut+=" ";
			infoPrintOut+=boost::lexical_cast<std::string>(serializationVersion)+".";
			App::uiThread->addStatusLine(infoPrintOut.c_str());
			infoPrintOut=_getStringOfVersionAndLicenseThatTheFileWasWrittenWith(vrepVersionThatWroteThis,licenseTypeThatWroteThis);
			if (infoPrintOut!="")
				App::uiThread->addStatusLine(infoPrintOut.c_str());
		}
		if ((result==-2)&&displayMessages)
		{
			App::uiThread->addStatusLine(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE),VMessageBox::OKELI);
		}
		if ((result==-1)&&displayMessages)
		{
			App::uiThread->addStatusLine(IDS_SERIALIZATION_VERSION_TOO_RECENT);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_TOO_RECENT),VMessageBox::OKELI);
		}
		if ((result==0)&&displayMessages)
		{
			App::uiThread->addStatusLine(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED),VMessageBox::OKELI);
		}
		bool removeLoadedModel=false;
		if (result==1)
		{
			App::ct->objCont->loadModel(serObj,false);
			serObj.readClose();
			if (displayMessages)
			{
				App::uiThread->addStatusLine(IDSNS_MODEL_LOADED);
				if (vrepVersionThatWroteThis>VREP_PROGRAM_VERSION_NB)
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_MODEL),strTranslate(IDS_MODEL_SAVED_WITH_MORE_RECENT_VERSION_WARNING),VMessageBox::OKELI);
			}
			if (!removeLoadedModel)
			{ // the first 3 lines are for backward compatibility (2010/07/14)
				std::string acknowledgement(CIloIlo::getModelAcknowledgementOLD_20100714());
				std::string tmp(acknowledgement);
				tt::removeSpacesAtBeginningAndEnd(tmp);
				if (tmp.length()==0)
				{ // now we search for the model base that contains the acknowledgment:
					std::vector<C3DObject*> loadedObjects;
					App::ct->objCont->getSelectedObjects(loadedObjects);
					for (int obba=0;obba<int(loadedObjects.size());obba++)
					{
						if (loadedObjects[obba]->getParent()==NULL)
						{
							acknowledgement=loadedObjects[obba]->getModelAcknowledgement();
							tmp=acknowledgement;
							tt::removeSpacesAtBeginningAndEnd(tmp);
							break;
						}
					}
				}
				if (tmp.length()!=0)
				{
					if (acknowledgmentPointerInReturn==NULL)
					{

						if (displayMessages)
							theAcknowledgement=acknowledgement;
					}
					else
						acknowledgmentPointerInReturn[0]=acknowledgement;
				}
			}
		}
		else
		{
			if (displayMessages)
				App::uiThread->addStatusLine(IDSNS_MODEL_COULD_NOT_BE_LOADED);
		}
		archive.close();
		file.close();
		if (removeLoadedModel)
		{
			std::vector<C3DObject*> toRemove;
			App::ct->objCont->getSelectedObjects(toRemove);
			App::ct->objCont->deselectObjects();
			for (int i=0;i<int(toRemove.size());i++)
				App::ct->objCont->eraseObject(toRemove[i]);
		}
		App::ct->objCont->removeFromSelectionAllExceptModelBase(false);
		App::ct->hierarchy->setRebuildHierarchyFlag();
		if (doUndoThingInHere)
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************

		if ((theAcknowledgement.length()!=0)&&displayMessages)
		{
			if (VThread::isCurrentThreadTheUiThread())
				App::mainWindow->uiThread_renderScene(true); // otherwise we won't see the new scene before the acknowledgment message
			App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDS_MODEL_CONTENT_ACKNOWLEDGMENTS),theAcknowledgement,VMessageBox::OKELI);
		}
	}
	else
	{
		if (displayMessages)
			App::uiThread->addStatusLine(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
	}
	return(result==1);
}
bool CFileMenu::saveUserInterfaces(const char* pathAndFilename,bool displayMessages,bool setCurrentDir,std::vector<int>* uiHandlesOrNullForAll)
{
	bool retVal=false; // means error
	if ((App::ct->buttonBlockContainer!=NULL)&&(App::ct->buttonBlockContainer->getUnassociatedNonSystemBlockCount()!=0))
	{ // ok, we have something to save!
		VFile myFile(pathAndFilename,VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
		myFile.setLength(0);
		VArchive archive(&myFile,VArchive::STORE);
		CSer ar(archive);

		std::string infoPrintOut(IDSNS_SAVING_CUSTOM_USER_INTERFACES);
		infoPrintOut+=" (";
		infoPrintOut+=std::string(pathAndFilename)+"). ";
		infoPrintOut+=" ";
		infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
		infoPrintOut+=" ";
		infoPrintOut+=boost::lexical_cast<std::string>(CSer::SER_SERIALIZATION_VERSION)+".";
		if (displayMessages)
			App::uiThread->addStatusLine(infoPrintOut.c_str());
		
		//**********************************
		ar.writeOpen();
		std::vector<CTextureProperty*> allTextureProperties;
		bool somethingWasWritten=false;
		for (int i=0;i<int(App::ct->buttonBlockContainer->allBlocks.size());i++)
		{
			if ((App::ct->buttonBlockContainer->allBlocks[i]->getAttributes()&sim_ui_property_systemblock)==0)
			{
				bool takeIt=false;
				if (uiHandlesOrNullForAll==NULL)
					takeIt=true;
				if (!takeIt)
				{
					for (int j=0;j<int(uiHandlesOrNullForAll->size());j++)
					{
						if (uiHandlesOrNullForAll->at(j)==App::ct->buttonBlockContainer->allBlocks[i]->getBlockID())
						{
							takeIt=true;
							break;
						}
					}
				}
				if (takeIt)
				{
					somethingWasWritten=true;
					ar.storeDataName(SER_BUTTON_BLOCK);
					ar.setCountingMode();
					App::ct->buttonBlockContainer->allBlocks[i]->serialize(ar);
					if (ar.setWritingMode())
						App::ct->buttonBlockContainer->allBlocks[i]->serialize(ar);
					App::ct->buttonBlockContainer->allBlocks[i]->getAllAttachedTextureProperties(allTextureProperties);
				}
			}
		}
		if (somethingWasWritten)
		{
			// Now make sure that every texture object appears no more than once and is not linked to any vision sensor, then serialize the related texture object:
			std::vector<CTextureProperty*> tpc(allTextureProperties);
			allTextureProperties.clear();
			for (int i=0;i<int(tpc.size());i++)
			{
				bool present=false;
				for (int j=0;j<int(allTextureProperties.size());j++)
				{
					if (allTextureProperties[j]->getTextureObjectID()==tpc[i]->getTextureObjectID())
					{
						present=true;
						break;
					}
				}
				if (!present)
				{
					allTextureProperties.push_back(tpc[i]);
					int objid=tpc[i]->getTextureObjectID();
					CTextureObject* to=App::ct->textureCont->getObject(objid);
					if (to!=NULL)
					{
						ar.storeDataName(SER_TEXTURE);
						ar.setCountingMode();
						to->serialize(ar);
						if (ar.setWritingMode())
							to->serialize(ar);
					}

				}
			}
			ar.storeDataName(SER_END_OF_FILE);
			ar.writeClose(App::userSettings->compressFiles);
			//**********************************
			retVal=true;

			if (displayMessages)
				App::uiThread->addStatusLine(IDSNS_DONE);
		}
		archive.close();
		myFile.close();
	}
	return(retVal);
}

bool CFileMenu::loadUserInterfaces(const char* pathAndFilename,bool displayMessages,bool setCurrentDir,std::vector<int>* uiHandles,bool doUndoThingInHere)
{
	bool retVal=false;
	if (uiHandles!=NULL)
		uiHandles->clear();
	if (VFile::doesFileExist(pathAndFilename))
	{
		App::ct->objCont->deselectObjects();
		if (setCurrentDir)
			App::directories->uiDirectory=App::directories->getPathFromFull(pathAndFilename);
		VFile file(pathAndFilename,VFile::READ|VFile::SHARE_DENY_NONE);
		VArchive archive(&file,VArchive::LOAD);
		CSer ar(archive);
		int serializationVersion;
		WORD vrepVersionThatWroteThis;
		int licenseTypeThatWroteThis;
		int result=ar.readOpen(serializationVersion,vrepVersionThatWroteThis,licenseTypeThatWroteThis);

		std::string infoPrintOut(tt::decorateString("",IDSNS_LOADING_UI," ("));
		infoPrintOut+=std::string(pathAndFilename)+"). ";

		if ((result==-3)&&displayMessages)
		{
			App::uiThread->addStatusLine(IDS_NOT_VALID_V_REP_FILE);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_NOT_VALID_V_REP_FILE),VMessageBox::OKELI);
		}
		if ((result!=-3)&&displayMessages)
		{
			infoPrintOut+=" ";
			infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
			infoPrintOut+=" ";
			infoPrintOut+=boost::lexical_cast<std::string>(serializationVersion)+".";
			App::uiThread->addStatusLine(infoPrintOut.c_str());
			infoPrintOut=_getStringOfVersionAndLicenseThatTheFileWasWrittenWith(vrepVersionThatWroteThis,licenseTypeThatWroteThis);
			if (infoPrintOut!="")
				App::uiThread->addStatusLine(infoPrintOut.c_str());
		}
		if ((result==-2)&&displayMessages)
		{
			App::uiThread->addStatusLine(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_NOT_SUPPORTED_ANYMORE),VMessageBox::OKELI);
		}
		if ((result==-1)&&displayMessages)
		{
			App::uiThread->addStatusLine(IDS_SERIALIZATION_VERSION_TOO_RECENT);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_SERIALIZATION_VERSION_TOO_RECENT),VMessageBox::OKELI);
		}
		if ((result==0)&&displayMessages)
		{
			App::uiThread->addStatusLine(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED);
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_SERIALIZATION),strTranslate(IDS_COMPRESSION_SCHEME_NOT_SUPPORTED),VMessageBox::OKELI);
		}
		if (result==1)
		{
			std::vector<CButtonBlock*> loadedButtonBlockList;
			std::vector<CTextureObject*> loadedTextureList;
			int byteQuantity;
			std::string theName="";
			while (theName.compare(SER_END_OF_FILE)!=0)
			{
				theName=ar.readDataName();
				if (theName.compare(SER_END_OF_FILE)!=0)
				{
					bool noHit=true;
					if (theName.compare(SER_END_OF_OBJECT)==0) // those 2 lines can probably be removed..
						noHit=false;
					if (theName.compare(SER_BUTTON_BLOCK)==0)
					{
						ar >> byteQuantity;
						CButtonBlock* it=new CButtonBlock(1,1,10,10,0);
						it->serialize(ar);
						loadedButtonBlockList.push_back(it);
						noHit=false;
					}
					if (theName.compare(SER_TEXTURE)==0)
					{
						ar >> byteQuantity;
						CTextureObject* it=new CTextureObject();
						it->serialize(ar);
						loadedTextureList.push_back(it);
						noHit=false;
					}
					if (noHit)
						ar.loadUnknownData();
				}
			}
			bool addUis=true;

			if (displayMessages)
			{
				App::uiThread->addStatusLine(IDSNS_UI_LOADED);
				if (vrepVersionThatWroteThis>VREP_PROGRAM_VERSION_NB)
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_CUSTOM_USER_INTERFACES),strTranslate(IDS_SAVED_WITH_MORE_RECENT_VERSION_WARNING),VMessageBox::OKELI);
			}

			if (addUis)
			{
				// We add the 2Delements to the container and do the mapping:
				std::vector<int> buttonBlockMapping;
				std::vector<int> textureObjectMapping;
				for (int i=0;i<int(loadedButtonBlockList.size());i++)
				{
					buttonBlockMapping.push_back(loadedButtonBlockList[i]->getBlockID()); // Old ID
					App::ct->buttonBlockContainer->insertBlock(loadedButtonBlockList[i],false);
					buttonBlockMapping.push_back(loadedButtonBlockList[i]->getBlockID()); // New ID
					if (uiHandles!=NULL)
						uiHandles->push_back(loadedButtonBlockList[i]->getBlockID());
				}
				App::ct->objCont->prepareFastLoadingMapping(buttonBlockMapping);

				for (int i=0;i<int(loadedTextureList.size());i++)
				{
					textureObjectMapping.push_back(loadedTextureList[i]->getObjectID()); // Old ID
					int nID=App::ct->textureCont->addObject(loadedTextureList[i],false);
					textureObjectMapping.push_back(nID); // New ID
				}
				App::ct->objCont->prepareFastLoadingMapping(textureObjectMapping);

				for (int i=0;i<int(loadedButtonBlockList.size());i++)
				{
					loadedButtonBlockList[i]->removeAllVisionSensorTextures();
					loadedButtonBlockList[i]->removeAllObjectAttachements();
					loadedButtonBlockList[i]->performTextureObjectLoadingMapping(&textureObjectMapping);
				}

				// We set ALL texture object dependencies (not just for loaded objects):
				App::ct->textureCont->clearAllDependencies();
				App::ct->buttonBlockContainer->setTextureDependencies();
				for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
				{
					CShape* it=App::ct->objCont->getShape(App::ct->objCont->shapeList[i]);
					it->geomData->setTextureDependencies(it->getID());
				}

				retVal=true;
			}
			else
			{
				for (int i=0;i<int(loadedButtonBlockList.size());i++)
					delete loadedButtonBlockList[i];
				for (int i=0;i<int(loadedTextureList.size());i++)
					delete loadedTextureList[i];
				retVal=false;
			}

			ar.readClose();
		}
		else
		{
			if (displayMessages)
				App::uiThread->addStatusLine(IDSNS_UI_COULD_NOT_BE_LOADED);
		}
		archive.close();
		file.close();

		// We need to make sure texture dependencies are ok:
		App::ct->textureCont->clearAllDependencies();
		for (int j=0;j<int(App::ct->objCont->shapeList.size());j++)
			App::ct->objCont->getShape(App::ct->objCont->shapeList[j])->geomData->setTextureDependencies(App::ct->objCont->shapeList[j]);
		App::ct->buttonBlockContainer->setTextureDependencies();

		if (doUndoThingInHere)
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	}
	else
	{
		if (displayMessages)
			App::uiThread->addStatusLine(IDSNS_ABORTED_FILE_DOES_NOT_EXIST);
	}
	return(retVal);
}

bool CFileMenu::saveScene(const char* pathAndFilename,bool displayMessages,bool setCurrentDir)
{ // There is a similar routine in CUndoBuffer!!

	App::ct->scintillaEditorContainer->applyChanges();
	void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_scenesave,NULL,NULL,NULL);
	delete[] (char*)returnVal;
	VFile myFile(pathAndFilename,VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE,true);
	if (myFile.getFile()==NULL)
	{ // to avoid an error when saving a file that was opened while still attached to an email for instance
		App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_FILE_ACCESS),strTranslate(IDSN_ACCESS_TO_FILE_WAS_DENIED),VMessageBox::OKELI);
		return(false);
	}
	App::ct->mainSettings->scenePathAndName=pathAndFilename;
	if (setCurrentDir)
		App::directories->sceneDirectory=App::directories->getPathFromFull(App::ct->mainSettings->scenePathAndName.c_str());
	myFile.setLength(0);
	VArchive archive(&myFile,VArchive::STORE);
	CSer serObj(archive);

	std::string infoPrintOut(IDSN_SAVING_SCENE);
	infoPrintOut+=" (";
	infoPrintOut+=std::string(pathAndFilename)+"). ";
	infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
	infoPrintOut+=" ";
	infoPrintOut+=boost::lexical_cast<std::string>(CSer::SER_SERIALIZATION_VERSION)+".";
	if (displayMessages)
		App::uiThread->addStatusLine(infoPrintOut.c_str());

	serObj.writeOpen();
	App::ct->objCont->saveScene(serObj);
	serObj.writeClose(App::userSettings->compressFiles);

	if (displayMessages)
		App::uiThread->addStatusLine(IDSNS_SCENE_WAS_SAVED);

	archive.close();
	myFile.close();

	App::ct->hierarchy->setRebuildHierarchyFlag(); // we might have saved under a different name, we need to reflect it

	return(true);
}

bool CFileMenu::saveModel(int modelBaseDummyID,const char* pathAndFilename,bool displayMessages,bool setCurrentDir)
{
	bool retVal=false;
	std::vector<int> sel;
	sel.push_back(modelBaseDummyID);
	App::ct->scintillaEditorContainer->applyChanges();
	if (sel.size()>0)
	{
		// Following is very important for root objects!
		CObjectEdition::addRootObjectChildrenToSelection(sel);
		void* plugRetVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_modelsave,NULL,NULL,NULL);
		delete[] (char*)plugRetVal;
		VFile myFile(pathAndFilename,VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
		myFile.setLength(0);
		VArchive archive(&myFile,VArchive::STORE);
		if (setCurrentDir)
			App::directories->modelDirectory=App::directories->getPathFromFull(pathAndFilename);
		CSer serObj(archive);

		std::string infoPrintOut(IDSNS_SAVING_MODEL);
		infoPrintOut+=" (";
		infoPrintOut+=std::string(pathAndFilename)+"). ";
		infoPrintOut+=IDSNS_SERIALIZATION_VERSION_IS;
		infoPrintOut+=" ";
		infoPrintOut+=boost::lexical_cast<std::string>(CSer::SER_SERIALIZATION_VERSION)+".";
		if (displayMessages)
			App::uiThread->addStatusLine(infoPrintOut.c_str());

		serObj.writeOpen();
		App::ct->copyBuffer->serializeCurrentSelection(serObj,&sel);
		serObj.writeClose(App::userSettings->compressFiles);

		if (displayMessages)
			App::uiThread->addStatusLine(IDSNS_MODEL_WAS_SAVED);

		archive.close();
		myFile.close();
		retVal=true;
	}
	return(retVal);
}

bool CFileMenu::_saveSceneWithDialogAndEverything()
{
	if (!App::ct->environment->getSceneLocked())
	{
		if (App::ct->mainSettings->scenePathAndName=="")
			return(_saveSceneAsWithDialogAndEverything());
		else
		{
			if ( (!App::ct->environment->getRequestFinalSave())||(VMessageBox::REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_FINAL_SCENE_SAVE_WARNING),VMessageBox::YES_NO)) )
			{
				if (App::ct->environment->getRequestFinalSave())
					App::ct->environment->setSceneLocked();
				std::string infoPrintOut(IDSN_SAVING_SCENE);
				infoPrintOut+="...";
				App::uiThread->addStatusLine(infoPrintOut.c_str());
				if (saveScene(App::ct->mainSettings->scenePathAndName.c_str(),true,true))
					_addToRecentlyOpenedScenes(App::ct->mainSettings->scenePathAndName);
			}
			return(true);
		}
	}
	else
	{
		App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SCENE),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMessageBox::OKELI);
		return(false);
	}
}

bool CFileMenu::_saveSceneAsWithDialogAndEverything()
{
	if (!App::ct->environment->getSceneLocked())
	{
		if ( (!App::ct->environment->getRequestFinalSave())||(VMessageBox::REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SAVE),strTranslate(IDS_FINAL_SCENE_SAVE_WARNING),VMessageBox::YES_NO)) )
		{
			if (App::ct->environment->getRequestFinalSave())
				App::ct->environment->setSceneLocked();
			std::string infoPrintOut(IDSN_SAVING_SCENE);
			infoPrintOut+="...";
			App::uiThread->addStatusLine(infoPrintOut.c_str());
			std::string initPath;
			if (App::ct->mainSettings->scenePathAndName.compare("")==0)
				initPath=App::directories->sceneDirectory;
			else
				initPath=App::directories->getPathFromFull(App::ct->mainSettings->scenePathAndName);
			std::string filenameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,tt::decorateString("",strTranslate(IDSN_SAVING_SCENE),"..."),initPath,App::ct->mainSettings->scenePathAndName,false,"V-REP Scene Files","ttt");
			if (filenameAndPath.length()!=0)
			{
				saveScene(filenameAndPath.c_str(),true,true);
				return(true);
			}
			else
			{
				App::uiThread->addStatusLine(IDSNS_ABORTED);
				return(false);
			}
		}
		else
			return(false);
	}
	else
	{
		App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SCENE),strTranslate(IDS_SCENE_IS_LOCKED_WARNING),VMessageBox::OKELI);
		return(false);
	}
}


std::string CFileMenu::_getStringOfVersionAndLicenseThatTheFileWasWrittenWith(WORD vrepVer,int licenseType)
{
	if (vrepVer==0)
		return("");
	std::string retStr;
	retStr=tt::decorateString("",IDSNS_FILE_WAS_PREVIOUSLY_WRITTEN_WITH_VREP_VERSION," ");
	int v=vrepVer;
	retStr+=char('0')+BYTE(v/10000);
	retStr+='.';
	v=v-(v/10000)*10000;
	retStr+=char('0')+BYTE(v/1000);
	v=v-(v/1000)*1000;
	retStr+=char('0')+BYTE(v/100);
	v=v-(v/100)*100;
	retStr+='.';
	retStr+=char('0')+BYTE(v/10);
	v=v-(v/10)*10;
	retStr+=char('0')+BYTE(v);
	if (licenseType!=-1)
	{
		licenseType=(licenseType|0x00040000)-0x00040000;
		if (licenseType==VREP_LICENSE_VREP_DEMO)
			retStr+=" (V-REP EVAL/STUDENT license)";
		if (licenseType==VREP_LICENSE_VREP_PRO_EDU)
			retStr+=" (V-REP PRO EDU license)";
		if ((licenseType==VREP_LICENSE_VREP_PRO)||(licenseType==VREP_LICENSE_VREP_PRO_P))
			retStr+=" (V-REP PRO license)";
		if (licenseType==VREP_LICENSE_VREP_SUBLICENSE)
			retStr+=" (custom V-REP license)";
		if (licenseType==VREP_LICENSE_VREP_PLAYER)
			retStr+=" (V-REP PLAYER license)";
		if (licenseType==VREP_LICENSE_VREP_NAKED)
			retStr+=" (V-REP license, custom compilation)";
	}
	return(retStr);
}

bool CFileMenu::heightfieldImportRoutine(const std::string& pathName)
{
	if (VFile::doesFileExist(pathName))
	{
		try
		{
			std::vector<std::vector<float>*> readData;
			// We read each line at a time, which gives rows:
			int minRow=-1;


			std::string ext(CTTUtil::getLowerCaseString(VVarious::splitPath_fileExtension(pathName).c_str()));
			if ((ext.compare("csv")!=0)&&(ext.compare("txt")!=0))
			{ // from image file
				int resX,resY,n;
				unsigned char* data=CImageLoader::load(pathName.c_str(),&resX,&resY,&n,3);
				if (data!=NULL)
				{
					if ( (resX>1)&&(resY>1) )
					{
						int bytesPerPixel=0;
						if (n==3)
							bytesPerPixel=3;
						if (n==4)
							bytesPerPixel=4;
						if (bytesPerPixel!=0)
						{
							for (int i=0;i<resY;i++)
							{
								std::vector<float>* lineVect=new std::vector<float>;
								for (int j=0;j<resX;j++)
									lineVect->push_back(float(data[bytesPerPixel*(i*resX+j)+0]+data[bytesPerPixel*(i*resX+j)+1]+data[bytesPerPixel*(i*resX+j)+2])/768.0f);
								readData.push_back(lineVect);
							}
							minRow=resX;
						}
					}
					delete[] data;
				}
				else
					App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSN_IMPORT),strTranslate(IDS_TEXTURE_FILE_COULD_NOT_BE_LOADED),VMessageBox::OKELI);
			}
			else
			{ // from csv or txt file:
				VFile file(pathName.c_str(),VFile::READ|VFile::SHARE_DENY_NONE);
				VArchive archive(&file,VArchive::LOAD);
				DWORD currentPos=0;
				std::string line;
				while (archive.readLine(currentPos,line,false))
				{
					std::string word;
					std::vector<float>* lineVect=new std::vector<float>;
					while (tt::extractCommaSeparatedWord(line,word))
					{
						tt::removeSpacesAtBeginningAndEnd(word);
						float val;
						if (tt::getValidFloat(word,val))
							lineVect->push_back(val);
						else
							break;
					}
					if (lineVect->size()!=0)
					{
						if ( (minRow==-1)||(int(lineVect->size())<minRow) )
							minRow=int(lineVect->size());
						readData.push_back(lineVect);
					}
					else
					{
						delete lineVect;
						break;
					}
				}
				archive.close();
				file.close();
			}
			if ( (readData.size()>1)&&(minRow>1) )
			{
				int xSize=minRow;
				int ySize=int(readData.size());
				CQDlgHeightfieldDimension theDialog(App::mainWindow);
				theDialog.xSize=10.0f;
				theDialog.xSizeTimesThisGivesYSize=float(ySize-1)/float(xSize-1);
				theDialog.ySize=theDialog.xSize*theDialog.xSizeTimesThisGivesYSize;
				theDialog.refresh();
				theDialog.makeDialogModal();

				float pointSpacing=theDialog.xSize/float(xSize-1);
				for (int i=0;i<int(readData.size());i++)
				{
					for (int j=0;j<int(readData[i]->size());j++)
					{
						readData[i]->at(j)*=theDialog.zScaling;
					}
				}
				int shapeHandle=apiAddHeightfieldToScene(xSize,pointSpacing,readData,0.0f,2);
				App::ct->objCont->deselectObjects();
				App::ct->objCont->addObjectToSelection(shapeHandle);

			}
			for (int i=0;i<int(readData.size());i++)
				delete readData[i];
			return(readData.size()!=0);
		}
		catch(VFILE_EXCEPTION_TYPE e)
		{
			VFile::reportAndHandleFileExceptionError(e);
			return(false);
		}
	}
	return(false);
}

int CFileMenu::apiAddHeightfieldToScene(int xSize,float pointSpacing,const std::vector<std::vector<float>*>& readData,float shadingAngle,int options)
{
	int ySize=int(readData.size());
	std::vector<float> allHeights;
	float maxHeight=-99999999.0f;
	float minHeight=+99999999.0f;
	for (int i=ySize-1;i>=0;i--)
	{
		for (int j=0;j<xSize;j++)
		{
			allHeights.push_back(readData[i]->at(j));
			if (readData[i]->at(j)>maxHeight)
				maxHeight=readData[i]->at(j);
			if (readData[i]->at(j)<minHeight)
				minHeight=readData[i]->at(j);
		}
	}
	CGeomProxy* geom=new CGeomProxy(allHeights,xSize,ySize,pointSpacing,maxHeight-minHeight);
	CShape* shape=new CShape();
	shape->setLocalTransformation(geom->getCreationTransformation());
	geom->setCreationTransformation(C7Vector::identityTransformation);
	shape->geomData=geom;
	App::ct->objCont->addObjectToScene(shape,false);
	shape->setCulling((options&1)!=0);
	shape->setVisibleEdges((options&2)!=0);
	((CGeometric*)shape->geomData->geomInfo)->setGouraudShadingAngle(shadingAngle);
	((CGeometric*)shape->geomData->geomInfo)->setEdgeThresholdAngle(shadingAngle);
	((CGeometric*)shape->geomData->geomInfo)->color.colors[0]=0.24f;
	((CGeometric*)shape->geomData->geomInfo)->color.colors[1]=0.18f;
	((CGeometric*)shape->geomData->geomInfo)->color.colors[2]=0.08f;
	((CGeometric*)shape->geomData->geomInfo)->color.colors[3]=1.0f;
	((CGeometric*)shape->geomData->geomInfo)->color.colors[4]=1.0f;
	((CGeometric*)shape->geomData->geomInfo)->color.colors[5]=1.0f;
	((CGeometric*)shape->geomData->geomInfo)->color.colors[6]=0.0f;
	((CGeometric*)shape->geomData->geomInfo)->color.colors[7]=0.0f;
	((CGeometric*)shape->geomData->geomInfo)->color.colors[8]=0.0f;
	std::string tempName(IDSOGL_HEIGHTFIELD);
	while (App::ct->objCont->getObject(tempName)!=NULL)
		tempName=tt::generateNewName_noDash(tempName);
	shape->setName(tempName);
	shape->alignBoundingBoxWithWorld();

	int propToRemove=sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable;
	shape->setMainPropertyLocal((shape->getMainPropertyLocal()|propToRemove)-propToRemove);
	shape->setRespondable(true);
	shape->setShapeIsDynamicallyStatic(true);

	return(shape->getID());
}

void CFileMenu::_addToRecentlyOpenedScenes(std::string filenameAndPath)
{
	CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
	std::string recentScenes[10];
	int sameIndex=-1;
	for (int i=0;i<10;i++)
	{
		std::string tmp("SIMSETTINGS_RECENTSCENE0");
		tmp[23]=48+i;
		cont.readData(tmp.c_str(),recentScenes[i]);
		if (recentScenes[i].compare(filenameAndPath)==0)
			sameIndex=i;
	}
	if (sameIndex==-1)
	{
		for (int i=8;i>=0;i--)
			recentScenes[i+1]=recentScenes[i];
	}
	else
	{
		for (int i=sameIndex;i>0;i--)
			recentScenes[i]=recentScenes[i-1];
	}
	recentScenes[0]=filenameAndPath;
	int cnt=0;
	for (int i=0;i<10;i++)
	{
		if (recentScenes[i].length()>3)
		{
			std::string tmp("SIMSETTINGS_RECENTSCENE0");
			tmp[23]=48+cnt;
			cont.writeData(tmp.c_str(),recentScenes[i],!App::userSettings->doNotWritePersistentData);
			cnt++;
		}
	}
}

void CFileMenu::_removeFromRecentlyOpenedScenes(std::string filenameAndPath)
{
	CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
	std::string recentScenes[10];
	int sameIndex=-1;
	for (int i=0;i<10;i++)
	{
		std::string tmp("SIMSETTINGS_RECENTSCENE0");
		tmp[23]=48+i;
		cont.readData(tmp.c_str(),recentScenes[i]);
		if (recentScenes[i].compare(filenameAndPath)==0)
			sameIndex=i;
	}
	if (sameIndex!=-1)
	{
		for (int i=sameIndex;i<9;i++)
			recentScenes[i]=recentScenes[i+1];
		recentScenes[9]="";
		int cnt=0;
		for (int i=0;i<10;i++)
		{
			if (recentScenes[i].length()>3)
			{
				std::string tmp("SIMSETTINGS_RECENTSCENE0");
				tmp[23]=48+cnt;
				cont.writeData(tmp.c_str(),recentScenes[i],!App::userSettings->doNotWritePersistentData);
				cnt++;
			}
		}
		for (int i=cnt;i<10;i++)
		{
			std::string tmp("SIMSETTINGS_RECENTSCENE0");
			tmp[23]=48+i;
			cont.writeData(tmp.c_str(),"",!App::userSettings->doNotWritePersistentData);
		}
	}
}
