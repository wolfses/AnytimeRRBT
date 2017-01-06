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

#ifndef QDLGJOINTDYN_H
#define QDLGJOINTDYN_H

#include "DlgEx.h"

namespace Ui {
    class CQDlgJointDyn;
}

class CQDlgJointDyn : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgJointDyn(QWidget *parent = 0);
    ~CQDlgJointDyn();

	void refresh();
	void cancelEvent();

private slots:
	void on_qqMotorEnabled_clicked();

	void on_qqTargetVelocity_editingFinished();

	void on_qqAdjustEngineProperties_clicked();

	void on_qqMaxForce_editingFinished();

	void on_qqApplyDynamicProperties_clicked();

	void on_qqControlEnabled_clicked();

	// void on_qqRegulateVelocity_clicked();

	void on_qqVelocityUpperLimit_editingFinished();

	// void on_qqRegulateForce_clicked();

	void on_qqTargetPosition_editingFinished();

	void on_qqP_editingFinished();

	void on_qqI_editingFinished();

	void on_qqD_editingFinished();

	void on_qqApplyControlParameters_clicked();

	void on_qqCustomControl_clicked();

	void on_qqPositionControl_clicked();

	void on_qqEditCustomControl_clicked();

	void on_qqSpringControl_clicked();

	void on_qqK_editingFinished();

	void on_qqC_editingFinished();

private:
    Ui::CQDlgJointDyn *ui;
};

#endif // QDLGJOINTDYN_H
