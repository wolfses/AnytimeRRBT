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
#include "VDialog.h"

VDialog::VDialog(QWidget* pParent) : BASE_DLG_CLASS(pParent,QT_MODELESS_DLG_STYLE)
{
}

VDialog::VDialog(QWidget* pParent,Qt::WindowFlags specialFlags) : BASE_DLG_CLASS(pParent,specialFlags)
{
}

VDialog::~VDialog()
{
//	printf("DLGSTYLE: %i\n",QT_MODAL_DLG_STYLE);
}

void VDialog::reject()
{
	cancelEvent();
}
void VDialog::accept()
{
	okEvent();
}

void VDialog::cancelEvent()
{ // virtual
}

void VDialog::okEvent()
{ // virtual
}

void VDialog::initializationEvent()
{ // virtual
}

void VDialog::defaultDialogInitializationRoutine()
{
}

void VDialog::defaultModalDialogEndRoutine(bool theResult)
{
	if (theResult)
		done(QDialog::Accepted);
	else
		done(QDialog::Rejected);
}


int VDialog::makeDialogModal()
{
	setFixedSize(size()); // To have fixed-size dialogs
	setModal(true);
	int result=exec();
	if (result==QDialog::Accepted)
		return(VDIALOG_MODAL_RETURN_OK);
	return(VDIALOG_MODAL_RETURN_CANCEL);
}

void VDialog::showDialog(bool showIt)
{
	if (showIt)
	{
		show();
		setFixedSize(size()); // To have fixed-size dialogs
	}
	else
		hide();
}

void VDialog::bringDialogToTop()
{
	activateWindow();
	raise();
}

void VDialog::getDialogPositionAndSize(int post[2],int sizet[2])
{ // pos and size can be NULL
	QRect geom(geometry());
	if (sizet!=NULL)
	{
		sizet[0]=geom.width();
		sizet[1]=geom.height();
	}
	if (post!=NULL)
	{
		post[0]=geom.x();
		post[1]=geom.y();
	}
}

void VDialog::setDialogPositionAndSize(int post[2],int sizet[2])
{ // pos and size can be NULL
	QRect geom(geometry());
	if (post!=NULL)
	{
		if (sizet==NULL)
			setGeometry(post[0],post[1],geom.width(),geom.height());
		else
			setGeometry(post[0],post[1],sizet[0],sizet[1]);
	}
	else
	{
		if (sizet!=NULL)
			setGeometry(geom.x(),geom.y(),sizet[0],sizet[1]);
	}
}
