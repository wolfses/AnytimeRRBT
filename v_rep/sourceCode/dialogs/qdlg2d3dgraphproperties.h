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

#ifndef QDLG2D3DGRAPHPROPERTIES_H
#define QDLG2D3DGRAPHPROPERTIES_H

#include "VDialog.h"
#include <QListWidgetItem>

namespace Ui {
    class CQDlg2D3DGraphProperties;
}

class CQDlg2D3DGraphProperties : public VDialog
{
    Q_OBJECT

public:
    explicit CQDlg2D3DGraphProperties(QWidget *parent = 0);
    ~CQDlg2D3DGraphProperties();

	void cancelEvent();
	void okEvent();

	void refresh();
	void updateObjectsInList();
	int getSelectedObjectID();
	void selectObjectInList(int objectID);

	bool inListSelectionRoutine;
	bool xyGraph;

private slots:
	void onDeletePressed();

	void on_qqAddNewCurve_clicked();

	void on_qqList_itemSelectionChanged();

	void on_qqList_itemChanged(QListWidgetItem *item);

	void on_qqVisible_clicked();

	void on_qqDisplayLabel_clicked();

	void on_qqLinkPoints_clicked();

	void on_qqAdjustColor_clicked();

	void on_qqDuplicate_clicked();

	void on_qqWidth_editingFinished();

	void on_qqRelativeToGraph_clicked();

	void on_qqRelativeToWorld_clicked();

	void on_qqClose_clicked(QAbstractButton *button);

	void on_qqAlwaysOnTop_clicked();

private:
    Ui::CQDlg2D3DGraphProperties *ui;
};

#endif // QDLG2D3DGRAPHPROPERTIES_H
