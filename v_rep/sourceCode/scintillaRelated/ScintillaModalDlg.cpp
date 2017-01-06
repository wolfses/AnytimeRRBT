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
#include "ScintillaModalDlg.h"
#include "v_rep_internal.h"
#include "LuaScriptFunctions.h"
#include "IloIlo.h"

#include <algorithm> 
#include "App.h"
#include "v_repStringTable.h"
#include <SciLexer.h>
#include <QCloseEvent>
#include <QVBoxLayout>

std::vector<char> CScintillaModalDlg::vrepKeywords1;
std::vector<char> CScintillaModalDlg::vrepKeywords2;

CScintillaModalDlg::CScintillaModalDlg(QWidget* pParent, Qt::WindowFlags f) : QDialog(pParent,QT_MODAL_SCINTILLA_DLG_STYLE)
{
	_scriptID=-1;
 //   setAttribute(Qt::WA_DeleteOnClose);
    _scintillaObject=new QsciScintilla;

	// Use following if using a QDialog!!
	QVBoxLayout *bl=new QVBoxLayout(this);
	bl->setContentsMargins(0,0,0,0);
	setLayout(bl);
	bl->addWidget(_scintillaObject);

// use following if using a QMainWindow!!!   setCentralWidget(_scintillaObject);

	QsciLexerLua* lexer=new QsciLexerLua;
	_scintillaObject->setLexer(lexer);
	_scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSTYLEBITS,5);
	_scintillaObject->setTabWidth(4);

	_setKeywords();
	int (*fn)(void*,int,int,int);
	void * ptr;

	fn=(int (__cdecl *)(void *,int,int,int))_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETDIRECTFUNCTION,0,long(0));
	ptr=(void *)_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETDIRECTPOINTER,0,long(0));
	fn(ptr,QsciScintillaBase::SCI_SETMARGINWIDTHN,0,48); // Line numbers
	fn(ptr,QsciScintillaBase::SCI_SETMARGINWIDTHN,1,0); // Symbols
	connect(_scintillaObject,SIGNAL(SCN_CHARADDED(int)),this,SLOT(_charAdded(int)));
	_setColorsAndMainStyles();
}

CScintillaModalDlg::~CScintillaModalDlg() 
{
	delete _scintillaObject;
}

bool CScintillaModalDlg::initialize(int scriptId,const char* titleText,bool readOnly)
{
	if ((App::operationalUIParts&sim_gui_scripteditor)==0)
		return(false);

	// Make sure that scripts cannot be viewed/edited with locked scenes:
	if (App::ct->environment->getSceneLocked())
	{
		App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Script"),strTranslate(IDS_SCENE_IS_LOCKED_WARNING_NO_SCRIPT_VIEWING),VMessageBox::OKELI);
		return(false);
	}

	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptId);
	if (it==NULL)
		return(false);

	_scriptID=scriptId;

	move(100,100);
	resize(480,380);

	setWindowTitle(titleText);
	_sendEditor(QsciScintillaBase::SCI_SETTEXT,0,(long)it->getScriptText());
	_sendEditor(QsciScintillaBase::SCI_EMPTYUNDOBUFFER); // Make sure the undo buffer is empty (otherwise the first undo will remove the whole script --> a bit confusing)
	if (readOnly)
		_sendEditor(QsciScintillaBase::SCI_SETREADONLY,1);
	return(true);
}

void CScintillaModalDlg::_setColorsAndMainStyles()
{ // backgroundStyle=0 Main script, 1=non-threaded, 2=threaded
	struct SScintillaColors
	{
		int iItem;
		COLORREF rgb;
	};
	const COLORREF black=RGB(0,0,0);
	const COLORREF darkGrey=RGB(64,64,64);
	const COLORREF mediumGrey=RGB(100,100,100);
	const COLORREF lightGrey=RGB(245,245,245);
//	const COLORREF lightCyan=RGB(190,212,212);
//	const COLORREF lightRed=RGB(225,195,195);
	const COLORREF white=RGB(255,255,255);
	const COLORREF green=RGB(0,164,0);
	const COLORREF red=RGB(152,0,0);
	const COLORREF red2=RGB(220,80,20);
	const COLORREF red3=RGB(152,64,0);
	const COLORREF blue=RGB(0,0,255);
	const COLORREF blueGrey=RGB(128,128,255);
	const COLORREF yellow=RGB(205,205,0);
	const COLORREF magenta=RGB(220,0,220);
	const COLORREF cyan=RGB(0,128,128);
//	const COLORREF magenta2=RGB(0,0,128);

	COLORREF background=lightGrey;

	static SScintillaColors syntaxColors[]= 
	{
		{SCE_LUA_COMMENT,green},
		{SCE_LUA_COMMENTLINE,green},
		{SCE_LUA_COMMENTDOC,green},
		{SCE_LUA_NUMBER,magenta},
		{SCE_LUA_STRING,yellow},
		{SCE_LUA_LITERALSTRING,yellow},
		{SCE_LUA_CHARACTER,yellow},
		{SCE_LUA_OPERATOR,black},
		{SCE_LUA_PREPROCESSOR,cyan},
		{SCE_LUA_WORD,blue},
		{SCE_LUA_WORD2,red},
		{SCE_LUA_WORD3,red2},
		{SCE_LUA_WORD4,red3},
		{SCE_LUA_IDENTIFIER,mediumGrey},
		{-1,0}
	};


	int fontSize=12;
	std::string theFont("Courier"); // since Scintilla 2.7.2 and Qt5.1.1, "Courier New" gives strange artifacts (with Mac and Linux)!
#ifdef MAC_VREP
	fontSize=16; // bigger fonts here
#endif
	if (App::userSettings->scriptEditorFontSize!=-1)
		fontSize=App::userSettings->scriptEditorFontSize;
	if (App::userSettings->scriptEditorFont.length()!=0)
		theFont=App::userSettings->scriptEditorFont;

	_setAStyle(QsciScintillaBase::STYLE_DEFAULT,black,background,fontSize,theFont.c_str()); // set global default style

	_scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETCARETFORE,(unsigned long)black);
	_scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLECLEARALL); // set all styles
	_setAStyle(QsciScintillaBase::STYLE_LINENUMBER,white,darkGrey);
	_scintillaObject->SendScintilla(QsciScintillaBase::SCI_SETSELBACK,1,blueGrey); // selection color

	// Set syntax colors
	for (int i=0;syntaxColors[i].iItem!=-1;i++)
		_setAStyle(syntaxColors[i].iItem,syntaxColors[i].rgb,background);
}

long CScintillaModalDlg::_sendEditor(unsigned int Msg,unsigned long wParam,long lParam)
{
	return (_scintillaObject->SendScintilla(Msg,wParam,lParam));
}

void CScintillaModalDlg::makeDialogModal()
{
	setModal(true);
	exec();
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(_scriptID);
	int l=_sendEditor(QsciScintillaBase::SCI_GETLENGTH);
	char* buff=new char[l+1];
	_sendEditor(QsciScintillaBase::SCI_GETTEXT,l+1,(long)buff);
	it->setScriptText(buff);
	delete[] buff;
}

void CScintillaModalDlg::_setAStyle(int style,COLORREF fore,COLORREF back,int size,const char *face)
{
	_scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETFORE,style,fore);
	_scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETBACK,style,back);
	if (size>=1)
		_scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETSIZE,style,size);
	if (face)
		_scintillaObject->SendScintilla(QsciScintillaBase::SCI_STYLESETFONT,style,reinterpret_cast<long>(face));
}

void CScintillaModalDlg::_setKeywords()
{
	if (vrepKeywords1.size()==0)
	{
		for (int i=0;simLuaCommands[i].name!="";i++)
		{
			for (int j=0;j<int(simLuaCommands[i].name.size());j++)
				vrepKeywords1.push_back(simLuaCommands[i].name[j]);
			vrepKeywords1.push_back(' ');
		}
		for (int i=0;i<int(CIloIlo::luaCustomFunctionContainer.allCustomFunctions.size());i++)
		{
			std::string n=CIloIlo::luaCustomFunctionContainer.allCustomFunctions[i]->getFunctionName();
			for (int j=0;j<int(n.size());j++)
				vrepKeywords1.push_back(n[j]);
			vrepKeywords1.push_back(' ');
		}
		vrepKeywords1.push_back(0);
	}
	_sendEditor(QsciScintillaBase::SCI_SETKEYWORDS,1,(long)&vrepKeywords1[0]);
	if (vrepKeywords2.size()==0)
	{
		for (int i=0;simLuaVariables[i].name!="";i++)
		{
			for (int j=0;j<int(simLuaVariables[i].name.size());j++)
				vrepKeywords2.push_back(simLuaVariables[i].name[j]);
			vrepKeywords2.push_back(' ');
		}
		for (int i=0;i<int(CIloIlo::luaCustomFunctionContainer.allCustomVariableNames.size());i++)
		{
			for (int j=0;j<int(CIloIlo::luaCustomFunctionContainer.allCustomVariableNames[i].size());j++)
				vrepKeywords2.push_back(CIloIlo::luaCustomFunctionContainer.allCustomVariableNames[i][j]);
			vrepKeywords2.push_back(' ');
		}
		vrepKeywords2.push_back(0);
	}
	_sendEditor(QsciScintillaBase::SCI_SETKEYWORDS,2,(long)&vrepKeywords2[0]);
}

void CScintillaModalDlg::_prepAutoCompletionList(const char txt[3])
{ 
	_autoCompletionList.clear();
	std::vector<std::string> t;
	for (int i=0;simLuaCommands[i].name!="";i++)
	{
		if (simLuaCommands[i].name.size()>=3)
		{
			bool same=true;
			for (int j=0;j<3;j++)
			{
				if (txt[j]!=simLuaCommands[i].name[j])
				{
					same=false;
					break;
				}
			}
			if (same)
				t.push_back(simLuaCommands[i].name);
		}
	}
	// Following is not elegant (almost same code as above)
	for (int i=0;simLuaVariables[i].name!="";i++)
	{
		if (simLuaVariables[i].name.size()>=3)
		{
			bool same=true;
			for (int j=0;j<3;j++)
			{
				if (txt[j]!=simLuaVariables[i].name[j])
				{
					same=false;
					break;
				}
			}
			if (same)
				t.push_back(simLuaVariables[i].name);
		}
	}
	// Following is not elegant (almost same code as above)
	for (int i=0;i<int(CIloIlo::luaCustomFunctionContainer.allCustomFunctions.size());i++)
	{
		std::string n=CIloIlo::luaCustomFunctionContainer.allCustomFunctions[i]->getFunctionName();
		if (n.size()>=3)
		{
			bool same=true;
			for (int j=0;j<3;j++)
			{
				if (txt[j]!=n[j])
				{
					same=false;
					break;
				}
			}
			if (same)
				t.push_back(n);
		}
	}
	// Following is not elegant (almost same code as above)
	for (int i=0;i<int(CIloIlo::luaCustomFunctionContainer.allCustomVariableNames.size());i++)
	{
		std::string n=CIloIlo::luaCustomFunctionContainer.allCustomVariableNames[i];
		if (n.size()>=3)
		{
			bool same=true;
			for (int j=0;j<3;j++)
			{
				if (txt[j]!=n[j])
				{
					same=false;
					break;
				}
			}
			if (same)
				t.push_back(n);
		}
	}
	std::sort(t.begin(),t.end());
	for (int i=0;i<int(t.size());i++)
	{
		_autoCompletionList+=t[i];
		if (i!=int(t.size()-1))
			_autoCompletionList+=' ';
	}
}

std::string CScintillaModalDlg::_getCallTip(const char* txt)
{ 
	std::string retVal="";
	int l=strlen(txt);
	for (int i=0;simLuaCommands[i].name!="";i++)
	{
		if (int(simLuaCommands[i].name.size())==l)
		{
			bool same=true;
			for (int j=0;j<l;j++)
			{
				if (txt[j]!=simLuaCommands[i].name[j])
				{
					same=false;
					break;
				}
			}
			if (same)
				return(simLuaCommands[i].callTip);
		}
	}
	// Following is not elegant (almost same code as above)
	for (int i=0;i<int(CIloIlo::luaCustomFunctionContainer.allCustomFunctions.size());i++)
	{
		std::string n=CIloIlo::luaCustomFunctionContainer.allCustomFunctions[i]->getFunctionName();
		if (int(n.size())==l)
		{
			bool same=true;
			for (int j=0;j<l;j++)
			{
				if (txt[j]!=n[j])
				{
					same=false;
					break;
				}
			}
			if (same)
				return(CIloIlo::luaCustomFunctionContainer.allCustomFunctions[i]->getCallTips());
		}
	}
	return(retVal);
}

void CScintillaModalDlg::_charAdded(int charAdded)
{
	if (_scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCACTIVE)!=0)
    { // Autocomplete is active
	    if (charAdded=='(')
		    _scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCCANCEL);
    }
	if (_scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCACTIVE)==0)
	{ // Autocomplete is not active
		if (_scintillaObject->SendScintilla(QsciScintillaBase::SCI_CALLTIPACTIVE)!=0)
		{ // CallTip is active
		}
		else
		{ // Calltip is not active
			if (charAdded=='(')
			{ // Do we need to activate a calltip?

				char linebuf[1000];
				int current=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETCURLINE,sizeof(linebuf),reinterpret_cast<long>(static_cast<char*>(linebuf)));
				int pos=_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETCURRENTPOS);
				int startword=current-1;
				while ((startword>0)&&(isalpha(linebuf[startword-1])||isdigit(linebuf[startword-1])||(linebuf[startword-1]=='_') )) // added the '_' statement on 2010/06/23 because custom lua functions with '_' wouldn't display tooltips
					startword--;
				linebuf[current-1]='\0';
				char* word=linebuf+startword;
				std::string s(_getCallTip(word));
				if (s!="")
				{

					int fontSize=12-4;
					std::string theFont("Courier"); // since Scintilla 2.7.2 and Qt5.1.1, "Courier New" gives strange artifacts (with Mac and Linux)!
#ifdef MAC_VREP
					fontSize=16-4; // bigger fonts here
#endif
					if (App::userSettings->scriptEditorFontSize!=-1)
						fontSize=App::userSettings->scriptEditorFontSize-4;
					if (App::userSettings->scriptEditorFont.length()!=0)
						theFont=App::userSettings->scriptEditorFont;
					_setAStyle(QsciScintillaBase::STYLE_CALLTIP,RGB(0,0,0),RGB(255,255,255),fontSize,theFont.c_str());
					_scintillaObject->SendScintilla(QsciScintillaBase::SCI_CALLTIPUSESTYLE,0);
					_scintillaObject->SendScintilla(QsciScintillaBase::SCI_CALLTIPSHOW,pos,(long)s.c_str());
				}
			}
			else
			{ // Do we need to activate autocomplete?
				int p=-1+_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETCURRENTPOS);
				if (p>=2)
				{
					char pref[3]={(char)_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETCHARAT,p-2),(char)_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETCHARAT,p-1),(char)_scintillaObject->SendScintilla(QsciScintillaBase::SCI_GETCHARAT,p-0)};
					_prepAutoCompletionList(pref);
					if (_autoCompletionList.size()!=0)
					{ // We need to activate autocomplete!
						_scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCSETAUTOHIDE,0);
						_scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCSTOPS,0,(long)" ()[]{}:;~`',.=*-+/?!@#$%^&|\\<>\"");
						_scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCSETMAXHEIGHT,10);
						_scintillaObject->SendScintilla(QsciScintillaBase::SCI_AUTOCSHOW,3,(long)&(_autoCompletionList[0]));
					}
				}
			}
		}
	}

}
