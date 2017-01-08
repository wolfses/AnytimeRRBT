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

#ifndef QDLGCOLLISIONS_H
#define QDLGCOLLISIONS_H

#include "DlgEx.h"
#include <QListWidgetItem>

namespace Ui {
	class CQDlgCollisions;
}

class CQDlgCollisions : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgCollisions(QWidget *parent = 0);
    ~CQDlgCollisions();

	void refresh();

	void cancelEvent();

	void updateObjectsInList();
	int getSelectedObjectID();
	void selectObjectInList(int objectID);
	bool inSelectionRoutine;

private slots:
	void onDeletePressed();
	void on_qqAddNewObject_clicked();

	void on_qqCollisionList_itemSelectionChanged();

	void on_qqCollisionList_itemChanged(QListWidgetItem *item);

	void on_qqEnableAll_clicked();

	void on_qqCollisionColor_clicked();

	void on_qqExplicitHandling_clicked();

	void on_qqColliderColorChanges_clicked();

	void on_qqComputeContour_clicked();

	void on_qqCollideeColorChanges_clicked();

	void on_qqAdjustContourColor_clicked();

	void on_qqContourWidth_editingFinished();

private:
    Ui::CQDlgCollisions *ui;
};

#endif // QDLGCOLLISIONS_H
