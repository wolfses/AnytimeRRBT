// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#ifndef MTBROBOTDIALOG_H
#define MTBROBOTDIALOG_H

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
	   SELECT_OBJECT_CMD,
	   TOGGLE_AUTOMATIC_CMD,
	   UPDATE_PRG_TEXT_CMD,
 };

namespace Ui {
    class CMtbRobotDialog;
}

class CMtbRobotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CMtbRobotDialog(QWidget *parent = 0);
    ~CMtbRobotDialog();

	void refresh();

	void makeVisible(bool visible);
	bool getVisible();
	void updateObjectsInList();
	void selectObjectInList(int objectHandle);
	int getSelectedObjectInList();

	void addCommand(SSimulationThreadCommand cmd);
	void handleCommands();
	void setState(bool stopped,int lastSelection);

	int dialogMenuItemHandle;
	bool inRefreshRoutine;

	void reject();

private slots:

	void on_qqAutomatic_clicked();

	void on_qqRobotList_itemSelectionChanged();

	void on_qqRobotProgram_textChanged();

private:
    Ui::CMtbRobotDialog *ui;

	std::vector<SSimulationThreadCommand> _simulThreadCommands;
	bool _simulationStopped;
	int _lastSelectedObject;
};

#endif // MTBROBOTDIALOG_H
