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

#ifndef QDLGPATHPLANNING_H
#define QDLGPATHPLANNING_H

#include "DlgEx.h"
#include <QListWidgetItem>
#include <QShortcut>
#include <QComboBox>

namespace Ui {
    class CQDlgPathPlanning;
}

class CQDlgPathPlanning : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgPathPlanning(QWidget *parent = 0);
    ~CQDlgPathPlanning();

	void refresh();

	void cancelEvent();

	void updateObjectsInList();
	int getSelectedObjectID();
	void selectObjectInList(int objectID);

	bool inMainRefreshRoutine;
	bool inListSelectionRoutine;
	bool noListSelectionAllowed;

	void fillComboWithCollisionOrDistanceEntities(QComboBox* combo,bool collisionEntities,bool insertAllAtBeginning,bool insertNoneAtBeginning,int idToExclude);
	bool selectItemInCombo(QComboBox* combo,int itemID);

	QShortcut* delKeyShortcut;
	QShortcut* backspaceKeyShortcut;

	private slots:
	void onDeletePressed();

	void on_qqList_itemChanged(QListWidgetItem *item);

	void on_qqList_itemSelectionChanged();

	void on_qqAddNew_clicked();

	void on_qqGoalCombo_currentIndexChanged(int index);

	void on_qqPathCombo_currentIndexChanged(int index);

	void on_qqCheckTypeCombo_currentIndexChanged(int index);

	void on_qqRobotCombo_currentIndexChanged(int index);

	void on_qqObstacleCombo_currentIndexChanged(int index);

	void on_qqMinDist_editingFinished();

	void on_qqMaxDist_editingFinished();

	void on_qqAdjustSearchParameters_clicked();

	void on_qqUsePartialPath_clicked();

	void on_qqShowNodes_clicked();

	void on_qqMaxCalcTime_editingFinished();

	void on_qqPostProcessingPasses_editingFinished();

	void on_qqComputePath_clicked();

private:
    Ui::CQDlgPathPlanning *ui;
};

#endif // QDLGPATHPLANNING_H
