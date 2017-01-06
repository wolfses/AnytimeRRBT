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

#include "SceneNode.h"

#include "Translate.h"
#include "Scale.h"
#include "Rotate.h"
#include "Matrix.h"


SceneNode::SceneNode()
{

}

SceneNode::SceneNode(const SceneNode& scnode)
{
	m_MeshID = scnode.m_MeshID;
	m_MaterialBindings = scnode.m_MaterialBindings;
	m_Name = scnode.m_Name;
	m_Children = scnode.m_Children;

	for(std::vector<Transformation*>::const_iterator it = scnode.m_Transformations.begin(); it!= scnode.m_Transformations.end(); it++) {
		Transformation *trans = *it;
		m_Transformations.push_back(trans->clone());
	}
}

SceneNode::~SceneNode()
{
	for(std::vector<Transformation*>::iterator it = m_Transformations.begin(); it!= m_Transformations.end(); it++) {
		delete (*it);
	}
	m_Transformations.clear();
}

const std::string& SceneNode::getMeshID() const
{
	return m_MeshID;
}

void SceneNode::setMeshID(const std::string& MeshID) 
{
	m_MeshID = MeshID;
}

const std::string& SceneNode::getName() const
{
	return m_Name;
}

void SceneNode::setName(const std::string& Name)
{
	m_Name = Name;
}

bool SceneNode::hasName() const
{
	return m_Name.length()>0;
}

const std::vector<Transformation*>& SceneNode::getTransformations() const
{
	return m_Transformations;
}

bool SceneNode::containsMesh() const
{
	return m_MeshID.length() > 0;
}

void SceneNode::addTransformation(const Transformation& Trans)
{
	switch(Trans.getTransformationType()) {
	case Transformation::Translate:
		m_Transformations.push_back(new Translate((Translate&)Trans));
		break;
	case Transformation::Scale:
		m_Transformations.push_back(new Scale((Scale&)Trans));
		break;
	case Transformation::Rotate:
		m_Transformations.push_back(new Rotate((Rotate&)Trans));
		break;
	case Transformation::Matrix:
		m_Transformations.push_back(new Matrix((Matrix&)Trans));
		break;
	}
}

const std::vector<std::pair<std::string,std::string> >& SceneNode::getMaterialBindings() const
{
	return m_MaterialBindings;
}

std::vector<std::pair<std::string,std::string> >& SceneNode::getMaterialBindings()
{
	return m_MaterialBindings;
}

const std::vector<SceneNode>& SceneNode::getChildren() const
{
	return m_Children;
}

std::vector<SceneNode>& SceneNode::getChildren()
{
	return m_Children;
}
