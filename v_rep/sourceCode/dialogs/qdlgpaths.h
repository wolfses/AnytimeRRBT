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

#ifndef QDLGPATHS_H
#define QDLGPATHS_H

#include "DlgEx.h"
#include "Path.h"

namespace Ui {
    class CQDlgPaths;
}

class CQDlgPaths : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgPaths(QWidget *parent = 0);
    ~CQDlgPaths();

	void refresh();

	void cancelEvent();

	bool inMainRefreshRoutine;

	CPathCont* getPathCont();
	CPath* getPath();

private slots:
	void on_qqEnableAll_clicked();

	void on_qqShowOrientation_clicked();

	void on_qqShowPathLine_clicked();

	void on_qqShowPosition_clicked();

	void on_qqAdjustColor_clicked();

	void on_qqLineSize_editingFinished();

	void on_qqControlPointSize_editingFinished();

	void on_qqDistanceCombo_currentIndexChanged(int index);

	void on_qqExplicitHandling_clicked();

	void on_qqInvertVelocity_clicked();

	void on_qqInfiniteAcceleration_clicked();

	void on_qqMaxAcceleration_editingFinished();

	void on_qqPosition_editingFinished();

	void on_qqTargetVelocity_editingFinished();

	void on_qqAdjustVelocityProfile_clicked();

	void on_qqConversionCoeff_editingFinished();

	void on_qqVirtualDistanceScaling_editingFinished();

	void on_qqApplyMotionProperties_clicked();

	void on_qqShowShapingDialog_clicked();

private:
    Ui::CQDlgPaths *ui;
};

#endif // QDLGPATHS_H
