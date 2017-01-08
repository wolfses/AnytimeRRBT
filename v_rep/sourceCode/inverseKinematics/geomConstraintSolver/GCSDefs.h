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

#ifndef _GLOBAL_GCS_
#define _GLOBAL_GCS_

#define IK_DIVISION_FACTOR 100.0f

// IK Object types
enum {	IK_JOINT_TYPE=0,
		IK_DUMMY_TYPE,
		IK_MESH_TYPE
};

// IK Graph Node types
enum {	IK_GRAPH_JOINT_TYPE=0,
		IK_GRAPH_OBJECT_TYPE
};

// IK Graph Object types
enum {	IK_GRAPH_LINK_OBJECT_TYPE=0,
		IK_GRAPH_TIP_OBJECT_TYPE,
		IK_GRAPH_PASSIVE_OBJECT_TYPE,
		IK_GRAPH_MESH_OBJECT_TYPE
};

// IK Graph Joint types
enum {	IK_GRAPH_REVOLUTE_JOINT_TYPE=0,
		IK_GRAPH_PRISMATIC_JOINT_TYPE,
		IK_GRAPH_SPHERICAL_JOINT_TYPE,
		IK_GRAPH_SCREW_JOINT_TYPE
};

// Ik constraints.
enum {
		IK_X_CONSTRAINT=1,
		IK_Y_CONSTRAINT=2,
		IK_Z_CONSTRAINT=4,
		IK_ALPHA_BETA_CONSTRAINT=8,
		IK_GAMMA_CONSTRAINT=16,
// FREE
		IK_AVOIDANCE_CONSTRAINT=64
};

#endif // !defined(_GLOBAL_GCS_)
