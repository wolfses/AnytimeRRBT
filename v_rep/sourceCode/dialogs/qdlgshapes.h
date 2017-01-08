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

#ifndef QDLGSHAPES_H
#define QDLGSHAPES_H

#include "DlgEx.h"

namespace Ui {
    class CQDlgShapes;
}

class CQDlgShapes : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgShapes(QWidget *parent = 0);
    ~CQDlgShapes();

	void refresh();

	void cancelEvent();

private slots:
	void on_qqBackfaceCulling_clicked();

	void on_qqWireframe_clicked();

	void on_qqSameColorInsideAndOutside_clicked();

	void on_qqInvertFaces_clicked();

	void on_qqShowEdges_clicked();

	void on_qqEdgesWidth_editingFinished();

	void on_qqShadingAngle_editingFinished();

	void on_qqApplyMain_clicked();

	void on_qqEditDynamics_clicked();

	void on_qqAdjustOutsideColor_clicked();

	void on_qqAdjustInsideColor_clicked();

	void on_qqAdjustEdgeColor_clicked();

	void on_qqApplyColors_clicked();

	void on_qqTexture_clicked();

	void on_qqGeometry_clicked();

	void on_qqDirtTexture_clicked();

	void on_qqClearTextures_clicked();

	void on_qqEdgesAngle_editingFinished();

	void on_qqInsideOutDisplay_clicked();

private:
    Ui::CQDlgShapes *ui;
};

#endif // QDLGSHAPES_H
