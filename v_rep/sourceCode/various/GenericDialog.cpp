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
#include "GenericDialog.h" 
#include "App.h"
#include "v_rep_internal.h"
#include "OGL.h"
#include "Tt.h"

CGenericDialog::CGenericDialog(const char* titleText,const char* mainText,int dialogType,bool modal,
							   const char* initialText,const float titleColor[6],const float dialogColor[6])
{
	_modal=modal;
	_createdInMainOrChildScript=false;

	bool dontCenter=((dialogType&sim_dlgstyle_dont_center)!=0);
	dialogType=(dialogType|sim_dlgstyle_dont_center)-sim_dlgstyle_dont_center; 
	_dialogType=dialogType;
	_editBoxText="";
	if (initialText!=NULL)
		_editBoxText=initialText;
	_editBoxButtonID=-1;
	_button1ID=-1;
	_button2ID=-1;
	_buttonBlockID=-1;
	_pressedButton=sim_dlgret_still_open;
	// Figure out the vertical sizes:
	// title:1
	// text: 2+what is required
	// edit box: 1+1
	// buttons: 1+2+1
	int vertSizeForText=2;
	std::vector<std::string> lines;
	int maxWidthTitle,maxWidth,maxHeight;
	// Make sure the title and initial text's width are also taken into account:
	ogl::getMultilineTextInfo(titleText,lines,&maxWidth,&maxHeight);
	lines.clear();
	ogl::getMultilineTextInfo(_editBoxText,lines,&maxWidthTitle,&maxHeight);
	lines.clear();
	maxWidthTitle=SIM_MAX(maxWidth,maxWidthTitle);
	ogl::getMultilineTextInfo(mainText,lines,&maxWidth,&maxHeight);
	maxWidth=SIM_MAX(maxWidth,maxWidthTitle);
	vertSizeForText+=maxHeight/16; // 16=14+2 (border)
	if (vertSizeForText<3)
		vertSizeForText=3;
	// Figure out the horizontal sizes:
	// left and right border: 1+1
	// Min content size (without border): 5+2+5 (from buttons)
	// Max content size (without border): 48
	int horizContentSize=maxWidth/16; // 16=14+2 (button borders)
	horizContentSize+=2;
	horizContentSize=tt::getLimitedInt(12,48,horizContentSize);
	// Now figure out the dialog dimensions:
	int dialogWidth=2+horizContentSize;
	int dialogHeight=1+vertSizeForText;
	if (dialogType==sim_dlgstyle_input)
		dialogHeight+=2+4;
	if ( (dialogType==sim_dlgstyle_ok)||(dialogType==sim_dlgstyle_ok_cancel)||(dialogType==sim_dlgstyle_yes_no) )
		dialogHeight+=4;

	float titleCol[6]={0.3f,0.3f,0.4f,1.0f,1.0f,1.0f};
	if (titleColor!=NULL)
	{
		for (int i=0;i<6;i++)
			titleCol[i]=titleColor[i];
	}

	float dialogCol[6]={0.75f,0.75f,0.80f,0.0f,0.0f,0.0f};
	if (dialogColor!=NULL)
	{
		for (int i=0;i<6;i++)
			dialogCol[i]=dialogColor[i];
	}
	CButtonBlock* b=new CButtonBlock(dialogWidth,dialogHeight,14,14,-1);
	b->setBlockName("SYSTEM_BLOCK_GENERIC_DLG");
	b->setGenericDialog(true);
	if (dontCenter)
		b->setAttributes((b->getAttributes()|sim_ui_property_settocenter)-sim_ui_property_settocenter);
	else
		b->setAttributes(b->getAttributes()|sim_ui_property_settocenter);

	App::ct->buttonBlockContainer->insertBlock(b,false);
	_buttonBlockID=b->getBlockID();
	CSoftButton* sb=new CSoftButton(titleText,0,0,dialogWidth,1);
	sb->setAttributes((sb->getAttributes()&(0xfff8-sim_buttonproperty_horizontallycentered))|sim_buttonproperty_label);
	_applyColor(sb,titleCol);

	b->insertButton(sb);

	// The text:
	sb=new CSoftButton(mainText,1,1,dialogWidth-2,vertSizeForText);
	sb->setAttributes((sb->getAttributes()&(0xfff8-sim_buttonproperty_horizontallycentered))|sim_buttonproperty_label);
	_applyColor(sb,dialogCol);
	b->insertButton(sb);

	// The two sides (going up to the edit box part if present):
	int vertPartH=vertSizeForText;
	if (dialogType==sim_dlgstyle_input)
		vertPartH+=2;
	sb=new CSoftButton("",0,1,1,vertPartH);
	sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_label);
	_applyColor(sb,dialogCol);
	b->insertButton(sb);

	sb=new CSoftButton("",dialogWidth-1,1,1,vertPartH);
	sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_label);
	_applyColor(sb,dialogCol);
	b->insertButton(sb);

	if (dialogType==sim_dlgstyle_message)
		return;
	int off=1+vertSizeForText;
	if (dialogType==sim_dlgstyle_input)
	{
		if (initialText==NULL)
			sb=new CSoftButton("",1,off,dialogWidth-2,1);
		else
			sb=new CSoftButton(initialText,1,off,dialogWidth-2,1);
		sb->setAttributes((sb->getAttributes()&(0xfff8-sim_buttonproperty_horizontallycentered))|sim_buttonproperty_editbox);
		b->insertButton(sb);
		_editBoxButtonID=sb->buttonID;

		sb=new CSoftButton("",1,off+1,dialogWidth-2,1);
		sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_label);
		_applyColor(sb,dialogCol);
		b->insertButton(sb);
		off+=2;
	}
	if ( (dialogType==sim_dlgstyle_ok_cancel)||(dialogType==sim_dlgstyle_yes_no) )
	{ // YES/NO, OK/CANCEL buttons:
		int interButtonSpace=2;
		if (dialogWidth&1)
			interButtonSpace=3;
		int sidesWidth=(dialogWidth-10-interButtonSpace)/2;
		//leftSide:
		sb=new CSoftButton("",0,off,sidesWidth,2);
		sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_label);
		_applyColor(sb,dialogCol);
		b->insertButton(sb);
		// MiddlePart:
		sb=new CSoftButton("",sidesWidth+5,off,interButtonSpace,2);
		sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_label);
		_applyColor(sb,dialogCol);
		b->insertButton(sb);
		//rightSide:
		sb=new CSoftButton("",dialogWidth-sidesWidth,off,sidesWidth,2);
		sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_label);
		_applyColor(sb,dialogCol);
		b->insertButton(sb);

		if (dialogType==sim_dlgstyle_ok_cancel)
			sb=new CSoftButton("OK",sidesWidth,off,5,2);
		else
			sb=new CSoftButton("YES",sidesWidth,off,5,2);
		sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_button|sim_buttonproperty_closeaction);
		_applyColor(sb,dialogCol);
		b->insertButton(sb);
		_button1ID=sb->buttonID;

		if (dialogType==sim_dlgstyle_ok_cancel)
			sb=new CSoftButton("CANCEL",sidesWidth+5+interButtonSpace,off,5,2);
		else
			sb=new CSoftButton("NO",sidesWidth+5+interButtonSpace,off,5,2);
		sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_button|sim_buttonproperty_closeaction);
		_applyColor(sb,dialogCol);
		b->insertButton(sb);
		_button2ID=sb->buttonID;
		off+=2;
	}
	if ( (dialogType==sim_dlgstyle_ok)||(dialogType==sim_dlgstyle_input) )
	{ // OK button:
		int buttonWidth=10;
		if (dialogWidth&1)
			buttonWidth=11;
		int sidesWidth=(dialogWidth-buttonWidth)/2;
		//leftSide:
		sb=new CSoftButton("",0,off,sidesWidth,2);
		sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_label);
		_applyColor(sb,dialogCol);
		b->insertButton(sb);
		//rightSide:
		sb=new CSoftButton("",dialogWidth-sidesWidth,off,sidesWidth,2);
		sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_label);
		_applyColor(sb,dialogCol);
		b->insertButton(sb);
		// The button:
		sb=new CSoftButton("OK",sidesWidth,off,buttonWidth,2);
		sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_button|sim_buttonproperty_closeaction);
		_applyColor(sb,dialogCol);
		b->insertButton(sb);
		_button1ID=sb->buttonID;
		off+=2;
	}
	sb=new CSoftButton("",0,off,dialogWidth,1);
	sb->setAttributes((sb->getAttributes()&0xfff8)|sim_buttonproperty_label);
	_applyColor(sb,dialogCol);
	b->insertButton(sb);
}

CGenericDialog::~CGenericDialog()
{
	App::ct->buttonBlockContainer->removeBlockFromID(_buttonBlockID);
}

void CGenericDialog::setPauseActive(bool pauseActive)
{
	CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(_buttonBlockID);
	if (it!=NULL)
	{
		it->setAttributes(it->getAttributes()|sim_ui_property_pauseactive);
		if (!pauseActive)
			it->setAttributes(it->getAttributes()-sim_ui_property_pauseactive);
	}
}

void CGenericDialog::setCreatedInMainOrChildScript(bool c)
{
	_createdInMainOrChildScript=c;
}

bool CGenericDialog::getCreatedInMainOrChildScript()
{
	return(_createdInMainOrChildScript);
}

void CGenericDialog::buttonPressed(int blockID,int buttonID)
{
	if (blockID!=_buttonBlockID)
		return;
	if (buttonID==_button1ID)
	{
		if ( (_dialogType==sim_dlgstyle_input)||(_dialogType==sim_dlgstyle_ok_cancel)||(_dialogType==sim_dlgstyle_ok) )
			_pressedButton=sim_dlgret_ok;
		if (_dialogType==sim_dlgstyle_yes_no)
			_pressedButton=sim_dlgret_yes;
	}
	if (buttonID==_button2ID)
	{
		if (_dialogType==sim_dlgstyle_ok_cancel)
			_pressedButton=sim_dlgret_cancel;
		if (_dialogType==sim_dlgstyle_yes_no)
			_pressedButton=sim_dlgret_no;
	}
}

void CGenericDialog::textChanged(int blockID,int buttonID,const char* newTxt)
{
	if (blockID!=_buttonBlockID)
		return;
	if (buttonID==_editBoxButtonID)
	{
		if (_dialogType==sim_dlgstyle_input)
			_editBoxText=newTxt;
	}
}

int CGenericDialog::getDialogResult()
{
	return(_pressedButton);
}

int CGenericDialog::getDialogBlockHandle()
{
	return(_buttonBlockID);
}

std::string CGenericDialog::getDialogString()
{
	return(_editBoxText);
}

void CGenericDialog::setGenericDialogID(int theNewID)
{
	_genericDialogID=theNewID;
}

int CGenericDialog::getGenericDialogID()
{
	return(_genericDialogID);
}

void CGenericDialog::setModal(bool modal)
{
	_modal=modal;
}

bool CGenericDialog::getModal()
{
	return(_modal);
}

void CGenericDialog::_applyColor(CSoftButton* button,float col[6])
{
	for (int i=0;i<3;i++)
	{
		button->backgroundColor[i]=col[i];
		button->downBackgroundColor[i]=col[i];
		button->textColor[i]=col[i+3];
	}
}

