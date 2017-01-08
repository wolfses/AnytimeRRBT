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
#include "v_rep_internal.h"
#include "undoBuffer.h"

#include "IloIlo.h"

CUndoBuffer::CUndoBuffer(const std::vector<char>& fullBuffer,const std::vector<std::string>& selObjNames,int bufferId)
{
	buffer.insert(buffer.end(),fullBuffer.begin(),fullBuffer.end());
	_selectedObjectNames.assign(selObjNames.begin(),selObjNames.end());
	_sameCountFromBeginning=0;
	_sameCountFromEnd=0;
	_bufferId=bufferId;
}

CUndoBuffer::~CUndoBuffer()
{
}

int CUndoBuffer::getBufferId()
{
	return(_bufferId);
}

bool CUndoBuffer::finalize(const std::vector<char>& previousFullBuffer,const std::vector<std::string>& previousSelObjNames)
{ // Return value false means: this buffer is exactly the same as "previousFullBuffer"
	_sameCountFromBeginning=0;
	_sameCountFromEnd=0;
	int minBufferLength=SIM_MIN(int(previousFullBuffer.size()),int(buffer.size()));
	for (int i=0;i<minBufferLength;i++)
	{
		if (previousFullBuffer[i]!=buffer[i])
			break;
		_sameCountFromBeginning++;
	}
	if ( (previousFullBuffer.size()==buffer.size())&&(_sameCountFromBeginning==int(buffer.size())) )
	{ // Buffer content is same! Same selection too?
		bool same=true;
		if (previousSelObjNames.size()==_selectedObjectNames.size())
		{
			for (int i=0;i<int(previousSelObjNames.size());i++)
			{
				if (previousSelObjNames[i].compare(_selectedObjectNames[i])!=0)
				{
					same=false;
					break;
				}
			}
		}
		else
			same=false;
		if (same)
			return(false); // buffer content is same, selection too!
		buffer.clear();
		{
			std::vector<char> ttmmpp(buffer);
			buffer.swap(ttmmpp); // release the reserved memory that is not used!
		}
		return(true);
	}
	if (_sameCountFromBeginning==int(buffer.size()))
	{ // Previous buffer's beginning is same as this buffer entirely!
		buffer.clear();
		{
			std::vector<char> ttmmpp(buffer);
			buffer.swap(ttmmpp); // release the reserved memory that is not used!
		}
		return(true);
	}
	// We release what is not needed anymore:
	buffer.erase(buffer.begin(),buffer.begin()+_sameCountFromBeginning);
	{
		std::vector<char> ttmmpp(buffer);
		buffer.swap(ttmmpp); // release the reserved memory that is not used!
	}
	if (_sameCountFromBeginning==int(previousFullBuffer.size()))
	{ // Previous buffer is entirely same as this buffer's beginning!
		return(true);
	}

	// Now we check from the end:
	while (true)
	{
		int j=previousFullBuffer.size()-1-_sameCountFromEnd;
		int k=buffer.size()-1-_sameCountFromEnd;
		if ((j<0)||(k<0))
			break;
		if (previousFullBuffer[j]!=buffer[k])
			break;
		_sameCountFromEnd++;
	}
	// We release what is not needed anymore:
	buffer.erase(buffer.end()-_sameCountFromEnd,buffer.end());
	{
		std::vector<char> ttmmpp(buffer);
		buffer.swap(ttmmpp); // release the reserved memory that is not used!
	}
	return(true);
}

void CUndoBuffer::getRestored(const std::vector<char>* previousFullBuffer,std::vector<char>& restoredBuffer,std::vector<std::string>& selObjNames)
{ // previousFullBuffer can be NULL. In that case _sameCountFromBeginning and _sameCountFromEnd should be 0
	restoredBuffer.clear();
	if (previousFullBuffer!=NULL)
		restoredBuffer.insert(restoredBuffer.end(),previousFullBuffer->begin(),previousFullBuffer->begin()+_sameCountFromBeginning);
	restoredBuffer.insert(restoredBuffer.end(),buffer.begin(),buffer.end());
	if (previousFullBuffer!=NULL)
		restoredBuffer.insert(restoredBuffer.end(),previousFullBuffer->end()-_sameCountFromEnd,previousFullBuffer->end());
	selObjNames.assign(_selectedObjectNames.begin(),_selectedObjectNames.end());
}

void CUndoBuffer::updateWithFullBuffer(const std::vector<char>& fullBuffer)
{
	_sameCountFromBeginning=0;
	_sameCountFromEnd=0;
	buffer.clear();
	buffer.insert(buffer.end(),fullBuffer.begin(),fullBuffer.end());
	{
		std::vector<char> ttmmpp(buffer);
		buffer.swap(ttmmpp); // release the reserved memory that is not used!
	}
}
