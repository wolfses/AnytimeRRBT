// This file is part of the COLLADA PLUGIN for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The COLLADA PLUGIN is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The COLLADA PLUGIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE COLLADA PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
//  
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the COLLADA PLUGIN.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include "colladadialog.h"
#include "ui_colladadialog.h"
#include "v_repLib.h"
#include <boost/format.hpp>
#include <vector>
#include <math.h>
#include "4X4Matrix.h"
#include "3Vector.h"
#include <boost/lexical_cast.hpp>
#include <algorithm>

std::vector<int> shapesAlreadyInExporter;
COLLADAExporter* animationExporter=NULL;
bool triggerAnimationExportPass=false;
int animationExportSceneCount=0;
std::vector<std::pair<std::string,std::string> > materialBindings;


void setVrepObjectName(int objectHandle,const char* desiredName)
{
	std::string baseName(desiredName);
	for (int i=0;i<int(baseName.size());i++)
	{ // Objects in V-REP can only contain a-z, A-Z, 0-9, '_' or exaclty one '#' optionally followed by a number
		char n=baseName[i];
		if ( ((n<'a')||(n>'z')) && ((n<'A')||(n>'Z')) && ((n<'0')||(n>'9')) )
			baseName[i]='_';
	}
	std::string objName(baseName);
	int suffix=2;
	while (simSetObjectName(objectHandle,objName.c_str())==-1)
		objName=baseName+boost::lexical_cast<std::string>(suffix++);
}

CColladaDialog::CColladaDialog(QWidget *parent) :
	QDialog(parent,Qt::Tool),
    ui(new Ui::CColladaDialog)
{
    ui->setupUi(this);
	_importScaling=1.0f;
	_exportScaling=1.0f;
	_mergeImported=false;
	_ungroupImported=false;
	_importMeshesOnly=false;
	_exportAnimation=false;
	_exportShapesOnly=true;
	refresh();
}

CColladaDialog::~CColladaDialog()
{
    delete ui;
}

void CColladaDialog::refresh()
{
	ui->qqMergeImport->setEnabled(_simulationStopped);
	ui->qqImportMeshesOnly->setEnabled(_simulationStopped&&(!_mergeImported));
	ui->qqImportUngroup->setEnabled(_simulationStopped&&(!_mergeImported));
	ui->qqImportScalingFactor->setEnabled(_simulationStopped);
	ui->qqImport->setEnabled(_simulationStopped);
	ui->qqExportAnimation->setEnabled(_simulationStopped);
	ui->qqExportShapesOnly->setEnabled(_simulationStopped);
	ui->qqExportScalingFactor->setEnabled(_simulationStopped);
	ui->qqExport->setEnabled(_simulationStopped&&(!_exportAnimation));

	ui->qqMergeImport->setChecked(_mergeImported);
	ui->qqImportMeshesOnly->setChecked(_importMeshesOnly);
	ui->qqImportUngroup->setChecked(_ungroupImported);
	std::string tmp(boost::str(boost::format("%#00.3f") % _importScaling));
	ui->qqImportScalingFactor->setText(tmp.c_str());
	ui->qqExportAnimation->setChecked(_exportAnimation);
	ui->qqExportShapesOnly->setChecked(_exportShapesOnly);
	tmp=boost::str(boost::format("%#00.3f") % _exportScaling);
	ui->qqExportScalingFactor->setText(tmp.c_str());
}

void CColladaDialog::makeVisible(bool visible)
{ // Called from the UI thread
	setVisible(visible);

	// Reflect the visibility state in the menu bar item:
	SSimulationThreadCommand cmd;
	cmd.cmdId=MAKE_VISIBLE_CMD;
	cmd.boolParams.push_back(visible);
	addCommand(cmd);
}

bool CColladaDialog::getVisible()
{ // Called from the UI thread
	return(isVisible());
}


void CColladaDialog::reject()
{ // Called from the UI thread
	// Reflect the visibility state in the menu bar item:
	SSimulationThreadCommand cmd;
	cmd.cmdId=MAKE_VISIBLE_CMD;
	cmd.boolParams.push_back(false);
	addCommand(cmd);

	done(0);
}


void CColladaDialog::on_qqMergeImport_clicked()
{
	_mergeImported=!_mergeImported;
	if (_mergeImported)
	{
		_importMeshesOnly=true;
		_ungroupImported=false;
	}
	refresh();
}

void CColladaDialog::on_qqImportMeshesOnly_clicked()
{
	_importMeshesOnly=!_importMeshesOnly;
	refresh();
}

void CColladaDialog::on_qqImportUngroup_clicked()
{
	_ungroupImported=!_ungroupImported;
	refresh();
}

void CColladaDialog::on_qqImportScalingFactor_editingFinished()
{
	float newVal;
	bool ok;
	newVal=ui->qqImportScalingFactor->text().toFloat(&ok);
	if (ok)
	{
		if (newVal>1000.0f)
			newVal=1000.0f;
		if (newVal<0.001f)
			newVal=0.001f;
		_importScaling=newVal;
	}
	refresh();
}

void CColladaDialog::setSimulationStopped(bool stopped)
{
	_simulationStopped=stopped;
}

void CColladaDialog::addCommand(SSimulationThreadCommand cmd)
{
	_simulThreadCommands.push_back(cmd);
}

void CColladaDialog::handleCommands()
{ // Called from the main SIM thread
	for (int i=0;i<int(_simulThreadCommands.size());i++)
	{
		SSimulationThreadCommand cmd=_simulThreadCommands[i];
		if (cmd.cmdId==MAKE_VISIBLE_CMD)
			simSetModuleMenuItemState(dialogMenuItemHandle,(cmd.boolParams[0]?3:1),"COLLADA import/export...");
		if (cmd.cmdId==IMPORT_CMD)
		{
			simChar* pathAndFile=simFileDialog(sim_filedlg_type_load,"COLLADA import","","","DAE Files","dae");
			if (pathAndFile!=NULL)
			{
				std::string file(pathAndFile);
				simReleaseBuffer(pathAndFile);
				COLLADAImporter imp;
				printf("Reading and parsing the XML file...\n");
				imp.LoadFile(file.c_str());
				printf("\nFinished reading and parsing the XML file. Now extracting items and building objects in V-REP...\n");
				const VisualScene* scene=NULL;
				for (int i=0;i<int(imp.getVisualScenes().size());i++)
				{
					if (imp.getVisualScenes()[i].getSceneID()==imp.getPrimarySceneID())
					{
						scene=&imp.getVisualScenes()[i];
						break;
					}
				}
				bool loadSuccess=false;
				bool foundErrors=false;
				if (scene!=NULL)
				{
					mat4 m;
					m.IdentityMatrix();
					std::vector<float> allVertices;
					int meshCount=0;
					int nodeCount=0;
					for (int i=0;i<int(scene->getSceneNodes().size());i++)
						meshCount+=getVertices(&imp,m,&scene->getSceneNodes()[i],allVertices,nodeCount);
					C3Vector minV;
					C3Vector maxV;
					for (int i=0;i<int(allVertices.size())/3;i++)
					{
						C3Vector v(&allVertices[3*i+0]);
						v*=_importScaling;
						if (i==0)
						{
							minV=v;
							maxV=v;
						}
						else
						{
							minV.keepMin(v);
							maxV.keepMax(v);
						}
					}
					C3Vector maxSizes(maxV-minV);
					float ww=maxSizes(0);
					if (maxSizes(1)>ww)
						ww=maxSizes(1);
					if (maxSizes(2)>ww)
						ww=maxSizes(2);
					m.IdentityMatrix();
					std::vector<float>* verticesP=NULL;
					std::vector<int>* indicesP=NULL;
					std::vector<float> vertices;
					std::vector<int> indices;
					if (nodeCount>1000)
						_importMeshesOnly=true;
					if (_mergeImported)
					{
						_importMeshesOnly=true;
						verticesP=&vertices;
						indicesP=&indices;
					}
					for (int i=0;i<int(scene->getSceneNodes().size());i++)
						foundErrors|=addNodeToScene(&imp,m,-1,&scene->getSceneNodes()[i],0.1f*ww,_importMeshesOnly,verticesP,indicesP,_importScaling);
					if ((verticesP!=NULL)&&(vertices.size()!=0)&&(indices.size()!=0))
					{ // we have a single shape to add (because otherwise it would be too heavy/too many objects):
						int anObj=simCreateMeshShape(2,20.0f*3.1415f/180.0f,&vertices[0],(int)vertices.size(),&indices[0],(int)indices.size(),NULL);
						setVrepObjectName(anObj,"ColladaImportedShape_merged");
					}
					loadSuccess=true;
				}
				if (!loadSuccess)
				{
					simMsgBox(sim_msgbox_type_warning,sim_msgbox_buttons_ok,"Error","Failed importing the file.");
				}
				else
				{
					if (foundErrors)
						simMsgBox(sim_msgbox_type_warning,sim_msgbox_buttons_ok,"Error","Found errors while importing the file.");
				}
			}
		}
		if (cmd.cmdId==EXPORT_CMD)
		{
			simChar* pathAndFile=simFileDialog(sim_filedlg_type_save,"COLLADA export","","","DAE Files","dae");
			if (pathAndFile!=NULL)
			{
				std::string file(pathAndFile);
				simReleaseBuffer(pathAndFile);

				COLLADAExporter exp;
				VisualScene vs;
				vs.setSceneID("0");
				vs.setName("0");
				exp.setPrimarySceneID("0");

				shapesAlreadyInExporter.clear();
				materialBindings.clear();
				addAllMeshesToExporter(&exp);
				// now write the nodes:

				int sceneVisibilityLayers;
				simGetIntegerParameter(sim_intparam_visible_layers,&sceneVisibilityLayers);
				int index=0;
				while (true)
				{
					int handle;
					if (_exportShapesOnly)
						handle=simGetObjects(index++,sim_object_shape_type);
					else
						handle=simGetObjects(index++,sim_handle_all);
					if (handle<0)
						break;
					int objectVisibilityLayers;
					simGetObjectIntParameter(handle,10,&objectVisibilityLayers);
					if ((simGetObjectParent(handle)==-1)||(_exportShapesOnly&&(sceneVisibilityLayers&objectVisibilityLayers)))
					{
						SceneNode* aNode=addObjectNodeForExport(handle,-1);
						vs.getSceneNodes().push_back(*aNode);
						delete aNode;
					}
				}
				exp.getVisualScenes().push_back(vs);
				exp.SaveFile(file.c_str());
			}
		}
	}
	_simulThreadCommands.clear();
}

void CColladaDialog::on_qqImport_clicked()
{
	SSimulationThreadCommand cmd;
	cmd.cmdId=IMPORT_CMD;
	cmd.boolParams.push_back(false);
	addCommand(cmd);
}

void CColladaDialog::on_qqExportAnimation_clicked()
{
	_exportAnimation=!_exportAnimation;
	refresh();
}

void CColladaDialog::on_qqExportShapesOnly_clicked()
{
	_exportShapesOnly=!_exportShapesOnly;
	refresh();
}

void CColladaDialog::on_qqExportScalingFactor_editingFinished()
{
	float newVal;
	bool ok;
	newVal=ui->qqExportScalingFactor->text().toFloat(&ok);
	if (ok)
	{
		if (newVal>1000.0f)
			newVal=1000.0f;
		if (newVal<0.001f)
			newVal=0.001f;
		_exportScaling=newVal;
	}
	refresh();
}

void CColladaDialog::on_qqExport_clicked()
{
	SSimulationThreadCommand cmd;
	cmd.cmdId=EXPORT_CMD;
	cmd.boolParams.push_back(false);
	addCommand(cmd);
}

void CColladaDialog::simulationAboutToStart()
{
	shapesAlreadyInExporter.clear();
	materialBindings.clear();
	triggerAnimationExportPass=false;
	animationExportSceneCount=0;
	if (_exportAnimation)
	{
		animationExporter=new COLLADAExporter();
		VisualScene vs;
		vs.setSceneID(boost::lexical_cast<std::string>(animationExportSceneCount));
		vs.setName(boost::lexical_cast<std::string>(animationExportSceneCount));
		animationExporter->setPrimarySceneID(boost::lexical_cast<std::string>(animationExportSceneCount));

		addAllMeshesToExporter(animationExporter);
		// now write the nodes:
		int sceneVisibilityLayers;
		simGetIntegerParameter(sim_intparam_visible_layers,&sceneVisibilityLayers);
		int index=0;
		while (true)
		{
			int handle;
			if (_exportShapesOnly)
				handle=simGetObjects(index++,sim_object_shape_type);
			else
				handle=simGetObjects(index++,sim_handle_all);
			if (handle<0)
				break;
			int objectVisibilityLayers;
			simGetObjectIntParameter(handle,10,&objectVisibilityLayers);
			if ((simGetObjectParent(handle)==-1)||(_exportShapesOnly&&(sceneVisibilityLayers&objectVisibilityLayers)))
			{
				SceneNode* aNode=addObjectNodeForExport(handle,-1);
				vs.getSceneNodes().push_back(*aNode);
				delete aNode;
			}
		}
		animationExporter->getVisualScenes().push_back(vs);

		animationExportSceneCount++;
	}
}

void CColladaDialog::mainScriptAboutToBeCalled()
{
	if (animationExporter!=NULL)
		triggerAnimationExportPass=true;
}

void CColladaDialog::renderingPassAboutToBeCalled()
{
	if ((animationExporter!=NULL)&&triggerAnimationExportPass)
	{
		VisualScene vs;
		vs.setSceneID(boost::lexical_cast<std::string>(animationExportSceneCount));
		vs.setName(boost::lexical_cast<std::string>(animationExportSceneCount));

		addAllMeshesToExporter(animationExporter);
		// now write the nodes:
		int sceneVisibilityLayers;
		simGetIntegerParameter(sim_intparam_visible_layers,&sceneVisibilityLayers);
		int index=0;
		while (true)
		{
			int handle;
			if (_exportShapesOnly)
				handle=simGetObjects(index++,sim_object_shape_type);
			else
				handle=simGetObjects(index++,sim_handle_all);
			if (handle<0)
				break;
			int objectVisibilityLayers;
			simGetObjectIntParameter(handle,10,&objectVisibilityLayers);
			if ((simGetObjectParent(handle)==-1)||(_exportShapesOnly&&(sceneVisibilityLayers&objectVisibilityLayers)))
			{
				SceneNode* aNode=addObjectNodeForExport(handle,-1);
				vs.getSceneNodes().push_back(*aNode);
				delete aNode;
			}
		}
		animationExporter->getVisualScenes().push_back(vs);

		animationExportSceneCount++;
		triggerAnimationExportPass=false;
	}
}

void CColladaDialog::simulationEnded()
{
	if (animationExporter!=NULL)
	{
		simChar* pathAndFile=simFileDialog(sim_filedlg_type_save,"COLLADA animation export","","","DAE Files","dae");
		if (pathAndFile!=NULL)
		{
			std::string file(pathAndFile);
			simReleaseBuffer(pathAndFile);
			animationExporter->SaveFile(file.c_str());
		}
		delete animationExporter;
		animationExporter=NULL;
		shapesAlreadyInExporter.clear();
	}
	_exportAnimation=false;
}

int CColladaDialog::getVertices(const COLLADAImporter* importer,const mat4& parentMatrix,const SceneNode* node,std::vector<float>& vertices,int& nodeCount)
{ // returns the mesh count
	int returnValue=0;
	nodeCount++;
	const std::string meshID(node->getMeshID());
	const Mesh* mesh=NULL;
	for (int i=0;i<int(importer->getMeshes().size());i++)
	{
		if (importer->getMeshes()[i].getID()==meshID)
		{
			mesh=&importer->getMeshes()[i];
			break;
		}
	}

	mat4 m=parentMatrix;
	for (int i=0;i<int(node->getTransformations().size());i++)
	{
		if (node->getTransformations()[i]->getTransformationType()==Transformation::Matrix)
		{
			Matrix* tr=(Matrix*)node->getTransformations()[i];
			m=m*tr->getMatrix();
		}
		if (node->getTransformations()[i]->getTransformationType()==Transformation::Translate)
		{
			Translate* tr=(Translate*)node->getTransformations()[i];
			m=m*mat4::TranslationMatrix(tr->getTranslation());
		}
		if (node->getTransformations()[i]->getTransformationType()==Transformation::Rotate)
		{
			Rotate* tr=(Rotate*)node->getTransformations()[i];
			m=m*mat4::RotationMatrix(tr->getRotationAxis(),tr->getRotationAngle());
		}
		if (node->getTransformations()[i]->getTransformationType()==Transformation::Scale)
		{
			Scale* tr=(Scale*)node->getTransformations()[i];
			m=m*mat4::ScaleMatrix(tr->getScale());
		}
	}

	if (mesh!=NULL)
	{
		returnValue++;
		for (int i=0;i<int(mesh->getVertices().size());i++)
		{
			float v[3]={mesh->getVertices()[i].X,mesh->getVertices()[i].Y,mesh->getVertices()[i].Z};
			vertices.push_back(v[0]*m(0,0)+v[1]*m(0,1)+v[2]*m(0,2)+m(0,3));
			vertices.push_back(v[0]*m(1,0)+v[1]*m(1,1)+v[2]*m(1,2)+m(1,3));
			vertices.push_back(v[0]*m(2,0)+v[1]*m(2,1)+v[2]*m(2,2)+m(2,3));
		}
	}
	for (int i=0;i<int(node->getChildren().size());i++)
		returnValue+=getVertices(importer,m,&node->getChildren()[i],vertices,nodeCount);
	return(returnValue);
}


bool CColladaDialog::addNodeToScene(const COLLADAImporter* importer,const mat4& parentMatrix,int parentObjectID,const SceneNode* node,float jointAndDummySizes,bool onlyShapes,std::vector<float>* _allVert,std::vector<int>* _allInd,float importScale)
{
	bool foundErrors=false;
	const std::string meshID(node->getMeshID());
	const Mesh* mesh=NULL;
	for (int i=0;i<int(importer->getMeshes().size());i++)
	{
		if (importer->getMeshes()[i].getID()==meshID)
		{
			mesh=&importer->getMeshes()[i];
			break;
		}
	}
	int theObjectHandle=-1;

	mat4 m=parentMatrix;
	for (int i=0;i<int(node->getTransformations().size());i++)
	{
		if (node->getTransformations()[i]->getTransformationType()==Transformation::Matrix)
		{
			Matrix* tr=(Matrix*)node->getTransformations()[i];
			m=m*tr->getMatrix();
		}
		if (node->getTransformations()[i]->getTransformationType()==Transformation::Translate)
		{
			Translate* tr=(Translate*)node->getTransformations()[i];

			if (!onlyShapes)
			{
				float vmx[4][4];
				for (int j=0;j<4;j++)
					for (int k=0;k<4;k++)
						vmx[j][k]=float(m(j,k));
				C4X4Matrix vm(vmx);

				C3Vector v(tr->getTranslation().X,tr->getTranslation().Y,tr->getTranslation().Z);
				v.normalize();
				float a=C3Vector::unitZVector.getAngle(v);
				C3Vector rotAxis(C3Vector::unitZVector^v);
				rotAxis.normalize();

				C4Vector w(a,rotAxis);
				C4X4Matrix jointFrameLocal(w.getMatrix(),C3Vector::zeroVector);
				vm*=jointFrameLocal;

				float s[2]={jointAndDummySizes,jointAndDummySizes*0.2f};
				int jh=simCreateJoint(sim_joint_prismatic_subtype,sim_jointmode_passive,0,s,NULL,NULL);
				vm.X*=importScale;
				simSetObjectPosition(jh,-1,vm.X.data);
				C3Vector euler(vm.M.getEulerAngles());
				simSetObjectOrientation(jh,-1,euler.data);
				simSetObjectParent(jh,parentObjectID,true);
				setVrepObjectName(jh,"colladaTransformationNode");
				parentObjectID=jh;
			}

			m=m*mat4::TranslationMatrix(tr->getTranslation());
		}
		if (node->getTransformations()[i]->getTransformationType()==Transformation::Rotate)
		{
			Rotate* tr=(Rotate*)node->getTransformations()[i];

			if (!onlyShapes)
			{
				float vmx[4][4];
				for (int j=0;j<4;j++)
					for (int k=0;k<4;k++)
						vmx[j][k]=float(m(j,k));
				C4X4Matrix vm(vmx);

				C3Vector v(tr->getRotationAxis().X,tr->getRotationAxis().Y,tr->getRotationAxis().Z);
				v.normalize();
				float a=C3Vector::unitZVector.getAngle(v);
				C3Vector rotAxis(C3Vector::unitZVector^v);
				rotAxis.normalize();

				C4Vector w(a,rotAxis);
				C4X4Matrix jointFrameLocal(w.getMatrix(),C3Vector::zeroVector);
				vm*=jointFrameLocal;

				float s[2]={jointAndDummySizes,jointAndDummySizes*0.2f};
				int jh=simCreateJoint(sim_joint_revolute_subtype,sim_jointmode_passive,0,s,NULL,NULL);
				vm.X*=importScale;
				simSetObjectPosition(jh,-1,vm.X.data);
				C3Vector euler(vm.M.getEulerAngles());
				simSetObjectOrientation(jh,-1,euler.data);
				simSetObjectParent(jh,parentObjectID,true);
				setVrepObjectName(jh,"colladaTransformationNode");
				parentObjectID=jh;
			}

			m=m*mat4::RotationMatrix(tr->getRotationAxis(),tr->getRotationAngle());
		}
		if (node->getTransformations()[i]->getTransformationType()==Transformation::Scale)
		{
			Scale* tr=(Scale*)node->getTransformations()[i];
			m=m*mat4::ScaleMatrix(tr->getScale());
		}
	}
	if (mesh!=NULL)
	{
		std::vector<float> vertices;
		std::vector<int> indices;
		int off=0;
		if (_allVert!=NULL)
			off=int(_allVert->size())/3;
		for (int i=0;i<int(mesh->getVertices().size());i++)
		{
			float v[3]={mesh->getVertices()[i].X,mesh->getVertices()[i].Y,mesh->getVertices()[i].Z};
			vertices.push_back((v[0]*m(0,0)+v[1]*m(0,1)+v[2]*m(0,2)+m(0,3))*importScale);
			vertices.push_back((v[0]*m(1,0)+v[1]*m(1,1)+v[2]*m(1,2)+m(1,3))*importScale);
			vertices.push_back((v[0]*m(2,0)+v[1]*m(2,1)+v[2]*m(2,2)+m(2,3))*importScale);
			if (_allVert!=NULL)
			{
				_allVert->push_back((v[0]*m(0,0)+v[1]*m(0,1)+v[2]*m(0,2)+m(0,3))*importScale);
				_allVert->push_back((v[0]*m(1,0)+v[1]*m(1,1)+v[2]*m(1,2)+m(1,3))*importScale);
				_allVert->push_back((v[0]*m(2,0)+v[1]*m(2,1)+v[2]*m(2,2)+m(2,3))*importScale);
			}
		}
		std::vector<int> subShapes;
		for (int i=0;i<int(mesh->getTriangleGroups().size());i++)
		{
			indices.clear();
			const TriangleGroup* group=&mesh->getTriangleGroups()[i];
			for (int j=0;j<int(group->getTriangleIndices().size());j++)
			{
				indices.push_back(group->getTriangleIndices()[j]);
				if (_allInd!=NULL)
					_allInd->push_back(group->getTriangleIndices()[j]+off);
			}
			if ((_allInd==NULL)&&(vertices.size()>0)&&(indices.size()>0))
			{
				printf("Building a shape... ");
				int anObj=simCreateMeshShape(2,20.0f*3.1415f/180.0f,&vertices[0],(int)vertices.size(),&indices[0],(int)indices.size(),NULL);
				if (i==0)
					theObjectHandle=anObj;
				else
					subShapes.push_back(anObj);

				boost::unordered_map<std::string,Material> materials=importer->getMaterials();
				std::vector<std::pair<std::string,std::string> > bind=node->getMaterialBindings();
				std::string matNam;
				for (int k=0;k<int(bind.size());k++)
				{
					if (bind[k].first.compare(group->getMaterialSymbol())==0)
					{
						matNam=bind[k].second;
						break;
					}
				}
				if (matNam!="")
				{
					Material& mat=materials[matNam];

					float col[3];
//					col[0]=mat.m_Ambient.X;
//					col[1]=mat.m_Ambient.Y;
//					col[2]=mat.m_Ambient.Z;
//					simSetShapeColor(anObj,NULL,0,col);
					col[0]=mat.m_Diffuse.X;
					col[1]=mat.m_Diffuse.Y;
					col[2]=mat.m_Diffuse.Z;
					simSetShapeColor(anObj,NULL,0,col); // 0 is diffuse/ambient component now!
					col[0]=mat.m_Specular.X;
					col[1]=mat.m_Specular.Y;
					col[2]=mat.m_Specular.Z;
					simSetShapeColor(anObj,NULL,2,col);
					col[0]=mat.m_Emmission.X;
					col[1]=mat.m_Emmission.Y;
					col[2]=mat.m_Emmission.Z;
					simSetShapeColor(anObj,NULL,3,col);
				}
				printf("done!\n");
			}
		}
		for (int i=0;i<int(mesh->getPolygonGroups().size());i++)
		{
			indices.clear();
			const PolygonGroup* group=&mesh->getPolygonGroups()[i];
			for (int j=0;j<int(group->getPolygons().size());j++)
			{
				const PPolygon* poly=&group->getPolygons()[j];
				if (poly->getVertexIndices().size()>2)
				{
					int zeroI=poly->getVertexIndices()[0];
					for (int k=0;k<int(poly->getVertexIndices().size())-2;k++)
					{
						indices.push_back(zeroI);
						indices.push_back(poly->getVertexIndices()[k+1]);
						indices.push_back(poly->getVertexIndices()[k+2]);
						if (_allInd!=NULL)
						{
							_allInd->push_back(zeroI+off);
							_allInd->push_back(poly->getVertexIndices()[k+1]+off);
							_allInd->push_back(poly->getVertexIndices()[k+2]+off);
						}
					}
				}
			}

			if ((_allInd==NULL)&&(vertices.size()>0)&&(indices.size()>0))
			{
				printf("Building a shape... ");
				int anObj=simCreateMeshShape(2,20.0f*3.1415f/180.0f,&vertices[0],(int)vertices.size(),&indices[0],(int)indices.size(),NULL);
				if (i==0)
					theObjectHandle=anObj;
				else
					subShapes.push_back(anObj);


				boost::unordered_map<std::string,Material> materials=importer->getMaterials();
				std::vector<std::pair<std::string,std::string> > bind=node->getMaterialBindings();
				std::string matNam;
				for (int k=0;k<int(bind.size());k++)
				{
					if (bind[k].first.compare(group->getMaterialSymbol())==0)
					{
						matNam=bind[k].second;
						break;
					}
				}
				if (matNam!="")
				{
					Material& mat=materials[matNam];

					float col[3];
//					col[0]=mat.m_Ambient.X;
//					col[1]=mat.m_Ambient.Y;
//					col[2]=mat.m_Ambient.Z;
//					simSetShapeColor(anObj,NULL,0,col);
					col[0]=mat.m_Diffuse.X;
					col[1]=mat.m_Diffuse.Y;
					col[2]=mat.m_Diffuse.Z;
					simSetShapeColor(anObj,NULL,0,col); // 0 is diffuse/ambient component now!
					col[0]=mat.m_Specular.X;
					col[1]=mat.m_Specular.Y;
					col[2]=mat.m_Specular.Z;
					simSetShapeColor(anObj,NULL,2,col);
					col[0]=mat.m_Emmission.X;
					col[1]=mat.m_Emmission.Y;
					col[2]=mat.m_Emmission.Z;
					simSetShapeColor(anObj,NULL,3,col);
				}
				printf("done!\n");
			}
		}

		if (_allInd==NULL)
		{
			if (subShapes.size()!=0)
			{
				if (subShapes.size()==1)
					theObjectHandle=subShapes[0];
				else
				{
					if (!_ungroupImported)
						theObjectHandle=simGroupShapes(&subShapes[0],int(subShapes.size()));
				}
			}
			if (theObjectHandle!=-1)
			{
				simSetObjectParent(theObjectHandle,parentObjectID,true);
				setVrepObjectName(theObjectHandle,node->getName().c_str());
			}
		}
	}

	if (theObjectHandle==-1)
	{ // we use a dummy instead
		if (!onlyShapes)
		{
			theObjectHandle=simCreateDummy(jointAndDummySizes*0.25f,NULL);
			float vmx[4][4];
			for (int j=0;j<4;j++)
				for (int k=0;k<4;k++)
					vmx[j][k]=float(m(j,k));
			C4X4Matrix vm(vmx);
			vm.X*=importScale;
			simSetObjectPosition(theObjectHandle,-1,vm.X.data);
			C3Vector euler(vm.M.getEulerAngles());
			simSetObjectOrientation(theObjectHandle,-1,euler.data);
			simSetObjectParent(theObjectHandle,parentObjectID,true);
			setVrepObjectName(theObjectHandle,node->getName().c_str());
		}
	}
	for (int i=0;i<int(node->getChildren().size());i++)
		foundErrors|=addNodeToScene(importer,m,theObjectHandle,&node->getChildren()[i],jointAndDummySizes,onlyShapes,_allVert,_allInd,importScale);
	return(foundErrors);
}

SceneNode* CColladaDialog::addObjectNodeForExport(int objectID,int parentObjectID)
{
	SceneNode* node=new SceneNode();
	float tr[12];
	simGetObjectMatrix(objectID,parentObjectID,tr);
	tr[3]*=_exportScaling;
	tr[7]*=_exportScaling;
	tr[11]*=_exportScaling;
	char* name=simGetObjectName(objectID);
	node->setName(name);

	int sceneVisibilityLayers;
	simGetIntegerParameter(sim_intparam_visible_layers,&sceneVisibilityLayers);
	int objectVisibilityLayers;
	simGetObjectIntParameter(objectID,10,&objectVisibilityLayers);
	if ((simGetObjectType(objectID)==sim_object_shape_type)&&(sceneVisibilityLayers&objectVisibilityLayers))
		node->setMeshID(boost::lexical_cast<std::string>(objectID));
	simReleaseBuffer(name);
	mat4 mm;
	for (int i=0;i<3;i++)
		for (int j=0;j<4;j++)
			mm(i,j)=tr[i*4+j];
	node->addTransformation(Matrix(mm));
	node->getMaterialBindings().assign(materialBindings.begin(),materialBindings.end());
	if (!_exportShapesOnly)
	{
		std::vector<int> objectsToExplore;
		objectsToExplore.push_back(objectID);
		while (objectsToExplore.size()>0)
		{
			int obj=objectsToExplore[0];
			objectsToExplore.erase(objectsToExplore.begin());
			if (obj!=objectID)
			{
				SceneNode* child=addObjectNodeForExport(obj,objectID);
				node->getChildren().push_back(*child);
				delete child;
			}
			else
			{
				int index=0;
				while (true)
				{
					int childID=simGetObjectChild(obj,index++);
					if (childID<0)
						break;
					objectsToExplore.push_back(childID);
				}
			}
		}
	}
	return(node);
}

void CColladaDialog::addAllMeshesToExporter(COLLADAExporter* exp)
{
	int index=0;
	int sceneVisibilityLayers;
	simGetIntegerParameter(sim_intparam_visible_layers,&sceneVisibilityLayers);
	while (true)
	{
		int handle=simGetObjects(index++,sim_object_shape_type);
		if (handle<0)
			break;
		int objectVisibilityLayers;
		simGetObjectIntParameter(handle,10,&objectVisibilityLayers);
		if (sceneVisibilityLayers&objectVisibilityLayers)
		{
			if (std::find(shapesAlreadyInExporter.begin(),shapesAlreadyInExporter.end(),handle)==shapesAlreadyInExporter.end())
			{ // That shape isn't yet in the exporter!
				shapesAlreadyInExporter.push_back(handle);
				std::vector<int> subShapes;
				// Save the selection state and clear selection:
				int selectionSize=simGetObjectSelectionSize();
				int* selectedObjects=new int[selectionSize];
				simGetObjectSelection(selectedObjects);
				simRemoveObjectFromSelection(sim_handle_all,-1);
				// is the shape a grouped shape?
				int res;
				simGetObjectIntParameter(handle,3016,&res);
				if (res!=0)
				{ // Yes! we need to decompose that shape
					// 1. We copy the shape:
					int copyHandle=handle;
					simCopyPasteObjects(&copyHandle,1,0);
/*					simAddObjectToSelection(sim_handle_single,handle);
					int initState=simGetBooleanParameter(sim_boolparam_full_model_copy_from_api);
					simSetBooleanParameter(sim_boolparam_full_model_copy_from_api,0);
					simCopyPasteSelectedObjects();
					simSetBooleanParameter(sim_boolparam_full_model_copy_from_api,initState);
					int copyHandle=simGetObjectLastSelection();
					*/
					// 2. decompose the shape until we have only simple shapes left:
					int shapeHandlesSize;
					int* shapeHandles=simUngroupShape(copyHandle,&shapeHandlesSize);
					for (int i=0;i<shapeHandlesSize;i++)
						subShapes.push_back(shapeHandles[i]);
					simReleaseBuffer((char*)shapeHandles);
				}
				else
					subShapes.push_back(handle);

				// Now add the shape (or shapes in case we had a grouped shape) to the exporter:
				std::vector<float> allVertices;
				Mesh m;
				char* name=simGetObjectName(handle);
				m.setName(name);
				simReleaseBuffer(name);
				m.setID(boost::lexical_cast<std::string>(handle));
				for (int shapeIndex=0;shapeIndex<int(subShapes.size());shapeIndex++)
				{
					float* vert;
					int vertSize;
					int* ind;
					int indSize;
					float* norm;
					int theSubShapeHandle=subShapes[shapeIndex];
					simGetShapeMesh(theSubShapeHandle,&vert,&vertSize,&ind,&indSize,&norm);

					C7Vector tr;
					simGetObjectPosition(theSubShapeHandle,handle,tr.X.data);
					tr.X*=_exportScaling;
					float q[4];
					simGetObjectQuaternion(theSubShapeHandle,handle,q);
					tr.Q(0)=q[3];
					tr.Q(1)=q[0];
					tr.Q(2)=q[1];
					tr.Q(3)=q[2];

					int off=int(allVertices.size())/3;
					for (int i=0;i<vertSize/3;i++)
					{
						C3Vector v(vert+3*i);
						v*=tr; // make sure the mesh is expressed relative to the original shape
						v*=_exportScaling;
						allVertices.push_back(v(0));
						allVertices.push_back(v(1));
						allVertices.push_back(v(2));
					}

					TriangleGroup t;
					for (int i=0;i<indSize;i++)
						t.getTriangleIndices().push_back(ind[i]+off);

					for (int i=0;i<indSize;i++)
					{
						C3Vector v(norm+3*i);
						v=tr.Q*v; // make sure the normals are expressed relative to the original shape (we discard the translation here!)
						vec3 w(v(0),v(1),v(2));
						t.getNormals().push_back(w);
					}
					for (int i=0;i<indSize;i++)
						t.getNormalIndices().push_back(i);

					simReleaseBuffer((char*)vert);
					simReleaseBuffer((char*)ind);
					simReleaseBuffer((char*)norm);

					t.setMaterialSymbol(boost::lexical_cast<std::string>(theSubShapeHandle));
					m.getTriangleGroups().push_back(t);
					Material mat;
					float data[3];
					simGetShapeColor(theSubShapeHandle,NULL,0,data);
					mat.m_Ambient.set(data[0],data[1],data[2]); // 0 is diffuse/ambient component now!
					simGetShapeColor(theSubShapeHandle,NULL,0,data);
					mat.m_Diffuse.set(data[0],data[1],data[2]); // 0 is diffuse/ambient component now!
					simGetShapeColor(theSubShapeHandle,NULL,2,data);
					mat.m_Specular.set(data[0],data[1],data[2]);
					simGetShapeColor(theSubShapeHandle,NULL,3,data);
					mat.m_Emmission.set(data[0],data[1],data[2]);
					std::pair<std::string,Material> a;
					a.first=boost::lexical_cast<std::string>(theSubShapeHandle);
					a.second=mat;
					exp->getMaterials().push_back(a);
					std::pair<std::string,std::string> bind;
					bind.first=boost::lexical_cast<std::string>(theSubShapeHandle);
					bind.second=boost::lexical_cast<std::string>(theSubShapeHandle);
					materialBindings.push_back(bind);
				}

				for (int i=0;i<int(allVertices.size())/3;i++)
				{
					C3Vector v(&allVertices[0]+3*i);
					vec3 w(v(0),v(1),v(2));
					m.getVertices().push_back(w);
				}
				exp->getMeshes().push_back(m);

				// Remove any shape that was duplicated (during the copy and ungroup operation):
				if (subShapes.size()>1)
				{
					for (int i=0;i<int(subShapes.size());i++)
						simRemoveObject(subShapes[i]);
				}

				// Restore the selection state:
				simRemoveObjectFromSelection(sim_handle_all,-1);
				for (int i=0;i<selectionSize;i++)
					simAddObjectToSelection(sim_handle_single,selectedObjects[i]);
				delete[] selectedObjects;
			}
		}
	}
}

int CColladaDialog::importSingleGroupedShape(const char* pathAndFile,bool preserveColors,float scaling)
{
	int retVal=-1;
	if (pathAndFile!=NULL)
	{
		std::string file(pathAndFile);
		COLLADAImporter imp;
		printf("Reading and parsing the XML file...\n");
		imp.LoadFile(file.c_str());
		printf("\nFinished reading and parsing the XML file. Now extracting items and building a single shape in V-REP...\n");
		const VisualScene* scene=NULL;
		for (int i=0;i<int(imp.getVisualScenes().size());i++)
		{
			if (imp.getVisualScenes()[i].getSceneID()==imp.getPrimarySceneID())
			{
				scene=&imp.getVisualScenes()[i];
				break;
			}
		}
		bool loadSuccess=false;
		bool foundErrors=false;
		if (scene!=NULL)
		{
			mat4 m;
			m.IdentityMatrix();
			std::vector<float> allVertices;
			int meshCount=0;
			int nodeCount=0;
			for (int i=0;i<int(scene->getSceneNodes().size());i++)
				meshCount+=getVertices(&imp,m,&scene->getSceneNodes()[i],allVertices,nodeCount);
			C3Vector minV;
			C3Vector maxV;
			for (int i=0;i<int(allVertices.size())/3;i++)
			{
				C3Vector v(&allVertices[3*i+0]);
				v*=scaling;
				if (i==0)
				{
					minV=v;
					maxV=v;
				}
				else
				{
					minV.keepMin(v);
					maxV.keepMax(v);
				}
			}
			C3Vector maxSizes(maxV-minV);
			float ww=0.1f; // anything will do here, all objects except shapes will be destroyed afterwards anyway!
			m.IdentityMatrix();
			std::vector<float>* verticesP=NULL;
			std::vector<int>* indicesP=NULL;
			std::vector<float> vertices;
			std::vector<int> indices;
			_importMeshesOnly=true;
			if (!preserveColors)
			{
				verticesP=&vertices;
				indicesP=&indices;
			}

			// Memorize initial objects:
			std::vector<int> initialObjects;
			int index=0;
			while (true)
			{
				int handle;
				handle=simGetObjects(index++,sim_handle_all);
				if (handle<0)
					break;
				initialObjects.push_back(handle);
			}

			// Import the new object(s):
			for (int i=0;i<int(scene->getSceneNodes().size());i++)
				foundErrors|=addNodeToScene(&imp,m,-1,&scene->getSceneNodes()[i],0.1f*ww,_importMeshesOnly,verticesP,indicesP,scaling);
			if ((verticesP!=NULL)&&(vertices.size()!=0)&&(indices.size()!=0))
			{ // we have a single shape to add
				int anObj=simCreateMeshShape(2,20.0f*3.1415f/180.0f,&vertices[0],(int)vertices.size(),&indices[0],(int)indices.size(),NULL);
				setVrepObjectName(anObj,"ColladaImportedShape_merged");
			}

			// Now group all shapes that were not present in the scene previously, and erase all other objects (normaly none):
			std::vector<int> shapesToGroup;
			std::vector<int> objectToRemove;
			index=0;
			while (true)
			{
				int handle;
				handle=simGetObjects(index++,sim_handle_all);
				if (handle<0)
					break;
				if (std::find(initialObjects.begin(),initialObjects.end(),handle)==initialObjects.end())
				{ // this object didn't exist before!
					if (simGetObjectType(handle)==sim_object_shape_type)
						shapesToGroup.push_back(handle);
					else
						objectToRemove.push_back(handle);
				}
			}
			for (int i=0;i<int(objectToRemove.size());i++)
				simRemoveObject(objectToRemove[i]);

			if (shapesToGroup.size()!=0)
			{
				if (shapesToGroup.size()==1)
					retVal=shapesToGroup[0]; // there was anyway only one imported shape!
				else
					retVal=simGroupShapes(&shapesToGroup[0],int(shapesToGroup.size()));
				loadSuccess=true;
			}
		}
		if (!loadSuccess)
			printf("Failed importing the file.\n");
		else
		{
			if (foundErrors)
				printf("Found errors while importing the file.\n");
		}
	}
	return(retVal);
}
