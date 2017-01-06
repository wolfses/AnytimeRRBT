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
#include "EdgeCont.h"

const int eCnt=3;
const int eCnt2=2*eCnt;

CEdgeCont::CEdgeCont()
{
}

CEdgeCont::~CEdgeCont()
{
	clearEdges();
}

int CEdgeCont::getEdgeIndex(int ind1,int ind2)
{
	int indS=SIM_MIN(ind1,ind2);
	int indB=SIM_MAX(ind1,ind2);
	if (indS>=int(_edges.size()/eCnt2))
		return(-1); // that edge doesn't exist
	// 1. Special case?
	int firstSlotVal=_edges[eCnt2*indS+2*0+0];
	if (firstSlotVal<-1)
	{ // yes, this vertex is shared among more than 3 edges
		for (int i=0;i<int(_auxEdges[-firstSlotVal-2].size()/2);i++)
		{
			if (_auxEdges[-firstSlotVal-2][2*i+0]==indB)
				return(_auxEdges[-firstSlotVal-2][2*i+1]);
		}
		return(-1);
	}
	else
	{ // regular case
		for (int i=0;i<eCnt;i++)
		{
			if (_edges[eCnt2*indS+2*i+0]==indB)
				return(_edges[eCnt2*indS+2*i+1]);
		}
		return(-1);
	}
}

void CEdgeCont::clearEdges()
{
	allEdges.clear();
	allEdgesVisibilityState.clear();
	_edges.clear();
	_auxEdges.clear();
}

int CEdgeCont::addEdge(int ind1,int ind2)
{
	int t=getEdgeIndex(ind1,ind2);
	if (t!=-1)
		return(t); // edge already exists!
	int indS=SIM_MIN(ind1,ind2);
	int indB=SIM_MAX(ind1,ind2);
	while (indS>=int(_edges.size()/eCnt2))
	{
		for (int i=0;i<eCnt;i++)
			_edges.push_back(-1);
	}
	// 1. Special case?
	int firstSlotVal=_edges[eCnt2*indS+2*0+0];
	if (firstSlotVal<-1)
	{ // yes!
		_auxEdges[-firstSlotVal-2].push_back(indB);
		int retVal=allEdges.size()/2;
		_auxEdges[-firstSlotVal-2].push_back(retVal);
		allEdges.push_back(indS);
		allEdges.push_back(indB);
		allEdgesVisibilityState.push_back(2); // default visibility (2010/08/03)
		return(retVal);
	}
	for (int i=0;i<eCnt;i++)
	{
		if (_edges[eCnt2*indS+2*i+0]==-1)
		{
			_edges[eCnt2*indS+2*i+0]=indB;
			int retVal=allEdges.size()/2;
			_edges[eCnt2*indS+2*i+1]=retVal;
			allEdges.push_back(indS);
			allEdges.push_back(indB);
			allEdgesVisibilityState.push_back(2); // default visibility (2010/08/03)
			return(retVal);
		}
	}
	// We have to prepare the special case:
	std::vector<int> dummy;
	_auxEdges.push_back(dummy);
	t=_auxEdges.size()-1;
	for (int i=0;i<eCnt2;i++)
		_auxEdges[t].push_back(_edges[eCnt2*indS+i]);
	_edges[eCnt2*indS+2*0+0]=-(int(_auxEdges.size())+1);
	_auxEdges[t].push_back(indB);
	int retVal=allEdges.size()/2;
	_auxEdges[t].push_back(retVal);
	allEdges.push_back(indS);
	allEdges.push_back(indB);
	allEdgesVisibilityState.push_back(2); // default visibility (2010/08/03)
	return(retVal);
}
