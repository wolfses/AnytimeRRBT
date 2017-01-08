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

#ifndef QDLGPATHPLANNINGPARAMS_H
#define QDLGPATHPLANNINGPARAMS_H

#include "VDialog.h"

namespace Ui {
    class CQDlgPathPlanningParams;
}

class CQDlgPathPlanningParams : public VDialog
{
    Q_OBJECT

public:
    explicit CQDlgPathPlanningParams(QWidget *parent = 0);
    ~CQDlgPathPlanningParams();

	void refresh();

	void cancelEvent();
	void okEvent();

	int taskID;
	bool inMainRefreshRoutine;

private slots:
	void on_qqClose_clicked(QAbstractButton *button);

	void on_qqHoloTypeCombo_currentIndexChanged(int index);

	void on_qqDeltaX_editingFinished();

	void on_qqDeltaY_editingFinished();

	void on_qqdeltaZ_editingFinished();

	void on_qqStepSizeL_editingFinished();

	void on_qqStepSizeA_editingFinished();

	void on_qqMinTurningCircle_editingFinished();

	void on_qqVisualizeArea_clicked();

	void on_qqMinX_editingFinished();

	void on_qqMinY_editingFinished();

	void on_qqMinZ_editingFinished();

	void on_qqMinD_editingFinished();

	void on_qqMinX_2_editingFinished();

	void on_qqMinY_2_editingFinished();

	void on_qqMinZ_2_editingFinished();

	void on_qqMinD_2_editingFinished();

	void on_qqMinusX_clicked();

	void on_qqPlusX_clicked();

	void on_qqMinusY_clicked();

	void on_qqPlusY_clicked();

	void on_qqMinusZ_clicked();

	void on_qqPlusZ_clicked();

	void on_qqMinusD_clicked();

	void on_qqPlusD_clicked();

private:
    Ui::CQDlgPathPlanningParams *ui;
};

#endif // QDLGPATHPLANNINGPARAMS_H
