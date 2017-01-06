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
#include "ScintillaEditorContainer.h"
#include "v_rep_internal.h"

#include "IloIlo.h"
#include "VVarious.h"
#include "App.h"

CScintillaEditorContainer::CScintillaEditorContainer()
{

}

CScintillaEditorContainer::~CScintillaEditorContainer()
{
	closeAllEditors();
}

void CScintillaEditorContainer::newSceneProcedure()
{

}

void CScintillaEditorContainer::simulationAboutToStart()
{

}

void CScintillaEditorContainer::simulationEnded()
{
}

void CScintillaEditorContainer::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

bool CScintillaEditorContainer::openEditorForScript(int scriptID)
{
	if (App::userSettings->useBuiltInScriptEditor())
	{
		for (int i=0;i<int(_allEditors.size());i++)
		{
			if (_allEditors[i]->getScriptID()==scriptID)
				return(true); // already open!
		}
	}
	CScintillaEditor* editor=new CScintillaEditor();
	if (editor->initialize(scriptID))
	{
		if (App::userSettings->useBuiltInScriptEditor())
			_allEditors.push_back(editor);
		else
		{
			delete editor;
			VVarious::executeExternalApplication(App::userSettings->externalScriptEditor,App::ct->luaScriptContainer->getScriptFromID(scriptID)->getFilenameForExternalScriptEditor().c_str(),App::directories->executableDirectory,VVARIOUS_SHOWNORMAL); // executable directory needed because otherwise the shellExecute command might switch directories!
		}
		return(true);
	}
	delete editor;
	return(false);
}

void CScintillaEditorContainer::closeAllEditors()
{
	for (int i=0;i<int(_allEditors.size());i++)
	{
		delete _allEditors[i];
	}
	_allEditors.clear();
}

bool CScintillaEditorContainer::closeEditor(int scriptID)
{ // false means the script was not opened nor hidden
	for (int i=0;i<int(_allEditors.size());i++)
	{
		if (_allEditors[i]->getScriptID()==scriptID)
		{
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptID);
			if (it!=NULL)
			{
				int posAndSize[4];
				_allEditors[i]->getWindowPosAndSize(posAndSize);
				it->setPreviousEditionWindowPosAndSize(posAndSize);
			}
			delete _allEditors[i];
			_allEditors.erase(_allEditors.begin()+i);
			return(true);
		}
	}
	return(false);
}

void CScintillaEditorContainer::requestClosing(void* dialogPointer)
{
	for (int i=0;i<int(_allEditors.size());i++)
		_allEditors[i]->requestClosing(dialogPointer);
}

void CScintillaEditorContainer::applyChanges()
{
	if (App::userSettings->useBuiltInScriptEditor())
	{
		for (int i=0;i<int(_allEditors.size());i++)
			_allEditors[i]->applyChanges();
	}
	else
	{
		for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
			App::ct->luaScriptContainer->allScripts[i]->fromFileToBuffer();
	}
}

bool CScintillaEditorContainer::areWindowsOpen()
{
	return(_allEditors.size()!=0);
}

void CScintillaEditorContainer::showOrHideAllEditors(bool show)
{
	for (int i=0;i<int(_allEditors.size());i++)
	{
		if (show)
			_allEditors[i]->showEditorButDontOpenIt();
		else
			_allEditors[i]->hideEditorButDontCloseIt();
	}
}

void CScintillaEditorContainer::updateWindowsExceptContentText()
{
	for (int i=0;i<int(_allEditors.size());i++)
	{
		if (_allEditors[i]->updateWindowExceptContentText())
		{
			closeEditor(_allEditors[i]->getScriptID());
			i--; // We need to reprocess this position
		}
	}
}
