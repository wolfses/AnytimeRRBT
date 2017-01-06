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

#ifndef QDLGIK_H
#define QDLGIK_H

#include "DlgEx.h"
#include <QListWidgetItem>
#include <QShortcut>

namespace Ui {
    class CQDlgIk;
}

class CQDlgIk : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgIk(QWidget *parent = 0);
    ~CQDlgIk();

	void refresh();

	void cancelEvent();

	void updateObjectsInList();
	int getSelectedObjectID();
	void selectObjectInList(int objectID);

	bool inMainRefreshRoutine;
	bool inListSelectionRoutine;
	bool noListSelectionAllowed;

	QShortcut* delKeyShortcut;
	QShortcut* backspaceKeyShortcut;

	private slots:
	void onDeletePressed();

	void on_qqIkEnabled_clicked();

	void on_qqAddNewGroup_clicked();

	void on_qqList_itemChanged(QListWidgetItem *item);

	void on_qqList_itemSelectionChanged();

	void on_qqUp_clicked();

	void on_qqDown_clicked();

	void on_qqExplicitHandling_clicked();

	void on_qqGroupIsActive_clicked();

	void on_qqRedundant_clicked();

	void on_qqIgnoreMaxStepSizes_clicked();

	void on_qqCalcMethodCombo_currentIndexChanged(int index);

	void on_qqDamping_editingFinished();

	void on_qqMaxIterations_editingFinished();

	void on_qqWeightJointLimits_editingFinished();

	void on_qqWeightObstacleAvoidance_editingFinished();

	void on_qqThresholdLinear_editingFinished();

	void on_qqThresholdAngular_editingFinished();

	void on_qqEditConditional_clicked();

	void on_qqEditObstacleAvoidance_clicked();

	void on_qqEditIkElements_clicked();

private:
    Ui::CQDlgIk *ui;
};

#endif // QDLGIK_H



