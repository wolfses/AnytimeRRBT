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

// Some parts of this code were inspired from Robert Umbehant's personal c compiler
// (http://www.codeproject.com/KB/cpp/Personal_C___Compiler.aspx)

#include "vrepPrecompiledHeader.h"
#include "ScintillaConsoleDlg.h"
#include "v_rep_internal.h"
#include "LuaScriptFunctions.h"
#include "IloIlo.h"

#include <QCloseEvent>
#include <QVBoxLayout>
#include "App.h"

CScintillaConsoleDlg::CScintillaConsoleDlg(CConsoleInitInfo* initInfo,QWidget* pParent, Qt::WindowFlags f) : QDialog(pParent,QT_MODELESS_SCINTILLA_DLG_STYLE)
{
    setAttribute(Qt::WA_DeleteOnClose);
    _scintillaObject=new QsciScintilla;

	// Use following if using a QDialog!!
	QVBoxLayout *bl=new QVBoxLayout(this);
	bl->setContentsMargins(0,0,0,0);
	setLayout(bl);
	bl->addWidget(_scintillaObject);
// use following if using a QMainWindow!!!   setCentralWidget(_scintillaObject);
	info=initInfo;
	_closingRequest=false;
	_initialized=false;
	_realShowState=true;
}

CScintillaConsoleDlg::~CScintillaConsoleDlg() 
{
	delete info;
	// _scintillaObject is normally automatically destroyed!
}

void CScintillaConsoleDlg::qtInitialize()
{
	struct SScintillaColors
	{
		int iItem;
		COLORREF rgb;
	};
	const COLORREF blueGrey=RGB(128,128,255);

	_scintillaObject->setTabWidth(4);
	sendEditor(QsciScintillaBase::SCI_USEPOPUP,0); // no popup!
	sendEditor(QsciScintillaBase::SCI_SETREADONLY,1);


	int fontSize=8;
	std::string theFont("Courier"); // since Scintilla 2.7.2 and Qt5.1.1, "Courier New" gives strange artifacts (with Mac and Linux)!
#ifdef MAC_VREP
	fontSize=12; // bigger fonts here
#endif
	if (App::userSettings->auxConsoleFontSize!=-1)
		fontSize=App::userSettings->auxConsoleFontSize;
	if (App::userSettings->auxConsoleFont.length()!=0)
		theFont=App::userSettings->auxConsoleFont;

	_setAStyle(QsciScintillaBase::STYLE_DEFAULT,RGB(int(info->_textColor[0]*255.0f),int(info->_textColor[1]*255.0f),int(info->_textColor[2]*255.0f)),RGB(int(info->_backgroundColor[0]*255.0f),int(info->_backgroundColor[1]*255.0f),int(info->_backgroundColor[2]*255.0f)),fontSize,theFont.c_str()); // set global default style

	if (info->_mode&2)
		sendEditor(QsciScintillaBase::SCI_SETWRAPMODE,QsciScintillaBase::SC_WRAP_WORD);
	else
		sendEditor(QsciScintillaBase::SCI_SETWRAPMODE,QsciScintillaBase::SC_WRAP_NONE);
	_scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLECLEARALL); // set all styles
	_scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSELBACK,1,blueGrey); // selection color

	move(info->_initialPosition[0],info->_initialPosition[1]);
	resize(info->_initialSize[0],info->_initialSize[1]);
	setWindowTitle(info->_title.c_str());

	_initialized=true;
}

long CScintillaConsoleDlg::sendEditor(unsigned int Msg,unsigned long wParam,long lParam)
{
	return (_scintillaObject->SendScintilla(Msg,wParam,lParam));
}

void CScintillaConsoleDlg::_setAStyle(int style,COLORREF fore,COLORREF back,int size,const char *face)
{
	_scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETFORE,style,fore);
	_scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETBACK,style,back);
	if (size>=1)
		_scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETSIZE,style,size);
	if (face)
		_scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETFONT,style,reinterpret_cast<long>(face));
}

void CScintillaConsoleDlg::closeEvent(QCloseEvent *event)
{
	event->ignore();
	if (info->_mode&4)
		_closingRequest=true;
}

bool CScintillaConsoleDlg::getIsInitialized()
{
	return(_initialized);
}

bool CScintillaConsoleDlg::simulationEnded()
{ // returns true if this console wants destruction
	return(info->_mode&1);
}

bool CScintillaConsoleDlg::getCloseRequested()
{ // returns true if this console wants destruction
	return(_closingRequest);
}

void CScintillaConsoleDlg::setCloseRequest()
{ 
	_closingRequest=true;
}

void CScintillaConsoleDlg::handleTextUpdate()
{
	if (_initialized)
	{
		int currentInstance=App::ct->getCurrentInstanceIndex();
		bool paused=App::ct->simulationList[info->_defaultInstance]->isSimulationPaused();
		bool diffInstance=(currentInstance!=info->_defaultInstance);

		bool additionalShowState=!( ( (info->_mode&8)&&paused )||( ((info->_mode&16)==0)&&diffInstance ) );
		
		if ((info->_manuallySetShowState&&additionalShowState)!=_realShowState)
		{
			_realShowState=info->_manuallySetShowState&&additionalShowState;
			if (_realShowState)
				show();
			else
				hide();
		}

		sendEditor(QsciScintillaBase::SCI_SETREADONLY,0);
		if (info->_clearBuffer)
		{
			sendEditor(QsciScintillaBase::SCI_CLEARALL);
			info->_clearBuffer=false; // forgot that and added it on 21/04/2011!!
		}
		if (info->_textToAdd.length()!=0)
		{
			int l=sendEditor(QsciScintillaBase::SCI_GETLENGTH);
			sendEditor(QsciScintillaBase::SCI_INSERTTEXT,l,(long)info->_textToAdd.c_str());
			int lines=sendEditor(QsciScintillaBase::SCI_GETLINECOUNT);
			if (lines>info->_maxLines)
			{ // we have to remove lines-_maxLines lines!
				sendEditor(QsciScintillaBase::SCI_SETSELECTIONSTART,0);
				sendEditor(QsciScintillaBase::SCI_SETSELECTIONEND,sendEditor(QsciScintillaBase::SCI_POSITIONFROMLINE,lines-info->_maxLines));
				sendEditor(QsciScintillaBase::SCI_CLEAR);
			}
			sendEditor(QsciScintillaBase::SCI_GOTOPOS,sendEditor(QsciScintillaBase::SCI_POSITIONFROMLINE,sendEditor(QsciScintillaBase::SCI_GETLINECOUNT)-1)); // set the cursor and move the view into position
		}
		info->_textToAdd="";
		sendEditor(QsciScintillaBase::SCI_SETREADONLY,1);
	}
}
