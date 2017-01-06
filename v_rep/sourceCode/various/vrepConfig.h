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

#ifndef VREP_CONFIG_INCLUDED
#define VREP_CONFIG_INCLUDED

#define VREP_LICENSE_VREP_DEMO			0x00000000
#define VREP_LICENSE_VREP_PRO_EDU		0x00001000
#define VREP_LICENSE_VREP_PRO			0x00002000
#define VREP_LICENSE_VREP_PRO_P			0x00003000
#define VREP_LICENSE_VREP_SUBLICENSE	0x00004000
#define VREP_LICENSE_VREP_PLAYER		0x00005000
#define VREP_LICENSE_VREP_NAKED			0x00006000


// 2.4.2 for the first personal release (built on 2010/03/14)
										// 2.4.3 released around 2010/03/28
										// 2.4.4 released around 2010/05/01
										// 2.4.5 released around 2010/06/08
										// 2.4.6 released around 2010/06/16
										// 2.4.7 released on 2010/06/27. Main changes: make pure shapes from tri edit mode
										// 2.4.8 released around 2010/08/15. Main changes: model browser + better object manipulation + collection can overide main properties
										// 2.4.9 released around 2010/09/13. Main changes: walking human model (path following and path planning)
										// 2.4.10 released end of September. Main changes: removed some scenes/models because of Sony/Honda
										// 2.4.11 released around October 14. Main change: embedded the ODE engine
										// 2.4.12 released around October 30. Main change: special function for omni wheel simulation, new scenes/models
										// 2.4.13 released around December 8. Main change: 3 api functions to handle particles. New models
										// 2.4.14 released around December 11. Only change: corrected a bug that made V-REP crash on MAC with VMWare
										// 2.4.15 released around December 20. Main change: real threads (with non-blocking sections (simSetThreadIsFree)), luaSockets, tubes, auxiliary consoles
										// 2.5.0 released around 2011/01/16. Bug correction in simSetRenderingSensorImage, changed simHandleModule. Added new API functions to import/export meshes, and to create shapes
										// 2.5.1 released around 10/02/2011. Bug correction in simGetObjectOrientation, added model(s), allowed free Lua strings as script parameter (e.g. with embedded zeros), many new models, one new tutorial
										// 2.5.2 released around 22/02/2011. Added heightfields, contact callbacks, floating view API functions 
										// 2.5.3 released around 20/03/2011. Many changes!!
										// 2.5.4 released around 21/04/2011. Many changes, mainly reworked the whole user manual
										// 2.5.5 released around 03/06/2011. Little changes, was on several trips (Japan-Shanghai-Germany)
										// 2.5.6 released around 14/06/2011. Improved signals, textures with text coord, new scene and bug correction (linked to joints with force reading)
										// 2.5.7 released around 25/06/2011. Now all objects can be model base, support for kinect, etc.
										// 2.5.8 released around 20/07/2011. Many small changes, and begin of the port (many functions already encapsulated
										// 2.5.9 released around 03/09/2011. Skyboxes, new models and other small details
										// 2.5.10 released around 21/10/2011. Heavy internal changes for cross platform preparation. A few new simple models and improved API
										// 2.5.11 released around 13/11/2011. Bug fixing (opening dialogs could crash or freeze V-REP
										// 2.5.12 released around 06/12/2011. Removed the parallel simulation possibility. Improved the plugin messaging. Added the view fitting. Increased undo point setting speed
										// 2.6.0 released around 26/01/2012. First version based on Qt. Many many changes!
										// 2.6.1 released around 28/02/2012. Auto-save, better dynamic content viewing, new models, bug correction!
										// 2.6.2 released around 18/03/2012. Brand new icons and toolbar buttons, page and scene selector, buttons for the edit modes, etc.
										// 2.6.3 released on 3/4/2012. New click and select, new coord. and transf. dialog, new video compression library. Player version can do more now (e.g. edit scripts)
										// 2.6.4 released on 24/5/2012. Remote API. Linux and Mac version are not beta anymore (v_repExtRemoteApi, v_repExtK3, v_repExtBubbleRob, bubbleRobClient & bubbleRobServer work on all 3 platforms)
										// 2.6.5 released on 4/7/2012. Remote Api bindings for Python, Java, Matlab and Urbi + first ROS plugin
										// 2.6.6 released around 20/8/2012. Full ROS interface, Reflexxes Motion Library type IV
										// 2.6.7 released on 20/9/2012. Mirrors, external IK, new texture mapping modes, etc.
										// 2.6.8 released around 1/12/2012. Support for add-ons, completely rewritten shape containers (CGeomProxy, CGeomWrap and CGeometric). Serialization version is now 16. Extracted mesh calculation, dynamics & path planning algorithms into shared libs
										// 3.0.0 released around 10/1/2013. V-REP is now open source
										// 3.0.1 released around 20/1/2013. Mainly convex shape support and convex decomposition routines
										// 3.0.2 released around 14/3/2013. Callback scripts, Qhull, improved URDF importer, various bug fixes, real threading for sensing commands (in beta)
										// 3.0.3 released around 29/4/2013. Several bug fixes, new API functions (e.g. related to paths), DNA transfer functionality, new models, improved remote API (several connections from same client now possible). See the doc for the full list of changes.
										// 3.0.4 released around 8/7/2013. First routines for motion planning. New vision sensor filter, 2 new callbacks to read the openGl content, several new API functions, etc.
										// 3.0.5 released around 27/10/2013. Vortex support, motion planning, Qt5.1.1, Threaded rendering, new texture fileformats, etc.
										// 3.1.0 released around 17/1/2014. Now GUI and SIM threads are separate. Ghost objects, Fixed FBOs, load recent menu, debug possibilities, headless, etc.
										// 3.1.1 released around 27/3/2014.
										// 3.1.2 released around 17/6/2014. Better motion planning, remote API much faster for large data, etc.

#define VREP_WEBPAGE_ADDRESS_TXT "www.coppeliarobotics.com"
#define VREP_WEBPAGE_EMAIL_TXT "info@coppeliarobotics.com"
#define VREP_DR_MARC_ANDREAS_FREESE_TXT "Dr. Marc Andreas Freese"



#define VREP_LICENSE_TYPE VREP_LICENSE_VREP_NAKED


#ifdef WIN_VREP
	#define VREP_LOWCASE_STRING_COMPARE(x,y) _stricmp(x,y) 
	#define VREP_BEEP Beep(4000,500);Beep(4000,500);Beep(4000,500)
#else
	#define VREP_LOWCASE_STRING_COMPARE(x,y) strcasecmp(x,y)
	#define VREP_BEEP QApplication::beep();VThread::sleep(500);QApplication::beep();VThread::sleep(500);QApplication::beep()
#endif

#define VREP_LUA_OBJLEN(x,y) lua_objlen(x,y)
#define VREP_SLASH "/"
#define _SECURE_SCL 0 // Disables the bound check for vectors (a bit faster)
#define SIM_ROTATE_LEFT_16(data,amount) (WORD(WORD(WORD(data) << (amount&0x0f)) | (WORD(data) >> (16-(amount&0x0f))))
#define SIM_MIN(a,b) (((a)<(b)) ? (a) : (b))
#define SIM_MAX(a,b) (((a)>(b)) ? (a) : (b))
#define SIM_IS_BIT_SET(var,bit) (((var) & (1<<(bit)))!=0)
#define SIM_SET_BIT(var,bit) ((var) |= (1<<(bit)))
#define SIM_CLEAR_BIT(var,bit) ((var) &= (~(1<<(bit))))
#define SIM_TOGGLE_BIT(var,bit) ((var) ^= (1<<(bit)))
#define SIM_SET_CLEAR_BIT(var,bit,on) ((on) ? SIM_SET_BIT((var),(bit)) : SIM_CLEAR_BIT((var),(bit)) )
#define SIM_RAND_FLOAT (rand()/(float)RAND_MAX)
#define SIM_VREP_SERVER_PACKET_SIZE 620
#define SIM_FLOATING_SERVER_PACKET_SIZE 320
#define strTranslate(text) text
#define QTPROPERTYBROWSER_COLOR_GREY QColor(244,244,244)
#define QTPROPERTYBROWSER_COLOR_RED QColor(255,240,240)
#define QTPROPERTYBROWSER_COLOR_GREEN QColor(240,255,240)
#define QTPROPERTYBROWSER_COLOR_BLUE QColor(240,240,255)
#define QTPROPERTYBROWSER_COLOR_YELLOW QColor(255,255,240)
#define QTPROPERTYBROWSER_COLOR_CYAN QColor(240,255,255)
#define QTPROPERTYBROWSER_COLOR_PURPLE QColor(255,240,255)

#ifdef MAC_VREP
	#pragma clang diagnostic ignored "-Wunused-parameter"
	#pragma clang diagnostic ignored "-Wunused-variable"
	#pragma clang diagnostic ignored "-Wstrict-aliasing"
	#pragma clang diagnostic ignored "-Wempty-body"
	#pragma clang diagnostic ignored "-Wwrite-strings"
#endif


#endif // VREP_CONFIG_INCLUDED
