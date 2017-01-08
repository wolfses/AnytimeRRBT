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

#pragma once

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscistyle.h>

#include <string>

class CConsoleInitInfo
{
public:
	CConsoleInitInfo(int mode,const char* title,int maxLines,const int thePos[2],const int theSize[2],const float textCol[3],const float backgroundCol[3],int currentInstance)
	{
		_defaultInstance=currentInstance;
		_mode=mode;
		_title=title;
		_maxLines=maxLines;
		_initialPosition[0]=100;
		_initialPosition[1]=100;
		_initialSize[0]=640;
		_initialSize[1]=200;
		_textColor[0]=0.0f;
		_textColor[1]=0.0f;
		_textColor[2]=0.0f;
		_backgroundColor[0]=1.0f;
		_backgroundColor[1]=1.0f;
		_backgroundColor[2]=1.0f;
		if (thePos!=NULL)
		{
			_initialPosition[0]=thePos[0];
			_initialPosition[1]=thePos[1];
		}
		if (theSize!=NULL)
		{
			_initialSize[0]=theSize[0];
			_initialSize[1]=theSize[1];
		}
		if (textCol!=NULL)
		{
			for (int i=0;i<3;i++)
			_textColor[i]=textCol[i];
		}
		if (backgroundCol!=NULL)
		{
			for (int i=0;i<3;i++)
			_backgroundColor[i]=backgroundCol[i];
		}
		_consoleHandle=-1;
		_manuallySetShowState=true;
		_clearBuffer=false;
	}
	virtual ~CConsoleInitInfo()
	{
	}
	void setHandle(int handle)
	{
		_consoleHandle=handle;
	}
	int getHandle()
	{
		return(_consoleHandle);
	}
	void addText(const char* txt)
	{
		if (txt==NULL)
		{
			_textToAdd="";
			_clearBuffer=true;
		}
		else
			_textToAdd+=txt;
	}
	void setShowState(bool show)
	{
		_manuallySetShowState=show;
	}
	int getDefaultInstance()
	{
		return(_defaultInstance);
	}

	int _mode;
	int _defaultInstance;
	std::string _title;
	int _maxLines;
	int _initialPosition[2];
	int _initialSize[2];
	float _textColor[3];
	float _backgroundColor[3];
	int _consoleHandle;
	bool _manuallySetShowState;
	std::string _textToAdd;
	bool _clearBuffer;
};

class CScintillaConsoleDlg : public QDialog
{
//	Q_OBJECT

public:
	CScintillaConsoleDlg(CConsoleInitInfo* initInfo,QWidget* pParent = NULL, Qt::WindowFlags f= 0);
	virtual ~CScintillaConsoleDlg();

    long sendEditor(unsigned int Msg,unsigned long wParam=0,long lParam=0);
    void closeEvent(QCloseEvent *event);
	void qtInitialize();

	bool getIsInitialized();
	bool simulationEnded(); // returns true if this console wants destruction
	bool getCloseRequested(); // returns true if this console wants destruction
	void setCloseRequest();
	void handleTextUpdate();

	void _setAStyle(int style,COLORREF fore,COLORREF back=RGB(0,0,0),int size=-1,const char *face=0);

	CConsoleInitInfo* info;

	bool _initialized;
	bool _realShowState;
	bool _closingRequest;

	QsciScintilla* _scintillaObject;
};
