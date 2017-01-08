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
#include "GlobalTextureContainer.h"
#include "Global.h"
#include "imgLoader.h"

CGlobalTextureContainer::CGlobalTextureContainer()
{
	int xres,yres;
	bool rgba;
	unsigned char* dat;

	dat=CImageLoader::loadQTgaImageData(":/targaFiles/world.tga",xres,yres,rgba,NULL);
	addObject(WORLD_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/s_shape.tga",xres,yres,rgba,NULL);
	addObject(SIMPLE_SHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/p_shape.tga",xres,yres,rgba,NULL);
	addObject(PURE_SHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/h_shape.tga",xres,yres,rgba,NULL);
	addObject(HEIGHTFIELD_SHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/mp_shape.tga",xres,yres,rgba,NULL);
	addObject(PURE_MULTISHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/m_shape.tga",xres,yres,rgba,NULL);
	addObject(MULTI_SHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/r_joint.tga",xres,yres,rgba,NULL);
	addObject(REVOLUTE_JOINT_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/p_joint.tga",xres,yres,rgba,NULL);
	addObject(PRISMATIC_JOINT_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/spheric.tga",xres,yres,rgba,NULL);
	addObject(SPHERICAL_JOINT_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/camera.tga",xres,yres,rgba,NULL);
	addObject(CAMERA_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/o_lighti.tga",xres,yres,rgba,NULL);
	addObject(OMNI_LIGHT_OFF_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/o_lighta.tga",xres,yres,rgba,NULL);
	addObject(OMNI_LIGHT_ON_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/s_lighti.tga",xres,yres,rgba,NULL);
	addObject(SPOT_LIGHT_OFF_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/s_lighta.tga",xres,yres,rgba,NULL);
	addObject(SPOT_LIGHT_ON_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/d_lighti.tga",xres,yres,rgba,NULL);
	addObject(DIR_LIGHT_OFF_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/d_lighta.tga",xres,yres,rgba,NULL);
	addObject(DIR_LIGHT_ON_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/graph.tga",xres,yres,rgba,NULL);
	addObject(GRAPH_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/dummy.tga",xres,yres,rgba,NULL);
	addObject(DUMMY_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/model.tga",xres,yres,rgba,NULL);
	addObject(MODEL_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/sensor.tga",xres,yres,rgba,NULL);
	addObject(PROXIMITYSENSOR_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/rendsens.tga",xres,yres,rgba,NULL);
	addObject(VISIONSENSOR_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/path.tga",xres,yres,rgba,NULL);
	addObject(PATH_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;

	dat=CImageLoader::loadQTgaImageData(":/targaFiles/mirror.tga",xres,yres,rgba,NULL);
	addObject(MIRROR_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/clipplane.tga",xres,yres,rgba,NULL);
	addObject(CLIPPLANE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;

	dat=CImageLoader::loadQTgaImageData(":/targaFiles/convexs.tga",xres,yres,rgba,NULL);
	addObject(CONVEX_SHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/convexm.tga",xres,yres,rgba,NULL);
	addObject(CONVEX_MULTISHAPE_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;

	dat=CImageLoader::loadQTgaImageData(":/targaFiles/minus.tga",xres,yres,rgba,NULL);
	addObject(MINUS_SIGN_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/plus.tga",xres,yres,rgba,NULL);
	addObject(PLUS_SIGN_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/mscript.tga",xres,yres,rgba,NULL);
	addObject(MAIN_SCRIPT_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/mcscript.tga",xres,yres,rgba,NULL);
	addObject(MAIN_SCRIPT_CUSTOMIZED_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/script.tga",xres,yres,rgba,NULL);
	addObject(SCRIPT_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/scriptthreaded.tga",xres,yres,rgba,NULL);
	addObject(SCRIPT_THREADED_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/scriptparameters.tga",xres,yres,rgba,NULL);
	addObject(SCRIPT_PARAMETERS_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/scriptparametersactive.tga",xres,yres,rgba,NULL);
	addObject(SCRIPT_PARAMETERS_ACTIVE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/pathpoint.tga",xres,yres,rgba,NULL);
	addObject(PATH_POINT_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/vertex.tga",xres,yres,rgba,NULL);
	addObject(VERTEX_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/triangle.tga",xres,yres,rgba,NULL);
	addObject(TRIANGLE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/edge.tga",xres,yres,rgba,NULL);
	addObject(EDGE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/2delement.tga",xres,yres,rgba,NULL);
	addObject(BUTTON_BLOCK_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/mill.tga",xres,yres,rgba,NULL);
	addObject(MILL_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/ftsensor.tga",xres,yres,rgba,NULL);
	addObject(FORCE_SENSOR_TREE_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/warning.tga",xres,yres,rgba,NULL);
	addObject(WARNING_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/dynamics.tga",xres,yres,rgba,NULL);
	addObject(DYNAMICS_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/folder.tga",xres,yres,rgba,NULL);
	addObject(FOLDER_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;

	dat=CImageLoader::loadQTgaImageData(":/targaFiles/checkedbgd.tga",xres,yres,rgba,NULL);
	addObject(CHECKED_BACKGROUND_PICTURE,xres,yres,rgba,false,true,dat);
	delete[] dat;
	

	// Cursors:
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_arrow.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_ARROW,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_finger.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_FINGER,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_alldir.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_ALL_DIR,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_horizdir.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_HORIZONTAL_DIR,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_vertdir.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_VERTICAL_DIR,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_slash.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_SLASH_DIR,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_backslash.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_BACKSLASH_DIR,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_nobutt.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_NO_BUTTON,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_leftbutt.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_LEFT_BUTTON,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_middlebutt.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_MIDDLE_BUTTON,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_wheel.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_WHEEL_BUTTON,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_rightbutt.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_RIGHT_BUTTON,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_shift.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_SHIFT_BUTTON,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/cur_ctrl.tga",xres,yres,rgba,NULL);
	addObject(CURSOR_CTRL_BUTTON,xres,yres,rgba,false,true,dat);
	delete[] dat;

	dat=CImageLoader::loadQTgaImageData(":/targaFiles/vrepWaterMark.tga",xres,yres,rgba,NULL);
	addObject(VREP_TAG,xres,yres,rgba,false,true,dat);
	delete[] dat;
	dat=CImageLoader::loadQTgaImageData(":/targaFiles/eduWaterMark.tga",xres,yres,rgba,NULL);
	addObject(EDU_TAG,xres,yres,rgba,false,true,dat);
	delete[] dat;
}

CGlobalTextureContainer::~CGlobalTextureContainer()
{
	removeAllObjects();
}

void CGlobalTextureContainer::addObject(int pictureIndex,int sizeX,int sizeY,bool rgba,bool horizFlip,bool vertFlip,unsigned char* data)
{
	CTextureObject* it=new CTextureObject(sizeX,sizeY);
	it->setImage(rgba,horizFlip,vertFlip,data);
	_allTextureObjects.push_back(it);
	_allPictureIndices.push_back(pictureIndex);
}

void CGlobalTextureContainer::removeAllObjects()
{
	for (int i=0;i<int(_allTextureObjects.size());i++)
		delete _allTextureObjects[i];
	_allTextureObjects.clear();
	_allPictureIndices.clear();
}

void CGlobalTextureContainer::startTextureDisplay(int pictureIndex)
{
	for (int i=0;i<int(_allPictureIndices.size());i++)
	{
		if (_allPictureIndices[i]==pictureIndex)
		{
			_allTextureObjects[i]->startTextureDisplay(true,false,false,false);
			break;
		}
	}
}

void CGlobalTextureContainer::endTextureDisplay()
{
	glDisable(GL_TEXTURE_2D);
}
