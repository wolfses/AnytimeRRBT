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
#include "qdlgmodelthumbnailvisu.h"
#include "ui_qdlgmodelthumbnailvisu.h"
#include "Thumbnail.h"
#include "App.h"

CQDlgModelThumbnailVisu::CQDlgModelThumbnailVisu(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgModelThumbnailVisu)
{
    ui->setupUi(this);
}

CQDlgModelThumbnailVisu::~CQDlgModelThumbnailVisu()
{
    delete ui;
}

void CQDlgModelThumbnailVisu::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgModelThumbnailVisu::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgModelThumbnailVisu::applyThumbnail(const CThumbnail* thumbnail)
{
	const char* rgba=((CThumbnail*)thumbnail)->getPointerToUncompressedImage();
	QImage img(128,128,QImage::Format_RGB32);
	QRgb value;
	for (int i=0;i<128;i++)
	{
		for (int j=0;j<128;j++)
		{
			char r=rgba[(i*128+j)*4+0];
			char g=rgba[(i*128+j)*4+1];
			char b=rgba[(i*128+j)*4+2];

			if (rgba[(i*128+j)*4+3]==0)
			{
				r=255;
				g=255;
				b=255;
			}
			value=qRgb(r,g,b);
			img.setPixel(j,127-i,value);
		}
	}
	QPixmap pimg;
	pimg.convertFromImage(img);
	ui->qqImage->setPixmap(pimg);
}

void CQDlgModelThumbnailVisu::on_qqYesNo_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgModelThumbnailVisu::on_qqYesNo_rejected()
{
	defaultModalDialogEndRoutine(false);
}
