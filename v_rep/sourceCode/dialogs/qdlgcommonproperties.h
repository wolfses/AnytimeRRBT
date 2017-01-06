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

#ifndef QDLGCOMMONPROPERTIES_H
#define QDLGCOMMONPROPERTIES_H

#include "DlgEx.h"

namespace Ui {
	class CQDlgCommonProperties;
}

class CQDlgCommonProperties : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgCommonProperties(QWidget *parent = 0);
    ~CQDlgCommonProperties();

	void refresh();

	void cancelEvent();

	bool inPlaceState;
	float scalingState;

private slots:
	void on_qqSelectable_clicked();

	void on_qqSelectBaseInstead_clicked();

	void on_qqDontShowInModelSelection_clicked();

	void on_qqApplyGeneralProperties_clicked();

	void on_a_1_clicked();
	void on_a_2_clicked();
	void on_a_3_clicked();
	void on_a_4_clicked();
	void on_a_5_clicked();
	void on_a_6_clicked();
	void on_a_7_clicked();
	void on_a_8_clicked();
	void on_a_9_clicked();
	void on_a_10_clicked();
	void on_a_11_clicked();
	void on_a_12_clicked();
	void on_a_13_clicked();
	void on_a_14_clicked();
	void on_a_15_clicked();
	void on_a_16_clicked();

	void on_qqApplyLayers_clicked();

	void on_qqModelBase_clicked();

	void on_qqEditModelProperties_clicked();

	void on_qqCollidable_clicked();

	void on_qqMeasurable_clicked();

	void on_qqPathPlanningIgnored_clicked();

	void on_qqRenderable_clicked();

	void on_qqCuttable_clicked();

	void on_qqDetectable_clicked();

	void on_qqEditDetectableDetails_clicked();

	void on_qqApplySpecialProperties_clicked();

	void on_qqSizeFactor_editingFinished();

	void on_qqIgnoreForViewFitting_clicked();

	void on_qqOpenScalingDialog_clicked();

	void on_qqOpenCustomDataDialog_clicked();

	void on_qqAssembling_clicked();

	void on_qqUpdatable_clicked();

	void on_qqSelectInvisible_clicked();

	void on_qqDepthInvisible_clicked();

	void on_qqSelfCollisionIndicator_editingFinished();

private:
    Ui::CQDlgCommonProperties *ui;
};

#endif // QDLGCOMMONPROPERTIES_H
