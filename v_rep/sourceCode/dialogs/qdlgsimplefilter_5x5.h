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

#ifndef QDLGSIMPLEFILTER_5X5_H
#define QDLGSIMPLEFILTER_5X5_H

#include "qdlgsimplefilter.h"

namespace Ui {
	class CQDlgSimpleFilter_5x5;
}

class CQDlgSimpleFilter_5x5 : public CQDlgSimpleFilter
{
    Q_OBJECT

public:
	explicit CQDlgSimpleFilter_5x5(QWidget *parent = 0);
	~CQDlgSimpleFilter_5x5();

	void refresh();

private slots:
	void on_qqOkCancel_accepted();

	void on_qqOkCancel_rejected();

	void on_qqPasses_editingFinished();

	void on_qqMultiplier_editingFinished();

	void on_qq0_editingFinished();
	void on_qq1_editingFinished();
	void on_qq2_editingFinished();
	void on_qq3_editingFinished();
	void on_qq4_editingFinished();
	void on_qq5_editingFinished();
	void on_qq6_editingFinished();
	void on_qq7_editingFinished();
	void on_qq8_editingFinished();
	void on_qq9_editingFinished();
	void on_qq10_editingFinished();
	void on_qq11_editingFinished();
	void on_qq12_editingFinished();
	void on_qq13_editingFinished();
	void on_qq14_editingFinished();
	void on_qq15_editingFinished();
	void on_qq16_editingFinished();
	void on_qq17_editingFinished();
	void on_qq18_editingFinished();
	void on_qq19_editingFinished();
	void on_qq20_editingFinished();
	void on_qq21_editingFinished();
	void on_qq22_editingFinished();
	void on_qq23_editingFinished();
	void on_qq24_editingFinished();

private:
	Ui::CQDlgSimpleFilter_5x5 *ui;
};

#endif // QDLGSIMPLEFILTER_5X5_H
