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
#include "undoBufferCont.h"

#include "IloIlo.h"
#include "pluginContainer.h"
#include "VDateTime.h"
#include "App.h"
#include "v_repStrings.h"

CUndoBufferCont::CUndoBufferCont()
{
	_commonInit();
}

CUndoBufferCont::~CUndoBufferCont()
{
	for (int i=0;i<int(_buffers.size());i++)
		delete _buffers[i];
	undoBufferArrays.clearAll();
}

int CUndoBufferCont::getNextBufferId()
{
	return(_nextBufferId);
}

void CUndoBufferCont::newSceneProcedure()
{
	_commonInit();
}

void CUndoBufferCont::simulationAboutToStart()
{
}

void CUndoBufferCont::simulationEnded()
{
}

void CUndoBufferCont::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{
}

void CUndoBufferCont::_commonInit()
{
	_currentStateIndex=-1;
	for (int i=0;i<int(_buffers.size());i++)
		delete _buffers[i];
	_buffers.clear();
	undoBufferArrays.clearAll();
	_announceChangeStartCalled=false;
	_announceChangeGradualCalledTime=-1;
	_sceneSaveMightBeNeeded=false;
	_undoPointSavingOrRestoringUnderWay=false;
	_inUndoRoutineNow=false;
	_nextBufferId=0;
}

bool CUndoBufferCont::isUndoSavingOrRestoringUnderWay()
{
	return(_undoPointSavingOrRestoringUnderWay);
}

bool CUndoBufferCont::isSceneSaveMaybeNeededFlagSet()
{
	return(_sceneSaveMightBeNeeded);
}

void CUndoBufferCont::clearSceneSaveMaybeNeededFlag()
{
	_sceneSaveMightBeNeeded=false;
}

bool CUndoBufferCont::announceChange()
{
	if (!_inUndoRoutineNow)
	{
		bool retVal=memorizeState();
		_announceChangeGradualCalledTime=-1;
		return(retVal);
	}
	return(false);
}

void CUndoBufferCont::announceChangeStart()
{
	if (!_inUndoRoutineNow)
		_announceChangeStartCalled=true;
}

void CUndoBufferCont::announceChangeEnd()
{
	if (!_inUndoRoutineNow)
	{
		if (_announceChangeStartCalled)
		{
			memorizeState();
			_announceChangeGradualCalledTime=-1;
		}
		_announceChangeStartCalled=false;
	}
}

void CUndoBufferCont::announceChangeGradual()
{
	if (!_inUndoRoutineNow)
	{
		if (_announceChangeGradualCalledTime==-1)
			_announceChangeGradualCalledTime=VDateTime::getTimeInMs();
	}
}

void CUndoBufferCont::memorizeStateIfNeeded()
{
	if (!_inUndoRoutineNow)
	{
		if (_announceChangeGradualCalledTime==-1)
			return;
		if (VDateTime::getTimeDiffInMs(_announceChangeGradualCalledTime)>5000)
		{
			memorizeState();
			_announceChangeGradualCalledTime=-1;
		}
	}
}

void CUndoBufferCont::emptyRedoBuffer()
{
	while (int(_buffers.size())>_currentStateIndex+1)
	{
		undoBufferArrays.removeDependenciesFromUndoBufferId(_buffers[_buffers.size()-1]->getBufferId());
		delete _buffers[_buffers.size()-1];
		_buffers.pop_back();
		App::ct->objCont->toolbarRefreshFlag=true;
	}
}

bool CUndoBufferCont::memorizeState()
{
	if (_inUndoRoutineNow)
		return(false);
	if (!App::ct->simulation->isSimulationStopped())
		return(false);
	// If we arrived here, we might have something to save:
	_sceneSaveMightBeNeeded=true;
	if (!_isGoodToMemorizeUndoOrRedo())
		return(false);
	static int tooLongExecutionCount=0;
	int startTime=VDateTime::getTimeInMs();
	bool retVal=true;
	std::vector<std::string> sel;
	for (int i=0;i<App::ct->objCont->getSelSize();i++)
		sel.push_back(App::ct->objCont->getObject(App::ct->objCont->getSelID(i))->getName());

	App::ct->scintillaEditorContainer->applyChanges();
	std::vector<char> newBuff;
	CSer serObj(newBuff);
	serObj.disableCountingModeExceptForExceptions();

	serObj.writeOpen();
	_undoPointSavingOrRestoringUnderWay=true;
	App::ct->objCont->saveScene(serObj); // This takes the 90% of time of the whole routine
	_undoPointSavingOrRestoringUnderWay=false;
	serObj.writeClose(false); // We don't wanna compression

	CUndoBuffer* it=new CUndoBuffer(newBuff,sel,_nextBufferId++);
	if (_currentStateIndex==-1)
	{ // first buffer, we just add it
		_buffers.push_back(it);
		_currentStateIndex=0;
	}
	else
	{ // We check with previous buffer:
		std::vector<char> prevBuff;
		std::vector<std::string> prevSelection;
		_getFullBuffer(_currentStateIndex,prevBuff,prevSelection);
		if (it->finalize(prevBuff,prevSelection))
		{ // different from previous, we remove forward buffers and add this one:
			while (int(_buffers.size())>_currentStateIndex+1)
			{
				undoBufferArrays.removeDependenciesFromUndoBufferId(_buffers[_buffers.size()-1]->getBufferId());
				delete _buffers[_buffers.size()-1];
				_buffers.pop_back();
			}
			_buffers.push_back(it);
			_currentStateIndex++;
		}
		else
		{
			undoBufferArrays.removeDependenciesFromUndoBufferId(it->getBufferId());
			delete it; // same as previous, we delete it
			retVal=false;
		}
	}

	std::vector<char> fullBuff;
	while ( (_getUsedMemory()+undoBufferArrays.getMemorySizeInBytes()>App::userSettings->undoRedoMaxBufferSize)||(int(_buffers.size())>App::userSettings->undoRedoLevelCount) )
	{ // We have to remove a few states at the beginning
		if (int(_buffers.size())<3)
			break; // at least 3 states!
		std::vector<std::string> dummySelection;
		_getFullBuffer(1,fullBuff,dummySelection);
		CUndoBuffer* it=_buffers[1];
		it->updateWithFullBuffer(fullBuff);
		undoBufferArrays.removeDependenciesFromUndoBufferId(_buffers[0]->getBufferId());
		delete _buffers[0];
		_buffers.erase(_buffers.begin());
		_currentStateIndex--;
	}
	if (VDateTime::getTimeDiffInMs(startTime)>App::userSettings->timeInMsForUndoRedoTooLongWarning)
		tooLongExecutionCount++;
	else
		tooLongExecutionCount=0;

	if (tooLongExecutionCount>=5)
	{
		static bool displayedMessage=false;
		if (!displayedMessage)
		{
			if (VMessageBox::REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Undo / redo"),strTranslate(IDS_INFO_UNDO_REDO_TAKES_TOO_LONG),VMessageBox::YES_NO))
				App::userSettings->setUndoRedoEnabled(false);
			displayedMessage=true;
		}
		tooLongExecutionCount=0;
	}

	App::ct->objCont->toolbarRefreshFlag=true;
	return(retVal);
}

bool CUndoBufferCont::_isGoodToMemorizeUndoOrRedo()
{
	if (!App::ct->simulation->isSimulationStopped())
		return(false);
	if ( (App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)&&(App::ct->objCont->getEditModeType()!=BUTTON_EDIT_MODE) )
		return(false);
	if (App::ct->oglSurface->isPageSelectionActive())
		return(false);
	if (App::ct->oglSurface->isViewSelectionActive())
		return(false);
	if (App::ct->oglSurface->isSceneSelectionActive())
		return(false);
	if (!App::userSettings->getUndoRedoEnabled())
		return(false);
	return(true);
}

bool CUndoBufferCont::canUndo()
{
	if (!_isGoodToMemorizeUndoOrRedo())
		return(false);
	return(_currentStateIndex>0);
}

bool CUndoBufferCont::canRedo()
{
	if (!_isGoodToMemorizeUndoOrRedo())
		return(false);
	return(_currentStateIndex<=(int(_buffers.size())-2));
}

void CUndoBufferCont::undo()
{
	if (_inUndoRoutineNow)
		return;

	if (App::ct->scintillaEditorContainer->areWindowsOpen())
	{
		App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_UNDO_REDO),strTranslate(IDS_UNDO_REDO_WITH_OPEN_SCRIPT_EDITOR_MESSAGE),VMessageBox::OKELI);
		return;
	}

	_inUndoRoutineNow=true;
	// 1. We memorize this position: NOOOOOOO!!! Can cause subtle errors!! 
	if (_announceChangeGradualCalledTime!=-1)
	{
		//	memorizeState();
		_announceChangeGradualCalledTime=-1;
	}
	if (_currentStateIndex<1)
	{
		_inUndoRoutineNow=false;
		return; // nothing to undo
	}

	// 2. We go back:
	_currentStateIndex--;
	std::vector<char> theBuff;
	std::vector<std::string> theSelection;
	_getFullBuffer(_currentStateIndex,theBuff,theSelection);
	// 3. Load the buffer:

	void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_abouttoundo,NULL,NULL,NULL);
	delete[] (char*)returnVal;

	/*
	if (App::ct->scintillaEditorContainer->areWindowsOpen())
	{
		App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_UNDO_REDO),strTranslate(IDS_UNDO_REDO_WITH_OPEN_SCRIPT_EDITOR_MESSAGE),VMessageBox::OKELI);
		App::ct->scintillaEditorContainer->closeAllEditors();
	}
	*/

	App::ct->objCont->deselectObjects();
	App::ct->simulation->stopSimulation(); // should be anyway stopped!
	App::ct->emptyScene(false);

	CSer serObj(theBuff);
	int serializationVersion;
	WORD dum0;
	int dum1;
	serObj.readOpen(serializationVersion,dum0,dum1);
	_undoPointSavingOrRestoringUnderWay=true;
	App::ct->objCont->loadScene(serObj,true);
	_undoPointSavingOrRestoringUnderWay=false;
	serObj.readClose();
	if (App::mainWindow!=NULL)
		App::mainWindow->refreshDimensions(); // this is important so that the new pages and views are set to the correct dimensions
	// 4. We select previously selected objects:
	for (int i=0;i<int(theSelection.size());i++)
	{
		C3DObject* theObj=App::ct->objCont->getObject(theSelection[i]);
		if (theObj!=NULL)
			App::ct->objCont->addObjectToSelection(theObj->getID());
	}
	_inUndoRoutineNow=false;
	
	returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_undoperformed,NULL,NULL,NULL);
	delete[] (char*)returnVal;
	App::ct->setModificationFlag(16); // undo called

	// 5. Dialog refresh:
	App::ct->objCont->setFullDialogRefreshFlag();
	App::ct->objCont->toolbarRefreshFlag=true;
}

void CUndoBufferCont::redo()
{
	if (_currentStateIndex>(int(_buffers.size())-2))
		return; // nothing to redo

	if (App::ct->scintillaEditorContainer->areWindowsOpen())
	{
		App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_UNDO_REDO),strTranslate(IDS_UNDO_REDO_WITH_OPEN_SCRIPT_EDITOR_MESSAGE),VMessageBox::OKELI);
		return;
	}

	// 2. We go forward:
	_currentStateIndex++;
	std::vector<char> theBuff;
	std::vector<std::string> theSelection;
	_getFullBuffer(_currentStateIndex,theBuff,theSelection);
	// 3. Load the buffer:

	void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_abouttoredo,NULL,NULL,NULL);
	delete[] (char*)returnVal;

/*
	if (App::ct->scintillaEditorContainer->areWindowsOpen())
	{
		App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_UNDO_REDO),strTranslate(IDS_UNDO_REDO_WITH_OPEN_SCRIPT_EDITOR_MESSAGE),VMessageBox::OKELI);
		App::ct->scintillaEditorContainer->closeAllEditors();
	}
*/

	App::ct->objCont->deselectObjects();
	App::ct->simulation->stopSimulation(); // should be anyway stopped!
	App::ct->emptyScene(false);

	CSer serObj(theBuff);
	int serializationVersion;
	WORD dum0;
	int dum1;
	serObj.readOpen(serializationVersion,dum0,dum1);
	_undoPointSavingOrRestoringUnderWay=true;
	App::ct->objCont->loadScene(serObj,true);
	_undoPointSavingOrRestoringUnderWay=false;
	serObj.readClose();
	if (App::mainWindow!=NULL)
		App::mainWindow->refreshDimensions(); // this is important so that the new pages and views are set to the correct dimensions


	// 4. We select previously selected objects:
	for (int i=0;i<int(theSelection.size());i++)
	{
		C3DObject* theObj=App::ct->objCont->getObject(theSelection[i]);
		if (theObj!=NULL)
			App::ct->objCont->addObjectToSelection(theObj->getID());
	}

	returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_redoperformed,NULL,NULL,NULL);
	delete[] (char*)returnVal;
	App::ct->setModificationFlag(32); // redo called

	// 5. Dialog refresh:
	App::ct->objCont->setFullDialogRefreshFlag();
	App::ct->objCont->toolbarRefreshFlag=true;
}

void CUndoBufferCont::_getFullBuffer(int index,std::vector<char>& fullBuff,std::vector<std::string>& selectedObjectNames)
{
	if ( (index>=int(_buffers.size()))||(index<0) )
	{
		fullBuff.clear();
		selectedObjectNames.clear();
		return;
	}
	if (index==0)
	{ // the first one
		_buffers[index]->getRestored(NULL,fullBuff,selectedObjectNames);
	}
	else
	{
		std::vector<char> prevBuff;
		_getFullBuffer(index-1,prevBuff,selectedObjectNames);
		_buffers[index]->getRestored(&prevBuff,fullBuff,selectedObjectNames);
	}
}

int CUndoBufferCont::_getUsedMemory()
{
	int retVal=0;
	for (int i=0;i<int(_buffers.size());i++)
		retVal+=int(_buffers[i]->buffer.size());
	return (retVal);
}
