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

#include "Source.h"
#include "tinyxml2.h"
#include <string>
#include "XMLHelper.h"
#include "StringHelper.h"

using namespace tinyxml2;

Source::Source() : m_UnitCount(0)
{

}

const std::string& Source::getID() const
{
	return m_ID;
}

void Source::setID(const std::string& ID)
{
	m_ID = ID;
}

const std::vector<float>& Source::getArrayValues() const
{
	return m_ArrayValues;
}

std::vector<float>& Source::getArrayValues()
{
	return m_ArrayValues;
}

int Source::getUnitCount() const
{
	return m_UnitCount;
}

void Source::setUnitCount(int Count)
{
	m_UnitCount = Count;
}

bool Source::parseXML(XMLElement *sourceNode,boost::unordered_map<std::string, std::vector<float> >& floatarrays)
{
	if(std::string(sourceNode->Name()) == "source") {
				
		XMLElement* technique = XMLHelper::GetChildElement(sourceNode,"technique_common");
		if(technique != NULL) {

			XMLElement* accessor = XMLHelper::GetChildElement(technique,"accessor");
			int count=0;
			int offset=0;
			int stride = 1;
			bool *usedparams;
			if(accessor != NULL)
			{			
				if(accessor->Attribute("stride") != NULL) {
					if(!StringHelper::from_string<int>(stride,accessor->Attribute("stride")))
						return false;
				}					
				if(accessor->Attribute("offset") != NULL) {
					if(!StringHelper::from_string<int>(offset,accessor->Attribute("offset")))
						return false;
				}
				if(accessor->Attribute("count") != NULL) {
					if(!StringHelper::from_string<int>(count,accessor->Attribute("count")))
						return false;
				}
				usedparams = new bool[stride];
				std::fill(usedparams,usedparams+stride,false);

				XMLElement *params = accessor->FirstChildElement();
				int j=0;
				while(params != NULL) {
					if(std::string(params->Name()) == "param") {
						if(params->Attribute("name") != NULL)
							usedparams[j] = true;
						j++;
					}
					params = params->NextSiblingElement();
				}
				
				std::string sourceID = accessor->Attribute("source");
				sourceID = sourceID.substr(1);		// remove leading "#"
				
				if(floatarrays.find(sourceID) == floatarrays.end()) {
					delete [] usedparams;
					return false;
				}
				std::vector<float>& fvalues = floatarrays[sourceID];

				if(sourceNode->Attribute("id") != NULL) {
					m_ID = sourceNode->Attribute("id");
				}
			
				m_ArrayValues.clear();

				int end = offset + stride*count;
				if(end > int(fvalues.size())) {
					delete [] usedparams;
					return false;
				}

				for(int i=offset;i<end;i++) {
					if(usedparams[(i-offset)%stride]) {
						m_ArrayValues.push_back(fvalues[i]);
					}
				}

				m_UnitCount = 0;

				for(int i=0;i<stride;i++) {
					if(usedparams[i])
						m_UnitCount++;
				}

				delete [] usedparams;
			}
			else
				return false;			
		}
		else
			return false;

		return true;
	}
	else
		return false;
}
