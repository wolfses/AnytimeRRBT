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
#include <vector>

class CScintillaModalDlg : public QDialog
{
	Q_OBJECT

public:
	CScintillaModalDlg(QWidget* pParent = NULL, Qt::WindowFlags f= 0);
	virtual ~CScintillaModalDlg();

	bool initialize(int scriptId,const char* titleText,bool readOnly);
	void makeDialogModal();

    long _sendEditor(unsigned int Msg,unsigned long wParam=0,long lParam=0);

protected:
	void _setColorsAndMainStyles();
	int _scriptID;

	void _setAStyle(int style,COLORREF fore,COLORREF back=RGB(0,0,0),int size=-1,const char *face=0);
	void _setKeywords();
	void _prepAutoCompletionList(const char txt[3]);
	std::string _getCallTip(const char* txt);

	std::string _autoCompletionList;
	static std::vector<char> vrepKeywords1;
	static std::vector<char> vrepKeywords2;

	QsciScintilla* _scintillaObject;

public slots:
    void _charAdded(int charAdded);
};

