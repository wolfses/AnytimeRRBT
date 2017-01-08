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
#include "qdlgshapes.h"
#include "ui_qdlgshapes.h"
#include "Tt.h"
#include "GV.h"
#include "qdlgmaterial.h"
#include "IloIlo.h"
#include "geometric.h"
#include "qdlgtextures.h"
#include "qdlggeometry.h"
#include "qdlgshapedyn.h"
#include "VFileDialog.h"
#include "App.h"
#include "imgLoader.h"
#include "v_repStrings.h"

CQDlgShapes::CQDlgShapes(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgShapes)
{
    ui->setupUi(this);
}

CQDlgShapes::~CQDlgShapes()
{
    delete ui;
}

void CQDlgShapes::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgShapes::refresh()
{
	bool sel=App::ct->objCont->isLastSelectionAShape();
	bool ssel=App::ct->objCont->isLastSelectionASimpleShape();
	int sc=App::ct->objCont->getShapeNumberInSelection();
	int ssc=App::ct->objCont->getSimpleShapeNumberInSelection();
	bool snr=App::ct->simulation->isSimulationStopped();
	CShape* it=NULL;
	if (sel)
	{
		it=App::ct->objCont->getShape(App::ct->objCont->getLastSelectionID());
	}

	ui->qqShadingAngle->setEnabled(ssel);
	ui->qqEdgesAngle->setEnabled(ssel);
	ui->qqBackfaceCulling->setEnabled(ssel);
	ui->qqSameColorInsideAndOutside->setEnabled(ssel);
	ui->qqWireframe->setEnabled(ssel);
	ui->qqShowEdges->setEnabled(ssel);
	ui->qqEdgesWidth->setEnabled(ssel&&it->getVisibleEdges());
	ui->qqInsideOutDisplay->setEnabled(ssel);

	ui->qqApplyColors->setEnabled(ssel&&(ssc>1));
	ui->qqAdjustEdgeColor->setEnabled(ssel);
	ui->qqAdjustOutsideColor->setEnabled(ssel);
	ui->qqAdjustInsideColor->setEnabled(ssel&&(!it->getInsideAndOutsideFacesSameColor()));
	ui->qqInvertFaces->setEnabled(sel);

	ui->qqGeometry->setEnabled(sel&&snr);
	ui->qqTexture->setEnabled(ssel&snr);
	ui->qqDirtTexture->setEnabled((sc>0)&&snr);
	ui->qqClearTextures->setEnabled((sc>0)&&snr);

	ui->qqApplyMain->setEnabled(sel&&(sc>1));

	ui->qqEditDynamics->setEnabled(sel);
	ui->qqEditDynamics->setChecked(CQDlgShapeDyn::showDynamicWindow);


	if (ssel)
	{
		ui->qqShadingAngle->setText(gv::getAngleStr(false,((CGeometric*)it->geomData->geomInfo)->getGouraudShadingAngle(),0).c_str());
		ui->qqEdgesAngle->setText(gv::getAngleStr(false,((CGeometric*)it->geomData->geomInfo)->getEdgeThresholdAngle(),0).c_str());
		ui->qqBackfaceCulling->setChecked(((CGeometric*)it->geomData->geomInfo)->getCulling());
		ui->qqSameColorInsideAndOutside->setChecked(((CGeometric*)it->geomData->geomInfo)->getInsideAndOutsideFacesSameColor());
		ui->qqWireframe->setChecked(((CGeometric*)it->geomData->geomInfo)->getWireframe());
		ui->qqShowEdges->setChecked(((CGeometric*)it->geomData->geomInfo)->getVisibleEdges());
		ui->qqEdgesWidth->setText(tt::FNb(0,((CGeometric*)it->geomData->geomInfo)->getEdgeWidth(),false).c_str());
		ui->qqInsideOutDisplay->setChecked(((CGeometric*)it->geomData->geomInfo)->getDisplayInverted());
	}
	else
	{
		ui->qqShadingAngle->setText("");
		ui->qqEdgesAngle->setText("");
		ui->qqBackfaceCulling->setChecked(false);
		ui->qqSameColorInsideAndOutside->setChecked(false);
		ui->qqWireframe->setChecked(false);
		ui->qqShowEdges->setChecked(false);
		ui->qqEdgesWidth->setText("");
		ui->qqInsideOutDisplay->setChecked(false);
	}
}

void CQDlgShapes::on_qqBackfaceCulling_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setCulling(!it->getCulling());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapes::on_qqWireframe_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setShapeWireframe(!it->getShapeWireframe());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapes::on_qqSameColorInsideAndOutside_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setInsideAndOutsideFacesSameColor(!it->getInsideAndOutsideFacesSameColor());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapes::on_qqInvertFaces_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->geomData->invertFrontBack();
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
}

void CQDlgShapes::on_qqShowEdges_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setVisibleEdges(!it->getVisibleEdges());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapes::on_qqEdgesWidth_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		int newVal=ui->qqEdgesWidth->text().toInt(&ok);
		if (ok)
		{
			shape->setEdgeWidth(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapes::on_qqShadingAngle_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqShadingAngle->text().toFloat(&ok);
		if (ok)
		{
			((CGeometric*)shape->geomData->geomInfo)->setGouraudShadingAngle(gv::userToRad*newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapes::on_qqApplyMain_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		if (App::ct->objCont->getSimpleShapeNumberInSelection()<2)
			return;
		CShape* last=(CShape*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CShape* it=App::ct->objCont->getShape(App::ct->objCont->getSelID(i));
			if (it!=NULL)
			{
				if ( (!it->isGrouping())&&(!last->isGrouping()) )
				{
					((CGeometric*)it->geomData->geomInfo)->setVisibleEdges(((CGeometric*)last->geomData->geomInfo)->getVisibleEdges());
					((CGeometric*)it->geomData->geomInfo)->setCulling(((CGeometric*)last->geomData->geomInfo)->getCulling());
					((CGeometric*)it->geomData->geomInfo)->setInsideAndOutsideFacesSameColor(((CGeometric*)last->geomData->geomInfo)->getInsideAndOutsideFacesSameColor());
					((CGeometric*)it->geomData->geomInfo)->setEdgeWidth(((CGeometric*)last->geomData->geomInfo)->getEdgeWidth());
					((CGeometric*)it->geomData->geomInfo)->setWireframe(((CGeometric*)last->geomData->geomInfo)->getWireframe());
					((CGeometric*)it->geomData->geomInfo)->setGouraudShadingAngle(((CGeometric*)last->geomData->geomInfo)->getGouraudShadingAngle());
					((CGeometric*)it->geomData->geomInfo)->setEdgeThresholdAngle(((CGeometric*)last->geomData->geomInfo)->getEdgeThresholdAngle());
					((CGeometric*)it->geomData->geomInfo)->setDisplayInverted(((CGeometric*)last->geomData->geomInfo)->getDisplayInverted());

					App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
				}
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapes::on_qqEditDynamics_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CQDlgShapeDyn::showDynamicWindow=!CQDlgShapeDyn::showDynamicWindow;
		if (App::mainWindow->dlgCont->isVisible(SHAPE_DYN_DLG)!=CQDlgShapeDyn::showDynamicWindow)
			App::mainWindow->dlgCont->toggle(SHAPE_DYN_DLG);
	}
}

void CQDlgShapes::on_qqAdjustOutsideColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&((CGeometric*)it->geomData->geomInfo)->color,App::mainWindow,true,true,true,true,true,true,true,true);
	}
}

void CQDlgShapes::on_qqAdjustInsideColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&((CGeometric*)it->geomData->geomInfo)->insideColor,App::mainWindow,true,true,true,true,true,true,true,true);
	}
}

void CQDlgShapes::on_qqAdjustEdgeColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&((CGeometric*)it->geomData->geomInfo)->edgeColor,App::mainWindow,true,false,false,true,false,false,true,true);
	}
}

void CQDlgShapes::on_qqApplyColors_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		if (App::ct->objCont->getSimpleShapeNumberInSelection()<2)
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CShape* it2=App::ct->objCont->getShape(App::ct->objCont->getSelID(i));
			if (it2!=NULL)
			{
				if ( (!it->isGrouping())&&(!it2->isGrouping()) )
				{
					((CGeometric*)it->geomData->geomInfo)->color.copyYourselfInto(&((CGeometric*)it2->geomData->geomInfo)->color);
					((CGeometric*)it->geomData->geomInfo)->insideColor.copyYourselfInto(&((CGeometric*)it2->geomData->geomInfo)->insideColor);
					((CGeometric*)it->geomData->geomInfo)->edgeColor.copyYourselfInto(&((CGeometric*)it2->geomData->geomInfo)->edgeColor);
					it2->actualizeContainsTransparentComponent();
					App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
				}
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
	}
}

void CQDlgShapes::on_qqTexture_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgTextures::display(identification,App::mainWindow);
	}
}

void CQDlgShapes::on_qqGeometry_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		int identification[3]={it->getLifeID(),0,it->getID()};
		CQDlgGeometry::display(identification,App::mainWindow);
	}
}

void CQDlgShapes::on_qqDirtTexture_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		// 1. Remove existing textures:
		std::vector<CShape*> shapeList;
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
		{
			CShape* shape=App::ct->objCont->getShape(App::ct->objCont->getSelID(i));
			if (shape!=NULL)
			{
				shapeList.push_back(shape);
				std::vector<CGeometric*> components;
				shape->geomData->geomInfo->getAllShapeComponentsCumulative(components);
				for (int j=0;j<int(components.size());j++)
				{
					CTextureProperty* tp=components[j]->getTextureProperty();
					if (tp!=NULL)
					{
						App::ct->textureCont->announceGeneralObjectWillBeErased(shape->getID(),-1);
						delete tp;
						components[j]->setTextureProperty(NULL);
					}
					components[j]->textureCoords_notCopiedNorSerialized.clear(); // discard existing texture coordinates
				}
			}
		}

		// 2. Load and apply the "dirt" texture:
		if (shapeList.size()!=0)
		{

		std::string tst(App::directories->textureDirectory);
		std::string filenameAndPath=App::uiThread->getOpenFileName(this,0,"Loading texture...",tst,"",true,"Image files","tga","jpg","jpeg","png","gif","bmp","tiff");
		if (filenameAndPath.length()!=0)
		{
			if (VFile::doesFileExist(filenameAndPath))
			{
				App::directories->textureDirectory=App::directories->getPathFromFull(filenameAndPath);
				int resX,resY,n;
				unsigned char* data=CImageLoader::load(filenameAndPath.c_str(),&resX,&resY,&n,0);
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

					for (int i=0;i<int(shapeList.size());i++)
					{
						CShape* shape=shapeList[i];
						std::vector<CGeometric*> components;
						shape->geomData->geomInfo->getAllShapeComponentsCumulative(components);
						for (int j=0;j<int(components.size());j++)
						{
							CGeometric* geom=components[j];
							textureObj->addDependentObject(shape->getID(),geom->getUniqueID());
						}
					}

					int textureID=App::ct->textureCont->addObject(textureObj,false); // might erase the textureObj and return a similar object already present!!

					for (int i=0;i<int(shapeList.size());i++)
					{
						CShape* shape=shapeList[i];
						C3Vector bbhs(shape->geomData->getBoundingBoxHalfSizes());
						float s=SIM_MAX(SIM_MAX(bbhs(0),bbhs(1)),bbhs(2))*2.0f;
						std::vector<CGeometric*> components;
						shape->geomData->geomInfo->getAllShapeComponentsCumulative(components);
						for (int j=0;j<int(components.size());j++)
						{
							CGeometric* geom=components[j];
							CTextureProperty* tp=new CTextureProperty(textureID);
							tp->setRepeatU(true);
							tp->setRepeatV(true);
							tp->setTextureMapMode(TEXTURE_COORD_CUBE);
							tp->setInterpolateColors(true);
							tp->setDecalTexture(false);
							tp->setTextureScaling(s,s);
							geom->setTextureProperty(tp);
						}
					}

				}
			}
		}
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
}

void CQDlgShapes::on_qqClearTextures_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
		{
			CShape* shape=App::ct->objCont->getShape(App::ct->objCont->getSelID(i));
			if (shape!=NULL)
			{
				std::vector<CGeometric*> components;
				shape->geomData->geomInfo->getAllShapeComponentsCumulative(components);
				for (int j=0;j<int(components.size());j++)
				{
					CTextureProperty* tp=components[j]->getTextureProperty();
					if (tp!=NULL)
					{
						App::ct->textureCont->announceGeneralObjectWillBeErased(shape->getID(),-1);
						delete tp;
						components[j]->setTextureProperty(NULL);
					}
					components[j]->textureCoords_notCopiedNorSerialized.clear(); // discard existing texture coordinates
				}
			}
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
}

void CQDlgShapes::on_qqEdgesAngle_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqEdgesAngle->text().toFloat(&ok);
		if (ok)
		{
			((CGeometric*)shape->geomData->geomInfo)->setEdgeThresholdAngle(gv::userToRad*newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapes::on_qqInsideOutDisplay_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionASimpleShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		((CGeometric*)it->geomData->geomInfo)->setDisplayInverted(!((CGeometric*)it->geomData->geomInfo)->getDisplayInverted());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}
