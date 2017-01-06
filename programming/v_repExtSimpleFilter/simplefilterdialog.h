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

#ifndef SIMPLEFILTERDIALOG_H
#define SIMPLEFILTERDIALOG_H

#include <QDialog>

namespace Ui {
	class CSimpleFilterDialog;
}

class CSimpleFilterDialog : public QDialog
{
    Q_OBJECT

public:
	explicit CSimpleFilterDialog(QWidget *parent = 0);
	~CSimpleFilterDialog();

	void refresh();

	int sizeValue;
	float thresholdValue;

private slots:


	void on_qqSize_editingFinished();

	void on_qqThreshold_editingFinished();

	void on_qqOkCancel_accepted();

	void on_qqOkCancel_rejected();

private:
	Ui::CSimpleFilterDialog *ui;
};

#endif // SIMPLEFILTERDIALOG_H
