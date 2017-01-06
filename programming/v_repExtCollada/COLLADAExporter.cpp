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

// Written by Alex Doumanoglou on behalf of Dr. Marc Freese

#include "COLLADAExporter.h"
#include "StringHelper.h"

#include "Transformation.h"
#include "Translate.h"
#include "Scale.h"
#include "Rotate.h"
#include "Matrix.h"

using namespace tinyxml2;

void COLLADAExporter::SaveSceneNode(XMLElement* parentNode,const SceneNode &scene)
{
	XMLDocument *doc = parentNode->GetDocument();
	XMLElement *currentNode = doc->NewElement("node");
	std::string nodeid = "node-" + StringHelper::to_string(m_NextNodeID++);
	currentNode->SetAttribute("id",nodeid.c_str());
	if(scene.hasName())
		currentNode->SetAttribute("name",scene.getName().c_str());

	parentNode->InsertEndChild(currentNode);
	
	/*
	XMLElement *matrixNode = doc->NewElement("matrix");
	currentNode->InsertFirstChild(matrixNode);

	XMLText * txtmatrix = doc->NewText(StringHelper::ConvertMat4ToString(scene.getTransfomrationMatrix()).c_str());
	matrixNode->InsertFirstChild(txtmatrix);
	*/


	for(std::vector<Transformation*>::const_iterator it = scene.getTransformations().begin(); it != scene.getTransformations().end(); it++) {
		Transformation * trans = *it;

		XMLElement *transNode = NULL;
		XMLText *txtTrans = NULL;

		switch(trans->getTransformationType()) {
		case Transformation::Translate:
			transNode = doc->NewElement("translate");
			currentNode->InsertEndChild(transNode);
			txtTrans = doc->NewText(StringHelper::ConvertVec3ToString(((Translate*)trans)->getTranslation()).c_str());
			transNode->InsertFirstChild(txtTrans);
			break;
		case Transformation::Scale:
			transNode = doc->NewElement("scale");
			currentNode->InsertEndChild(transNode);
			txtTrans = doc->NewText(StringHelper::ConvertVec3ToString(((Scale*)trans)->getScale()).c_str());
			transNode->InsertFirstChild(txtTrans);
			break;
		case Transformation::Rotate:
			transNode = doc->NewElement("rotate");
			currentNode->InsertEndChild(transNode);
			txtTrans = doc->NewText((StringHelper::ConvertVec3ToString(((Rotate*)trans)->getRotationAxis()) + " " + StringHelper::to_string(((Rotate*)trans)->getRotationAngle())).c_str());
			transNode->InsertFirstChild(txtTrans);
			break;
		case Transformation::Matrix:
			transNode = doc->NewElement("matrix");
			currentNode->InsertEndChild(transNode);
			txtTrans = doc->NewText(StringHelper::ConvertMat4ToString(((Matrix*)trans)->getMatrix()).c_str());
			transNode->InsertFirstChild(txtTrans);
		}
	}
	if(scene.containsMesh()) {
		XMLElement *instGeom = doc->NewElement("instance_geometry");
		instGeom->SetAttribute("url",("#"+scene.getMeshID()).c_str());
		currentNode->InsertEndChild(instGeom);

		if(scene.getMaterialBindings().size()>0) {
			XMLElement *bindmat = doc->NewElement("bind_material");
			instGeom->InsertEndChild(bindmat);
			XMLElement *techniqueCommon = doc->NewElement("technique_common");
			bindmat->InsertFirstChild(techniqueCommon);
			for(std::vector<std::pair<std::string,std::string> >::const_iterator it = scene.getMaterialBindings().begin(); it != scene.getMaterialBindings().end(); it++) {
				XMLElement *instmat = doc->NewElement("instance_material");
				instmat->SetAttribute("symbol",it->first.c_str());
				instmat->SetAttribute("target",("#"+it->second).c_str());
				techniqueCommon->InsertEndChild(instmat);
			}
		}
	}
	for(std::vector<SceneNode>::const_iterator it = scene.getChildren().begin(); it != scene.getChildren().end(); it++) {
		SaveSceneNode(currentNode,*it);
	}
}
void COLLADAExporter::SaveSceneNodes(XMLElement* rootNode)
{
	XMLDocument *doc = rootNode->GetDocument();
	XMLElement *libvisualscenes = doc->NewElement("library_visual_scenes");
	rootNode->InsertEndChild(libvisualscenes);
	int sceneid = 1;
	for(std::vector<VisualScene>::const_iterator it = m_VisualScenes.begin(); it != m_VisualScenes.end(); it++) {
		XMLElement *visualscene = doc->NewElement("visual_scene");
		std::string sceneID = it->getSceneID();
		if(sceneID != "")
			visualscene->SetAttribute("id",sceneID.c_str());
		else
			visualscene->SetAttribute("id",("VisualScene-"+StringHelper::to_string(sceneid++)).c_str());
		if(it->hasName())
			visualscene->SetAttribute("name",it->getName().c_str());
		libvisualscenes->InsertEndChild(visualscene);
		for(std::vector<SceneNode>::const_iterator it2 = it->getSceneNodes().begin();it2 != it->getSceneNodes().end(); it2++) {
			SaveSceneNode(visualscene,*it2);
		}
	}
}
void COLLADAExporter::SaveSource(tinyxml2::XMLElement* parentNode,const std::vector<vec3>& values,const std::string& sourceid, const std::string& sourcename)
{
	XMLDocument *doc = parentNode->GetDocument();
	XMLElement *sourceNode = doc->NewElement("source");
	sourceNode->SetAttribute("id",sourceid.c_str());
	sourceNode->SetAttribute("name",sourcename.c_str());
	parentNode->InsertEndChild(sourceNode);
	XMLElement* floatArray = doc->NewElement("float_array");
	floatArray->SetAttribute("id",(sourceid+"-array").c_str());
	floatArray->SetAttribute("count",int(values.size()*3));
	sourceNode->InsertFirstChild(floatArray);
	XMLText *txtArray = doc->NewText(StringHelper::ConvertVec3ArrayToString(values).c_str());	
	floatArray->InsertFirstChild(txtArray);
	XMLElement *techniqueCommon = doc->NewElement("technique_common");
	sourceNode->InsertEndChild(techniqueCommon);
	XMLElement* accessor = doc->NewElement("accessor");
	accessor->SetAttribute("source",("#"+sourceid+"-array").c_str());
	accessor->SetAttribute("count",int(values.size()));
	accessor->SetAttribute("stride",3);
	techniqueCommon->InsertFirstChild(accessor);
	XMLElement *paramX = doc->NewElement("param");
	XMLElement *paramY = doc->NewElement("param");
	XMLElement *paramZ = doc->NewElement("param");
	paramX->SetAttribute("name","X");
	paramX->SetAttribute("type","float");
	paramY->SetAttribute("name","Y");
	paramY->SetAttribute("type","float");
	paramZ->SetAttribute("name","Z");
	paramZ->SetAttribute("type","float");
	accessor->InsertEndChild(paramX);
	accessor->InsertEndChild(paramY);
	accessor->InsertEndChild(paramZ);
}
bool COLLADAExporter::SaveGeometry(tinyxml2::XMLElement* geometriesNode,const Mesh& mesh)
{
	XMLDocument *doc = geometriesNode->GetDocument();
	XMLElement* geometry = doc->NewElement("geometry");
	geometry->SetAttribute("id",mesh.getID().c_str());
	if(mesh.hasName())
		geometry->SetAttribute("name",mesh.getName().c_str());

	geometriesNode->InsertEndChild(geometry);
	XMLElement *meshNode = geometriesNode->GetDocument()->NewElement("mesh");
	geometry->InsertFirstChild(meshNode);

	int posSourceID = m_NextPosSourceID++;
	int normalSourceID = m_NextNormalSourceID;
	std::string geometryID = mesh.getID();
	if(geometryID == "")
		geometryID = "Geometry"+StringHelper::to_string(m_NextGeometryID++);

	std::string possourceid = geometryID+"-positions-"+StringHelper::to_string(posSourceID);
	SaveSource(meshNode,mesh.getVertices(),possourceid,"position");

	for(std::vector<TriangleGroup>::const_iterator it = mesh.getTriangleGroups().begin(); it != mesh.getTriangleGroups().end(); it++){
		if((*it).getNormals().size()>0 && ((*it).getNormalIndices().size() == (*it).getTriangleIndices().size())) {
			std::string sourceid = geometryID+"-normals-"+StringHelper::to_string(normalSourceID++);
			SaveSource(meshNode,(*it).getNormals(),sourceid,"normal");
		}
	}

	XMLElement *verticesNode = doc->NewElement("vertices");
	verticesNode->SetAttribute("id",(geometryID+"-vertices").c_str());
	meshNode->InsertEndChild(verticesNode);
	XMLElement *input = doc->NewElement("input");
	input->SetAttribute("semantic","POSITION");
	input->SetAttribute("source",("#"+possourceid).c_str());
	verticesNode->InsertFirstChild(input);

	for(std::vector<TriangleGroup>::const_iterator it = mesh.getTriangleGroups().begin(); it != mesh.getTriangleGroups().end(); it++){
		if((*it).getTriangleIndices().size()>0) {
			XMLElement *trianglesNode = doc->NewElement("triangles");
			trianglesNode->SetAttribute("count",int((*it).getTriangleIndices().size())/3);
			if((*it).getMaterialSymbol() != "")
				trianglesNode->SetAttribute("material",(*it).getMaterialSymbol().c_str());
			meshNode->InsertEndChild(trianglesNode);
			input = doc->NewElement("input");
			input->SetAttribute("offset",0);
			input->SetAttribute("semantic","VERTEX");
			input->SetAttribute("source",("#"+geometryID+"-vertices").c_str());
			trianglesNode->InsertEndChild(input);
			bool hasNormals = false;
			if(((*it).getNormals().size()>0) && ((*it).getNormalIndices().size() == (*it).getTriangleIndices().size()))
				hasNormals = true;

			if(hasNormals) {
				input = doc->NewElement("input");
				input->SetAttribute("offset",1);
				input->SetAttribute("semantic","NORMAL");
				input->SetAttribute("source",("#"+geometryID+"-normals-"+StringHelper::to_string(m_NextNormalSourceID++)).c_str());
				trianglesNode->InsertEndChild(input);
			}

			XMLElement *pElement = doc->NewElement("p");
			trianglesNode->InsertEndChild(pElement);
			std::string pstring;

			for(int i=0;i<int(it->getTriangleIndices().size());i++) {
				if(i>0)
					pstring+=" ";
				pstring += StringHelper::to_string((*it).getTriangleIndices()[i]);
				if(hasNormals) {
					pstring += " " + StringHelper::to_string((*it).getNormalIndices()[i]);
				}
			}
			XMLText *txt = doc->NewText(pstring.c_str());
			pElement->InsertFirstChild(txt);
		}
	}

	return true;
}
void COLLADAExporter::SaveMaterials(XMLElement* rootNode)
{
	XMLDocument *doc = rootNode->GetDocument();
	XMLElement *libmaterials = doc->NewElement("library_materials");
	rootNode->InsertFirstChild(libmaterials);
	for(std::vector<std::pair<std::string,Material> >::iterator it = m_Materials.begin();it != m_Materials.end(); it++) {
		XMLElement *material = doc->NewElement("material");
		material->SetAttribute("id",it->first.c_str());
		material->SetAttribute("name",it->first.c_str());
		libmaterials->InsertEndChild(material);
		XMLElement *insteffect = doc->NewElement("instance_effect");
		insteffect->SetAttribute("url",("#"+it->first+"-fx").c_str());
		material->InsertFirstChild(insteffect);
	}

	XMLElement *libeffects = doc->NewElement("library_effects");
	rootNode->InsertFirstChild(libeffects);
	for(std::vector<std::pair<std::string,Material> >::iterator it = m_Materials.begin();it != m_Materials.end(); it++) {
		XMLElement *effect = doc->NewElement("effect");
		effect->SetAttribute("id",(it->first+"-fx").c_str());
		libeffects->InsertEndChild(effect);
		XMLElement *profCommon = doc->NewElement("profile_COMMON");
		effect->InsertFirstChild(profCommon);
		XMLElement *technique = doc->NewElement("technique");
		technique->SetAttribute("sid","common");
		profCommon->InsertFirstChild(technique);
		XMLElement *phong = doc->NewElement("phong");
		technique->InsertFirstChild(phong);

		XMLElement* ambient = doc->NewElement("ambient");
		phong->InsertEndChild(ambient);
		XMLElement* color = doc->NewElement("color");
		ambient->InsertFirstChild(color);
		XMLText* txtColor = doc->NewText((StringHelper::ConvertVec3ToString(it->second.m_Ambient)+" 1").c_str());		// append 1 for alpha value
		color->InsertFirstChild(txtColor);

		XMLElement* diffuse = doc->NewElement("diffuse");
		phong->InsertEndChild(diffuse);
		color = doc->NewElement("color");
		diffuse->InsertFirstChild(color);
		txtColor = doc->NewText((StringHelper::ConvertVec3ToString(it->second.m_Diffuse)+" 1").c_str());				// apppend 1 for alpha value
		color->InsertFirstChild(txtColor);

		XMLElement* specular = doc->NewElement("specular");
		phong->InsertEndChild(specular);
		color = doc->NewElement("color");
		specular->InsertFirstChild(color);
		txtColor = doc->NewText((StringHelper::ConvertVec3ToString(it->second.m_Specular)+" 1").c_str());				// apppend 1 for alpha value
		color->InsertFirstChild(txtColor);

		XMLElement* emission = doc->NewElement("emission");
		phong->InsertEndChild(emission);
		color = doc->NewElement("color");
		emission->InsertFirstChild(color);
		txtColor = doc->NewText((StringHelper::ConvertVec3ToString(it->second.m_Emmission)+" 1").c_str());				// apppend 1 for alpha value
		color->InsertFirstChild(txtColor);
	}

}
bool COLLADAExporter::SaveFile(const std::string& filename)
{	
	m_NextNodeID = m_NextGeometryID = m_NextPosSourceID = m_NextNormalSourceID = 1;

	XMLDocument doc;
	XMLElement* root = doc.NewElement("COLLADA");
	root->SetAttribute("xmlns","http://www.collada.org/2005/11/COLLADASchema");
	root->SetAttribute("version","1.4.1");
	doc.InsertFirstChild(root);

	XMLElement *libgeometries = doc.NewElement("library_geometries");
	root->InsertEndChild(libgeometries);
	for(std::vector<Mesh>::const_iterator it = m_Meshes.begin();it != m_Meshes.end(); it++) {
		SaveGeometry(libgeometries,*it);
	}

	SaveMaterials(root);
	SaveSceneNodes(root);

	XMLElement *scene = doc.NewElement("scene");
	root->InsertEndChild(scene);
	XMLElement *instance_vs = doc.NewElement("instance_visual_scene");
	scene->InsertFirstChild(instance_vs);
	instance_vs->SetAttribute("url",("#"+m_PrimarySceneID).c_str());

	// save file
	FILE *outfile;
	outfile = fopen(filename.c_str(),"w");
	if(outfile == NULL)
		return false;
	fprintf(outfile,"%s","<?xml version=\"1.0\"?>\n");
	doc.SaveFile(outfile);
	fclose(outfile);
	return true;
}

const std::vector<Mesh>& COLLADAExporter::getMeshes() const
{
	return m_Meshes;
}
std::vector<Mesh>& COLLADAExporter::getMeshes()
{
	return m_Meshes;
}

const std::vector<std::pair<std::string,Material> >& COLLADAExporter::getMaterials() const
{
	return m_Materials;
}

std::vector<std::pair<std::string,Material> >& COLLADAExporter::getMaterials()
{
	return m_Materials;
}

const std::vector<VisualScene>& COLLADAExporter::getVisualScenes() const
{
	return m_VisualScenes;
}
std::vector<VisualScene>& COLLADAExporter::getVisualScenes()
{
	return m_VisualScenes;
}

const std::string& COLLADAExporter::getPrimarySceneID() const
{
	return m_PrimarySceneID;
}

void COLLADAExporter::setPrimarySceneID(const std::string& PrimarySceneID) 
{
	m_PrimarySceneID = PrimarySceneID;
}
