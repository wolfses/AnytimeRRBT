// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include "simplefilterdialog.h"
#include "ui_simplefilterdialog.h"
#include "v_repLib.h"

CSimpleFilterDialog::CSimpleFilterDialog(QWidget *parent) :
	QDialog(parent,Qt::Tool),
	ui(new Ui::CSimpleFilterDialog)
{
    ui->setupUi(this);
}

CSimpleFilterDialog::~CSimpleFilterDialog()
{
    delete ui;
}

void CSimpleFilterDialog::refresh()
{

	ui->qqSize->setText(QString::number(sizeValue));
	ui->qqThreshold->setText(QString::number(thresholdValue));
}


void CSimpleFilterDialog::on_qqSize_editingFinished()
{
	bool ok;
	int v=ui->qqSize->text().toInt(&ok);
	if (ok)
	{
		if (v<1)
			v=1;
		if (v>100)
			v=100;
		sizeValue=v;
	}
	refresh();
}

void CSimpleFilterDialog::on_qqThreshold_editingFinished()
{
	bool ok;
	float v=ui->qqThreshold->text().toFloat(&ok);
	if (ok)
	{
		if (v<0.0f)
			v=0.0f;
		if (v>1.0f)
			v=1.0f;
		thresholdValue=v;
	}
	refresh();
}

void CSimpleFilterDialog::on_qqOkCancel_accepted()
{
	done(QDialog::Accepted);
}

void CSimpleFilterDialog::on_qqOkCancel_rejected()
{
	done(QDialog::Rejected);
}
