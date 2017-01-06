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

// Not elegant. Requires refactoring

#include "vrepPrecompiledHeader.h"
#include "qdlgtextures.h"
#include "ui_qdlgtextures.h"
#include "Tt.h"
#include "GV.h"
#include "OGL.h"
#include "IloIlo.h"
#include "imgLoader.h"
#include "geometric.h"
#include "qdlgtextureselection.h"
#include "qdlgtextureloadoptions.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>
#include "VFileDialog.h"
#include "App.h"

CQDlgTextures::CQDlgTextures(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgTextures)
{
    ui->setupUi(this);
	_identification[0]=-1;
	_identification[1]=-1;
	_identification[2]=-1;
	_identification[3]=-1;
	if (App::mainWindow!=NULL)
		App::mainWindow->dlgCont->close(TEXTURE_DLG);
	inMainRefreshRoutine=false;
}

CQDlgTextures::~CQDlgTextures()
{
    delete ui;
}

void CQDlgTextures::refresh()
{
	if (!isLinkedDataValid())
		return;
	inMainRefreshRoutine=true;
	CTextureProperty* tp=NULL;
	CShape* shape=NULL;
	CButtonBlock* block=NULL;
	CSoftButton* button=NULL;
	bool applyTexture3D=false;
	bool usingFixedTextureCoordinates=false;
	bool foundTextureCoordinatesOnShape=false;
	bool forbidU=false;
	bool forbidV=false;
	if (_identification[1]==0)
	{ // texture is linked to a shape
		shape=App::ct->objCont->getShape(_identification[2]);
		tp=((CGeometric*)shape->geomData->geomInfo)->getTextureProperty();
		if (tp!=NULL)
		{
			usingFixedTextureCoordinates=tp->getFixedCoordinates();
			forbidU=(tp->getTextureMapMode()==TEXTURE_COORD_CYLINDER)||(tp->getTextureMapMode()==TEXTURE_COORD_SPHERE);
			forbidV=(tp->getTextureMapMode()==TEXTURE_COORD_SPHERE);
		}
		else
			foundTextureCoordinatesOnShape=(((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.size()!=0);
		applyTexture3D=true;
	}
	if (_identification[1]==1)
	{ // texture is linked to a 2DElement
		block=App::ct->buttonBlockContainer->getBlockWithID(_identification[2]);
		tp=block->getTextureProperty();
	}
	if (_identification[1]==2)
	{ // texture is linked to a 2DElement button
		block=App::ct->buttonBlockContainer->getBlockWithID(_identification[2]);
		button=block->getButtonWithUniqueID(_identification[3]);
		tp=button->getTextureProperty();
	}

	// First enable/disable & initialize the common part:
	ui->qqX->setEnabled((tp!=NULL)&&applyTexture3D&&(!usingFixedTextureCoordinates));
	ui->qqY->setEnabled((tp!=NULL)&&applyTexture3D&&(!usingFixedTextureCoordinates));
	ui->qqU->setEnabled((tp!=NULL)&&applyTexture3D&&(!usingFixedTextureCoordinates)&&(!forbidU));
	ui->qqV->setEnabled((tp!=NULL)&&applyTexture3D&&(!usingFixedTextureCoordinates)&&(!forbidV));
	ui->qqInterpolate->setEnabled(tp!=NULL);
	ui->qqRepeatU->setEnabled((tp!=NULL)&&applyTexture3D&&(!usingFixedTextureCoordinates)&&(!forbidU));
	ui->qqRepeatV->setEnabled((tp!=NULL)&&applyTexture3D&&(!usingFixedTextureCoordinates)&&(!forbidV));
	ui->qqLoad->setEnabled(tp==NULL);
	// Now enable/disable the 3D part:
	ui->qqZ->setEnabled((tp!=NULL)&&applyTexture3D&&(!usingFixedTextureCoordinates));
	ui->qqAlpha->setEnabled((tp!=NULL)&&applyTexture3D&&(!usingFixedTextureCoordinates));
	ui->qqBeta->setEnabled((tp!=NULL)&&applyTexture3D&&(!usingFixedTextureCoordinates));
	ui->qqGamma->setEnabled((tp!=NULL)&&applyTexture3D&&(!usingFixedTextureCoordinates));
	ui->qqModulate->setEnabled(tp!=NULL);
	ui->qqMapMode->setEnabled((tp!=NULL)&&applyTexture3D);

	ui->qqMapMode->clear();

	if (tp!=NULL)
	{
		C7Vector v(tp->getTextureRelativeConfig());
		C3Vector euler(v.Q.getEulerAngles());
		float scalingX,scalingY;
		tp->getTextureScaling(scalingX,scalingY);
		if (shape!=NULL)
		{
			if (usingFixedTextureCoordinates)
			{
				ui->qqX->setText("");
				ui->qqY->setText("");
				ui->qqZ->setText("");
				ui->qqAlpha->setText("");
				ui->qqBeta->setText("");
				ui->qqGamma->setText("");

				ui->qqU->setText("");
				ui->qqV->setText("");
				ui->qqMapMode->addItem(strTranslate(IDSN_MAP_COORD_IMPORTED),QVariant(-1));
			}
			else
			{
				ui->qqX->setText(gv::getSizeStr(true,v.X(0)).c_str());
				ui->qqY->setText(gv::getSizeStr(true,v.X(1)).c_str());
				ui->qqZ->setText(gv::getSizeStr(true,v.X(2)).c_str());
				ui->qqAlpha->setText(gv::getAngleStr(true,euler(0)).c_str());
				ui->qqBeta->setText(gv::getAngleStr(true,euler(1)).c_str());
				ui->qqGamma->setText(gv::getAngleStr(true,euler(2)).c_str());

				ui->qqU->setText(tt::FNb(0,scalingX,2,true).c_str());
				ui->qqV->setText(tt::FNb(0,scalingY,2,true).c_str());
			}
			ui->qqMapMode->addItem(strTranslate(IDSN_MAP_COORD_PROJECTION),QVariant(TEXTURE_COORD_PLANE));
			ui->qqMapMode->addItem(strTranslate(IDSN_MAP_COORD_CYLINDER),QVariant(TEXTURE_COORD_CYLINDER));
			ui->qqMapMode->addItem(strTranslate(IDSN_MAP_COORD_SPHERE),QVariant(TEXTURE_COORD_SPHERE));
			ui->qqMapMode->addItem(strTranslate(IDSN_MAP_COORD_CUBE),QVariant(TEXTURE_COORD_CUBE));
			for (int i=0;i<ui->qqMapMode->count();i++)
			{
				int mm=tp->getTextureMapMode();
				if (usingFixedTextureCoordinates)
					mm=-1;
				if (ui->qqMapMode->itemData(i).toInt()==mm)
				{
					ui->qqMapMode->setCurrentIndex(i);
					break;
				}
			}

			ui->qqRepeatU->setChecked(tp->getRepeatU());
			ui->qqRepeatV->setChecked(tp->getRepeatV());
		}
		else
		{ // for now 2DElement textures are not very flexible!
			ui->qqX->setText("");
			ui->qqY->setText("");
			ui->qqZ->setText("");
			ui->qqAlpha->setText("");
			ui->qqBeta->setText("");
			ui->qqGamma->setText("");

			ui->qqU->setText("");
			ui->qqV->setText("");

			ui->qqModulate->setChecked(false);

			ui->qqRepeatU->setChecked(false);
			ui->qqRepeatV->setChecked(false);
		}
		ui->qqInterpolate->setChecked(tp->getInterpolateColors());
		ui->qqModulate->setChecked(!tp->getDecalTexture());
		ui->qqRemoveSelect->setText(strTranslate(IDS_REMOVE_TEXTURE));
		ui->qqRemoveSelect->setEnabled(true);
		std::string textureName=strTranslate(IDS_TEXTURE_NAME_NONE);
		if ((tp->getTextureObjectID()>=SIM_IDSTART_TEXTURE)&&(tp->getTextureObjectID()<=SIM_IDEND_TEXTURE))
		{ // we have a static texture
			CTextureObject* to=App::ct->textureCont->getObject(tp->getTextureObjectID());
			if (to!=NULL)
			{
				textureName=to->getObjectName();
				int sx,sy;
				to->getTextureSize(sx,sy);
				textureName+=" [";
				textureName+=boost::lexical_cast<std::string>(sx)+"x"+boost::lexical_cast<std::string>(sy)+"] ";
				textureName+=tt::decorateString(" (",strTranslate(IDSN_STATIC_TEXTURE),")");
			}
		}
		else
		{ // we have a dynamic texture
			CVisionSensor* rs=App::ct->objCont->getVisionSensor(tp->getTextureObjectID());
			if (rs!=NULL)
			{
				textureName=rs->getName();
				int s[2];
				rs->getRealResolution(s);
				textureName+=" [";
				textureName+=boost::lexical_cast<std::string>(s[0])+"x"+boost::lexical_cast<std::string>(s[1])+"] ";
				textureName+=tt::decorateString(" (",strTranslate(IDSN_DYNAMIC_TEXTURE),")");
			}
		}
		ui->qqTextureName->setText(textureName.c_str());

		if (foundTextureCoordinatesOnShape||usingFixedTextureCoordinates)
			ui->qqTextureCoordinates->setText(strTranslate(IDS_FROM_SHAPE_IMPORT));
		else
			ui->qqTextureCoordinates->setText(strTranslate(IDS_CALCULATED));
	}
	else
	{

		ui->qqX->setText("");
		ui->qqY->setText("");
		ui->qqZ->setText("");
		ui->qqAlpha->setText("");
		ui->qqBeta->setText("");
		ui->qqGamma->setText("");

		ui->qqU->setText("");
		ui->qqV->setText("");

		ui->qqModulate->setChecked(false);
		ui->qqInterpolate->setChecked(false);
		ui->qqRepeatU->setChecked(false);
		ui->qqRepeatV->setChecked(false);

		ui->qqTextureName->setText(strTranslate(IDS_TEXTURE_NAME_NONE));
		ui->qqRemoveSelect->setText(strTranslate(IDS_SELECT_TEXTURE_FROM_EXISTING));
		// Check if there are already existing textures:
		ui->qqRemoveSelect->setEnabled( (App::ct->textureCont->getObjectAtIndex(0)!=NULL)||(App::ct->objCont->visionSensorList.size()!=0) );

		if (foundTextureCoordinatesOnShape)
			ui->qqTextureCoordinates->setText(strTranslate(IDS_FROM_SHAPE_IMPORT));
		else
			ui->qqTextureCoordinates->setText(strTranslate(IDS_TEXTURE_NAME_NONE)); // Actually just "none"
	}
	inMainRefreshRoutine=false;
}

bool CQDlgTextures::needsDestruction()
{
	if (!isLinkedDataValid())
		return(true);
	return(CDlgEx::needsDestruction());
}

void CQDlgTextures::_initialize(int identification[4])
{
	_identification[0]=identification[0];
	_identification[1]=identification[1];
	_identification[2]=identification[2];
	_identification[3]=identification[3];
	refresh();
}

bool CQDlgTextures::isLinkedDataValid()
{
	if (!CLifeControl::isAlive(_identification[0]))
		return(false);
	if ( (App::ct->simulation==NULL)||(App::ct->objCont==NULL) )
		return(false);
	if (!App::ct->simulation->isSimulationStopped())
		return(false);
	if ((App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)&&(App::ct->objCont->getEditModeType()!=BUTTON_EDIT_MODE))
		return(false);

	if (_identification[1]==0)
	{ // we have a 3DObject here
		if (App::ct->objCont!=NULL)
			return (App::ct->objCont->getLastSelectionID()==_identification[2]);
	}
	if (_identification[1]==1)
	{ // we have a 2DElement here
		if ( (App::ct->objCont!=NULL)&&(App::ct->buttonBlockContainer!=NULL) )
		{
			if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
				return (App::ct->buttonBlockContainer->getBlockInEdition()==_identification[2]);
		}
	}
	if (_identification[1]==2)
	{ // we have a 2DElement button here
		if ( (App::ct->objCont!=NULL)&&(App::ct->buttonBlockContainer!=NULL) )
		{
			if ( (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)&&(App::ct->buttonBlockContainer->getBlockInEdition()==_identification[2]) )
			{
				if (App::ct->buttonBlockContainer->selectedButtons.size()==1)
				{
					CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(_identification[2]);
					if (itBlock!=NULL)
					{
						VPoint size;
						itBlock->getBlockSize(size);
						int butt=App::ct->buttonBlockContainer->selectedButtons[0];
						CSoftButton* but=itBlock->getButtonAtPos(butt%size.x,butt/size.x);
						if (but!=NULL)
							return (but->getUniqueID()==_identification[3]);
					}
				}
			}
		}
	}
	return(false);
}

void CQDlgTextures::display(int identification[4],QWidget* theParentWindow)
{
	if (App::mainWindow==NULL)
		return;
	App::mainWindow->dlgCont->close(TEXTURE_DLG);
	if (App::mainWindow->dlgCont->openOrBringToFront(TEXTURE_DLG))
	{
		CQDlgTextures* tex=(CQDlgTextures*)App::mainWindow->dlgCont->getDialog(TEXTURE_DLG);
		if (tex!=NULL)
			tex->_initialize(identification);
	}
}

void CQDlgTextures::displayModal(int identification[4],QWidget* theParentWindow)
{
	if (App::mainWindow==NULL)
		return;
	App::mainWindow->dlgCont->close(TEXTURE_DLG);

	CQDlgTextures it(theParentWindow);
	it._initialize(identification);
	it.makeDialogModal();
}

void CQDlgTextures::_setTextureConfig(int index)
{
	QLineEdit* ww[6]={ui->qqX,ui->qqY,ui->qqZ,ui->qqAlpha,ui->qqBeta,ui->qqGamma};
	if (!isLinkedDataValid())
		return;
	CTextureProperty* tp=NULL;
	CShape* shape=NULL;
	if (_identification[1]==0)
	{ // texture is linked to a shape
		shape=App::ct->objCont->getShape(_identification[2]);
		tp=((CGeometric*)shape->geomData->geomInfo)->getTextureProperty();
		bool ok;
		float newVal=ww[index]->text().toFloat(&ok);
		if ((tp!=NULL)&&ok)
		{
			C7Vector tr(tp->getTextureRelativeConfig());
			if (index<3)
			{ // position
				newVal=tt::getLimitedFloat(-100.0f,100.0f,newVal*gv::userToMeter);
				tr.X(index)=newVal;
			}
			else
			{ // orientation
				C3Vector euler(tr.Q.getEulerAngles());
				euler(index-3)=newVal*gv::userToRad;
				tr.Q.setEulerAngles(euler);
			}
			tp->setTextureRelativeConfig(tr);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
	}
}

void CQDlgTextures::_setTextureScaling(int index)
{
	QLineEdit* ww[2]={ui->qqU,ui->qqV};
	if (!isLinkedDataValid())
		return;
	CTextureProperty* tp=NULL;
	CShape* shape=NULL;
	if (_identification[1]==0)
	{ // texture is linked to a shape
		shape=App::ct->objCont->getShape(_identification[2]);
		tp=((CGeometric*)shape->geomData->geomInfo)->getTextureProperty();
		bool ok;
		float newVal=ww[index]->text().toFloat(&ok);
		if ((tp!=NULL)&&ok)
		{
			float x,y;
			tp->getTextureScaling(x,y);
			if (newVal>=0.0f)
				newVal=tt::getLimitedFloat(0.001f,1000.0f,newVal);
			else
				newVal=tt::getLimitedFloat(-1000.0f,-0.001f,newVal);
			if (index==0)
				tp->setTextureScaling(newVal,y);
			else
				tp->setTextureScaling(x,newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
	}
}

void CQDlgTextures::_setTextureBooleanProperty(int index)
{
	if (!isLinkedDataValid())
		return;
	CTextureProperty* tp=NULL;
	if (_identification[1]==0)
	{ // texture is linked to a shape
		CShape* shape=App::ct->objCont->getShape(_identification[2]);
		tp=((CGeometric*)shape->geomData->geomInfo)->getTextureProperty();
		if (tp!=NULL)
		{
			if (index==0)
				tp->setInterpolateColors(!tp->getInterpolateColors());
			if (index==1)
				tp->setDecalTexture(!tp->getDecalTexture());
			if (index==2)
				tp->setRepeatU(!tp->getRepeatU());
			if (index==3)
				tp->setRepeatV(!tp->getRepeatV());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
	}
	if (_identification[1]==1)
	{ // texture is linked to a 2DElement
		CButtonBlock* block=App::ct->buttonBlockContainer->getBlockWithID(_identification[2]);
		tp=block->getTextureProperty();
		if (tp!=NULL)
		{
			if (index==0)
				tp->setInterpolateColors(!tp->getInterpolateColors());
			if (index==1)
				tp->setDecalTexture(!tp->getDecalTexture());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
	}
	if (_identification[1]==2)
	{ // texture is linked to a 2DElement button
		CButtonBlock* block=App::ct->buttonBlockContainer->getBlockWithID(_identification[2]);
		CSoftButton* button=block->getButtonWithUniqueID(_identification[3]);
		tp=button->getTextureProperty();
		if (tp!=NULL)
		{
			if (index==0)
				tp->setInterpolateColors(!tp->getInterpolateColors());
			if (index==1)
				tp->setDecalTexture(!tp->getDecalTexture());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
	}
}

void CQDlgTextures::cancelEvent() // this was empty before VDialog wrap thing
{ // We just hide the dialog and destroy it at next rendering pass
	_identification[0]=-1;
	if (isModal()) // this condition and next line on 20/5/2013: on Linux the dlg couldn't be closed! Thanks to Ulrich Schwesinger
		defaultModalDialogEndRoutine(false);
	else
		CDlgEx::cancelEvent();
}

bool CQDlgTextures::doesInstanceSwitchRequireDestruction()
{
	return(true);
}


void CQDlgTextures::on_qqAlpha_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureConfig(3);
		refresh();
	}
}

void CQDlgTextures::on_qqBeta_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureConfig(4);
		refresh();
	}
}

void CQDlgTextures::on_qqGamma_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureConfig(5);
		refresh();
	}
}

void CQDlgTextures::on_qqX_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureConfig(0);
		refresh();
	}
}

void CQDlgTextures::on_qqY_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureConfig(1);
		refresh();
	}
}

void CQDlgTextures::on_qqZ_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureConfig(2);
		refresh();
	}
}

void CQDlgTextures::on_qqU_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureScaling(0);
		refresh();
	}
}

void CQDlgTextures::on_qqV_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureScaling(1);
		refresh();
	}
}

void CQDlgTextures::on_qqInterpolate_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureBooleanProperty(0);
		refresh();
	}
}

void CQDlgTextures::on_qqModulate_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureBooleanProperty(1);
		refresh();
	}
}

void CQDlgTextures::on_qqRepeatU_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureBooleanProperty(2);
		refresh();
	}
}

void CQDlgTextures::on_qqRepeatV_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		_setTextureBooleanProperty(3);
		refresh();
	}
}

void CQDlgTextures::on_qqRemoveSelect_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA // IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (!isLinkedDataValid())
			return;
		if (_identification[1]==0)
		{ // texture is linked to a shape
			CShape* shape=App::ct->objCont->getShape(_identification[2]);
			CTextureProperty* tp=((CGeometric*)shape->geomData->geomInfo)->getTextureProperty();
			if (tp!=NULL)
			{ // remove the texture
				if (tp->getFixedCoordinates())
				{ // we put the fixed texture coordinates back to the shape:
					std::vector<float> dummyVert;
					std::vector<int> dummyInd;
					std::vector<float>* tc=tp->getTextureCoordinates(0,C7Vector::identityTransformation,dummyVert,dummyInd);
					if (tc!=NULL)
						((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.assign(tc->begin(),tc->end());
				}
				App::ct->textureCont->announceGeneralObjectWillBeErased(shape->getID(),-1);
				delete tp;
				((CGeometric*)shape->geomData->geomInfo)->setTextureProperty(NULL);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
			else
			{ // add an existing texture
				CQDlgTextureSelection dlg(this);
				dlg.makeDialogModal();
				if (dlg.selectedTextureObject!=-1)
				{
					if ((dlg.selectedTextureObject>=SIM_IDSTART_TEXTURE)&&(dlg.selectedTextureObject<=SIM_IDEND_TEXTURE))
					{
						CTextureObject* to=App::ct->textureCont->getObject(dlg.selectedTextureObject);
						to->addDependentObject(shape->getID(),((CGeometric*)shape->geomData->geomInfo)->getUniqueID());
					}
					tp=new CTextureProperty(dlg.selectedTextureObject);
					((CGeometric*)shape->geomData->geomInfo)->setTextureProperty(tp);
					// Following 2 since 12/6/2011 because now by default we have the modulate mode (non-decal)
					((CGeometric*)shape->geomData->geomInfo)->color.setColor(0.5f,0.5f,0.5f,AMBIENT_MODE);
					((CGeometric*)shape->geomData->geomInfo)->insideColor.setColor(0.5f,0.5f,0.5f,AMBIENT_MODE);

					std::vector<float> wvert;
					std::vector<int> wind;
					((CGeometric*)shape->geomData->geomInfo)->getCumulativeMeshes(wvert,&wind,NULL);

					if (((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.size()/2==wind.size())
					{ // we have texture coordinate data attached to the shape's geometry (was added during shape import)
						App::uiThread->messageBox_information(App::mainWindow,strTranslate("Texture coordinates"),strTranslate(IDS_USING_EXISTING_TEXTURE_COORDINATES),VMessageBox::OKELI);
						tp->setFixedCoordinates(&((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized);
						((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.clear();
					}

					App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				}
			}
		}
		if (_identification[1]==1)
		{ // texture is linked to a 2DElement
			CButtonBlock* block=App::ct->buttonBlockContainer->getBlockWithID(_identification[2]);
			CTextureProperty* tp=block->getTextureProperty();
			if (tp!=NULL)
			{ // remove the texture
				App::ct->textureCont->announceGeneralObjectWillBeErased(block->getBlockID(),0);
				delete tp;
				block->setTextureProperty(NULL);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
			else
			{ // add an existing texture
				CQDlgTextureSelection dlg(this);
				dlg.makeDialogModal();
				if (dlg.selectedTextureObject!=-1)
				{
					if ((dlg.selectedTextureObject>=SIM_IDSTART_TEXTURE)&&(dlg.selectedTextureObject<=SIM_IDEND_TEXTURE))
					{
						CTextureObject* to=App::ct->textureCont->getObject(dlg.selectedTextureObject);
						to->addDependentObject(block->getBlockID(),0);
					}
					tp=new CTextureProperty(dlg.selectedTextureObject);
					tp->setDecalTexture(true); // 13/1/2012
					block->setTextureProperty(tp);
					App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				}
			}
		}
		if (_identification[1]==2)
		{ // texture is linked to a 2DElement button
			CButtonBlock* block=App::ct->buttonBlockContainer->getBlockWithID(_identification[2]);
			CSoftButton* button=block->getButtonWithUniqueID(_identification[3]);
			CTextureProperty* tp=button->getTextureProperty();
			if (tp!=NULL)
			{ // remove the texture
				App::ct->textureCont->announceGeneralObjectWillBeErased(_identification[2],_identification[3]);
				delete tp;
				button->setTextureProperty(NULL);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
			else
			{ // add an existing texture
				CQDlgTextureSelection dlg(this);
				dlg.makeDialogModal();
				if (dlg.selectedTextureObject!=-1)
				{
					if ((dlg.selectedTextureObject>=SIM_IDSTART_TEXTURE)&&(dlg.selectedTextureObject<=SIM_IDEND_TEXTURE))
					{
						CTextureObject* to=App::ct->textureCont->getObject(dlg.selectedTextureObject);
						to->addDependentObject(block->getBlockID(),_identification[3]);
					}
					tp=new CTextureProperty(dlg.selectedTextureObject);
					tp->setDecalTexture(true); // 13/1/2012
					button->setTextureProperty(tp);
					App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				}
			}
		}
		refresh();
	}
}

void CQDlgTextures::on_qqLoad_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (!isLinkedDataValid())
			return;
		CTextureProperty* tp=NULL;
		CShape* shape=NULL;
		CButtonBlock* block=NULL;
		CSoftButton* button=NULL;
		if (_identification[1]==0)
		{ // texture is linked to a shape
			shape=App::ct->objCont->getShape(_identification[2]);
			tp=((CGeometric*)shape->geomData->geomInfo)->getTextureProperty();
		}
		if (_identification[1]==1)
		{ // texture is linked to a 2DElement
			block=App::ct->buttonBlockContainer->getBlockWithID(_identification[2]);
			tp=block->getTextureProperty();
		}
		if (_identification[1]==2)
		{ // texture is linked to a 2DElement button
			block=App::ct->buttonBlockContainer->getBlockWithID(_identification[2]);
			button=block->getButtonWithUniqueID(_identification[3]);
			tp=button->getTextureProperty();
		}

		if (tp==NULL)
		{
			std::string tst(App::directories->textureDirectory);
			std::string filenameAndPath=App::uiThread->getOpenFileName(this,0,"Loading texture...",tst,"",true,"Image files","tga","jpg","jpeg","png","gif","bmp","tiff");

			if (filenameAndPath.length()!=0)
			{
				if (VFile::doesFileExist(filenameAndPath))
				{
					CQDlgTextureLoadOptions dlg(App::mainWindow);
					dlg.refresh();
					dlg.makeDialogModal();
					int scaleTo=0;
					if (dlg.scale)
						scaleTo=dlg.scaleTo;
					App::directories->textureDirectory=App::directories->getPathFromFull(filenameAndPath);
					int resX,resY,n;
					unsigned char* data=CImageLoader::load(filenameAndPath.c_str(),&resX,&resY,&n,0,scaleTo);
					bool rgba=(n==4);
					if (n<3)
					{
						delete[] data;
						data=NULL;
					}
					if (data==NULL)
						App::uiThread->messageBox_critical(App::mainWindow,strTranslate("Texture"),strTranslate(IDS_TEXTURE_FILE_COULD_NOT_BE_LOADED),VMessageBox::OKELI);
					else
					{
						CTextureObject* textureObj=new CTextureObject(resX,resY);
						textureObj->setImage(rgba,false,false,data); // keep false,false
						textureObj->setObjectName(App::directories->getNameFromFull(filenameAndPath).c_str());
						delete[] data;
						if (shape!=NULL)
							textureObj->addDependentObject(shape->getID(),((CGeometric*)shape->geomData->geomInfo)->getUniqueID());
						if (block!=NULL)
						{
							if (button==NULL)
								textureObj->addDependentObject(block->getBlockID(),0); // 0 is for background texture on 2DElement
							else
								textureObj->addDependentObject(block->getBlockID(),button->getUniqueID()); // Unique ID starts exceptionnally at 1
						}
						int textureID=App::ct->textureCont->addObject(textureObj,false); // might erase the textureObj and return a similar object already present!!
						CTextureProperty* tp=new CTextureProperty(textureID);
						if (shape!=NULL)
						{
							// Following 2 since 12/6/2011 because now by default we have the modulate mode (non-decal)
							((CGeometric*)shape->geomData->geomInfo)->color.setColor(0.5f,0.5f,0.5f,AMBIENT_MODE);
							((CGeometric*)shape->geomData->geomInfo)->insideColor.setColor(0.5f,0.5f,0.5f,AMBIENT_MODE);
							((CGeometric*)shape->geomData->geomInfo)->setTextureProperty(tp);

							std::vector<float> wvert;
							std::vector<int> wind;
							((CGeometric*)shape->geomData->geomInfo)->getCumulativeMeshes(wvert,&wind,NULL);
	//printf("indSize:%i, vtSize:%i\n",wind.size(),((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.size());
							if (((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.size()/2==wind.size())
							{ // we have texture coordinate data attached to the shape's geometry (was added during shape import)
								App::uiThread->messageBox_information(App::mainWindow,strTranslate("Texture coordinates"),strTranslate(IDS_USING_EXISTING_TEXTURE_COORDINATES),VMessageBox::OKELI);
								tp->setFixedCoordinates(&((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized);
								((CGeometric*)shape->geomData->geomInfo)->textureCoords_notCopiedNorSerialized.clear();
							}
						}
						if (block!=NULL)
						{
							tp->setDecalTexture(true); // 13/1/2012
							if (button==NULL)
								block->setTextureProperty(tp);
							else
								button->setTextureProperty(tp);
						}
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************

						// Check if the resolution is a power of 2:
						int oResX=resX;
						int oResY=resY;
						resX&=(32768-1);
						resY&=(32768-1);
						WORD tmp=32768;
						while (tmp!=1)
						{
							if (resX&tmp)
								resX&=tmp;
							if (resY&tmp)
								resY&=tmp;
							tmp/=2;
						}
						if ((oResX!=resX)||(oResY!=resY))
							App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Texture"),strTranslate(IDS_TEXTURE_RESOLUTION_NOT_POWER_OF_TWO_WARNING),VMessageBox::OKELI);
					}
				}
			}
		}
		refresh();
	}
}

void CQDlgTextures::on_qqMapMode_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!inMainRefreshRoutine)
		{
			if (!isLinkedDataValid())
				return;
			if (_identification[1]==0)
			{ // texture is linked to a shape
				CShape* shape=App::ct->objCont->getShape(_identification[2]);
				CTextureProperty* tp=((CGeometric*)shape->geomData->geomInfo)->getTextureProperty();
				if (tp!=NULL)
				{
					bool usingFixedTextureCoordinates=tp->getFixedCoordinates();
					int mode=ui->qqMapMode->itemData(ui->qqMapMode->currentIndex()).toInt();
					int previousMode=tp->getTextureMapMode();
					bool setOk=false;
					if ((mode==TEXTURE_COORD_PLANE)&&((previousMode!=mode)||usingFixedTextureCoordinates) )
						setOk=true;
					if ((mode==TEXTURE_COORD_CYLINDER)&&(previousMode!=mode))
						setOk=true;
					if ((mode==TEXTURE_COORD_SPHERE)&&(previousMode!=mode))
						setOk=true;
					if ((mode==TEXTURE_COORD_CUBE)&&(previousMode!=mode))
						setOk=true;
					if (setOk)
					{
						tp->setTextureMapMode(mode);
						App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
					}
					refresh();
				}
			}
		}
	}
}
