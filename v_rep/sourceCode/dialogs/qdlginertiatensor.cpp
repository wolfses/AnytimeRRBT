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
#include "qdlginertiatensor.h"
#include "ui_qdlginertiatensor.h"
#include "v_rep_internal.h"
#include "App.h"
#include "Tt.h"
#include "IloIlo.h"
#include "GV.h"

CQDlgInertiaTensor::CQDlgInertiaTensor(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
	ui(new Ui::CQDlgInertiaTensor)
{
    ui->setupUi(this);
}

CQDlgInertiaTensor::~CQDlgInertiaTensor()
{
    delete ui;
}

void CQDlgInertiaTensor::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgInertiaTensor::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgInertiaTensor::refresh()
{
	ui->qqI11->setText(gv::getInertiaOverMassStr(tensor[0]).c_str());
	ui->qqI21->setText(gv::getInertiaOverMassStr(tensor[1]).c_str());
	ui->qqI31->setText(gv::getInertiaOverMassStr(tensor[2]).c_str());
	ui->qqI12->setText(gv::getInertiaOverMassStr(tensor[3]).c_str());
	ui->qqI22->setText(gv::getInertiaOverMassStr(tensor[4]).c_str());
	ui->qqI32->setText(gv::getInertiaOverMassStr(tensor[5]).c_str());
	ui->qqI13->setText(gv::getInertiaOverMassStr(tensor[6]).c_str());
	ui->qqI23->setText(gv::getInertiaOverMassStr(tensor[7]).c_str());
	ui->qqI33->setText(gv::getInertiaOverMassStr(tensor[8]).c_str());

	ui->qqMx->setText(gv::getSizeStr(true,com[0]).c_str());
	ui->qqMy->setText(gv::getSizeStr(true,com[1]).c_str());
	ui->qqMz->setText(gv::getSizeStr(true,com[2]).c_str());

	ui->qqShapeFrame->setChecked(relativeToShapeFrame);
	ui->qqWorldFrame->setChecked(!relativeToShapeFrame);
}

void CQDlgInertiaTensor::on_qqI11_editingFinished()
{
	bool ok;
	float newVal=ui->qqI11->text().toFloat(&ok);
	if (ok)
		tensor[0]=newVal;
	refresh();
}

void CQDlgInertiaTensor::on_qqI21_editingFinished()
{
	bool ok;
	float newVal=ui->qqI21->text().toFloat(&ok);
	if (ok)
	{
		tensor[1]=newVal;
		tensor[3]=newVal;
	}
	refresh();
}

void CQDlgInertiaTensor::on_qqI31_editingFinished()
{
	bool ok;
	float newVal=ui->qqI31->text().toFloat(&ok);
	if (ok)
	{
		tensor[2]=newVal;
		tensor[6]=newVal;
	}
	refresh();
}

void CQDlgInertiaTensor::on_qqI12_editingFinished()
{
	bool ok;
	float newVal=ui->qqI12->text().toFloat(&ok);
	if (ok)
	{
		tensor[3]=newVal;
		tensor[1]=newVal;
	}
	refresh();
}

void CQDlgInertiaTensor::on_qqI22_editingFinished()
{
	bool ok;
	float newVal=ui->qqI22->text().toFloat(&ok);
	if (ok)
		tensor[4]=newVal;
	refresh();
}

void CQDlgInertiaTensor::on_qqI32_editingFinished()
{
	bool ok;
	float newVal=ui->qqI32->text().toFloat(&ok);
	if (ok)
	{
		tensor[5]=newVal;
		tensor[7]=newVal;
	}
	refresh();
}

void CQDlgInertiaTensor::on_qqI13_editingFinished()
{
	bool ok;
	float newVal=ui->qqI13->text().toFloat(&ok);
	if (ok)
	{
		tensor[6]=newVal;
		tensor[2]=newVal;
	}
	refresh();
}

void CQDlgInertiaTensor::on_qqI23_editingFinished()
{
	bool ok;
	float newVal=ui->qqI23->text().toFloat(&ok);
	if (ok)
	{
		tensor[7]=newVal;
		tensor[5]=newVal;
	}
	refresh();
}

void CQDlgInertiaTensor::on_qqI33_editingFinished()
{
	bool ok;
	float newVal=ui->qqI33->text().toFloat(&ok);
	if (ok)
		tensor[8]=newVal;
	refresh();
}

void CQDlgInertiaTensor::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgInertiaTensor::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgInertiaTensor::on_qqMx_editingFinished()
{
	bool ok;
	float newVal=ui->qqMx->text().toFloat(&ok);
	if (ok)
		com[0]=newVal;
	refresh();
}

void CQDlgInertiaTensor::on_qqMy_editingFinished()
{
	bool ok;
	float newVal=ui->qqMy->text().toFloat(&ok);
	if (ok)
		com[1]=newVal;
	refresh();
}

void CQDlgInertiaTensor::on_qqMz_editingFinished()
{
	bool ok;
	float newVal=ui->qqMz->text().toFloat(&ok);
	if (ok)
		com[2]=newVal;
	refresh();
}

void CQDlgInertiaTensor::on_qqShapeFrame_clicked()
{
	if (!relativeToShapeFrame)
	{
		CShape* it=App::ct->objCont->getShape(shapeHandle);
		C7Vector shapeTr(it->getCumulativeTransformation());
		C7Vector shapeTrInv(shapeTr.getInverse());
		C3Vector com_(com);
		com_*=shapeTrInv;
		com_.copyTo(com);

		C3X3Matrix m;
		m.copyFromInterface(tensor);
		m=shapeTrInv.Q.getMatrix()*m*shapeTr.Q.getMatrix();
		m.copyToInterface(tensor);
	}
	relativeToShapeFrame=true;
	refresh();
}

void CQDlgInertiaTensor::on_qqWorldFrame_clicked()
{
	if (relativeToShapeFrame)
	{
		CShape* it=App::ct->objCont->getShape(shapeHandle);
		C7Vector shapeTr(it->getCumulativeTransformation());
		C7Vector shapeTrInv(shapeTr.getInverse());
		C3Vector com_(com);
		com_*=shapeTr;
		com_.copyTo(com);

		C3X3Matrix m;
		m.copyFromInterface(tensor);
		m=shapeTr.Q.getMatrix()*m*shapeTrInv.Q.getMatrix();
		m.copyToInterface(tensor);
	}
	relativeToShapeFrame=false;
	refresh();
}
