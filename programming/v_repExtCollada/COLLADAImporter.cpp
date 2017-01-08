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

#include "COLLADAImporter.h"
#include "XMLHelper.h"
#include "StringHelper.h"
#include <algorithm>
#include "tinyxml2.h"
#include <iostream>
#include <boost/unordered_map.hpp>
#include "Source.h"
#include "Mesh.h"
#include "mat4.h"

using namespace tinyxml2;

Matrix COLLADAImporter::LoadXMLMatrix(tinyxml2::XMLElement* matrix)
{
	XMLText* text = matrix->FirstChild()->ToText();
	if(text == NULL)
		return mat4::IdentityMatrix();
	std::vector<float> elements = StringHelper::ConvertStringToTArray<float>(text->Value());
	if(elements.size() != 16)
		return mat4::IdentityMatrix();
	mat4 mat;
	mat(0,0) = elements[0];
	mat(0,1) = elements[1];
	mat(0,2) = elements[2];
	mat(0,3) = elements[3];
	mat(1,0) = elements[4];
	mat(1,1) = elements[5];
	mat(1,2) = elements[6];
	mat(1,3) = elements[7];
	mat(2,0) = elements[8];
	mat(2,1) = elements[9];
	mat(2,2) = elements[10];
	mat(2,3) = elements[11];
	mat(3,0) = elements[12];
	mat(3,1) = elements[13];
	mat(3,2) = elements[14];
	mat(3,3) = elements[15];
	return Matrix(mat);
}
Translate COLLADAImporter::LoadXMLTranslation(tinyxml2::XMLElement* translation)
{
	XMLText* text = translation->FirstChild()->ToText();
	if(text == NULL)
		return Translate();
	std::vector<float> elements = StringHelper::ConvertStringToTArray<float>(text->Value());
	if(elements.size() != 3)
		return Translate();
	vec3 t(elements[0],elements[1],elements[2]);
	return Translate(t);
}
Rotate COLLADAImporter::LoadXMLRotation(tinyxml2::XMLElement* rotation)
{
	XMLText* text = rotation->FirstChild()->ToText();
	if(text == NULL)
		return Rotate();
	std::vector<float> elements = StringHelper::ConvertStringToTArray<float>(text->Value());
	if(elements.size() != 4)
		return Rotate();
	vec3 axis(elements[0],elements[1],elements[2]);	
	return Rotate(axis,elements[3]);
}
Scale COLLADAImporter::LoadXMLScale(tinyxml2::XMLElement* scale)
{
	XMLText* text = scale->FirstChild()->ToText();
	if(text == NULL)
		return Scale();
	std::vector<float> elements = StringHelper::ConvertStringToTArray<float>(text->Value());
	if(elements.size() != 3)
		return Scale();
	vec3 t(elements[0],elements[1],elements[2]);
	return Scale(t);
}

void COLLADAImporter::LoadFloatArrays(XMLElement *geometryNode, boost::unordered_map<std::string, std::vector<float> >& map)
{	 
	std::vector<XMLElement *> geometries = XMLHelper::GetChildElements(geometryNode,"geometry");
	std::vector<XMLElement *> meshes;
	for(std::vector<XMLElement *>::const_iterator it = geometries.begin(); it != geometries.end(); it++) {
		XMLElement *mesh;
		if((mesh = XMLHelper::GetChildElement(*it,"mesh")) != NULL) {
			meshes.push_back(mesh);
		}
	}

	std::string id;
	for(std::vector<XMLElement *>::const_iterator mit = meshes.begin(); mit != meshes.end(); mit++){
		std::vector<XMLElement *> sources = XMLHelper::GetChildElements(*mit,"source");		
		for(std::vector<XMLElement *>::const_iterator it = sources.begin(); it != sources.end(); it++) {
			XMLElement *float_array = XMLHelper::GetChildElement(*it,"float_array");
			if(float_array != NULL) {
				if(XMLHelper::GetElementAttribute(float_array,"id",id)) {
					XMLText *innerText = float_array->FirstChild()->ToText();
					std::string farray(innerText->Value());
					std::vector<float> fvalues;
					//fvalues = StringHelper::ConvertStringToFloatArray(farray);
					fvalues = StringHelper::ConvertStringToTArray<float>(farray);
				
					int arraycount;
					if(!StringHelper::from_string<int>(arraycount,float_array->Attribute("count"))) {
						continue;					
					}

					while(int(fvalues.size()) < arraycount) {
						fvalues.push_back(0.0f);
					}

					map[id] = fvalues;
				}
			}
		}
	}
}

void COLLADAImporter::visitNode(XMLElement* node,SceneNode* parentNode,std::vector<SceneNode>& rootScene)
{	
	XMLElement* child = node->FirstChildElement();
	SceneNode scnode;
	std::string nodeName;
	if(XMLHelper::GetElementAttribute(node,"name",nodeName))
		scnode.setName(nodeName);

	while(child != NULL) {
		std::string name = child->Name();
		if(name == "matrix")
			scnode.addTransformation(LoadXMLMatrix(child));
		else if(name == "translate")
			scnode.addTransformation(LoadXMLTranslation(child));
		else if(name == "rotate")
			scnode.addTransformation(LoadXMLRotation(child));
		else if(name == "scale")
			scnode.addTransformation(LoadXMLScale(child));
		else if(name == "instance_geometry") {
			std::string meshid;
			if(XMLHelper::GetElementAttribute(child,"url",meshid)) {				
				meshid = meshid.substr(1);	// remove leading "#"
				scnode.setMeshID(meshid);				
				// load binding materials
				XMLElement* bindmaterial = XMLHelper::GetChildElement(child,"bind_material");
				if(bindmaterial != NULL) {
					XMLElement* techniqueCommon = XMLHelper::GetChildElement(bindmaterial,"technique_common");
					if(techniqueCommon != NULL) {
						XMLElement *instanceMaterial = techniqueCommon->FirstChildElement();
						while(instanceMaterial != NULL) {
							if(std::string(instanceMaterial->Name()) == "instance_material") {
								std::string symbol;
								std::string target;
								bool b1 = XMLHelper::GetElementAttribute(instanceMaterial,"symbol",symbol);
								bool b2 = XMLHelper::GetElementAttribute(instanceMaterial,"target",target);
								if(b1&&b2) {
									target = target.substr(1);	// remove leading "#"
									if(LoadMaterial(node->GetDocument()->FirstChildElement(),target)) {
										std::pair<std::string,std::string> pair(symbol,target);
										scnode.getMaterialBindings().push_back(pair);
									}
								}
							}
							instanceMaterial = instanceMaterial->NextSiblingElement();
						}
					}
				}				
			}
		}
		else if(name == "node") {						
			visitNode(child,&scnode,rootScene);			
		}
		else if(name == "instance_node") {
			std::string id;
			if(XMLHelper::GetElementAttribute(child,"url",id)) {
				id = id.substr(1);	//remove leading "#"
				XMLElement *node2visit = XMLHelper::GetChildElement(node->GetDocument()->FirstChildElement(),"id",id);
				if(node2visit != NULL) {					
					visitNode(node2visit,&scnode,rootScene);					
				}
			}
		}
		child = child->NextSiblingElement();
	}
	if(parentNode != NULL)
		parentNode->getChildren().push_back(scnode);
	else
		rootScene.push_back(scnode);

}
void COLLADAImporter::LoadScene(XMLElement* scene)
{
	XMLElement* node = scene->FirstChildElement();
	VisualScene vs;
	std::string sceneID;
	if(XMLHelper::GetElementAttribute(scene,"id",sceneID))
		vs.setSceneID(sceneID);
	std::string sceneName;
	if(XMLHelper::GetElementAttribute(scene,"name",sceneName))
		vs.setName(sceneName);
	
	m_VisualScenes.push_back(vs);
	while(node != NULL) {

		if(std::string(node->Name()) == "node") {					
			visitNode(node,NULL,m_VisualScenes[m_VisualScenes.size()-1].getSceneNodes());
		}
		node = node->NextSiblingElement();
	}
	
}
bool COLLADAImporter::LoadMaterial(XMLElement* colladaRootNode,const std::string& id)
{
	if(m_Materials.find(id) != m_Materials.end())			// material already loaded
		return true;

	XMLElement *materials = XMLHelper::GetChildElement(colladaRootNode,"library_materials");
	if(materials == NULL)
		return false;
	XMLElement *material = XMLHelper::GetChildElement(materials,"material","id",id);
	if(material == NULL)
		return false;
	XMLElement *effect = XMLHelper::GetChildElement(material,"instance_effect");
	if(effect == NULL)
		return false;
	std::string effectid;
	if(!XMLHelper::GetElementAttribute(effect,"url",effectid))
		return false;
	effectid = effectid.substr(1);		// remove leading "#"

	XMLElement *effects = XMLHelper::GetChildElement(colladaRootNode,"library_effects");
	if(effects == NULL)
		return false;
	effect = XMLHelper::GetChildElement(effects,"id",effectid);
	if(effect == NULL)
		return false;
	XMLElement *profile = XMLHelper::GetChildElement(effect,"profile_COMMON");
	if(profile == NULL)
		return false;
	XMLElement *technique = XMLHelper::GetChildElement(profile,"technique");
	if(technique == NULL)
		return false;

	XMLElement *bp = XMLHelper::GetChildElement(technique,"blinn");
	if(bp == NULL) {
		bp = XMLHelper::GetChildElement(technique,"phong");
		if(bp == NULL) {
			bp = XMLHelper::GetChildElement(technique,"lambert");
			if(bp == NULL)
				return false;
		}
			
	}

	Material mat;
	// ambient
	XMLElement *ambient = XMLHelper::GetChildElement(bp,"ambient");
	if(ambient != NULL) {	
		XMLElement *color = XMLHelper::GetChildElement(ambient,"color");
		if(color != NULL) {
			XMLText *value = color->FirstChild()->ToText();
			if(value != NULL) {		
				std::vector<float> c = StringHelper::ConvertStringToTArray<float>(value->Value());
				if(c.size() >= 3 )			
					mat.m_Ambient = vec3(c[0],c[1],c[2]);
			}
		}
	}

	//diffuse
	XMLElement *diffuse = XMLHelper::GetChildElement(bp,"diffuse");
	if(diffuse != NULL) {		
		XMLElement* color = XMLHelper::GetChildElement(diffuse,"color");
		if(color != NULL) {		
			XMLText* value = color->FirstChild()->ToText();
			if(value != NULL) {				
				std::vector<float> c = StringHelper::ConvertStringToTArray<float>(value->Value());
				if(c.size() >= 3 )					
					mat.m_Diffuse = vec3(c[0],c[1],c[2]);
			}
		}
	}

	//specular
	XMLElement *specular = XMLHelper::GetChildElement(bp,"specular");
	if(specular != NULL) {		
		XMLElement* color = XMLHelper::GetChildElement(specular,"color");
		if(color != NULL) {		
			XMLText* value = color->FirstChild()->ToText();
			if(value != NULL) {
				std::vector<float> c = StringHelper::ConvertStringToTArray<float>(value->Value());
				if(c.size() >= 3 )		
					mat.m_Specular = vec3(c[0],c[1],c[2]);
			}
		}
	}

	//emission
	XMLElement *emission = XMLHelper::GetChildElement(bp,"emission");
	if(emission != NULL) {
		XMLElement* color = XMLHelper::GetChildElement(emission,"color");
		if(color != NULL) {			
			XMLText* value = color->FirstChild()->ToText();
			if(value != NULL) {			
				std::vector<float> c = StringHelper::ConvertStringToTArray<float>(value->Value());
				if(c.size() >= 3 )
					mat.m_Emmission = vec3(c[0],c[1],c[2]);
			}
		}
	}
	m_Materials[id] = mat;
	return true;
}
bool COLLADAImporter::LoadGeometry(XMLElement* geometryNode,boost::unordered_map<std::string, std::vector<float> >& floatarrays)
{
	boost::unordered_map<std::string,Source> sources;

	XMLElement *mesh = XMLHelper::GetChildElement(geometryNode,"mesh");
	if(mesh == NULL)
		mesh = XMLHelper::GetChildElement(geometryNode,"convex_mesh");
	if(mesh == NULL)
		return false;

	// load all source elements under <mesh>
	std::vector<XMLElement*> xmlsources = XMLHelper::GetChildElements(mesh,"source");
	for(std::vector<XMLElement*>::iterator it = xmlsources.begin();it != xmlsources.end(); it++) {
		Source s;
		if(s.parseXML(*it,floatarrays))
			sources[s.getID()] = s;
		else
			return false;
	}

	XMLElement* xmlvertices = XMLHelper::GetChildElement(mesh,"vertices");
	if(xmlvertices == NULL)
		return false;
	if(xmlvertices->Attribute("id") == NULL)
		return false;

	// get <vertrices> id
	std::string verticesid = xmlvertices->Attribute("id");
	// get <vertices>, <input> with semantic="POSITION"
	XMLElement *positionInput = XMLHelper::GetChildElement(xmlvertices,"input","semantic","POSITION");
	std::string positionid;
	if(!XMLHelper::GetElementAttribute(positionInput,"source",positionid))
		return false;
	
	positionid = positionid.substr(1);	// remove leading "#"

	
	Mesh mmesh;
	std::string geometryid;
	if(XMLHelper::GetElementAttribute(geometryNode,"id",geometryid))
		mmesh.setID(geometryid);

	std::string geometryName;
	if(XMLHelper::GetElementAttribute(geometryNode,"name",geometryName))
		mmesh.setName(geometryName);

	Source& vertices = sources[positionid];
	int ucount = vertices.getUnitCount();
	if(ucount < 3)
		return false;
	int vcount = vertices.getArrayValues().size() / ucount;
	for(int i=0;i<vcount;i++) {
		int idx = ucount*i;
		vec3 v(vertices.getArrayValues()[idx],vertices.getArrayValues()[idx+1],vertices.getArrayValues()[idx+2]);
		mmesh.getVertices().push_back(v);
	}

	// get <triangles> nodes
	std::vector<XMLElement*> xmltriangles = XMLHelper::GetChildElements(mesh,"triangles");
	for(std::vector<XMLElement*>::iterator it = xmltriangles.begin();it!=xmltriangles.end();it++){
		XMLElement * triangles = *it;					
		if(triangles->Attribute("count") != NULL) {
			// get triangle count
			int tricount=0;
			if(!StringHelper::from_string<int>(tricount,std::string(triangles->Attribute("count"))))
				continue;

			std::string materialsymbol;
			if(!XMLHelper::GetElementAttribute(triangles,"material",materialsymbol))
				materialsymbol = "";

			std::string temp;
			int vertices_offset;
			// get vertices <input>
			XMLElement* verticesInput = XMLHelper::GetChildElement(triangles,"input","semantic","VERTEX");						
			if(!XMLHelper::GetElementAttribute(verticesInput,"source",temp))
				return false;
			temp = temp.substr(1);
			if(temp != verticesid)		// verify source is the same as vertices id loaded above in current mesh
				return false;
			if(!XMLHelper::GetElementAttribute(verticesInput,"offset",temp))		// get vertices offset in <p> element
				return false;
			if(!StringHelper::from_string(vertices_offset,temp))
				return false;

			std::string normalid;
			int normal_offset;						
			bool hasNormals;

			// get normals <input>
			XMLElement* normalInput = XMLHelper::GetChildElement(triangles,"input","semantic","NORMAL");
			if(!XMLHelper::GetElementAttribute(normalInput,"source",normalid))
				hasNormals = false;
			else {
				normalid = normalid.substr(1);
				hasNormals = true;
			}
											
			if(hasNormals && !XMLHelper::GetElementAttribute(normalInput,"offset",temp))	// get offset for normals in <p> element
				return false;
			else {
				if(!StringHelper::from_string<int>(normal_offset,temp))
					return false;
			}

			XMLElement *pelement = XMLHelper::GetChildElement(triangles,"p");
			if(pelement == NULL)
				return false;

			XMLText *innerText = pelement->FirstChild()->ToText();
			if(innerText == NULL)
				return false;
			std::string tarray(innerText->Value());
			std::vector<int> IndexArray = StringHelper::ConvertStringToTArray<int>(tarray);
							
			int indexcount = IndexArray.size();
			if((indexcount % (3*tricount)) != 0)
				return false;
			int indexstride = indexcount/(3*tricount);																				

			TriangleGroup trigroup;
			trigroup.setMaterialSymbol(materialsymbol);

			// TODO: add check for positionid & normalid			

			if(hasNormals) {
				Source& vertices = sources[normalid];
				ucount = vertices.getUnitCount();
				if(ucount < 3)
					return false;
				int vcount = vertices.getArrayValues().size() / ucount;
				for(int i=0;i<vcount;i++) {
					int idx = ucount*i;
					vec3 v(vertices.getArrayValues()[idx],vertices.getArrayValues()[idx+1],vertices.getArrayValues()[idx+2]);
					trigroup.getNormals().push_back(v);
				}
			}

			for(int i=0;i<tricount*3;i++) {
				int vpos = i*indexstride + vertices_offset;
				trigroup.getTriangleIndices().push_back(IndexArray[vpos]);
				if(hasNormals) {
					vpos = i*indexstride + normal_offset;
					trigroup.getNormalIndices().push_back(IndexArray[vpos]);
				}
			}
			mmesh.getTriangleGroups().push_back(trigroup);			
		}
		else
			continue;
	}
	// get <polygons> nodes

	std::vector<XMLElement*> xmlpolygons = XMLHelper::GetChildElements(mesh,"polygons");
	for(std::vector<XMLElement*>::iterator it = xmlpolygons.begin(); it != xmlpolygons.end(); it++) {
		XMLElement * polygons = *it;	
		if(polygons->Attribute("count") != NULL) {
			// get polygon count
			int polycount=0;
			if(!StringHelper::from_string<int>(polycount,std::string(polygons->Attribute("count"))))
				continue;

			std::string materialsymbol;
			if(!XMLHelper::GetElementAttribute(polygons,"material",materialsymbol))
				materialsymbol = "";

			std::string temp;
			int vertices_offset;
			// get vertices <input>
			XMLElement* verticesInput = XMLHelper::GetChildElement(polygons,"input","semantic","VERTEX");						
			if(!XMLHelper::GetElementAttribute(verticesInput,"source",temp))
				return false;
			temp = temp.substr(1);
			if(temp != verticesid)		// verify source is the same as vertices id loaded above in current mesh
				return false;
			if(!XMLHelper::GetElementAttribute(verticesInput,"offset",temp))		// get vertices offset in <p> element
				return false;
			if(!StringHelper::from_string(vertices_offset,temp))
				return false;

			std::string normalid;
			int normal_offset;						
			bool hasNormals;

			// get normals <input>
			XMLElement* normalInput = XMLHelper::GetChildElement(polygons,"input","semantic","NORMAL");
			if(!XMLHelper::GetElementAttribute(normalInput,"source",normalid))
				hasNormals = false;
			else {
				normalid = normalid.substr(1);
				hasNormals = true;
			}
											
			if(hasNormals && !XMLHelper::GetElementAttribute(normalInput,"offset",temp))	// get offset for normals in <p> element
				return false;
			else {
				if(!StringHelper::from_string<int>(normal_offset,temp))
					return false;
			}

			int stridepervertex = 0;
			
			std::vector<XMLElement*> inputs = XMLHelper::GetChildElements(polygons,"input");
			
			for(std::vector<XMLElement*>::const_iterator it = inputs.begin(); it != inputs.end(); it++) {				
				if(!XMLHelper::GetElementAttribute(*it,"offset",temp))
					return false;
				int offset=0;
				if(!StringHelper::from_string<int>(offset,temp))
					return false;				
				stridepervertex = std::max(stridepervertex,offset);
			}
			stridepervertex ++;

			PolygonGroup polygroup;
			polygroup.setMaterialSymbol(materialsymbol);

			
			if(hasNormals) {
				Source& vertices = sources[normalid];
				ucount = vertices.getUnitCount();
				if(ucount < 3)				// normals have at least 3 components (X,Y,Z)
					return false;
				int vcount = vertices.getArrayValues().size() / ucount;
				for(int i=0;i<vcount;i++) {
					int idx = ucount*i;
					vec3 v(vertices.getArrayValues()[idx],vertices.getArrayValues()[idx+1],vertices.getArrayValues()[idx+2]);
					polygroup.getNormals().push_back(v);
				}
			}

			std::vector<XMLElement *> pelements = XMLHelper::GetChildElements(polygons,"p");
			for(std::vector<XMLElement*>::iterator it = pelements.begin(); it != pelements.end(); it++) {
				XMLElement *pelement = *it;
				XMLText *innerText = pelement->FirstChild()->ToText();
				if(innerText == NULL)
					return false;
				std::string tarray(innerText->Value());
				std::vector<int> IndexArray = StringHelper::ConvertStringToTArray<int>(tarray);
				if((IndexArray.size() % stridepervertex)  != 0)
					continue;

				int currentIndexOffset = 0;
				int vertexCount = IndexArray.size() / stridepervertex;
				PPolygon poly;
				for(int i=0;i<vertexCount;i++) {
					poly.getVertexIndices().push_back(IndexArray[currentIndexOffset+vertices_offset]);
					if(hasNormals) {
						poly.getNormalIndices().push_back(IndexArray[currentIndexOffset+normal_offset]);
					}
					currentIndexOffset += stridepervertex;
				}
				polygroup.getPolygons().push_back(poly);
			}
			mmesh.getPolygonGroups().push_back(polygroup);
		}
		else
			continue;
	}

	// get <polylist> nodes
	std::vector<XMLElement*> xmlpolylist = XMLHelper::GetChildElements(mesh,"polylist");
	for(std::vector<XMLElement*>::iterator it = xmlpolylist.begin();it!=xmlpolylist.end();it++){
		XMLElement * polylist = *it;					
		if(polylist->Attribute("count") != NULL) {
			// get polygon count
			int polycount=0;
			if(!StringHelper::from_string<int>(polycount,std::string(polylist->Attribute("count"))))
				continue;

			std::string materialsymbol;
			if(!XMLHelper::GetElementAttribute(polylist,"material",materialsymbol))
				materialsymbol = "";

			std::string temp;
			int vertices_offset;
			// get vertices <input>
			XMLElement* verticesInput = XMLHelper::GetChildElement(polylist,"input","semantic","VERTEX");						
			if(!XMLHelper::GetElementAttribute(verticesInput,"source",temp))
				return false;
			temp = temp.substr(1);
			if(temp != verticesid)		// verify source is the same as vertices id loaded above in current mesh
				return false;
			if(!XMLHelper::GetElementAttribute(verticesInput,"offset",temp))		// get vertices offset in <p> element
				return false;
			if(!StringHelper::from_string(vertices_offset,temp))
				return false;

			std::string normalid;
			int normal_offset;						
			bool hasNormals;

			// get normals <input>
			XMLElement* normalInput = XMLHelper::GetChildElement(polylist,"input","semantic","NORMAL");
			if(!XMLHelper::GetElementAttribute(normalInput,"source",normalid))
				hasNormals = false;
			else {
				normalid = normalid.substr(1);
				hasNormals = true;
			}
											
			if(hasNormals && !XMLHelper::GetElementAttribute(normalInput,"offset",temp))	// get offset for normals in <p> element
				return false;
			else {
				if(!StringHelper::from_string<int>(normal_offset,temp))
					return false;
			}

			XMLElement *vcountelement = XMLHelper::GetChildElement(polylist,"vcount");
			if(vcountelement == NULL)
				return false;
			XMLText *vcountinnerText = vcountelement->FirstChild()->ToText();
			if(vcountinnerText == NULL)
				return false;
			
			std::string vcarray(vcountinnerText->Value());
			std::vector<int> vCountArray = StringHelper::ConvertStringToTArray<int>(vcarray);
			
			if(int(vCountArray.size()) != polycount)
				return false;

			int totalVertices = 0;
			for(std::vector<int>::const_iterator it = vCountArray.begin(); it != vCountArray.end(); it++) {
				totalVertices += *it;
			}

			XMLElement *pelement = XMLHelper::GetChildElement(polylist,"p");
			if(pelement == NULL)
				return false;

			XMLText *innerText = pelement->FirstChild()->ToText();
			if(innerText == NULL)
				return false;
			std::string tarray(innerText->Value());
			std::vector<int> IndexArray = StringHelper::ConvertStringToTArray<int>(tarray);
							
			int indexcount = IndexArray.size();
			if((indexcount % totalVertices) != 0)
				return false;																				

			PolygonGroup polygroup;
			polygroup.setMaterialSymbol(materialsymbol);
			
			// TODO: add check for positionid & normalid			

			if(hasNormals) {
				Source& vertices = sources[normalid];
				ucount = vertices.getUnitCount();
				if(ucount < 3)				// normals have at least 3 components (X,Y,Z)
					return false;
				int vcount = vertices.getArrayValues().size() / ucount;
				for(int i=0;i<vcount;i++) {
					int idx = ucount*i;
					vec3 v(vertices.getArrayValues()[idx],vertices.getArrayValues()[idx+1],vertices.getArrayValues()[idx+2]);
					polygroup.getNormals().push_back(v);
				}
			}

			int stridepervertex = 0;
			
			std::vector<XMLElement*> inputs = XMLHelper::GetChildElements(polylist,"input");
			
			for(std::vector<XMLElement*>::const_iterator it = inputs.begin(); it != inputs.end(); it++) {				
				if(!XMLHelper::GetElementAttribute(*it,"offset",temp))
					return false;
				int offset=0;
				if(!StringHelper::from_string<int>(offset,temp))
					return false;				
				stridepervertex = std::max(stridepervertex,offset);
			}
			stridepervertex ++;

			if(int(IndexArray.size()) != totalVertices*stridepervertex)
				return false;

			int currentIndexOffset = 0;
			for(int i=0;i<polycount;i++) {
				PPolygon poly;
				for(int j=0;j<vCountArray[i];j++) {
					poly.getVertexIndices().push_back(IndexArray[currentIndexOffset+vertices_offset]);
					if(hasNormals) {
						poly.getNormalIndices().push_back(IndexArray[currentIndexOffset+normal_offset]);
					}
					currentIndexOffset += stridepervertex;
				}
				polygroup.getPolygons().push_back(poly);
			}
			mmesh.getPolygonGroups().push_back(polygroup);
		}
		else
			continue;
	}

	if((mmesh.getTriangleGroups().size()>0) || (mmesh.getPolygonGroups().size() > 0)) {
		m_Meshes.push_back(mmesh);
		return true;
	}
	else
		return false;	
}

bool COLLADAImporter::LoadFile(const std::string& filename)
{
	XMLDocument doc;
	doc.LoadFile(filename.c_str());

	XMLElement* root = doc.FirstChildElement();	
	if((root != NULL) && (std::string(root->Name()) == "COLLADA")) {
		
		m_Meshes.clear();
		m_Materials.clear();

		XMLElement* geometries = XMLHelper::GetChildElement(root,"library_geometries");
		if(geometries == NULL)
			return false;

		boost::unordered_map<std::string, std::vector<float> > floatmap;
		LoadFloatArrays(geometries,floatmap);

		XMLElement* geometry = geometries->FirstChildElement();
		while(geometry != NULL) {				
			if(std::string(geometry->Name()) == "geometry")
			{
				LoadGeometry(geometry,floatmap);
			}
			geometry =  geometry->NextSiblingElement();
		}

		XMLElement* visualscenes = XMLHelper::GetChildElement(root,"library_visual_scenes");
		if(visualscenes == NULL)
			return true;

		XMLElement* scene = visualscenes->FirstChildElement();
		while(scene != NULL) {
			if(std::string(scene->Name()) == "visual_scene") {
				LoadScene(scene);
			}
			scene = scene->NextSiblingElement();
		}

		scene = XMLHelper::GetChildElement(root,"scene");
		if(scene != NULL) {
			XMLElement *instance_vs = XMLHelper::GetChildElement(scene,"instance_visual_scene");
			if(instance_vs != NULL) {
				std::string sceneID;
				if(XMLHelper::GetElementAttribute(instance_vs,"url",sceneID))
					m_PrimarySceneID = sceneID.substr(1);		// remove leading "#"
			}
		}

		return true;
	}
	else
		return false;

}

const std::vector<Mesh>& COLLADAImporter::getMeshes() const
{
	return m_Meshes;
}

const boost::unordered_map<std::string,Material>& COLLADAImporter::getMaterials() const
{
	return m_Materials;
}

const std::vector<VisualScene>& COLLADAImporter::getVisualScenes() const
{
	return m_VisualScenes;
}

const std::string& COLLADAImporter::getPrimarySceneID() const
{
	return m_PrimarySceneID;
}
