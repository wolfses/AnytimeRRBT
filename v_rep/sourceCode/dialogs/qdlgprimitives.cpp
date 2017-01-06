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
#include "qdlgprimitives.h"
#include "ui_qdlgprimitives.h"
#include "IloIlo.h"
#include "Tt.h"
#include "GV.h"
#include "v_repStrings.h"
#include "App.h"

CQDlgPrimitives::CQDlgPrimitives(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgPrimitives)
{
    ui->setupUi(this);
}

CQDlgPrimitives::~CQDlgPrimitives()
{
    delete ui;
}

void CQDlgPrimitives::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgPrimitives::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgPrimitives::initialize(int type,C3Vector* sizes)
{
	if (sizes!=NULL)
	{
		xSize=(*sizes)(0);
		ySize=(*sizes)(1);
		zSize=(*sizes)(2);
	}
	else
	{
		xSize=0.1f;
		ySize=0.1f;
		zSize=0.1f;
	}
	subdivX=0;
	subdivY=0;
	subdivZ=0;
	if (type==2)
		faces=16;
	else
		faces=0;
	sides=32;
	discSubdiv=0;
	smooth=true;
	open=false;
	pure=((type!=0)&&(type!=4)); // (planes and discs are almost never used pure!)
	cone=false;
	density=1000.0f;
	negativeVolume=false;
	negativeVolumeScaling=0.5f;
	primitiveType=type;
	sizesAreLocked=(sizes!=NULL);
	refresh();
}


void CQDlgPrimitives::refresh()
{
	ui->qqPure->setChecked(pure);
	ui->qqDensity->setEnabled(pure);
	ui->qqNegativeVolume->setEnabled(((primitiveType==1)||(primitiveType==2)||(primitiveType==3)));
	ui->qqNegativeVolume->setChecked(negativeVolume);
	ui->qqNegativeVolumeScaling->setEnabled(((primitiveType==1)||(primitiveType==2)||(primitiveType==3))&&negativeVolume);
	ui->qqDensity->setText(gv::getDensityStr(density).c_str());

	ui->qqXSize->setText(gv::getSizeStr(false,xSize,0).c_str());
	ui->qqYSize->setText(gv::getSizeStr(false,ySize,0).c_str());

	if (primitiveType==0)
	{ // Plane
		setWindowTitle(strTranslate(IDSN_PRIMITIVE_PLANE));

		ui->qqXSize->setEnabled(!sizesAreLocked);
		ui->qqYSize->setEnabled(!sizesAreLocked);
		ui->qqZSize->setEnabled(false);

		ui->qqXSubdiv->setEnabled(true);
		ui->qqYSubdiv->setEnabled(true);
		ui->qqZSubdiv->setEnabled(false);

		ui->qqFaces->setEnabled(false);
		ui->qqSides->setEnabled(false);

		ui->qqZSize->setText("");

		ui->qqFaces->setText("");
		ui->qqSides->setText("");

		ui->qqXSubdiv->setText(tt::FNb(0,subdivX,false).c_str());
		ui->qqYSubdiv->setText(tt::FNb(0,subdivY,false).c_str());
		ui->qqZSubdiv->setText("");

		ui->qqSmooth->setEnabled(false);
		ui->qqSmooth->setChecked(false);

		ui->qqOpen->setEnabled(false);
		ui->qqOpen->setChecked(false);

		ui->qqDiscSubdiv->setEnabled(false);
		ui->qqDiscSubdiv->setText("");

		ui->qqCone->setEnabled(false);
		ui->qqCone->setChecked(false);

		ui->qqNegativeVolumeScaling->setText("");
	}
	if (primitiveType==1)
	{ // Box
		setWindowTitle(strTranslate(IDSN_PRIMITIVE_RECTANGLE));

		ui->qqXSize->setEnabled(!sizesAreLocked);
		ui->qqYSize->setEnabled(!sizesAreLocked);
		ui->qqZSize->setEnabled(!sizesAreLocked);

		ui->qqXSubdiv->setEnabled(true);
		ui->qqYSubdiv->setEnabled(true);
		ui->qqZSubdiv->setEnabled(true);

		ui->qqFaces->setEnabled(false);
		ui->qqSides->setEnabled(false);

		ui->qqZSize->setText(gv::getSizeStr(false,zSize,0).c_str());

		ui->qqFaces->setText("");
		ui->qqSides->setText("");

		ui->qqXSubdiv->setText(tt::FNb(0,subdivX,false).c_str());
		ui->qqYSubdiv->setText(tt::FNb(0,subdivY,false).c_str());
		ui->qqZSubdiv->setText(tt::FNb(0,subdivZ,false).c_str());

		ui->qqSmooth->setEnabled(false);
		ui->qqSmooth->setChecked(false);

		ui->qqOpen->setEnabled(false);
		ui->qqOpen->setChecked(false);

		ui->qqDiscSubdiv->setEnabled(false);
		ui->qqDiscSubdiv->setText("");

		ui->qqCone->setEnabled(false);
		ui->qqCone->setChecked(false);

		ui->qqNegativeVolumeScaling->setText(tt::FNb(0,negativeVolumeScaling,2,false).c_str());
	}
	if (primitiveType==2)
	{ // Sphere
		setWindowTitle(strTranslate(IDSN_PRIMITIVE_SPHERE));

		ui->qqXSize->setEnabled(!sizesAreLocked);

		// Remove (!pure) in following 2 lines if pure spheroids are allowed;
		ui->qqYSize->setEnabled((!pure)&&(!sizesAreLocked));
		ui->qqZSize->setEnabled((!pure)&&(!sizesAreLocked));

		ui->qqXSubdiv->setEnabled(false);
		ui->qqYSubdiv->setEnabled(false);
		ui->qqZSubdiv->setEnabled(false);

		ui->qqFaces->setEnabled(true);
		ui->qqSides->setEnabled(true);

		ui->qqZSize->setText(gv::getSizeStr(false,zSize,0).c_str());

		ui->qqFaces->setText(tt::FNb(0,faces,false).c_str());
		ui->qqSides->setText(tt::FNb(0,sides,false).c_str());


		ui->qqXSubdiv->setText("");
		ui->qqYSubdiv->setText("");
		ui->qqZSubdiv->setText("");

		ui->qqSmooth->setEnabled(true);
		ui->qqSmooth->setChecked(smooth);

		ui->qqOpen->setEnabled(false);
		ui->qqOpen->setChecked(false);

		ui->qqDiscSubdiv->setEnabled(false);
		ui->qqDiscSubdiv->setText("");

		ui->qqCone->setEnabled(false);
		ui->qqCone->setChecked(false);

		ui->qqNegativeVolumeScaling->setText(tt::FNb(0,negativeVolumeScaling,2,false).c_str());
	}
	if (primitiveType==3)
	{ // Cylinder
		setWindowTitle(strTranslate(IDSN_PRIMITIVE_CYLINDER));

		ui->qqXSize->setEnabled(!sizesAreLocked);
		ui->qqYSize->setEnabled((!pure)&&(!sizesAreLocked));
		ui->qqZSize->setEnabled(!sizesAreLocked);

		ui->qqXSubdiv->setEnabled(false);
		ui->qqYSubdiv->setEnabled(false);
		ui->qqZSubdiv->setEnabled(false);

		ui->qqFaces->setEnabled(true);
		ui->qqSides->setEnabled(true);

		ui->qqZSize->setText(gv::getSizeStr(false,zSize,0).c_str());

		ui->qqFaces->setText(tt::FNb(0,faces,false).c_str());
		ui->qqSides->setText(tt::FNb(0,sides,false).c_str());

		ui->qqXSubdiv->setText("");
		ui->qqYSubdiv->setText("");
		ui->qqZSubdiv->setText("");

		ui->qqSmooth->setEnabled(true);
		ui->qqSmooth->setChecked(smooth);

		ui->qqOpen->setEnabled(!pure);
		ui->qqOpen->setChecked(open);

		ui->qqDiscSubdiv->setEnabled(true);
		ui->qqDiscSubdiv->setText(tt::FNb(0,discSubdiv,false).c_str());

		ui->qqCone->setEnabled(true);
		ui->qqCone->setChecked(cone);

		ui->qqNegativeVolumeScaling->setText(tt::FNb(0,negativeVolumeScaling,2,false).c_str());
	}
	if (primitiveType==4)
	{ // Disc
		setWindowTitle(strTranslate(IDSN_PRIMITIVE_DISC));

		ui->qqXSize->setEnabled(!sizesAreLocked);
		ui->qqYSize->setEnabled((!pure)&&(!sizesAreLocked));
		ui->qqZSize->setEnabled(false);

		ui->qqXSubdiv->setEnabled(false);
		ui->qqYSubdiv->setEnabled(false);
		ui->qqZSubdiv->setEnabled(false);

		ui->qqFaces->setEnabled(false);
		ui->qqSides->setEnabled(true);

		ui->qqZSize->setText("");

		ui->qqFaces->setText("");
		ui->qqSides->setText(tt::FNb(0,sides,false).c_str());

		ui->qqXSubdiv->setText("");
		ui->qqYSubdiv->setText("");
		ui->qqZSubdiv->setText("");

		ui->qqSmooth->setEnabled(false);
		ui->qqSmooth->setChecked(false);

		ui->qqOpen->setEnabled(false);
		ui->qqOpen->setChecked(false);

		ui->qqDiscSubdiv->setEnabled(true);
		ui->qqDiscSubdiv->setText(tt::FNb(0,discSubdiv,false).c_str());

		ui->qqCone->setEnabled(false);
		ui->qqCone->setChecked(false);

		ui->qqNegativeVolumeScaling->setText("");
	}

}


void CQDlgPrimitives::on_qqPure_clicked()
{
	pure=!pure;
	_adjustValuesForPurePrimitive();

	if (pure&&negativeVolume)
		App::uiThread->messageBox_warning(this,strTranslate("Primitives"),strTranslate(IDS_WARNING_WHEN_SELECTING_NEGATIVE_VOLUME),VMessageBox::OKELI);
	if (pure&&cone)
		App::uiThread->messageBox_warning(this,strTranslate("Primitives"),strTranslate(IDS_WARNING_WHEN_SELECTING_PURE_CONE),VMessageBox::OKELI);
	if (pure&&((sides<32)||((faces<16)&&(primitiveType==2))))
		App::uiThread->messageBox_warning(this,strTranslate("Primitives"),strTranslate(IDS_WARNING_WHEN_PURE_SHAPES_HAVE_LOW_POLYCOUNT),VMessageBox::OKELI);

	refresh();
}

void CQDlgPrimitives::on_qqXSize_editingFinished()
{
	bool ok;
	float newVal=ui->qqXSize->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0001f,1000.0f,newVal);
		xSize=newVal*gv::userToMeter;
		_adjustValuesForPurePrimitive();
	}
	refresh();
}

void CQDlgPrimitives::on_qqYSize_editingFinished()
{
	bool ok;
	float newVal=ui->qqYSize->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0001f,1000.0f,newVal);
		ySize=newVal*gv::userToMeter;
		_adjustValuesForPurePrimitive();
	}
	refresh();
}

void CQDlgPrimitives::on_qqZSize_editingFinished()
{
	bool ok;
	float newVal=ui->qqZSize->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0001f,1000.0f,newVal);
		zSize=newVal*gv::userToMeter;
		_adjustValuesForPurePrimitive();
	}
	refresh();
}

void CQDlgPrimitives::on_qqXSubdiv_editingFinished()
{
	bool ok;
	int newVal=ui->qqXSubdiv->text().toInt(&ok);
	if (ok)
	{
		tt::limitValue(0,100,newVal);
		subdivX=newVal;
		_adjustValuesForPurePrimitive();
	}
	refresh();
}

void CQDlgPrimitives::on_qqYSubdiv_editingFinished()
{
	bool ok;
	int newVal=ui->qqYSubdiv->text().toInt(&ok);
	if (ok)
	{
		tt::limitValue(0,100,newVal);
		subdivY=newVal;
		_adjustValuesForPurePrimitive();
	}
	refresh();
}

void CQDlgPrimitives::on_qqZSubdiv_editingFinished()
{
	bool ok;
	int newVal=ui->qqZSubdiv->text().toInt(&ok);
	if (ok)
	{
		tt::limitValue(0,100,newVal);
		subdivZ=newVal;
		_adjustValuesForPurePrimitive();
	}
	refresh();
}

void CQDlgPrimitives::on_qqSides_editingFinished()
{
	bool ok;
	int newVal=ui->qqSides->text().toInt(&ok);
	if (ok)
	{
		tt::limitValue(3,100,newVal);
		if (pure&&(newVal<32)&&(newVal<sides))
			App::uiThread->messageBox_warning(this,strTranslate("Primitives"),strTranslate(IDS_WARNING_WHEN_PURE_SHAPES_HAVE_LOW_POLYCOUNT),VMessageBox::OKELI);
		sides=newVal;
		_adjustValuesForPurePrimitive();
	}
	refresh();
}

void CQDlgPrimitives::on_qqFaces_editingFinished()
{
	bool ok;
	int newVal=ui->qqFaces->text().toInt(&ok);
	if (ok)
	{
		if (primitiveType==2) // Sphere
		{
			tt::limitValue(2,50,newVal);
			if (pure&&(newVal<16)&&(newVal<faces))
				App::uiThread->messageBox_warning(this,strTranslate("Primitives"),strTranslate(IDS_WARNING_WHEN_PURE_SHAPES_HAVE_LOW_POLYCOUNT),VMessageBox::OKELI);
		}
		if (primitiveType==3) // Cylinder
			tt::limitValue(0,50,newVal);
		faces=newVal;
		_adjustValuesForPurePrimitive();
	}
	refresh();
}

void CQDlgPrimitives::on_qqDiscSubdiv_editingFinished()
{
	bool ok;
	int newVal=ui->qqDiscSubdiv->text().toInt(&ok);
	if (ok)
	{
		tt::limitValue(0,50,newVal);
		discSubdiv=newVal;
		_adjustValuesForPurePrimitive();
	}
	refresh();
}

void CQDlgPrimitives::on_qqSmooth_clicked()
{
	smooth=!smooth;
	_adjustValuesForPurePrimitive();
	refresh();
}

void CQDlgPrimitives::on_qqOpen_clicked()
{
	open=!open;
	if (open)
		pure=false;
	_adjustValuesForPurePrimitive();
	refresh();
}

void CQDlgPrimitives::on_qqCone_clicked()
{
	cone=!cone;
	if (cone)
		negativeVolume=false;
	_adjustValuesForPurePrimitive();
	if (pure&&cone)
		App::uiThread->messageBox_warning(this,strTranslate("Primitives"),strTranslate(IDS_WARNING_WHEN_SELECTING_PURE_CONE),VMessageBox::OKELI);
	refresh();
}

void CQDlgPrimitives::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgPrimitives::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgPrimitives::_adjustValuesForPurePrimitive()
{
	if (pure)
	{
		if (primitiveType==2)
		{ // sphere
			// Comment out following if pure spheroids are allowed;
			ySize=xSize;
			zSize=xSize;
		}
		if (primitiveType==3)
		{ // cylinder
			ySize=xSize;
			open=false;
		}
		if (primitiveType==4)
		{ // disc
			ySize=xSize;
		}
	}
}

void CQDlgPrimitives::on_qqDensity_editingFinished()
{
	bool ok;
	float newVal=ui->qqDensity->text().toFloat(&ok);
	if (ok)
	{
		density=newVal*gv::userToDensity;
		tt::limitValue(0.1f,100000.0f,density);
	}
	refresh();
}

void CQDlgPrimitives::on_qqNegativeVolume_clicked()
{
	negativeVolume=!negativeVolume;
	if (negativeVolume)
		cone=false;
	if (pure&&negativeVolume)
		App::uiThread->messageBox_warning(this,strTranslate("Primitives"),strTranslate(IDS_WARNING_WHEN_SELECTING_NEGATIVE_VOLUME),VMessageBox::OKELI);
	refresh();
}

void CQDlgPrimitives::on_qqNegativeVolumeScaling_editingFinished()
{
	bool ok;
	float newVal=ui->qqNegativeVolumeScaling->text().toFloat(&ok);
	if (ok)
	{
		negativeVolumeScaling=newVal;
		tt::limitValue(0.01f,0.99f,negativeVolumeScaling);
	}
	refresh();
}
