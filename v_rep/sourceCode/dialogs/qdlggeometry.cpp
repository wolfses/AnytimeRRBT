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

// Not very elegant. Requires refactoring

#include "vrepPrecompiledHeader.h"
#include "qdlggeometry.h"
#include "ui_qdlggeometry.h"
#include "Tt.h"
#include "GV.h"
#include "IloIlo.h"
#include "geometric.h"
#include "v_repStringTable.h"
#include "App.h"

CQDlgGeometry::CQDlgGeometry(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgGeometry)
{
    ui->setupUi(this);
	_identification[0]=-1;
	_identification[1]=-1;
	_identification[2]=-1;
	if (App::mainWindow!=NULL)
		App::mainWindow->dlgCont->close(GEOMETRY_DLG);
}

CQDlgGeometry::~CQDlgGeometry()
{
    delete ui;
}

void CQDlgGeometry::refresh()
{
	if (!isLinkedDataValid())
		return;
	if (!insideRefreshTriggered)
		_setCurrentSizes();
	insideRefreshTriggered=false;
	CShape* shape=App::ct->objCont->getShape(_identification[2]);
	if (shape==NULL)
		return;
	bool g=!shape->geomData->geomInfo->isGeometric();
	ui->qqSizeX->setText(gv::getSizeStr(false,sizeVal[0],0).c_str());
	ui->qqSizeY->setText(gv::getSizeStr(false,sizeVal[1],0).c_str());
	ui->qqSizeZ->setText(gv::getSizeStr(false,sizeVal[2],0).c_str());
	ui->qqScaleX->setText(tt::FNb(0,scaleVal[0],3,true).c_str());
	ui->qqScaleY->setText(tt::FNb(0,scaleVal[1],3,true).c_str());
	ui->qqScaleZ->setText(tt::FNb(0,scaleVal[2],3,true).c_str());
	ui->qqAlpha->setText(gv::getAngleStr(true,rotationVal[0],-1).c_str());
	ui->qqBeta->setText(gv::getAngleStr(true,rotationVal[1],-1).c_str());
	ui->qqGamma->setText(gv::getAngleStr(true,rotationVal[2],-1).c_str());
	bool canScaleFreely=(!g)&&(shape->geomData->geomInfo->getPurePrimitiveType()!=sim_pure_primitive_spheroid);
	ui->qqKeepProp->setChecked(keepProp||(!canScaleFreely));
	ui->qqKeepProp->setEnabled(canScaleFreely);
	ui->qqAlpha->setEnabled((!isPureShape)||g);
	ui->qqBeta->setEnabled((!isPureShape)||g);
	ui->qqGamma->setEnabled((!isPureShape)||g);
	ui->qqApplyEuler->setEnabled((!isPureShape)||g);
	bool simpleSphereCuboidOrCylinder=isPureShape&&(!g)&&((shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_spheroid)||(shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_cuboid)||(shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_cylinder));
	ui->qqInsideScaling->setEnabled(simpleSphereCuboidOrCylinder);
	if (simpleSphereCuboidOrCylinder)
		ui->qqInsideScaling->setText(tt::FNb(0,((CGeometric*)shape->geomData->geomInfo)->getPurePrimitiveInsideScaling(),6,false).c_str());
	else
		ui->qqInsideScaling->setText("");
	std::string shapeTypeText;
	if (isPureShape)
	{
		if (g)
			shapeTypeText=IDSN_PURE_GROUPED_SHAPE;
		else
		{
			if (shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_heightfield)
				shapeTypeText=IDSN_HEIGHTFIELD_SHAPE;
			if (shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_plane)
				shapeTypeText=IDSN_PURE_SIMPLE_SHAPE_PLANE_;
			if (shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_disc)
				shapeTypeText=IDSN_PURE_SIMPLE_SHAPE_DISC_;
			if (shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_cuboid)
				shapeTypeText=IDSN_PURE_SIMPLE_SHAPE_CUBOID_;
			if (shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_spheroid)
				shapeTypeText=IDSN_PURE_SIMPLE_SHAPE_SPHEROID_;
			if (shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_cylinder)
				shapeTypeText=IDSN_PURE_SIMPLE_SHAPE_CYLINDER_;
			if (shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_cone)
				shapeTypeText=IDSN_PURE_SIMPLE_SHAPE_CONE_;
		}
	}
	else
	{
		if (g)
		{
			if (isConvex)
				shapeTypeText=IDSN_GROUPED_CONVEX_SHAPE;
			else
				shapeTypeText=IDSN_GROUPED_RANDOM_SHAPE;
		}
		else
		{
			if (isConvex)
				shapeTypeText=IDSN_SIMPLE_CONVEX_SHAPE;
			else
				shapeTypeText=IDSN_SIMPLE_RANDOM_SHAPE;
		}
	}
	ui->qqShapeType->setText(shapeTypeText.c_str());

	setWindowTitle(titleText.c_str());
	ui->qqVertexCnt->setText(tt::FNb(vertexCount).c_str());
	ui->qqTriangleCnt->setText(tt::FNb(triangleCount).c_str());
}

bool CQDlgGeometry::needsDestruction()
{
	if (!isLinkedDataValid())
		return(true);
	return(CDlgEx::needsDestruction());
}

void CQDlgGeometry::_initialize(int identification[3])
{
	_identification[0]=identification[0];
	_identification[1]=identification[1];
	_identification[2]=identification[2];
	scaleVal[0]=1.0f;
	scaleVal[1]=1.0f;
	scaleVal[2]=1.0f;
	rotationVal[0]=0.0f;
	rotationVal[1]=0.0f;
	rotationVal[2]=0.0f;
	_setCurrentSizes();
	keepProp=true;
	isPureShape=true;
	isConvex=true;
	isGroup=false;
	if (_identification[1]==0)
	{
		CShape* shape=App::ct->objCont->getShape(_identification[2]);
		if (shape!=NULL)
		{
			titleText=strTranslate("Geometry associated with '");
			titleText+=shape->getName();
			titleText+="'";
			std::vector<float> wvert;
			std::vector<int> wind;
			shape->geomData->geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
			vertexCount=wvert.size()/3;
			triangleCount=wind.size()/3;
			isPureShape=shape->geomData->geomInfo->isPure();
			isConvex=shape->geomData->geomInfo->isConvex();
			isGroup=!shape->geomData->geomInfo->isGeometric();
		}
	}
	insideRefreshTriggered=true;
	refresh();
}

void CQDlgGeometry::_setCurrentSizes()
{
	if (_identification[1]==0)
	{ // texture is linked to a shape
		CShape* shape=App::ct->objCont->getShape(_identification[2]);
		if (shape!=NULL)
		{
			C3Vector bbhalfSizes(shape->geomData->getBoundingBoxHalfSizes());
			sizeVal[0]=2.0f*bbhalfSizes(0);
			sizeVal[1]=2.0f*bbhalfSizes(1);
			sizeVal[2]=2.0f*bbhalfSizes(2);
		}
	}
}

bool CQDlgGeometry::isLinkedDataValid()
{
	if (!CLifeControl::isAlive(_identification[0]))
		return(false);
	if ( (App::ct->simulation==NULL)||(App::ct->objCont==NULL) )
		return(false);
	if (!App::ct->simulation->isSimulationStopped())
		return(false);
	if (App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)
		return(false);
	if (_identification[1]==0)
	{ // we have a 3DObject here
		if (App::ct->objCont!=NULL)
		{
			if (App::ct->objCont->getLastSelectionID()!=_identification[2])
				return(false);
		}
	}
	return(true);
}

void CQDlgGeometry::display(int identification[3],QWidget* theParentWindow)
{
	if (App::mainWindow==NULL)
		return;
	App::mainWindow->dlgCont->close(GEOMETRY_DLG);
	if (App::mainWindow->dlgCont->openOrBringToFront(GEOMETRY_DLG))
	{
		CQDlgGeometry* geom=(CQDlgGeometry*)App::mainWindow->dlgCont->getDialog(GEOMETRY_DLG);
		if (geom!=NULL)
			geom->_initialize(identification);
	}
}

void CQDlgGeometry::cancelEvent()
{ // We just hide the dialog and destroy it at next rendering pass
	_identification[0]=-1;
	CDlgEx::cancelEvent();
}

bool CQDlgGeometry::doesInstanceSwitchRequireDestruction()
{
	return(true);
}

void CQDlgGeometry::_readSize(int index)
{
	QLineEdit* ww[3]={ui->qqSizeX,ui->qqSizeY,ui->qqSizeZ};
	if (!isLinkedDataValid())
		return;
	CShape* shape=NULL;
	if (_identification[1]==0)
	{ // geometry is linked to a shape
		shape=App::ct->objCont->getShape(_identification[2]);
		bool ok;
		float newVal=ww[index]->text().toFloat(&ok);
		if (ok)
		{
			newVal=tt::getLimitedFloat(0.0001f,1000.0f,newVal*gv::userToMeter);
			float sc=1.0f;

			C3Vector bbhalfSizes(shape->geomData->getBoundingBoxHalfSizes());

			if ((sizeVal[index]!=0.0f)&&(bbhalfSizes(index)!=0.0f)) // imagine we have a plane that has dims x*y*0!

				sc=newVal/sizeVal[index];
			if (keepProp)
			{
				for (int i=0;i<3;i++)
					sizeVal[i]*=sc;
			}
			else
			{
				if (bbhalfSizes(index)==0.0f)
					newVal=0.0f; // imagine we have a plane that has dims x*y*0!
				sizeVal[index]=newVal;
			}

			if (shape->geomData->geomInfo->isGeometric()&&(shape->geomData->geomInfo->getPurePrimitiveType()!=sim_pure_primitive_spheroid))
			{
				if ( (shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_disc)||
					(shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_cylinder)||
					(shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_cone)||
					(shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_heightfield) )
				{
					if (index==0)
						sizeVal[1]=sizeVal[0];
					if (index==1)
						sizeVal[0]=sizeVal[1];
				}
			}
			else
			{ // groups and spheroids have only iso-scaling
				if (!keepProp)
				{ // should normally never happen (grouped shapes have the "keepProp" flag set)
					if (index==0)
					{
						sizeVal[1]*=sc;
						sizeVal[2]*=sc;
					}
					if (index==1)
					{
						sizeVal[0]*=sc;
						sizeVal[2]*=sc;
					}
					if (index==2)
					{
						sizeVal[0]*=sc;
						sizeVal[1]*=sc;
					}
				}
			}
		}
	}
}

void CQDlgGeometry::_readScaling(int index)
{
	QLineEdit* ww[3]={ui->qqScaleX,ui->qqScaleY,ui->qqScaleZ};
	if (!isLinkedDataValid())
		return;
	CShape* shape=NULL;
	if (_identification[1]==0)
	{ // geometry is linked to a shape
		shape=App::ct->objCont->getShape(_identification[2]);
		bool ok;
		float newVal=ww[index]->text().toFloat(&ok);
		if (!keepProp)
		{ // imagine we have a plane that has dims x*y*0!
			C3Vector bbhalfSizes(shape->geomData->getBoundingBoxHalfSizes());
			if (bbhalfSizes(index)==0.0f)
				newVal=1.0f;
		}
		if (ok)
		{
			if ((newVal>=0)||isPureShape||isConvex) // pure or convex shapes should never be flipped!
				newVal=tt::getLimitedFloat(0.0001f,1000.0f,newVal);
			else
				newVal=tt::getLimitedFloat(-1000.0f,-0.0001f,newVal);

			float sc=1.0f;
			if (scaleVal[index]!=0.0f)
				sc=newVal/scaleVal[index];
			if (keepProp)
			{
				for (int i=0;i<3;i++)
					scaleVal[i]*=sc;
			}
			else
				scaleVal[index]=newVal;

			if (shape->geomData->geomInfo->isGeometric()&&(shape->geomData->geomInfo->getPurePrimitiveType()!=sim_pure_primitive_spheroid))
			{
				if ( (shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_disc)||
					(shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_cylinder)||
					(shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_cone)||
					(shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_heightfield) )
				{
					if (index==0)
						scaleVal[1]=scaleVal[0];
					if (index==1)
						scaleVal[0]=scaleVal[1];
				}
			}
			else
			{ // groups and spheroids have only iso-scaling
				if (index==0)
					scaleVal[1]=scaleVal[2]=scaleVal[0];
				if (index==1)
					scaleVal[0]=scaleVal[2]=scaleVal[1];
				if (index==2)
					scaleVal[0]=scaleVal[1]=scaleVal[2];
			}
		}
	}
}

void CQDlgGeometry::_readRotation(int index)
{
	QLineEdit* ww[3]={ui->qqAlpha,ui->qqBeta,ui->qqGamma};
	if ((!isLinkedDataValid())||(isPureShape&&(!isGroup)))
		return;
//	CShape* shape=NULL;
	if (_identification[1]==0)
	{ // geometry is linked to a shape
//		shape=App::ct->objCont->getShape(_identification[2]);
		bool ok;
		float newVal=ww[index]->text().toFloat(&ok);
		if (ok)
		{
			rotationVal[index]=newVal*gv::userToRad;
			C4Vector tr(rotationVal[0],rotationVal[1],rotationVal[2]);
			C3Vector euler(tr.getEulerAngles());
			euler.copyTo(rotationVal);
		}
	}
}

void CQDlgGeometry::on_qqKeepProp_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			keepProp=!keepProp;
			scaleVal[0]=1.0f;
			scaleVal[1]=1.0f;
			scaleVal[2]=1.0f;
			_setCurrentSizes(); // to reset sizes
			insideRefreshTriggered=true;
			refresh();
		}
	}
}

void CQDlgGeometry::on_qqSizeX_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			_readSize(0);
			insideRefreshTriggered=true;
			refresh();
		}
	}
}

void CQDlgGeometry::on_qqSizeY_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			_readSize(1);
			insideRefreshTriggered=true;
			refresh();
		}
	}
}

void CQDlgGeometry::on_qqSizeZ_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			_readSize(2);
			insideRefreshTriggered=true;
			refresh();
		}
	}
}

void CQDlgGeometry::on_qqScaleX_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			_readScaling(0);
			insideRefreshTriggered=true;
			refresh();
		}
	}
}

void CQDlgGeometry::on_qqScaleY_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			_readScaling(1);
			insideRefreshTriggered=true;
			refresh();
		}
	}
}

void CQDlgGeometry::on_qqScaleZ_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			_readScaling(2);
			insideRefreshTriggered=true;
			refresh();
		}
	}
}

void CQDlgGeometry::on_qqApplySize_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (isLinkedDataValid())
		{
			if (_identification[1]==0)
			{ // geometry is linked to a shape
				CShape* shape=App::ct->objCont->getShape(_identification[2]);
				if (shape!=NULL)
				{
					C3Vector bbhalfSizes(shape->geomData->getBoundingBoxHalfSizes());
					float xSizeOriginal=2.0f*bbhalfSizes(0);
					float ySizeOriginal=2.0f*bbhalfSizes(1);
					float zSizeOriginal=2.0f*bbhalfSizes(2);
					float s[3]={1.0f,1.0f,1.0f}; // imagine we have a plane that has dims x*y*0! keep default at 1.0
					if (xSizeOriginal!=0.0f)
						s[0]=sizeVal[0]/xSizeOriginal;
					if (ySizeOriginal!=0.0f)
						s[1]=sizeVal[1]/ySizeOriginal;
					if (zSizeOriginal!=0.0f)
						s[2]=sizeVal[2]/zSizeOriginal;
					shape->geomData->scale(s[0],s[1],s[2]);
					App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
					_setCurrentSizes();
					insideRefreshTriggered=true;
					refresh();
				}
			}
		}
	}
}

void CQDlgGeometry::on_qqApplyScale_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (isLinkedDataValid())
		{
			if (_identification[1]==0)
			{ // geometry is linked to a shape
				CShape* shape=App::ct->objCont->getShape(_identification[2]);
				if (shape!=NULL)
				{
					shape->geomData->scale(scaleVal[0],scaleVal[1],scaleVal[2]);
					App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
					_setCurrentSizes();
					insideRefreshTriggered=true;
					refresh();
				}
			}
		}
	}
}

void CQDlgGeometry::on_qqAlpha_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			_readRotation(0);
			insideRefreshTriggered=true;
			refresh();
		}
	}
}

void CQDlgGeometry::on_qqBeta_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			_readRotation(1);
			insideRefreshTriggered=true;
			refresh();
		}
	}
}

void CQDlgGeometry::on_qqGamma_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			_readRotation(2);
			insideRefreshTriggered=true;
			refresh();
		}
	}
}

void CQDlgGeometry::on_qqApplyEuler_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (isLinkedDataValid())
		{
			if (_identification[1]==0)
			{ // geometry is linked to a shape
				CShape* shape=App::ct->objCont->getShape(_identification[2]);
				if (shape!=NULL)
				{
					C7Vector localBefore(shape->getLocalTransformationPart1());
					C7Vector parentCumul(shape->getParentCumulativeTransformation());
					C7Vector tr;
					tr.setIdentity();
					tr.Q.setEulerAngles(rotationVal[0],rotationVal[1],rotationVal[2]);
					shape->setLocalTransformation(parentCumul.getInverse()*tr.getInverse());
					shape->alignBoundingBoxWithWorld();
					shape->setLocalTransformation(localBefore*tr*parentCumul*shape->getLocalTransformation());
					App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
					insideRefreshTriggered=true;
					refresh();
				}
			}
		}
	}
}

void CQDlgGeometry::on_qqInsideScaling_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (isLinkedDataValid())
		{
			if (_identification[1]==0)
			{ // geometry is linked to a shape
				CShape* shape=App::ct->objCont->getShape(_identification[2]);
				if (shape!=NULL)
				{
					bool ok;
					float newVal=ui->qqInsideScaling->text().toFloat(&ok);
					if (ok)
					{
						if (shape->geomData->geomInfo->isGeometric())
						{
							if ( (shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_spheroid)||
								(shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_cylinder)||
								(shape->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_cuboid) )
							{
								if ((newVal!=0.0f)&&(((CGeometric*)shape->geomData->geomInfo)->getPurePrimitiveInsideScaling()==0.0f))
									App::uiThread->messageBox_warning(App::mainWindow,IDSN_PURE_HOLLOW_SHAPES,IDSN_PURE_HOLLOW_SHAPES_ONLY_WITH_VORTEX,VMessageBox::OKELI);
								((CGeometric*)shape->geomData->geomInfo)->setPurePrimitiveInsideScaling(newVal);
								App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
							}
						}
					}
					refresh();
				}
			}
		}
	}
}
