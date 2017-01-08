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
#include "qdlgconvexdecomposition.h"
#include "ui_qdlgconvexdecomposition.h"
#include "Tt.h"
#include "GV.h"
#include "App.h"

bool CQDlgConvexDecomposition::addExtraDistPoints=true;
bool CQDlgConvexDecomposition::addFacesPoints=true;
int CQDlgConvexDecomposition::nClusters=1;
int CQDlgConvexDecomposition::maxHullVertices=200; // from 100 to 200 on 5/2/2014
float CQDlgConvexDecomposition::maxConcavity=100.0f;
float CQDlgConvexDecomposition::smallClusterThreshold=0.25f;
int CQDlgConvexDecomposition::maxTrianglesInDecimatedMesh=500;
float CQDlgConvexDecomposition::maxConnectDist=30.0f;
bool CQDlgConvexDecomposition::individuallyConsiderMultishapeComponents=false;
bool CQDlgConvexDecomposition::randomColors=false;
int CQDlgConvexDecomposition::maxIterations=4;

CQDlgConvexDecomposition::CQDlgConvexDecomposition(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
	ui(new Ui::CQDlgConvexDecomposition)
{
    ui->setupUi(this);
}

CQDlgConvexDecomposition::~CQDlgConvexDecomposition()
{
    delete ui;
}

void CQDlgConvexDecomposition::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgConvexDecomposition::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgConvexDecomposition::refresh()
{
	ui->qqIndividualComponents->setChecked(individuallyConsiderMultishapeComponents);
	ui->qqRandomColors->setChecked(randomColors);
	ui->qqExtraDistPoints->setChecked(addExtraDistPoints);
	ui->qqExtraFacesPoints->setChecked(addFacesPoints);
	ui->qqClusters->setText(tt::FNb(0,nClusters,false).c_str());
	ui->qqConcavity->setText(gv::getSizeStr(false,maxConcavity).c_str());
	ui->qqConnectDist->setText(gv::getSizeStr(false,maxConnectDist).c_str());
	ui->qqTargetBlabla->setText(tt::FNb(0,maxTrianglesInDecimatedMesh,false).c_str());
	ui->qqMaxHullVertices->setText(tt::FNb(0,maxHullVertices,false).c_str());
	ui->qqSmallClusterThreshold->setText(tt::FNb(0,smallClusterThreshold,2,false).c_str());
	ui->qqMaxIterations->setText(tt::FNb(0,maxIterations,false).c_str());
}


void CQDlgConvexDecomposition::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgConvexDecomposition::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgConvexDecomposition::on_qqExtraDistPoints_clicked()
{
	addExtraDistPoints=!addExtraDistPoints;
	refresh();
}

void CQDlgConvexDecomposition::on_qqExtraFacesPoints_clicked()
{
	addFacesPoints=!addFacesPoints;
	refresh();
}

void CQDlgConvexDecomposition::on_qqClusters_editingFinished()
{
	bool ok;
	int newVal=ui->qqClusters->text().toInt(&ok);
	if (ok)
		nClusters=tt::getLimitedInt(1,100,newVal);
	refresh();
}

void CQDlgConvexDecomposition::on_qqConcavity_editingFinished()
{
	bool ok;
	float newVal=ui->qqConcavity->text().toFloat(&ok);
	if (ok)
	{
		newVal*=gv::userToMeter;
		maxConcavity=tt::getLimitedFloat(0.01f,100000.0f,newVal);
	}
	refresh();
}

void CQDlgConvexDecomposition::on_qqConnectDist_editingFinished()
{
	bool ok;
	float newVal=ui->qqConnectDist->text().toFloat(&ok);
	if (ok)
	{
		newVal*=gv::userToMeter;
		maxConnectDist=tt::getLimitedFloat(0.001f,1000.0f,newVal);
	}
	refresh();
}

void CQDlgConvexDecomposition::on_qqTargetBlabla_editingFinished()
{
	bool ok;
	int newVal=ui->qqTargetBlabla->text().toInt(&ok);
	if (ok)
		maxTrianglesInDecimatedMesh=tt::getLimitedInt(4,100000,newVal);
	refresh();
}

void CQDlgConvexDecomposition::on_qqMaxHullVertices_editingFinished()
{
	bool ok;
	int newVal=ui->qqMaxHullVertices->text().toInt(&ok);
	if (ok)
		maxHullVertices=tt::getLimitedInt(4,100000,newVal);
	refresh();
}

void CQDlgConvexDecomposition::on_qqSmallClusterThreshold_editingFinished()
{
	bool ok;
	float newVal=ui->qqSmallClusterThreshold->text().toFloat(&ok);
	if (ok)
		smallClusterThreshold=tt::getLimitedFloat(0.01f,1.0f,newVal);
	refresh();
}

void CQDlgConvexDecomposition::on_qqIndividualComponents_clicked()
{
	individuallyConsiderMultishapeComponents=!individuallyConsiderMultishapeComponents;
	refresh();
}

void CQDlgConvexDecomposition::on_qqRandomColors_clicked()
{
	randomColors=!randomColors;
	refresh();
}

void CQDlgConvexDecomposition::on_qqMaxIterations_editingFinished()
{
	bool ok;
	int newVal=ui->qqMaxIterations->text().toInt(&ok);
	if (ok)
		maxIterations=tt::getLimitedInt(1,10,newVal);
	refresh();
}
