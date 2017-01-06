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
#include "ScintillaEditor.h"
#include "ScintillaDlg.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscistyle.h>
#include "IloIlo.h"

#include "v_rep_internal.h"
#include "v_repStrings.h"
#include "App.h"

CScintillaEditor::CScintillaEditor()
{
	_dlg=NULL;
	_scriptID=-1;
	_scriptLifeID=-1;
	_closeRequest=false;
	_currentWindowTitle="";
}

CScintillaEditor::~CScintillaEditor() 
{
	_closeDlgWindow(true);
}

int CScintillaEditor::getScriptID()
{
	return(_scriptID);
}

void CScintillaEditor::showEditorButDontOpenIt()
{
	if (_dlg!=NULL)
		_dlg->setShowWindow(true);
}

void CScintillaEditor::hideEditorButDontCloseIt()
{
	if (_dlg!=NULL)
		_dlg->setShowWindow(false);
}


bool CScintillaEditor::initialize(int scriptID)
{
	if ((App::operationalUIParts&sim_gui_scripteditor)==0)
		return(false);
	// Make sure that scripts cannot be viewed/edited with locked scenes:
	if (App::ct->environment->getSceneLocked())
	{
		App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Script"),strTranslate(IDS_SCENE_IS_LOCKED_WARNING_NO_SCRIPT_VIEWING),VMessageBox::OKELI);
		return(false);
	}

	if (App::userSettings->useBuiltInScriptEditor())
	{
		_dlg=new CScintillaDlg(App::mainWindow);
		if (_dlg==NULL)
			return(false);
	}
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptID);
	if (it==NULL)
	{
		if (App::userSettings->useBuiltInScriptEditor())
			_closeDlgWindow(true);
		return(false);
	}
	if (App::userSettings->useBuiltInScriptEditor())
		_dlg->setScriptID(scriptID);
	if (it->getScriptType()==sim_scripttype_mainscript)
	{ 
		if (it->isDefaultMainScript())
		{ // Display warning
			if (VMessageBox::REPLY_YES!=App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Main script"),strTranslate(IDS_MAINSCRIPT_EDITION_WARNING),VMessageBox::YES_NO))
			{
				if (App::userSettings->useBuiltInScriptEditor())
					_closeDlgWindow(false);
				return(false);
			}
			// Have have to make the main script a default main script!
			it->setCustomizedMainScript(true);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
	}
	if (App::userSettings->useBuiltInScriptEditor())
	{
		int posAndSize[4];
		it->getPreviousEditionWindowPosAndSize(posAndSize);
		_dlg->initPhase1(posAndSize);
		_scriptID=scriptID;
		_scriptLifeID=it->getLifeID();
		updateWindowExceptContentText();
		_dlg->initPhase2(it->getScriptText());
		if (it->getScriptType()==sim_scripttype_mainscript)
			_dlg->setColorsAndMainStyles(0);
		else
		{
			if (it->getThreadedExecution())
				_dlg->setColorsAndMainStyles(2);
			else
				_dlg->setColorsAndMainStyles(1);
		}
	}
	return(true);
}

void CScintillaEditor::getWindowPosAndSize(int posAndSize[4])
{
	_dlg->getWindowPosAndSize(posAndSize);
}

bool CScintillaEditor::updateWindowExceptContentText()
{ // return value means true means we need to close the editor
	if (_dlg==NULL)
		return(true);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(_scriptID);
	if ( (it==NULL)||_closeRequest )
		return(true);
	if (_scriptLifeID!=it->getLifeID())
		return(true);

	std::string tmp(it->getDescriptiveName());

	if (tmp.compare(_currentWindowTitle)!=0)
	{
		_dlg->setWindowTitleText(tmp);
		_currentWindowTitle=tmp;
	}
	return(false);
}

void CScintillaEditor::newSceneProcedure()
{

}

void CScintillaEditor::requestClosing(void* dialogPointer)
{
	if (dialogPointer==(void*)_dlg)
		_closeRequest=true;
}

void CScintillaEditor::_closeDlgWindow(bool announceChange)
{ // announceChange is true by default!
	if (_dlg==NULL)
		return;
	bool changed=applyChanges();
	delete _dlg;
	_dlg=NULL;
	_scriptID=-1;
	_scriptLifeID=-1;
	_currentWindowTitle="";
	if (announceChange&&changed)
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
}

bool CScintillaEditor::applyChanges()
{
	if (_dlg==NULL)
		return(false);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(_scriptID);
	if (it==NULL)
		return(false);
	if (_scriptLifeID!=it->getLifeID())
		return(false);
	int l=_dlg->sendEditor(QsciScintillaBase::SCI_GETLENGTH);
	char* buff=new char[l+1];
	_dlg->sendEditor(QsciScintillaBase::SCI_GETTEXT,l+1,(long)buff);

	bool changed=false;
	char* originalBuff=it->getScriptText();
	int originalL=it->getScriptTextLength();
	if (originalL==l+1)
	{
		for (int i=0;i<l;i++)
		{
			if (originalBuff[i]!=buff[i])
			{
				changed=true;
				break;
			}
		}
	}
	else
		changed=true;
	it->setScriptText(buff);
	delete[] buff;
	return(changed);
}
