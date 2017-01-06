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
#include "VMessageBox.h"
#include <QMessageBox>

WORD VMessageBox::OKELI			=0;
WORD VMessageBox::YES_NO		=1;
WORD VMessageBox::YES_NO_CANCEL	=2;
WORD VMessageBox::OK_CANCEL		=3;

WORD VMessageBox::APP_MODAL		=8;
WORD VMessageBox::INFO_TYPE		=16;
WORD VMessageBox::QUESTION_TYPE	=32;
WORD VMessageBox::WARNING_TYPE	=64;
WORD VMessageBox::CRITICAL_TYPE	=128;

WORD VMessageBox::REPLY_CANCEL	=0;
WORD VMessageBox::REPLY_NO		=1;
WORD VMessageBox::REPLY_YES		=2;
WORD VMessageBox::REPLY_OK		=3;
WORD VMessageBox::REPLY_ERROR	=4;

WORD VMessageBox::informationSystemModal_(QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // Don't forget: parent can be NULL at application start-up!
	flags|=INFO_TYPE;
	return(_displayBox(parent,title,message,flags));
}

WORD VMessageBox::information_(QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // Don't forget: parent can be NULL at application start-up!
	flags|=APP_MODAL|INFO_TYPE;
	return(_displayBox(parent,title,message,flags));
}

WORD VMessageBox::question_(QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // Don't forget: parent can be NULL at application start-up!
	flags|=APP_MODAL|QUESTION_TYPE;
	return(_displayBox(parent,title,message,flags));
}

WORD VMessageBox::warning_(QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // Don't forget: parent can be NULL at application start-up!
	flags|=APP_MODAL|WARNING_TYPE;
	return(_displayBox(parent,title,message,flags));
}

WORD VMessageBox::critical_(QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // Don't forget: parent can be NULL at application start-up!
	flags|=APP_MODAL|CRITICAL_TYPE;
	return(_displayBox(parent,title,message,flags));
}

WORD VMessageBox::_displayBox(QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // Don't forget: parent can be NULL at application start-up!
	WORD lower=(flags&7);
	WORD upper=flags-lower;

	QMessageBox msg(parent);
	msg.setWindowTitle(title.c_str());
	msg.setText(message.c_str());
	if (lower==OKELI)
		msg.addButton(QMessageBox::Ok);
	if (lower==YES_NO)
	{
		msg.addButton(QMessageBox::Yes);
		msg.addButton(QMessageBox::No);
	}
	if (lower==YES_NO_CANCEL)
	{
		msg.addButton(QMessageBox::Yes);
		msg.addButton(QMessageBox::No);
		msg.addButton(QMessageBox::Cancel);
	}
	if (lower==OK_CANCEL)
	{
		msg.addButton(QMessageBox::Ok);
		msg.addButton(QMessageBox::Cancel);
	}
	if (upper&INFO_TYPE)
		msg.setIcon(QMessageBox::Information);
	if (upper&QUESTION_TYPE)
		msg.setIcon(QMessageBox::Question);
	if (upper&WARNING_TYPE)
		msg.setIcon(QMessageBox::Warning);
	if (upper&CRITICAL_TYPE)
		msg.setIcon(QMessageBox::Critical);

// Following doesn't always work correctly (at least on Qt: the dialog sometimes simply doesn't display!!!!!):
//	if (alwaysOnTop)
//		msg.setWindowFlags(Qt::WindowStaysOnTopHint);

	int result=msg.exec();


	if (result==QMessageBox::Cancel)
		return(REPLY_CANCEL);
	if (result==QMessageBox::No)
		return(REPLY_NO);
	if (result==QMessageBox::Yes)
		return(REPLY_YES);
	if (result==QMessageBox::Ok)
		return(REPLY_OK);
	return(REPLY_CANCEL); // Can happen when the close button was clicked!
}
