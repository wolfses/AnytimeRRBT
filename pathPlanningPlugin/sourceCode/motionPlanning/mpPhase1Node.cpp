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
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include "mpPhase1Node.h"
#include "v_repLib.h"

CmpPhase1Node::CmpPhase1Node(int jointCount,const float* _jointPositions,const C7Vector& _tipPose)
{
	jointPositions=new float[jointCount];
// KEEP!!	neighbourIndices=new int[jointCount*2];
	tipPose=_tipPose;
	attributes=0;
	for (int i=0;i<jointCount;i++)
		jointPositions[i]=_jointPositions[i];
// KEEP!!	for (int i=0;i<jointCount*2;i++)
// KEEP!!		neighbourIndices[i]=-1;
}

CmpPhase1Node::~CmpPhase1Node()
{
// KEEP!!	delete[] neighbourIndices;
	delete[] jointPositions;
}

CmpPhase1Node* CmpPhase1Node::copyYourself(int jointCount)
{
	CmpPhase1Node* newNode=new CmpPhase1Node(jointCount,jointPositions,tipPose);
// KEEP!!	for (int i=0;i<jointCount*2;i++)
// KEEP!!		newNode->neighbourIndices[i]=neighbourIndices[i];
	newNode->attributes=attributes;
	return(newNode);
}

/* KEEP!!
void CmpPhase1Node::setNeighbours(int index,int theNeighbourIndex)
{
	neighbourIndices[index]=theNeighbourIndex;
}
*/

/*
void CmpPhase1Node::setColliding(bool colliding)
{
	if (colliding)
		attributes&=254;
	else
		attributes|=1;
}
*/
