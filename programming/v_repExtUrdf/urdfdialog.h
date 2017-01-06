// This file is part of the URDF PLUGIN for V-REP
//  
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// A big thanks to Ignacio Tartavull for his precious help!
// 
// The URDF PLUGIN is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The URDF PLUGIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE URDF PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the URDF PLUGIN.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------

//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

// The URDF plugin is courtesy of Ignacio Tartavull. A few modifications by Marc.

#ifndef URDFDIALOG_H
#define URDFDIALOG_H

#include <QDialog>

struct SSimulationThreadCommand
{
	int cmdId;
	std::vector<bool> boolParams;
	std::vector<int> intParams;
	std::vector<float> floatParams;
	std::vector<std::string> stringParams;
};

enum { MAKE_VISIBLE_CMD=0,
	   IMPORT_CMD,
	   WARNING_MSG_CMD,

 };

namespace Ui {
    class CUrdfDialog;
}

class CUrdfDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CUrdfDialog(QWidget *parent = 0);
    ~CUrdfDialog();

	void refresh();

	void makeVisible(bool visible);
	bool getVisible();

	int dialogMenuItemHandle;

	void reject();

	void addCommand(SSimulationThreadCommand cmd);
	void handleCommands();
	void setSimulationStopped(bool stopped);


private:
	std::vector<SSimulationThreadCommand> _simulThreadCommands;
	static bool hideCollisionLinks;
	static bool hideJoints;
	static bool convexDecompose;
	static bool showConvexDecompositionDlg;
	static bool createVisualIfNone;
	static bool centerModel;
	static bool prepareModel;
	static bool noSelfCollision;
	static bool positionCtrl;
	static bool simulationStopped;

private slots:
	void on_qqImport_clicked();
	void on_qqCollisionLinksHidden_clicked();

	void on_qqJointsHidden_clicked();

	void on_qqConvexDecompose_clicked();

	void on_qqConvexDecomposeDlg_clicked();

	void on_qqCreateVisualLinks_clicked();

	void on_qqCenterModel_clicked();

	void on_qqModelDefinition_clicked();

	void on_qqAlternateMasks_clicked();

	void on_qqPositionCtrl_clicked();

private:
    Ui::CUrdfDialog *ui;
};

#endif // URDFDIALOG_H
