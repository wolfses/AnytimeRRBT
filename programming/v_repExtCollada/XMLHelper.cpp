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

#include "XMLHelper.h"

using namespace tinyxml2;

XMLElement *XMLHelper::GetChildElement(tinyxml2::XMLElement* rootNode,const std::string& childName,bool SearchAnyDepth)
{
	if(rootNode == NULL) {
		return NULL;
	}

	XMLElement* node = rootNode->FirstChildElement();

	while(node != NULL) {				
		if(std::string(node->Name()) == childName) {
			return node;
		}
		
		if((node->FirstChildElement() != NULL) && SearchAnyDepth) {
			XMLElement* result = GetChildElement(node,childName,SearchAnyDepth);
			if(result != NULL)
				return result;
		}
		node = node->NextSiblingElement();
	}
	return NULL;
}

XMLElement *XMLHelper::GetChildElement(tinyxml2::XMLElement* rootNode,const std::string& childName)
{
	return GetChildElement(rootNode,childName,false);
}


XMLElement *XMLHelper::GetChildElement(tinyxml2::XMLElement* rootNode, const std::string& attributeName,const std::string& attributeValue)
{
	if(rootNode == NULL) {
		return NULL;
	}

	XMLElement* node = rootNode->FirstChildElement();

	while(node != NULL) {	
		if(node->Attribute(attributeName.c_str()) != NULL) {
			if(std::string(node->Attribute(attributeName.c_str())) == attributeValue) {
				return node;
			}
		}
		
		if(node->FirstChildElement() != NULL) {
			XMLElement* result = GetChildElement(node,attributeName,attributeValue);
			if(result != NULL)
				return result;
		}
		node = node->NextSiblingElement();
	}
	return NULL;
}

XMLElement *XMLHelper::GetChildElement(tinyxml2::XMLElement* rootNode, const std::string& childName,const std::string& attributeName,const std::string& attributeValue)
{
	if(rootNode == NULL) {
		return NULL;
	}

	XMLElement* node = rootNode->FirstChildElement();

	while(node != NULL) {	
		if((std::string(node->Name()) == childName) && (node->Attribute(attributeName.c_str()) != NULL)) {
			if(std::string(node->Attribute(attributeName.c_str())) == attributeValue) {
				return node;
			}
		}
		
		if(node->FirstChildElement() != NULL) {
			XMLElement* result = GetChildElement(node,childName,attributeName,attributeValue);
			if(result != NULL)
				return result;
		}
		node = node->NextSiblingElement();
	}
	return NULL;
}

std::vector<tinyxml2::XMLElement*> XMLHelper::GetChildElements(tinyxml2::XMLElement* rootNode,const std::string& childName)
{
	std::vector<XMLElement*> elements;

	if(rootNode == NULL) {
		return elements;
	}

	XMLElement* node = rootNode->FirstChildElement();

	while(node != NULL) {				
		if(std::string(node->Name()) == childName) {
			elements.push_back(node);
		}
		node = node->NextSiblingElement();
	}
	return elements;
}

bool XMLHelper::GetElementAttribute(tinyxml2::XMLElement* node,const std::string attributeName,std::string& attributeValue)
{
	if(node == NULL)
		return false;
	if(node->Attribute(attributeName.c_str()) != NULL) {
		attributeValue = std::string(node->Attribute(attributeName.c_str()));
		return true;
	}
	else
		return false;
}
