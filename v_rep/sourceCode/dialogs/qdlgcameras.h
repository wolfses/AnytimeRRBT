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

#ifndef QDLGCAMERAS_H
#define QDLGCAMERAS_H
#include "DlgEx.h"

namespace Ui {
    class CQDlgCameras;
}

class CQDlgCameras : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgCameras(QWidget *parent = 0);
    ~CQDlgCameras();

	void refresh();

	void cancelEvent();

	bool inMainRefreshRoutine;

private slots:
	void on_qqPerspectiveProjectionAngle_editingFinished();

	void on_qqOrthographicProjectionSize_editingFinished();

	void on_qqNearClipping_editingFinished();

	void on_qqFarClipping_editingFinished();

	void on_qqTrackedCombo_currentIndexChanged(int index);

	void on_qqShowFog_clicked();

	void on_qqKeepHeadUp_clicked();

	void on_qqManipProxy_clicked();

	void on_qqAlongX_clicked();

	void on_qqAlongY_clicked();

	void on_qqAlongZ_clicked();

	void on_qqFullRotation_clicked();

	void on_qqTilting_clicked();

	void on_qqColorA_clicked();

	void on_qqColorB_clicked();

	void on_qqSize_editingFinished();

	void on_qqLocalLights_clicked();

	void on_qqAllowPicking_clicked();

private:
    Ui::CQDlgCameras *ui;
};

#endif // QDLGCAMERAS_H
