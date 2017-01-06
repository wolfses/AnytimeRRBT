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
#include "qdlgtranslations.h"
#include "ui_qdlgtranslations.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"
#include "IloIlo.h"
#include "v_repStrings.h"

bool CQDlgTranslations::forbidManipWhenRunning;
bool CQDlgTranslations::forbidManipWhenNotRunning;
int CQDlgTranslations::manipulationModePermission;
int CQDlgTranslations::manipulationTranslationRelativeTo;
float CQDlgTranslations::manipulationTranslationStepSize;

int CQDlgTranslations::coordMode=0; //0=abs,1=rel to parent
float CQDlgTranslations::rotAngles[3]={0.0f,0.0f,0.0f};
float CQDlgTranslations::translationValues[3]={0.0f,0.0f,0.0f};
float CQDlgTranslations::scalingValues[3]={1.0f,1.0f,1.0f};;
int CQDlgTranslations::transfMode=0; //0=abs,1=rel to parent,2=rel to self

CQDlgTranslations::CQDlgTranslations(QWidget *parent) :
	CDlgEx(parent),
	ui(new Ui::CQDlgTranslations)
{
    ui->setupUi(this);
	/*
	for (int i=0;i<3;i++)
	{
		rotAngles[i]=0.0f;
		translationValues[i]=0.0f;
		scalingValues[i]=1.0f;
	}
	coordMode=0;
	transfMode=0;
	*/
	lastLastSelectionID=-1;
}

CQDlgTranslations::~CQDlgTranslations()
{
    delete ui;
}

void CQDlgTranslations::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgTranslations::refresh()
{
	int editMode=App::ct->objCont->getEditModeType();
	int lastSelID=App::ct->objCont->getLastSelectionID();
	if ((lastSelID!=-1)&&(lastLastSelectionID!=lastSelID))
	{
		C3DObject* object=App::ct->objCont->getLastSelection();
		forbidManipWhenRunning=object->getObjectManipulationDisabledDuringSimulation();
		forbidManipWhenNotRunning=object->getObjectManipulationDisabledDuringNonSimulation();
		manipulationModePermission=object->getObjectManipulationModePermissions();
		manipulationTranslationRelativeTo=object->getObjectManipulationTranslationRelativeTo();
		manipulationTranslationStepSize=object->getNonDefaultTranslationStepSize();
	}
	lastLastSelectionID=lastSelID;
	bool mNotDisabled=(lastSelID!=-1);


	ui->qqDisabledWhenRunning->setEnabled(mNotDisabled);
	ui->qqDisabledWhenNotRunning->setEnabled(mNotDisabled);
	ui->qqPosWorld->setEnabled(mNotDisabled);
	ui->qqPosParent->setEnabled(mNotDisabled);
	ui->qqPosOwn->setEnabled(mNotDisabled);
	ui->qqPosX->setEnabled(mNotDisabled);
	ui->qqPosY->setEnabled(mNotDisabled);
	ui->qqPosZ->setEnabled(mNotDisabled);
	ui->qqPosCombo->setEnabled(mNotDisabled);

	ui->qqDisabledWhenRunning->setChecked(mNotDisabled&&forbidManipWhenRunning);
	ui->qqDisabledWhenNotRunning->setChecked(mNotDisabled&&forbidManipWhenNotRunning);

	ui->qqPosWorld->setChecked(mNotDisabled&&(manipulationTranslationRelativeTo==0));
	ui->qqPosParent->setChecked(mNotDisabled&&(manipulationTranslationRelativeTo==1));
	ui->qqPosOwn->setChecked(mNotDisabled&&(manipulationTranslationRelativeTo==2));

	ui->qqPosX->setChecked(mNotDisabled&&(manipulationModePermission&0x01));
	ui->qqPosY->setChecked(mNotDisabled&&(manipulationModePermission&0x02));
	ui->qqPosZ->setChecked(mNotDisabled&&(manipulationModePermission&0x04));

	ui->qqPosCombo->clear();
	if (mNotDisabled)
	{
		ui->qqPosCombo->addItem(IDS_DEFAULT,QVariant(0));
		ui->qqPosCombo->addItem(gv::getSizeStr(false,0.001f).c_str(),QVariant(1));
		ui->qqPosCombo->addItem(gv::getSizeStr(false,0.002f).c_str(),QVariant(2));
		ui->qqPosCombo->addItem(gv::getSizeStr(false,0.005f).c_str(),QVariant(5));
		ui->qqPosCombo->addItem(gv::getSizeStr(false,0.01f).c_str(),QVariant(10));
		ui->qqPosCombo->addItem(gv::getSizeStr(false,0.025f).c_str(),QVariant(25));
		ui->qqPosCombo->addItem(gv::getSizeStr(false,0.05f).c_str(),QVariant(50));
		ui->qqPosCombo->addItem(gv::getSizeStr(false,0.1f).c_str(),QVariant(100));
		ui->qqPosCombo->addItem(gv::getSizeStr(false,0.25f).c_str(),QVariant(250));
		ui->qqPosCombo->addItem(gv::getSizeStr(false,0.5f).c_str(),QVariant(500));
		CIloIlo::selectItemOfCombobox(ui->qqPosCombo,int((manipulationTranslationStepSize+0.0005f)*1000.0f));
	}
	ui->qqRememberManipulationSettings->setEnabled(mNotDisabled);

	if (editMode==NO_EDIT_MODE)
	{
		bool sel=(App::ct->objCont->getSelSize()!=0);
		bool bigSel=(App::ct->objCont->getSelSize()>1);
//		bool noSim=App::ct->simulation->isSimulationStopped();
		_enableCoordinatePart(sel,true,bigSel,true);
		_enableCoordinatePart(sel,false,bigSel,false);
		_enableTransformationPart(sel,0,sel,true);
		_enableTransformationPart(sel,1,sel,false);
		_enableTransformationPart(sel&&(transfMode!=2),2,sel&&(transfMode!=2),false);
		C3DObject* object=App::ct->objCont->getLastSelection();
		if (sel&&(object!=NULL))
		{
			// Coordinate part:
			C3Vector euler,pos;
			if (coordMode==0)
			{
				euler=object->getCumulativeTransformationPart1().Q.getEulerAngles();
				pos=object->getCumulativeTransformationPart1().X;
			}
			else
			{
				euler=object->getLocalTransformationPart1().Q.getEulerAngles();
				pos=object->getLocalTransformationPart1().X;
			}
			ui->qqCoordX->setText(gv::getSizeStr(true,pos(0)).c_str());
			ui->qqCoordY->setText(gv::getSizeStr(true,pos(1)).c_str());
			ui->qqCoordZ->setText(gv::getSizeStr(true,pos(2)).c_str());
			ui->qqCoordWorld->setChecked(coordMode==0);
			ui->qqCoordParent->setChecked(coordMode==1);
			// Transformation part:
			_setValuesTransformationPart(0,true);
			_setValuesTransformationPart(1,false);
			_setValuesTransformationPart(2,false);
		}
		else
		{
			_setDefaultValuesCoordinatePart(true,true);
			_setDefaultValuesCoordinatePart(false,false);
			_setDefaultValuesTransformationPart(0,true);
			_setDefaultValuesTransformationPart(1,false);
			_setDefaultValuesTransformationPart(2,false);
		}
	}
	else
	{ // We are in an edit mode
		if (editMode&(BUTTON_EDIT_MODE|TRIANGLE_EDIT_MODE|EDGE_EDIT_MODE))
		{
			_enableCoordinatePart(false,true,false,true);
			_enableCoordinatePart(false,false,false,false);
			_enableTransformationPart(false,0,false,true);
			_enableTransformationPart(false,1,false,false);
			_enableTransformationPart(false,2,false,false);
			_setDefaultValuesCoordinatePart(true,true);
			_setDefaultValuesCoordinatePart(false,false);
			_setDefaultValuesTransformationPart(0,true);
			_setDefaultValuesTransformationPart(1,false);
			_setDefaultValuesTransformationPart(2,false);
		}
		else
		{ // Vertex or path edit mode
			if (editMode&VERTEX_EDIT_MODE)
			{
				bool sel=(App::ct->objCont->editModeBuffer.size()!=0);
				bool bigSel=(App::ct->objCont->editModeBuffer.size()>1);
				if (transfMode==2)
					transfMode=1;

				_setDefaultValuesCoordinatePart(true,true);
				_enableCoordinatePart(sel,false,bigSel,true);
				_enableTransformationPart(sel,0,sel,false);
				_enableTransformationPart(sel,1,sel,false);
				_enableTransformationPart(sel&&(transfMode!=2),2,sel&&(transfMode!=2),false);
				ui->qqTransfWorld->setEnabled(sel);
				ui->qqTransfParent->setEnabled(sel);
				ui->qqTransfOwn->setEnabled(false);
				CShape* shape=App::ct->objCont->getShape(App::ct->objCont->getEditModeObjectID());
				if (sel&&(shape!=NULL))
				{
					// Coordinate part:
					int ind=App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1];
					C3Vector pos(App::ct->objCont->_editionVertices[3*ind+0],App::ct->objCont->_editionVertices[3*ind+1],App::ct->objCont->_editionVertices[3*ind+2]);
					if (coordMode==0)
						pos=shape->getCumulativeTransformation()*pos;
					ui->qqCoordX->setText(gv::getSizeStr(true,pos(0)).c_str());
					ui->qqCoordY->setText(gv::getSizeStr(true,pos(1)).c_str());
					ui->qqCoordZ->setText(gv::getSizeStr(true,pos(2)).c_str());
					ui->qqCoordWorld->setChecked(coordMode==0);
					ui->qqCoordParent->setChecked(coordMode==1);
					// Transformation part:
					_setValuesTransformationPart(0,true);
					_setValuesTransformationPart(1,false);
					_setValuesTransformationPart(2,false);
				}
				else
				{
					_setDefaultValuesCoordinatePart(false,false);
					_setDefaultValuesTransformationPart(0,true);
					_setDefaultValuesTransformationPart(1,false);
					_setDefaultValuesTransformationPart(2,false);
				}
			}
			if (editMode&PATH_EDIT_MODE)
			{
				bool sel=(App::ct->objCont->editModeBuffer.size()!=0);
				bool bigSel=(App::ct->objCont->editModeBuffer.size()>1);

				_enableCoordinatePart(sel,true,bigSel,true);
				_enableCoordinatePart(sel,false,bigSel,false);
				_enableTransformationPart(sel,0,sel,true);
				_enableTransformationPart(sel,1,sel,false);
				_enableTransformationPart(sel&&(transfMode!=2),2,sel&&(transfMode!=2),false);
				if (sel)
				{
					CPath* path=App::ct->objCont->getPath(App::ct->objCont->getEditModeObjectID());
					int ind=App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1];
					CSimplePathPoint* pp=App::ct->objCont->_editionPath->getSimplePathPoint(ind);
					if (sel&&(path!=NULL)&&(pp!=NULL))
					{
						// Coordinate part:
						C7Vector tr(pp->getTransformation());
						if (coordMode==0)
							tr=path->getCumulativeTransformation()*tr;
						C3Vector euler(tr.Q.getEulerAngles());
						ui->qqCoordX->setText(gv::getSizeStr(true,tr.X(0)).c_str());
						ui->qqCoordY->setText(gv::getSizeStr(true,tr.X(1)).c_str());
						ui->qqCoordZ->setText(gv::getSizeStr(true,tr.X(2)).c_str());
						ui->qqCoordWorld->setChecked(coordMode==0);
						ui->qqCoordParent->setChecked(coordMode==1);
						// Transformation part:
						_setValuesTransformationPart(0,true);
						_setValuesTransformationPart(1,false);
						_setValuesTransformationPart(2,false);
					}
					else
					{
						_setDefaultValuesCoordinatePart(false,false);
						_setDefaultValuesTransformationPart(0,true);
						_setDefaultValuesTransformationPart(1,false);
						_setDefaultValuesTransformationPart(2,false);
					}
				}
				else
				{
					_setDefaultValuesCoordinatePart(true,true);
					_setDefaultValuesCoordinatePart(false,false);
					_setDefaultValuesTransformationPart(0,true);
					_setDefaultValuesTransformationPart(1,false);
					_setDefaultValuesTransformationPart(2,false);
				}
			}
		}
	}
}

void CQDlgTranslations::_enableCoordinatePart(bool enableState,bool anglePart,bool enableButtons,bool alsoRadioButtons)
{
	if (!anglePart)
	{
		ui->qqCoordX->setEnabled(enableState);
		ui->qqCoordY->setEnabled(enableState);
		ui->qqCoordZ->setEnabled(enableState);
		ui->qqCoordApplyPos->setEnabled(enableButtons);
		ui->qqCoordApplyX->setEnabled(enableButtons);
		ui->qqCoordApplyY->setEnabled(enableButtons);
		ui->qqCoordApplyZ->setEnabled(enableButtons);
	}
	if (alsoRadioButtons)
	{
		ui->qqCoordWorld->setEnabled(enableState);
		ui->qqCoordParent->setEnabled(enableState);
	}
}

void CQDlgTranslations::_enableTransformationPart(bool enableState,int partIndex,bool enableButtons,bool alsoRadioButtons)
{
	if (partIndex==1)
	{
		ui->qqTransfX->setEnabled(enableState);
		ui->qqTransfY->setEnabled(enableState);
		ui->qqTransfZ->setEnabled(enableState);
		ui->qqTransfApplyPos->setEnabled(enableButtons);
	}
	if (partIndex==2)
	{
		ui->qqTransfSX->setEnabled(enableState);
		ui->qqTransfSY->setEnabled(enableState);
		ui->qqTransfSZ->setEnabled(enableState);
		ui->qqTransfApplyScale->setEnabled(enableButtons);
	}
	if (alsoRadioButtons)
	{
		ui->qqTransfWorld->setEnabled(enableState);
		ui->qqTransfParent->setEnabled(enableState);
		ui->qqTransfOwn->setEnabled(enableState);
	}
}

void CQDlgTranslations::_setDefaultValuesCoordinatePart(bool anglePart,bool alsoRadioButtons)
{
	if (!anglePart)
	{
		ui->qqCoordX->setText("");
		ui->qqCoordY->setText("");
		ui->qqCoordZ->setText("");
	}
	if (alsoRadioButtons)
	{
		ui->qqCoordWorld->setChecked(false);
		ui->qqCoordParent->setChecked(false);
	}
}

void CQDlgTranslations::_setDefaultValuesTransformationPart(int partIndex,bool alsoRadioButtons)
{
	if (partIndex==1)
	{
		ui->qqTransfX->setText("");
		ui->qqTransfY->setText("");
		ui->qqTransfZ->setText("");
	}
	if (partIndex==2)
	{
		ui->qqTransfSX->setText("");
		ui->qqTransfSY->setText("");
		ui->qqTransfSZ->setText("");
	}
	if (alsoRadioButtons)
	{
		ui->qqTransfWorld->setChecked(false);
		ui->qqTransfParent->setChecked(false);
		ui->qqTransfOwn->setChecked(false);
	}
}

void CQDlgTranslations::_setValuesTransformationPart(int partIndex,bool alsoRadioButtons)
{
	if (partIndex==1)
	{
		ui->qqTransfX->setText(gv::getSizeStr(true,translationValues[0]).c_str());
		ui->qqTransfY->setText(gv::getSizeStr(true,translationValues[1]).c_str());
		ui->qqTransfZ->setText(gv::getSizeStr(true,translationValues[2]).c_str());
	}
	if (partIndex==2)
	{
		ui->qqTransfSX->setText(tt::FNb(0,scalingValues[0],3,true).c_str());
		ui->qqTransfSY->setText(tt::FNb(0,scalingValues[1],3,true).c_str());
		ui->qqTransfSZ->setText(tt::FNb(0,scalingValues[2],3,true).c_str());
	}
	if (alsoRadioButtons)
	{
		ui->qqTransfWorld->setChecked(transfMode==0);
		ui->qqTransfParent->setChecked(transfMode==1);
		ui->qqTransfOwn->setChecked(transfMode==2);
	}
}

void CQDlgTranslations::_setCoord_userUnit(float newValueInUserUnit,bool orientation,int index)
{
	int editMode=App::ct->objCont->getEditModeType();
	C3DObject* object=App::ct->objCont->getLastSelection();
	if ( (editMode==NO_EDIT_MODE)&&(object!=NULL) )
	{
		C7Vector tr;
		if (coordMode==0)
			tr=object->getCumulativeTransformationPart1();
		else
			tr=object->getLocalTransformationPart1();
		tr=_getNewTransf(tr,newValueInUserUnit,orientation,index);
		if (coordMode==0)
			object->setLocalTransformation(object->getParentCumulativeTransformation().getInverse()*tr);
		else
			object->setLocalTransformation(tr);
	}
	if ( (editMode&PATH_EDIT_MODE)&&(App::ct->objCont->editModeBuffer.size()!=0)&&(App::ct->objCont->_editionPath!=NULL) )
	{
		CPathCont* pathCont=App::ct->objCont->_editionPath;
		int ind=App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1];
		CSimplePathPoint* pp=pathCont->getSimplePathPoint(ind);
		CPath* path=App::ct->objCont->getPath(App::ct->objCont->getEditModeObjectID());
		if ( (pp!=NULL)&&(path!=NULL) )
		{
			C7Vector tr(pp->getTransformation());
			if (coordMode==0)
				tr=path->getCumulativeTransformationPart1()*tr;
			tr=_getNewTransf(tr,newValueInUserUnit,orientation,index);
			if (coordMode==0)
				pp->setTransformation(path->getCumulativeTransformation().getInverse()*tr,pathCont->getAttributes());
			else
				pp->setTransformation(tr,pathCont->getAttributes());
			pathCont->actualizePath();
		}
	}
	if ( (editMode&VERTEX_EDIT_MODE)&&(App::ct->objCont->editModeBuffer.size()!=0) )
	{
		int ind=App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1];
		C3Vector v(App::ct->objCont->_editionVertices[3*ind+0],App::ct->objCont->_editionVertices[3*ind+1],App::ct->objCont->_editionVertices[3*ind+2]);
		CShape* shape=App::ct->objCont->getShape(App::ct->objCont->getEditModeObjectID());
		if (shape!=NULL)
		{
			C7Vector tr;
			tr.setIdentity();
			tr.X=v;
			if (coordMode==0)
				tr=shape->getCumulativeTransformationPart1()*tr;
			tr=_getNewTransf(tr,newValueInUserUnit,orientation,index);
			if (coordMode==0)
				tr=shape->getCumulativeTransformation().getInverse()*tr;
			App::ct->objCont->_editionVertices[3*ind+0]=tr.X(0);
			App::ct->objCont->_editionVertices[3*ind+1]=tr.X(1);
			App::ct->objCont->_editionVertices[3*ind+2]=tr.X(2);
		}
	}
}

C7Vector CQDlgTranslations::_getNewTransf(const C7Vector& transf,float newValueInUserUnit,bool orientation,int index)
{
	C7Vector retVal(transf);
	if (orientation)
	{
		C3Vector euler(retVal.Q.getEulerAngles());
		euler(index)=newValueInUserUnit*gv::userToRad;
		retVal.Q.setEulerAngles(euler(0),euler(1),euler(2));
	}
	else
		retVal.X(index)=newValueInUserUnit*gv::userToMeter;
	return(retVal);
}

void CQDlgTranslations::_applyCoord(bool orientation,int mask)
{
	int editMode=App::ct->objCont->getEditModeType();
	C3DObject* object=App::ct->objCont->getLastSelection();
	int objSelSize=App::ct->objCont->getSelSize();
	int editObjSelSize=App::ct->objCont->editModeBuffer.size();
	if ( (editMode==NO_EDIT_MODE)&&(object!=NULL)&&(objSelSize>1) )
	{
		C7Vector tr;
		if (coordMode==0)
			tr=object->getCumulativeTransformationPart1();
		else
			tr=object->getLocalTransformationPart1();
		for (int i=0;i<objSelSize-1;i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getSelID(i));
			C7Vector trIt;
			if (coordMode==0)
				trIt=it->getCumulativeTransformationPart1();
			else
				trIt=it->getLocalTransformationPart1();
			_copyTransf(tr,trIt,orientation,mask);
			if (coordMode==0)
				it->setLocalTransformation(it->getParentCumulativeTransformation().getInverse()*trIt);
			else
				it->setLocalTransformation(trIt);
		}
	}
	if ( (editMode&PATH_EDIT_MODE)&&(editObjSelSize>1)&&(App::ct->objCont->_editionPath!=NULL) )
	{
		CPathCont* pathCont=App::ct->objCont->_editionPath;
		int ind=App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1];
		CSimplePathPoint* pp=pathCont->getSimplePathPoint(ind);
		CPath* path=App::ct->objCont->getPath(App::ct->objCont->getEditModeObjectID());
		if ( (pp!=NULL)&&(path!=NULL) )
		{
			C7Vector tr(pp->getTransformation());
			if (coordMode==0)
				tr=path->getCumulativeTransformationPart1()*tr;
			for (int i=0;i<editObjSelSize-1;i++)
			{
				CSimplePathPoint* ppIt=pathCont->getSimplePathPoint(App::ct->objCont->editModeBuffer[i]);
				if (ppIt!=NULL)
				{
					C7Vector trIt(ppIt->getTransformation());
					if (coordMode==0)
						trIt=path->getCumulativeTransformationPart1()*trIt;
					_copyTransf(tr,trIt,orientation,mask);
					if (coordMode==0)
						trIt=path->getCumulativeTransformationPart1().getInverse()*trIt;
					ppIt->setTransformation(trIt,pathCont->getAttributes());
				}
			}
			pathCont->actualizePath();
		}
	}
	if ( (editMode&VERTEX_EDIT_MODE)&&(editObjSelSize>1) )
	{
		int ind=App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1];
		C3Vector v(App::ct->objCont->_editionVertices[3*ind+0],App::ct->objCont->_editionVertices[3*ind+1],App::ct->objCont->_editionVertices[3*ind+2]);
		CShape* shape=App::ct->objCont->getShape(App::ct->objCont->getEditModeObjectID());
		if (shape!=NULL)
		{
			C7Vector tr;
			tr.setIdentity();
			tr.X=v;
			if (coordMode==0)
				tr=shape->getCumulativeTransformationPart1()*tr;
			for (int i=0;i<editObjSelSize-1;i++)
			{
				ind=App::ct->objCont->editModeBuffer[i];
				C7Vector trIt;
				trIt.setIdentity();
				trIt.X.set(&App::ct->objCont->_editionVertices[3*ind+0]);
				if (coordMode==0)
					trIt=shape->getCumulativeTransformationPart1()*trIt;
				_copyTransf(tr,trIt,orientation,mask);
				if (coordMode==0)
					trIt=shape->getCumulativeTransformationPart1().getInverse()*trIt;
				App::ct->objCont->_editionVertices[3*ind+0]=trIt.X(0);
				App::ct->objCont->_editionVertices[3*ind+1]=trIt.X(1);
				App::ct->objCont->_editionVertices[3*ind+2]=trIt.X(2);
			}
		}
	}
}

void CQDlgTranslations::_copyTransf(const C7Vector& tr,C7Vector& trIt,bool orientation,int mask)
{
	if (orientation)
		trIt.Q=tr.Q;
	else
	{
		if (mask&1)
			trIt.X(0)=tr.X(0);
		if (mask&2)
			trIt.X(1)=tr.X(1);
		if (mask&4)
			trIt.X(2)=tr.X(2);
	}
}

void CQDlgTranslations::_applyTransformation(int t)
{ // t==0: rotation, t==1: translation, t==2: scaling
	int editMode=App::ct->objCont->getEditModeType();
	int objSelSize=App::ct->objCont->getSelSize();
	int editObjSelSize=App::ct->objCont->editModeBuffer.size();
	if ( (editMode==NO_EDIT_MODE)&&(objSelSize>0) )
	{
		for (int i=0;i<objSelSize;i++)
		{
			C3DObject* object=App::ct->objCont->getObject(App::ct->objCont->getSelID(i));
			bool hasParentPresent=false;
			if ((transfMode==0)&&(t!=2)) // scaling is different!
			{ // We do a transformation relative to the world. If this object has a parent that also is selected, we don't process this object!
				C3DObject* p=object->getParent();
				while (p!=NULL)
				{
					for (int j=0;j<objSelSize;j++)
					{
						if (App::ct->objCont->getSelID(j)==p->getID())
						{
							hasParentPresent=true;
							break;
						}
					}
					if (hasParentPresent)
						break;
					p=p->getParent();
				}
			}
			if (!hasParentPresent)
			{
				C7Vector tr;
				if (transfMode==0)
					tr=object->getCumulativeTransformationPart1();
				else
					tr=object->getLocalTransformationPart1();
				_transform(tr,t,transfMode==2);
				if (transfMode==0)
					tr=object->getParentCumulativeTransformation().getInverse()*tr;
				object->setLocalTransformation(tr);
			}
		}
	}
	if ( (editMode&PATH_EDIT_MODE)&&(editObjSelSize>0)&&(App::ct->objCont->_editionPath!=NULL) )
	{
		CPathCont* pathCont=App::ct->objCont->_editionPath;
		CPath* path=App::ct->objCont->getPath(App::ct->objCont->getEditModeObjectID());
		for (int i=0;i<editObjSelSize;i++)
		{
			CSimplePathPoint* pp=pathCont->getSimplePathPoint(App::ct->objCont->editModeBuffer[i]);
			if ( (pp!=NULL)&&(path!=NULL) )
			{
				C7Vector tr(pp->getTransformation());
				if (transfMode==0)
					tr=path->getCumulativeTransformationPart1()*tr;
				_transform(tr,t,transfMode==2);
				if (transfMode==0)
					tr=path->getCumulativeTransformationPart1().getInverse()*tr;
				pp->setTransformation(tr,pathCont->getAttributes());
			}
		}
		pathCont->actualizePath();
	}
	if ( (editMode&VERTEX_EDIT_MODE)&&(editObjSelSize>0) )
	{
		CShape* shape=App::ct->objCont->getShape(App::ct->objCont->getEditModeObjectID());
		if (shape!=NULL)
		{
			for (int i=0;i<editObjSelSize;i++)
			{
				C7Vector tr;
				tr.setIdentity();
				int ind=App::ct->objCont->editModeBuffer[i];
				tr.X.set(&App::ct->objCont->_editionVertices[3*ind+0]);
				if (transfMode==0)
					tr=shape->getCumulativeTransformationPart1()*tr;
				_transform(tr,t,transfMode==2);
				if (transfMode==0)
					tr=shape->getCumulativeTransformationPart1().getInverse()*tr;
				App::ct->objCont->_editionVertices[3*ind+0]=tr.X(0);
				App::ct->objCont->_editionVertices[3*ind+1]=tr.X(1);
				App::ct->objCont->_editionVertices[3*ind+2]=tr.X(2);
			}
		}
	}
}

void CQDlgTranslations::_transform(C7Vector& tr,int t,bool self)
{ // t==0: rotation, t==1: translation, t==2: scaling
	if (t==2)
	{
		tr.X(0)=tr.X(0)*scalingValues[0];
		tr.X(1)=tr.X(1)*scalingValues[1];
		tr.X(2)=tr.X(2)*scalingValues[2];
	}
	else
	{
		C7Vector m;
		m.setIdentity();
		if (t==0)
			m.Q.setEulerAngles(rotAngles[0],rotAngles[1],rotAngles[2]);
		if (t==1)
			m.X.set(translationValues);
		if (self)
			tr=tr*m;
		else
			tr=m*tr;
	}
}


void CQDlgTranslations::on_qqCoordWorld_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		coordMode=0;
		refresh();
	}
}

void CQDlgTranslations::on_qqCoordParent_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		coordMode=1;
		refresh();
	}
}

void CQDlgTranslations::on_qqCoordX_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqCoordX->text().toFloat(&ok);
		if (ok)
		{
			_setCoord_userUnit(newVal,false,0);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgTranslations::on_qqCoordY_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqCoordY->text().toFloat(&ok);
		if (ok)
		{
			_setCoord_userUnit(newVal,false,1);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgTranslations::on_qqCoordZ_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqCoordZ->text().toFloat(&ok);
		if (ok)
		{
			_setCoord_userUnit(newVal,false,2);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgTranslations::on_qqCoordApplyPos_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_applyCoord(false,1+2+4);
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgTranslations::on_qqCoordApplyX_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_applyCoord(false,1);
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgTranslations::on_qqCoordApplyY_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_applyCoord(false,2);
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgTranslations::on_qqCoordApplyZ_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_applyCoord(false,4);
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgTranslations::on_qqTransfWorld_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		transfMode=0;
		refresh();
	}
}

void CQDlgTranslations::on_qqTransfParent_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		transfMode=1;
		refresh();
	}
}

void CQDlgTranslations::on_qqTransfOwn_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		transfMode=2;
		refresh();
	}
}


void CQDlgTranslations::on_qqTransfX_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqTransfX->text().toFloat(&ok);
		if (ok)
		{
			newVal*=gv::userToMeter;
			tt::limitValue(-1000000.0f,+1000000.0f,newVal);
			translationValues[0]=newVal;
		}
		refresh();
	}
}

void CQDlgTranslations::on_qqTransfY_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqTransfY->text().toFloat(&ok);
		if (ok)
		{
			newVal*=gv::userToMeter;
			tt::limitValue(-1000000.0f,+1000000.0f,newVal);
			translationValues[1]=newVal;
		}
		refresh();
	}
}

void CQDlgTranslations::on_qqTransfZ_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqTransfZ->text().toFloat(&ok);
		if (ok)
		{
			newVal*=gv::userToMeter;
			tt::limitValue(-1000000.0f,+1000000.0f,newVal);
			translationValues[2]=newVal;
		}
		refresh();
	}
}

void CQDlgTranslations::on_qqTransfApplyPos_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_applyTransformation(1);
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgTranslations::on_qqTransfSX_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqTransfSX->text().toFloat(&ok);
		if (ok)
		{
			tt::limitValue(-1000000.0f,1000000.0f,newVal);
			scalingValues[0]=newVal;
		}
		refresh();
	}
}

void CQDlgTranslations::on_qqTransfSY_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqTransfSY->text().toFloat(&ok);
		if (ok)
		{
			tt::limitValue(-1000000.0f,1000000.0f,newVal);
			scalingValues[1]=newVal;
		}
		refresh();
	}
}

void CQDlgTranslations::on_qqTransfSZ_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqTransfSZ->text().toFloat(&ok);
		if (ok)
		{
			tt::limitValue(-1000000.0f,1000000.0f,newVal);
			scalingValues[2]=newVal;
		}
		refresh();
	}
}

void CQDlgTranslations::on_qqTransfApplyScale_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_applyTransformation(2);
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}





void CQDlgTranslations::on_qqDisabledWhenNotRunning_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		forbidManipWhenNotRunning=!forbidManipWhenNotRunning;
		refresh();
	}
}

void CQDlgTranslations::on_qqDisabledWhenRunning_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		forbidManipWhenRunning=!forbidManipWhenRunning;
		refresh();
	}
}

void CQDlgTranslations::on_qqPosWorld_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		manipulationTranslationRelativeTo=0;
		refresh();
	}
}

void CQDlgTranslations::on_qqPosParent_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		manipulationTranslationRelativeTo=1;
		refresh();
	}
}

void CQDlgTranslations::on_qqPosOwn_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		manipulationTranslationRelativeTo=2;
		refresh();
	}
}

void CQDlgTranslations::on_qqPosX_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		manipulationModePermission=manipulationModePermission^0x01;
		int low=manipulationModePermission&0x07;
		int high=manipulationModePermission&56;
		if (low==0x07)
			low=3;
		if (low==0)
			low=1;
		manipulationModePermission=low+high;
		refresh();
	}
}

void CQDlgTranslations::on_qqPosY_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		manipulationModePermission=manipulationModePermission^0x02;
		int low=manipulationModePermission&0x07;
		int high=manipulationModePermission&56;
		if (low==0x07)
			low=3;
		if (low==0)
			low=2;
		manipulationModePermission=low+high;
		refresh();
	}
}

void CQDlgTranslations::on_qqPosZ_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		manipulationModePermission=manipulationModePermission^0x04;
		int low=manipulationModePermission&0x07;
		int high=manipulationModePermission&56;
		if (low==0x07)
			low=6;
		if (low==0)
			low=4;
		manipulationModePermission=low+high;
		refresh();
	}
}

void CQDlgTranslations::on_qqPosCombo_activated(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int data=ui->qqPosCombo->itemData(index).toInt();
		manipulationTranslationStepSize=float(data)/1000.0f;
		refresh();
	}
}


void CQDlgTranslations::on_qqRememberManipulationSettings_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		for (int i=0;i<int(App::ct->objCont->getSelSize());i++)
		{
			C3DObject* object=App::ct->objCont->getObject(App::ct->objCont->getSelID(i));
			if (object!=NULL)
			{
				object->setObjectManipulationDisabledDuringSimulation(forbidManipWhenRunning);
				object->setObjectManipulationDisabledDuringNonSimulation(forbidManipWhenNotRunning);

				int m=object->getObjectManipulationModePermissions()&56; // keep the orientation part
				m|=(manipulationModePermission&7); // overwrite just the position part
				object->setObjectManipulationModePermissions(m);

				object->setObjectManipulationTranslationRelativeTo(manipulationTranslationRelativeTo);
				object->setNonDefaultTranslationStepSize(manipulationTranslationStepSize);
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}
