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

#ifndef QDLGUIBUTTONS_H
#define QDLGUIBUTTONS_H

#include "DlgEx.h"
#include "SoftButton.h"

namespace Ui {
    class CQDlgUiButtons;
}

class CQDlgUiButtons : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgUiButtons(QWidget *parent = 0);
    ~CQDlgUiButtons();

	void refresh();

	void cancelEvent();

	bool inMainRefreshRoutine;

	CSoftButton* getLastSelectedButton();

private slots:
	void on_qqInsertButtons_clicked();

	void on_qqInsertMergedButton_clicked();

	void on_qqRemoveButtons_clicked();

	void on_qqButtonHandle_editingFinished();

	void on_qqTypeCombo_currentIndexChanged(int index);

	void on_qqEnabled_clicked();

	void on_qqStayDown_clicked();

	void on_qqRollUp_clicked();

	void on_qqCenteredH_clicked();

	void on_qqUpDownEvent_clicked();

	void on_qqCloseAction_clicked();

	void on_qqCenteredV_clicked();

	void on_qqBorderless_clicked();

	void on_qqIgnoreMouse_clicked();

	void on_qqApplyType_clicked();

	void on_qqLabelUp_editingFinished();

	void on_qqLabelDown_editingFinished();

	void on_qqApplyLabel_clicked();

	void on_qqColorUp_clicked();

	void on_qqColorDown_clicked();

	void on_qqColorLabel_clicked();

	void on_qqApplyColor_clicked();

	void on_qqTransparent_clicked();

	void on_qqNoBackground_clicked();

	void on_qqSetTexture_clicked();

	void on_qqApplyOtherProperties_clicked();

private:
    Ui::CQDlgUiButtons *ui;
};

#endif // QDLGUIBUTTONS_H
