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

#ifndef QDLGOPENGLSETTINGS_H
#define QDLGOPENGLSETTINGS_H

#include "VDialog.h"

namespace Ui {
	class CQDlgOpenGlSettings;
}

class CQDlgOpenGlSettings : public VDialog
{
    Q_OBJECT

public:
	explicit CQDlgOpenGlSettings(QWidget *parent = 0);
	~CQDlgOpenGlSettings();

	void cancelEvent();
	void okEvent();

	void refresh();

	int offscreenContextType;
	int fboType;
	int exclusiveGuiRendering;
	bool compatibilityTweak1;
	bool glFinish_normal;
	bool glFinish_visionSensors;
	int glVersionMajor;
	int glVersionMinor;
	int idleFps;
	bool forceExt;

private slots:

	void on_qqOk_accepted();

	void on_qqContextDefault_clicked();

	void on_qqContextOffscreen_clicked();

	void on_qqContextVisibleWindow_clicked();

	void on_qqContextInvisibleWindow_clicked();

	void on_qqFboDefault_clicked();

	void on_qqFboNonQt_clicked();

	void on_qqFboQt_clicked();

	void on_qqGuiRenderingOnly_clicked();

	void on_qqCompatibilityTweak1_clicked();

	void on_qqUsingGlFinish_clicked();

	void on_qqUsingGlFinishVisionSensors_clicked();

	void on_qqMajorOpenGlVersion_editingFinished();

	void on_qqMinorOpenGlVersion_editingFinished();

	void on_qqIdleFps_editingFinished();

	void on_qqForceExt_clicked();

private:
	Ui::CQDlgOpenGlSettings *ui;
};

#endif // QDLGOPENGLSETTINGS_H
