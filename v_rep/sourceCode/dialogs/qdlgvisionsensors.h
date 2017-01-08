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

#ifndef QDLGRENDERINGSENSORS_H
#define QDLGRENDERINGSENSORS_H

#include "DlgEx.h"

namespace Ui {
    class CQDlgVisionSensors;
}

class CQDlgVisionSensors : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgVisionSensors(QWidget *parent = 0);
    ~CQDlgVisionSensors();

	void refresh();

	void cancelEvent();

	bool inMainRefreshRoutine;

	static bool showFilterWindow;

private slots:
	void on_qqEnabled_clicked();

	void on_qqExplicitHandling_clicked();

	void on_qqExternalInput_clicked();

	void on_qqPerspective_clicked();

	void on_qqLocalLights_clicked();

	void on_qqShowNotDetecting_clicked();

	void on_qqShowFog_clicked();

	void on_qqShowDetecting_clicked();

	void on_qqNearPlane_editingFinished();

	void on_qqFarPlane_editingFinished();

	void on_qqPerspectiveAngleOrOrthographicSize_editingFinished();

	void on_qqResX_editingFinished();

	void on_qqResY_editingFinished();

	void on_qqSizeX_editingFinished();

	void on_qqSizeY_editingFinished();

	void on_qqSizeZ_editingFinished();

	void on_qqAdjustImageColor_clicked();

	void on_qqApplyMainProperties_clicked();

	void on_qqAdjustFilters_clicked();

	void on_qqCasingColorPassive_clicked();

	void on_qqCasingColorActive_clicked();

	void on_qqVolumeColorPassive_clicked();

	void on_qqVolumeColorActive_clicked();

	void on_qqApplyColors_clicked();

	void on_qqEntityCombo_currentIndexChanged(int index);

	void on_qqIgnoreRGB_clicked();

	void on_qqIgnoreDepth_clicked();

private:
    Ui::CQDlgVisionSensors *ui;
};

#endif // QDLGRENDERINGSENSORS_H
