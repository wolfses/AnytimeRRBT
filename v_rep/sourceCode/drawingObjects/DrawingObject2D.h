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

#include <string>

enum { // Keep values and order!! Is serialized!!! (well, not really, but for compatibility reasons!)
	sim_drawing2d_plus=0,
	sim_drawing2d_cross,
	sim_drawing2d_frame,
	sim_drawing2d_square,
	sim_drawing2d_circle,
	sim_drawing2d_disc,
	sim_drawing2d_arrow,
	sim_drawing2d_line,
	sim_drawing2d_triangle,
	sim_drawing2d_rectangle,
	sim_drawing2d_text,

	sim_drawing2d_blackbkground	=0x00000100,
	sim_drawing2d_pixelsize		=0x00000200,
};


class CDrawingObject2D  
{
public:
	CDrawingObject2D(int theObjectType,float* vertices,float col[3],float size,float dir,char* txt);
	virtual ~CDrawingObject2D();

	void draw(int minViewPos[2],int maxViewPos[2]);

protected:
	int _objectType;
	float _size;
	float _vertices[6];
	float _col[3];
	float _dir;
	std::string _txt;
};
