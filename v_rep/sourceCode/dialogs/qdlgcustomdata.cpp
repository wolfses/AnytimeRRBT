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

#include "vrepPrecompiledHeader.h"
#include "qdlgcustomdata.h"
#include "ui_qdlgcustomdata.h"
#include "Tt.h"
#include "IloIlo.h"
#include "App.h"
#include "v_repStrings.h"

CQDlgCustomData::CQDlgCustomData(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgCustomData)
{
	ui->setupUi(this);
}

CQDlgCustomData::~CQDlgCustomData()
{
    delete ui;
}

void CQDlgCustomData::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgCustomData::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgCustomData::refresh()
{
	C3DObject* ls=App::ct->objCont->getLastSelection();
	std::string tmp;
	int j=0;
	int header;
	while (ls->getObjectCustomDataHeader(j++,header))
	{
		tmp+=tt::FNb(header);
		tmp+="[";
		tmp+=tt::FNb(ls->getObjectCustomDataLength(header));
		tmp+="],";
	}
	if (tmp!="")
		tmp.erase(tmp.end()-1);
	ui->qqAttachedCustomData->setText(tmp.c_str());

}

void CQDlgCustomData::on_qqAddCustomData_clicked()
{ // format: header,number,number,etc. float numbers require a ".", otherwise it is an int
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		C3DObject* ls=App::ct->objCont->getLastSelection();
		if (ls!=NULL)
		{
			std::string line(ui->qqNewCustomData->text().toStdString());
			std::string word;
			tt::extractCommaSeparatedWord(line,word);
			tt::removeSpacesAtBeginningAndEnd(word);
			bool badFormat=true;
			std::vector<unsigned char> buff;
			int header;
			if (word.find(".")==std::string::npos)
			{
				if (tt::getValidInt(word,header))
				{ // ok, we have a valid header
					badFormat=false;
					std::string lineCopy(line);
					while (tt::extractCommaSeparatedWord(line,word))
					{
						bool wrong=true;
						tt::removeSpacesAtBeginningAndEnd(word);
						if (word.find(".")==std::string::npos)
						{
							int number;
							if (tt::getValidInt(word,number))
							{
								wrong=false;
								for (int j=0;j<int(sizeof(number));j++)
									buff.push_back(((unsigned char*)&number)[j]);
							}
						}
						else
						{
							float number;
							if (tt::getValidFloat(word,number))
							{
								wrong=false;
								for (int j=0;j<int(sizeof(number));j++)
									buff.push_back(((unsigned char*)&number)[j]);
							}
						}
						if (wrong)
						{
							badFormat=true;
							break;
						}
					}
					if (badFormat)
					{
						line=lineCopy;
						if ((word.find(",")==std::string::npos)&&(line.length()>=1))
						{
							badFormat=false;
							for (int j=0;j<int(line.length());j++)
								buff.push_back(line[j]);
						}
					}
				}
			}
			if ( (!badFormat)&&(buff.size()!=0) )
			{
				int l=ls->getObjectCustomDataLength(header);
				char* prevBuff=new char[l];
				ls->getObjectCustomData(header,prevBuff);
				std::vector<char> fullBuff(prevBuff,prevBuff+l);
				delete[] prevBuff;
				fullBuff.insert(fullBuff.end(),buff.begin(),buff.end());
				ls->setObjectCustomData(header,(char*)&fullBuff[0],(int)fullBuff.size());
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				App::ct->objCont->setFullDialogRefreshFlag();
				refresh();
			}
			else
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_CUSTOM_DATA),strTranslate(IDS_CUSTOM_DATA_STRING_BAD_FORMAT),VMessageBox::OKELI);
		}
	}
}

void CQDlgCustomData::on_qqClearCustomData_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		C3DObject* ls=App::ct->objCont->getLastSelection();
		if (ls!=NULL)
		{
			std::string word(ui->qqNewCustomData->text().toStdString());
			tt::removeSpacesAtBeginningAndEnd(word);
			int header;
			bool badFormat=true;
			if (word.find(".")==std::string::npos)
			{
				if (tt::getValidInt(word,header))
				{ // ok, we have a valid header
					badFormat=false;
					ls->setObjectCustomData(header,NULL,0);
					App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					App::ct->objCont->setFullDialogRefreshFlag();
					refresh();
				}
			}
			if (badFormat)
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_CUSTOM_DATA),strTranslate(IDS_CUSTOM_DATA_STRING_BAD_FORMAT),VMessageBox::OKELI);
		}
	}
}

void CQDlgCustomData::on_qqClose_clicked(QAbstractButton *button)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		defaultModalDialogEndRoutine(true);
	}
}
