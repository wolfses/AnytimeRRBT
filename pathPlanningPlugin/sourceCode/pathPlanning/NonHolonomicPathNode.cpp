// This file is part of the PATH PLANNING PLUGIN for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The PATH PLANNING PLUGIN is licensed under the terms of EITHER:
//   1. PATH PLANNING PLUGIN commercial license (contact us for details)
//   2. PATH PLANNING PLUGIN educational license (see below)
// 
// PATH PLANNING PLUGIN educational license:
// -------------------------------------------------------------------
// The PATH PLANNING PLUGIN educational license applies only to EDUCATIONAL
// ENTITIES composed by following people and institutions:
// 
// 1. Hobbyists, students, teachers and professors
// 2. Schools and universities
// 
// EDUCATIONAL ENTITIES do NOT include companies, research institutions,
// non-profit organisations, foundations, etc.
// 
// An EDUCATIONAL ENTITY may use, modify, compile and distribute the
// modified/unmodified PATH PLANNING PLUGIN under following conditions:
//  
// 1. Distribution should be free of charge.
// 2. Distribution should be to EDUCATIONAL ENTITIES only.
// 3. Usage should be non-commercial.
// 4. Altered source versions must be plainly marked as such and distributed
//    along with any compiled code.
// 5. When using the PATH PLANNING PLUGIN in conjunction with V-REP, the "EDU"
//    watermark in the V-REP scene view should not be removed.
// 6. The origin of the PATH PLANNING PLUGIN must not be misrepresented. you must
//    not claim that you wrote the original software.
// 
// THE PATH PLANNING PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR
// IMPLIED WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.1.2 on June 16th 2014

#include "NonHolonomicPathNode.h"
#include "pathPlanningInterface.h"
#include "v_repLib.h"

CNonHolonomicPathNode::CNonHolonomicPathNode()
{
	parent=0;
}

CNonHolonomicPathNode::CNonHolonomicPathNode(CNonHolonomicPathNode* aNode)
{
	parent=0;
	values[0]=aNode->values[0];
	values[1]=aNode->values[1];
	values[2]=aNode->values[2];
	length_from_parent = aNode->length_from_parent;
	length_from_root = aNode->length_from_root;
}

CNonHolonomicPathNode::CNonHolonomicPathNode(float a,float b,float c)
{
	parent=0;
	values[0]=a;
	values[1]=b;
	values[2]=c;
	length_from_parent = 0;
	length_from_root = 0;
}


CNonHolonomicPathNode::~CNonHolonomicPathNode()
{
}
