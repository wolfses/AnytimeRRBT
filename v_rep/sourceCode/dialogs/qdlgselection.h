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

#ifndef QDLGSELECTION_H
#define QDLGSELECTION_H

#include "DlgEx.h"

namespace Ui {
	class CQDlgSelection;
}

class CQDlgSelection : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgSelection(QWidget *parent = 0);
    ~CQDlgSelection();
	void refresh();

protected:
	std::vector<int> list;
	void processIt();

private slots:
	void on_simpleShapesButton_clicked();

	void on_groupedShapesButton_clicked();

	void on_dummiesButton_clicked();

	void on_millsButton_clicked();

	void on_camerasButton_clicked();

	void on_lightsButton_clicked();

	void on_proximitySensorsButton_clicked();

	void on_renderingSensorsButton_clicked();

	void on_forceSensorsButton_clicked();

	void on_jointsButton_clicked();

	void on_graphsButton_clicked();

	void on_pathsButton_clicked();

	void on_clearSelectionButton_clicked();

	void on_invertSelectionButton_clicked();

	void on_mirrorsButton_clicked();

private:
    Ui::CQDlgSelection *ui;
};

#endif // QDLGSELECTION_H
