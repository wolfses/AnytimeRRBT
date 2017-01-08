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
#include "qdlgimportoptions.h"
#include "ui_qdlgimportoptions.h"
#include "Tt.h"
#include "Global.h"
#include "App.h"


CQDlgImportOptions::CQDlgImportOptions(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
	ui(new Ui::CQDlgImportOptions)
{
    ui->setupUi(this);
	scaleF=1.0f;
	rotX=0.0f;
}

CQDlgImportOptions::~CQDlgImportOptions()
{
    delete ui;
}

void CQDlgImportOptions::cancelEvent()
{
//	defaultModalDialogEndRoutine(false);
}

void CQDlgImportOptions::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgImportOptions::refresh()
{
	ui->qqM->setChecked(sizeIndex==0);
	ui->qqFt->setChecked(sizeIndex==1);
	ui->qq10Cm->setChecked(sizeIndex==2);
	ui->qqIn->setChecked(sizeIndex==3);
	ui->qqCm->setChecked(sizeIndex==4);
	ui->qqMm->setChecked(sizeIndex==5);

	ui->qqY->setChecked(!zIsUp);
	ui->qqZ->setChecked(zIsUp);

	simScaleSelectedObjects_internal(scaleF,true);
	std::vector<int> sel;
	App::ct->objCont->getSelectedObjects(sel);
	C4X4Matrix _r;
	_r.setIdentity();
	_r.M.buildXRotation(rotX);
	C7Vector rot(_r);
	for (int i=0;i<int(sel.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(sel[i]);
		it->setSizeFactor(1.0f);
		if (fabs(rotX)>0.01)
		{
			C7Vector tr(it->getLocalTransformation());
			it->setLocalTransformation(rot*tr);
		}
	}

	int pageIndex=App::ct->oglSurface->pageCont->getActivePageIndex();
	CSPage* page=App::ct->oglSurface->pageCont->getPage(pageIndex);
	if (page!=NULL)
	{
		int ind=page->getLastMouseDownViewIndex();
		if (ind==-1)
			ind=0;
		CSView* view=page->getView(ind);
		if (view!=NULL)
		{
			CCamera* cam=App::ct->objCont->getCamera(view->getLinkedObjectID());
			if ( (cam!=NULL) )
			{
				int viewSize[2];
				view->getViewSize(viewSize);

				cam->frameSceneOrSelectedObjects(float(viewSize[0])/float(viewSize[1]),view->getPerspectiveDisplay(),NULL,true,true,1.0f);
				cam->setFogTimer(4.0f);
			}
		}
	}
	App::mainWindow->uiThread_renderScene(true); // otherwise we won't see the new scene content correctly
}

void CQDlgImportOptions::on_qqOk_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgImportOptions::on_qqM_clicked()
{
	scaleF=1.0f;
	rotX=0.0f;
	if (sizeIndex==1)
		scaleF=100.0f/30.48f; // foot to meter
	if (sizeIndex==2)
		scaleF=10.0f; // 10cm to meter
	if (sizeIndex==3)
		scaleF=100.0f/2.54f; // inch to meter
	if (sizeIndex==4)
		scaleF=100.0f; // cm to meter
	if (sizeIndex==5)
		scaleF=1000.0f; // mm to meter
	sizeIndex=0;
	refresh();
}

void CQDlgImportOptions::on_qqFt_clicked()
{
	scaleF=1.0f;
	rotX=0.0f;
	if (sizeIndex==0)
		scaleF=0.3048f; // meter to foot
	if (sizeIndex==2)
		scaleF=3.048f; // 10cm to foot
	if (sizeIndex==3)
		scaleF=12.0f; // inch to foot
	if (sizeIndex==4)
		scaleF=30.48f; // cm to foot
	if (sizeIndex==5)
		scaleF=304.8f; // mm to foot
	sizeIndex=1;
	refresh();
}

void CQDlgImportOptions::on_qq10Cm_clicked()
{
	scaleF=1.0f;
	rotX=0.0f;
	if (sizeIndex==0)
		scaleF=0.1f; // meter to 10cm
	if (sizeIndex==1)
		scaleF=10.0f/30.48f; // foot to 10cm
	if (sizeIndex==3)
		scaleF=10.0f/2.54f; // inch to 10cm
	if (sizeIndex==4)
		scaleF=10.0f; // cm to 10cm
	if (sizeIndex==5)
		scaleF=100.0f; // mm to 10cm
	sizeIndex=2;
	refresh();
}

void CQDlgImportOptions::on_qqIn_clicked()
{
	scaleF=1.0f;
	rotX=0.0f;
	if (sizeIndex==0)
		scaleF=0.0254f; // meter to inch
	if (sizeIndex==1)
		scaleF=1.0f/12.0f; // foot to inch
	if (sizeIndex==2)
		scaleF=0.254f; // 10cm to inch
	if (sizeIndex==4)
		scaleF=2.54f; // cm to inch
	if (sizeIndex==5)
		scaleF=25.4f; // mm to inch
	sizeIndex=3;
	refresh();
}

void CQDlgImportOptions::on_qqCm_clicked()
{
	scaleF=1.0f;
	rotX=0.0f;
	if (sizeIndex==0)
		scaleF=0.01f; // meter to cm
	if (sizeIndex==1)
		scaleF=1.0f/30.48f; // foot to cm
	if (sizeIndex==2)
		scaleF=0.1f; // 10cm to cm
	if (sizeIndex==3)
		scaleF=1.0f/2.54f; // inch to cm
	if (sizeIndex==5)
		scaleF=10.0f; // mm to cm
	sizeIndex=4;
	refresh();
}

void CQDlgImportOptions::on_qqMm_clicked()
{
	scaleF=1.0f;
	rotX=0.0f;
	if (sizeIndex==0)
		scaleF=0.001f; // meter to mm
	if (sizeIndex==1)
		scaleF=0.1f/30.48f; // foot to mm
	if (sizeIndex==2)
		scaleF=0.01f; // 10cm to mm
	if (sizeIndex==3)
		scaleF=0.1f/2.54f; // inch to mm
	if (sizeIndex==4)
		scaleF=0.1f; // cm to mm
	sizeIndex=5;
	refresh();
}

void CQDlgImportOptions::on_qqY_clicked()
{
	scaleF=1.0f;
	rotX=0.0f;
	if (zIsUp)
		rotX=piValD2;
	zIsUp=false;
	refresh();
}

void CQDlgImportOptions::on_qqZ_clicked()
{
	scaleF=1.0f;
	rotX=0.0f;
	if (!zIsUp)
		rotX=-piValD2;
	zIsUp=true;
	refresh();
}

