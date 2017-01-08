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

#pragma once

#include <vector>

struct SUndoBufferIntArray
{
	int _identifier;
	std::vector<int> _buffer;
	std::vector<int> _undoBufferDependencyIds;
};

struct SUndoBufferFloatArray
{
	int _identifier;
	std::vector<float> _buffer;
	std::vector<int> _undoBufferDependencyIds;
};

struct SUndoBufferUCharArray
{
	int _identifier;
	std::vector<unsigned char> _buffer;
	std::vector<int> _undoBufferDependencyIds;
};

class CUndoBufferArrays
{
public:
	CUndoBufferArrays();
	virtual ~CUndoBufferArrays();

	int addVertexBuffer(const std::vector<float>& buff,int undoBufferId);
	int addIndexBuffer(const std::vector<int>& buff,int undoBufferId);
	int addNormalsBuffer(const std::vector<float>& buff,int undoBufferId);
	int addTextureBuffer(const std::vector<unsigned char>& buff,int undoBufferId);

	void getVertexBuffer(int id,std::vector<float>& buff);
	void getIndexBuffer(int id,std::vector<int>& buff);
	void getNormalsBuffer(int id,std::vector<float>& buff);
	void getTextureBuffer(int id,std::vector<unsigned char>& buff);

	void removeDependenciesFromUndoBufferId(int undoBufferId);
	void clearAll();
	int getMemorySizeInBytes();
private:

	bool _areFloatBuffersSame(const std::vector<float>& buff1,const std::vector<float>& buff2);
	bool _areIntBuffersSame(const std::vector<int>& buff1,const std::vector<int>& buff2);
	bool _areUCharBuffersSame(const std::vector<unsigned char>& buff1,const std::vector<unsigned char>& buff2);

	std::vector<SUndoBufferFloatArray> _vertexBuffers;
	std::vector<SUndoBufferIntArray> _indexBuffers;
	std::vector<SUndoBufferFloatArray> _normalsBuffers;
	std::vector<SUndoBufferUCharArray> _textureBuffers;
	int _nextId;
};
