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

#ifndef QDLGGRAPHS_H
#define QDLGGRAPHS_H

#include "DlgEx.h"
#include <QListWidgetItem>
#include <QShortcut>

namespace Ui {
    class CQDlgGraphs;
}

class CQDlgGraphs : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgGraphs(QWidget *parent = 0);
    ~CQDlgGraphs();

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
	void on_qqAddNewDataStream_clicked();

	void on_qqRecordingList_itemSelectionChanged();

	void on_qqRecordingList_itemChanged(QListWidgetItem *item);

	void on_qqExplicitHandling_clicked();

	void on_qqBufferIsCyclic_clicked();

	void on_qqShowXYZPlanes_clicked();

	void on_qqObjectSize_editingFinished();

	void on_qqBufferSize_editingFinished();

	void on_qqRemoveAllStatics_clicked();

	void on_qqDisplayGrids_clicked();

	void on_qqDisplayGridValues_clicked();

	void on_qqAdjustBackgroundColor_clicked();

	void on_qqAdjustGridColor_clicked();

	void on_qqTransformationCombo_currentIndexChanged(int index);

	void on_qqTransformationCoeff_editingFinished();

	void on_qqTransformationOffset_editingFinished();

	void on_qqMovingAveragePeriod_editingFinished();

	void on_qqTimeGraphVisible_clicked();

	void on_qqShowLabel_clicked();

	void on_qqLinkPoints_clicked();

	void on_qqAdjustCurveColor_clicked();

	void on_qqDuplicateToStatic_clicked();

	void on_qqEditXYGraphs_clicked();

	void on_qqEdit3DCurves_clicked();

private:
    Ui::CQDlgGraphs *ui;
};

#endif // QDLGGRAPHS_H
