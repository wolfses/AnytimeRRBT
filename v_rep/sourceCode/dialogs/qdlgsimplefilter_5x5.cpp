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
#include "qdlgsimplefilter_5x5.h"
#include "ui_qdlgsimplefilter_5x5.h"
#include "GV.h"
#include "Tt.h"

CQDlgSimpleFilter_5x5::CQDlgSimpleFilter_5x5(QWidget *parent) :
	CQDlgSimpleFilter(parent),
	ui(new Ui::CQDlgSimpleFilter_5x5)
{
    ui->setupUi(this);
}

CQDlgSimpleFilter_5x5::~CQDlgSimpleFilter_5x5()
{
    delete ui;
}

void CQDlgSimpleFilter_5x5::refresh()
{
	ui->qqPasses->setText(tt::FNb(_intParameters[0]).c_str());
	ui->qqMultiplier->setText(tt::FNb(0,_floatParameters[25],4,true).c_str());
	ui->qq0->setText(tt::FNb(0,_floatParameters[0],4,true).c_str());
	ui->qq1->setText(tt::FNb(0,_floatParameters[1],4,true).c_str());
	ui->qq2->setText(tt::FNb(0,_floatParameters[2],4,true).c_str());
	ui->qq3->setText(tt::FNb(0,_floatParameters[3],4,true).c_str());
	ui->qq4->setText(tt::FNb(0,_floatParameters[4],4,true).c_str());
	ui->qq5->setText(tt::FNb(0,_floatParameters[5],4,true).c_str());
	ui->qq6->setText(tt::FNb(0,_floatParameters[6],4,true).c_str());
	ui->qq7->setText(tt::FNb(0,_floatParameters[7],4,true).c_str());
	ui->qq8->setText(tt::FNb(0,_floatParameters[8],4,true).c_str());
	ui->qq9->setText(tt::FNb(0,_floatParameters[9],4,true).c_str());
	ui->qq10->setText(tt::FNb(0,_floatParameters[10],4,true).c_str());
	ui->qq11->setText(tt::FNb(0,_floatParameters[11],4,true).c_str());
	ui->qq12->setText(tt::FNb(0,_floatParameters[12],4,true).c_str());
	ui->qq13->setText(tt::FNb(0,_floatParameters[13],4,true).c_str());
	ui->qq14->setText(tt::FNb(0,_floatParameters[14],4,true).c_str());
	ui->qq15->setText(tt::FNb(0,_floatParameters[15],4,true).c_str());
	ui->qq16->setText(tt::FNb(0,_floatParameters[16],4,true).c_str());
	ui->qq17->setText(tt::FNb(0,_floatParameters[17],4,true).c_str());
	ui->qq18->setText(tt::FNb(0,_floatParameters[18],4,true).c_str());
	ui->qq19->setText(tt::FNb(0,_floatParameters[19],4,true).c_str());
	ui->qq20->setText(tt::FNb(0,_floatParameters[20],4,true).c_str());
	ui->qq21->setText(tt::FNb(0,_floatParameters[21],4,true).c_str());
	ui->qq22->setText(tt::FNb(0,_floatParameters[22],4,true).c_str());
	ui->qq23->setText(tt::FNb(0,_floatParameters[23],4,true).c_str());
	ui->qq24->setText(tt::FNb(0,_floatParameters[24],4,true).c_str());
}

void CQDlgSimpleFilter_5x5::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgSimpleFilter_5x5::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgSimpleFilter_5x5::on_qqPasses_editingFinished()
{
	bool ok;
	int newVal=ui->qqPasses->text().toInt(&ok);
	if (ok)
	{
		tt::limitValue(1,10,newVal);
		_intParameters[0]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qqMultiplier_editingFinished()
{
	bool ok;
	float newVal=ui->qqMultiplier->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[25]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq0_editingFinished()
{
	bool ok;
	float newVal=ui->qq0->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[0]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq1_editingFinished()
{
	bool ok;
	float newVal=ui->qq1->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[1]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq2_editingFinished()
{
	bool ok;
	float newVal=ui->qq2->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[2]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq3_editingFinished()
{
	bool ok;
	float newVal=ui->qq3->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[3]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq4_editingFinished()
{
	bool ok;
	float newVal=ui->qq4->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[4]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq5_editingFinished()
{
	bool ok;
	float newVal=ui->qq5->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[5]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq6_editingFinished()
{
	bool ok;
	float newVal=ui->qq6->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[6]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq7_editingFinished()
{
	bool ok;
	float newVal=ui->qq7->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[7]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq8_editingFinished()
{
	bool ok;
	float newVal=ui->qq8->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[8]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq9_editingFinished()
{
	bool ok;
	float newVal=ui->qq9->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[9]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq10_editingFinished()
{
	bool ok;
	float newVal=ui->qq10->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[10]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq11_editingFinished()
{
	bool ok;
	float newVal=ui->qq11->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[11]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq12_editingFinished()
{
	bool ok;
	float newVal=ui->qq12->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[12]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq13_editingFinished()
{
	bool ok;
	float newVal=ui->qq13->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[13]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq14_editingFinished()
{
	bool ok;
	float newVal=ui->qq14->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[14]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq15_editingFinished()
{
	bool ok;
	float newVal=ui->qq15->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[15]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq16_editingFinished()
{
	bool ok;
	float newVal=ui->qq16->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[16]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq17_editingFinished()
{
	bool ok;
	float newVal=ui->qq17->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[17]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq18_editingFinished()
{
	bool ok;
	float newVal=ui->qq18->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[18]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq19_editingFinished()
{
	bool ok;
	float newVal=ui->qq19->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[19]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq20_editingFinished()
{
	bool ok;
	float newVal=ui->qq20->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[20]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq21_editingFinished()
{
	bool ok;
	float newVal=ui->qq21->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[21]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq22_editingFinished()
{
	bool ok;
	float newVal=ui->qq22->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[22]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq23_editingFinished()
{
	bool ok;
	float newVal=ui->qq23->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[23]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_5x5::on_qq24_editingFinished()
{
	bool ok;
	float newVal=ui->qq24->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[24]=newVal;
	}
	refresh();
}
