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
#include "UserSettings.h"
#include "ConfReaderAndWriter.h"
#include "Global.h"
#include "GV.h"
#include "IloIlo.h"
#include "ThreadPool.h"
#include "debugLogFile.h"
#include "Tt.h"
#include "IloIlo.h"
#include "OGL.h"
#include "v_repStrings.h"
#include "VVarious.h"
#include "App.h"

#define USER_SETTINGS_FILENAME "usrset.txt"

#define _USR_CORRECT_COLORS_AND_LIGHTS "correctColorsAndLights"
#define _USR_ANTIALIASING "antialiasing"
#define _USR_DISPLAY_WORLD_REF "displayWorldRef"
#define _USR_USE_GLFINISH "useGlFinish"
#define _USR_USE_GLFINISH_VISION_SENSORS "useGlFinish_visionSensors"
#define _USR_OGL_COMPATIBILITY_TWEAK_1 "oglCompatibilityTweak1"

#define _USR_STEREO_DIST "stereoDist"
#define _USR_VSYNC "vsync"
#define _USR_DEBUG_OPENGL "debugOpenGl"
#define _USR_WORK_THREAD_COUNT "workThreadCount"
#define _USR_DISPLAY_BOUNDING_BOXES "displayBoundingBoxeWhenObjectSelected"
#define _USR_REMOVE_IDENTICAL_VERTICES "removeIdenticalVertices"
#define _USR_IDENTICAL_VERTICES_TOLERANCE "identicalVerticesTolerance"
#define _USR_REMOVE_IDENTICAL_TRIANGLES "removeIdenticalTriangles"
#define _USR_TRIANGLE_WINDING_CHECK "triangleWindingCheck"
#define _USR_PROCESSOR_CORE_AFFINITY "processorCoreAffinity"
#define _USR_DYNAMIC_ACTIVITY_RANGE "dynamicActivityRange"
#define _USR_FREE_SERVER_PORT_START "freeServerPortStart"
#define _USR_FREE_SERVER_PORT_RANGE "freeServerPortRange"
#define _USR_GUI_FONT_SIZE_WIN "guiFontSize_Win"
#define _USR_GUI_FONT_SIZE_MAC "guiFontSize_Mac"
#define _USR_GUI_FONT_SIZE_LINUX "guiFontSize_Linux"
#define _USR_DEGREE_UNIT "unit_degrees"
#define _USR_METER_UNIT "unit_meters"
#define _USR_SECOND_UNIT "unit_seconds"
#define _USR_TRANSLATION_STEP_SIZE "objectTranslationStepSize"
#define _USR_ROTATION_STEP_SIZE "objectRotationStepSize"
#define _USR_COMPRESS_FILES "compressFiles"
#define _USR_TRIANGLE_COUNT_IN_OBB "triCountInOBB"
#define _USR_APPROXIMATED_NORMALS "saveApproxNormals"
#define _USR_PACK_INDICES "packIndices"
#define _USR_UNDO_REDO_ENABLED "undoRedoEnabled"
#define _USR_UNDO_REDO_LEVEL_COUNT "undoRedoLevelCount"
#define _USR_IDLE_FPS "idleFps"
#define _USR_UNDO_REDO_MAX_BUFFER_SIZE "undoRedoMaxBufferSize"
#define _USR_ALWAYS_SHOW_CONSOLE "alwaysShowConsole"
#define _USR_DEBUG_GUI_SIM_THREAD_SYNC_ACTIVITY "debugGuiThreadAndSimThreadSynchronizationActivity"
#define _USR_DEBUG_GUI_SIM_THREAD_SYNC_FAILS_ACTIVITY "debugGuiThreadAndSimThreadSynchronizationLockFailActivity"
#define _USR_DEBUG_INTERNAL_FUNCTION_ACCESS "debugInternalFunctionAccess"
#define _USR_DEBUG_C_API_ACCESS "debugCApiAccess"
#define _USR_DEBUG_LUA_API_ACCESS "debugLuaApiAccess"
#define _USR_DEBUG_THREAD_SWITCHES "debugThreadSwitches"
#define _USR_DEBUG_EASYLOCK_ACTIVITY "debugEasylockActivity"
#define _USR_DEBUG_TO_FILE "sendDebugInformationToFile"
#define _USR_FORCE_BUG_FIX_REL_30002 "forceBugFix_rel30002"
#define _USR_ALLOW_TRANSPARENT_DIALOGS	"allowTransparentDialogs"
#define _USR_DIALOG_TRANSPARENCY_FACTOR "dialogTransparencyFactor"
#define _USR_STATUSBAR_INITIALLY_VISIBLE "statusbarInitiallyVisible"
#define _USR_AUTO_SAVE_DELAY "autoSaveDelay"
#define _USR_BULLET_SERIALIZATION_BUFFER "bulletSerializationBuffer"
#define _USR_TIME_FOR_UNDO_REDO_TOO_LONG_WARNING "timeInMsForUndoRedoTooLongWarning"
#define _USR_FORCE_DEPRECATED_FILEFORMAT_LOAD "forceDeprecatedFileformatLoad"
#define _USR_APPLICATION_TWEAK1 "applicationTweak1"
#define _USR_APPLICATION_TWEAK2 "applicationTweak2"
#define _USR_APPLICATION_TWEAK3 "applicationTweak3"
#define _USR_ABORT_SCRIPT_EXECUTION_BUTTON "abortScriptExecutionButton"
#define _USR_RENDERING_SURFACE_VERTICAL_SHIFT "renderingSurfaceVShift"
#define _USR_RENDERING_SURFACE_VERTICAL_RESIZE "renderingSurfaceVResize"
#define _USR_USE_EXTERNAL_SCRIPT_EDITOR "useExternalScriptEditor"
#define _USR_EXTERNAL_SCRIPT_EDITOR "externalScriptEditor"
#define _USR_DIRECTORY_FOR_SCENES "defaultDirectoryForScenes"
#define _USR_DIRECTORY_FOR_MODELS "defaultDirectoryForModels"
#define _USR_DIRECTORY_FOR_CAD "defaultDirectoryForCadFiles"
#define _USR_DIRECTORY_FOR_MISC "defaultDirectoryForMiscFiles"
#define _USR_THREADED_RENDERING_DURING_SIMULATION "threadedRenderingDuringSimulation"
#define _USR_OFFSCREEN_CONTEXT_TYPE "offscreenContextType"
#define _USR_FBO_TYPE "fboType"
#define _USR_FORCE_FBO_VIA_EXT "forceFboViaExt"
#define _USR_DESIRED_OPENGL_MAJOR "desiredOpenGlMajor"
#define _USR_DESIRED_OPENGL_MINOR "desiredOpenGlMinor"

#define _USR_VISION_SENSORS_USE_GUI_WINDOWED "visionSensorsUseGuiThread_windowed"
#define _USR_VISION_SENSORS_USE_GUI_HEADLESS "visionSensorsUseGuiThread_headless"
#define _USR_FILE_DIALOGS_NATIVE "fileDialogs"
#define _USR_MOUSE_WHEEL_ZOOM_FACTOR "mouseWheelZoomFactor"


#define _USR_RETINA_SCALING_1_X "retinaScaling1X"
#define _USR_RETINA_SCALING_1_Y "retinaScaling1Y"
#define _USR_RETINA_SCALING_2_X "retinaScaling2X"
#define _USR_RETINA_SCALING_2_Y "retinaScaling2Y"
#define _USR_RETINA_SCALING_3_X "retinaScaling3X"
#define _USR_RETINA_SCALING_3_Y "retinaScaling3Y"

#define _USR_DO_NOT_WRITE_PERSISTENT_DATA "doNotWritePersistentData"
#define _USR_DO_NOT_SHOW_OPENGL_SETTINGS_MESSAGE "doNotShowOpenglSettingsMessage"
#define _USR_DO_NOT_SHOW_CRASH_RECOVERY_MESSAGE "doNotShowCrashRecoveryMessage"
#define _USR_DO_NOT_SHOW_UPDATE_CHECK_MESSAGE "doNotShowUpdateCheckMessage"

#define _USR_SCRIPT_EDITOR_FONT "scriptEditorFont"
#define _USR_SCRIPT_EDITOR_FONT_SIZE "scriptEditorFontSize"
#define _USR_AUX_CONSOLE_FONT "auxConsoleFont"
#define _USR_AUX_CONSOLE_FONT_SIZE "auxConsoleFontSize"

#define _USR_CONNECTION_ADDRESS "conParam1"
#define _USR_CONNECTION_PORT "conParam2"


CUserSettings::CUserSettings()
{
	// Debugging section:
	// *****************************
	alwaysShowConsole=false;
	//CSimAndUiThreadSync::setShowActivityInConsole(dummyBool);
	//CSimAndUiThreadSync::setShowLockFailsActivityInConsole(dummyBool);
	//CFuncDebug::setDebugMask(dummyInt);
	//CThreadPool::setShowThreadSwitches(dummyBool);
	//CEasyLock::setShowActivity(dummyBool);
	//CDebugLogFile::setDebugToFile(dummyBool);


	// Rendering section:
	// *****************************
	_idleFps=8;
	threadedRenderingDuringSimulation=0; // keep 0, will otherwise lead to very frequent crashes!!
	desiredOpenGlMajor=-1; // default
	desiredOpenGlMinor=-1; // default
	offscreenContextType=-1; // default type
	fboType=-1; // default type
	forceFboViaExt=false; // default
	oglCompatibilityTweak1=false;
	visionSensorsUseGuiThread_windowed=-1; // default
	visionSensorsUseGuiThread_headless=-1; // default
	useGlFinish=false;
	useGlFinish_visionSensors=false;
	vsync=0;
	debugOpenGl=false;
	stereoDist=0.0f; // default, no stereo!


	// Visual section:
	// *****************************
	retinaScaling1X=0.0f;
	retinaScaling1Y=0.0f;
	retinaScaling2X=0.0f;
	retinaScaling2Y=0.0f;
	retinaScaling3X=0.0f;
	retinaScaling3Y=0.0f;
	renderingSurfaceVShift=0;
	renderingSurfaceVResize=0;
	scriptEditorFont=""; // default
	scriptEditorFontSize=-1; // default
	auxConsoleFont=""; // default
	auxConsoleFontSize=-1; // default
	displayWorldReference=true;
	antiAliasing=false;
	displayBoundingBoxeWhenObjectSelected=true;
	guiFontSize_Win=11;
	guiFontSize_Mac=10;
	guiFontSize_Linux=10;
	allowTransparentDialogs=false;
	dialogTransparencyFactor=0.4f;
	statusbarInitiallyVisible=true;


	// Directories section:
	// *****************************
	defaultDirectoryForScenes="";
	defaultDirectoryForModels="";
	defaultDirectoryForCadFiles="";
	defaultDirectoryForMiscFiles="";


	// Serialization section:
	// *****************************
	autoSaveDelay=2; // 2 minutes for an auto-save by default. set to 0 for no auto-save
	doNotWritePersistentData=false;
	compressFiles=true;
	saveApproxNormals=true;
	packIndices=true;
	fileDialogs=-1; // default
	bulletSerializationBuffer=10000000;


	// Undo/redo section:
	// *****************************
	_undoRedoEnabled=true;
	undoRedoLevelCount=16;
	undoRedoMaxBufferSize=80000000;
	timeInMsForUndoRedoTooLongWarning=200;


	// Messaging section:
	// *****************************
	doNotShowOpenglSettingsMessage=false;
	doNotShowCrashRecoveryMessage=false;
	doNotShowUpdateCheckMessage=false;


	// Compatibility section:
	// *****************************
	correctColorsAndLights=true;
	forceDeprecatedFileformatLoad=false;
	applicationTweak1=-1; // default
	applicationTweak2=-1; // default
	applicationTweak3=-1; // default




	// Various section:
	// *****************************
	mouseWheelZoomFactor=1.0f; // default
	dynamicActivityRange=1000.0f;
	gv::setToDegrees();
	gv::setToMeters();
	gv::setToSeconds();
	_translationStepSize=0.025f;
	_rotationStepSize=5.0f*degToRad;
	workThreadCount=0; // disabled for now (still in the testing phase)
	//CThreadPool::setProcessorCoreAffinity(processorCoreAffinity);
	freeServerPortStart=20000;
	CIloIlo::nextfreeServerPortToUse=freeServerPortStart;
	freeServerPortRange=2000;
	_useExternalScriptEditor=false;
	externalScriptEditor="c:/Program Files (x86)/Notepad++/notepad++";
	_useExternalScriptEditorToSave=_useExternalScriptEditor;
	externalScriptEditorToSave=externalScriptEditor;
	abortScriptExecutionButton=2;
	triCountInOBB=8; // gave best results in 2009/07/21
	identicalVerticesCheck=true;
	identicalVerticesTolerance=0.0001f;
	identicalTrianglesCheck=true;
	identicalTrianglesWindingCheck=true;
	forceBugFix_rel30002=false;


	// Other, not serialized:
	groupSelectionColor.setDefaultValues();
	groupSelectionColor.setColor(0.75f,0.0f,0.4f,AMBIENT_MODE);
	groupSelectionColor.setColor(0.2f,0.0f,0.1f,EMISSION_MODE);


	loadUserSettings();
}

CUserSettings::~CUserSettings()
{
	
}

bool CUserSettings::useBuiltInScriptEditor()
{
	return(!_useExternalScriptEditor);
}

void CUserSettings::setUseBuiltInScriptEditorToSave(bool u)
{
	_useExternalScriptEditorToSave=!u;
}

bool CUserSettings::getUseBuiltInScriptEditorToSave()
{
	return(!_useExternalScriptEditorToSave);
}

void CUserSettings::setTranslationStepSize(float s)
{
	float sc=1.0f;
	if ((s>=0.0075f)&&(s<0.075f))
		sc=10.0f;
	if (s>=0.075f)
		sc=100.0f;
	if (s<0.0015f*sc)
		s=0.001f*sc;
	else
	{
		if (s<0.00375f*sc)
		{
			if (sc<2.0f)
				s=0.002f*sc;
			else
				s=0.0025f*sc;
		}
		else
			s=0.005f*sc;
	}
	_translationStepSize=s;
}
float CUserSettings::getTranslationStepSize()
{
	return(_translationStepSize);
}

void CUserSettings::setRotationStepSize(float s)
{
	if (s<1.5f*degToRad)
		s=1.0f*degToRad;
	else
	{
		if (s<3.5f*degToRad)
			s=2.0f*degToRad;
		else
		{
			if (s<7.5f*degToRad)
				s=5.0f*degToRad;
			else
			{
				if (s<12.5f*degToRad)
					s=10.0f*degToRad;
				else
				{
					if (s<22.5f*degToRad)
						s=15.0f*degToRad;
					else
					{
						if (s<37.5f*degToRad)
							s=30.0f*degToRad;
						else
							s=45.0f*degToRad;
					}
				}
			}
		}
	}
	_rotationStepSize=s;
}

float CUserSettings::getRotationStepSize()
{
	return(_rotationStepSize);
}

void CUserSettings::applyRequestedDefaultPaths()
{
	if (defaultDirectoryForScenes.length()!=0)
		App::directories->sceneDirectory=defaultDirectoryForScenes;
	if (defaultDirectoryForModels.length()!=0)
		App::directories->modelDirectory=defaultDirectoryForModels;
	if (defaultDirectoryForCadFiles.length()!=0)
		App::directories->cadFormatDirectory=defaultDirectoryForCadFiles;
	if (defaultDirectoryForMiscFiles.length()!=0)
		App::directories->otherFilesDirectory=defaultDirectoryForMiscFiles;
	VVarious::removePathFinalSlashOrBackslash(App::directories->sceneDirectory);
	VVarious::removePathFinalSlashOrBackslash(App::directories->modelDirectory);
	VVarious::removePathFinalSlashOrBackslash(App::directories->cadFormatDirectory);
	VVarious::removePathFinalSlashOrBackslash(App::directories->otherFilesDirectory);
}

void CUserSettings::setUndoRedoEnabled(bool isEnabled)
{
	_undoRedoEnabled=isEnabled;
	if (App::ct->undoBufferContainer!=NULL)
		App::ct->undoBufferContainer->newSceneProcedure();
}

bool CUserSettings::getUndoRedoEnabled()
{
	return(_undoRedoEnabled);
}

int CUserSettings::getIdleFps()
{
	return(_idleFps);
}

void CUserSettings::setIdleFps(int fps)
{
	_idleFps=tt::getLimitedInt(1,25,fps);
}

void CUserSettings::saveUserSettings()
{
	CConfReaderAndWriter c;

	c.addRandomLine("// Debugging");
	c.addRandomLine("// =================================================");
	c.addBoolean(_USR_ALWAYS_SHOW_CONSOLE,alwaysShowConsole,"");
	c.addBoolean(_USR_DEBUG_GUI_SIM_THREAD_SYNC_ACTIVITY,CSimAndUiThreadSync::getShowActivityInConsole(),"will also slow down V-REP");
	c.addBoolean(_USR_DEBUG_GUI_SIM_THREAD_SYNC_FAILS_ACTIVITY,CSimAndUiThreadSync::getShowLockFailsActivityInConsole(),"");
	c.addBoolean(_USR_DEBUG_INTERNAL_FUNCTION_ACCESS,(CFuncDebug::getDebugMask()&1)!=0,"will also heavily slow down V-REP");
	c.addBoolean(_USR_DEBUG_C_API_ACCESS,(CFuncDebug::getDebugMask()&2)!=0,"will also drastically slow down V-REP");
	c.addBoolean(_USR_DEBUG_LUA_API_ACCESS,(CFuncDebug::getDebugMask()&4)!=0,"will also slow down V-REP");
	c.addBoolean(_USR_DEBUG_THREAD_SWITCHES,CEasyLock::getShowActivity(),"will also slow down V-REP");
	c.addBoolean(_USR_DEBUG_EASYLOCK_ACTIVITY,CThreadPool::getShowThreadSwitches(),"will also slow down V-REP");
	c.addBoolean(_USR_DEBUG_TO_FILE,CDebugLogFile::getDebugToFile(),"if true, debug info is sent to debugLog.txt");
	c.addRandomLine("");
	c.addRandomLine("");


	c.addRandomLine("// Rendering");
	c.addRandomLine("// =================================================");
	c.addInteger(_USR_IDLE_FPS,_idleFps,"");
	c.addInteger(_USR_THREADED_RENDERING_DURING_SIMULATION,threadedRenderingDuringSimulation,"recommended to keep 0 (-1=disabled, 0=pre-enabled, 1=enabled).");
	c.addInteger(_USR_DESIRED_OPENGL_MAJOR,desiredOpenGlMajor,"recommended to keep -1.");
	c.addInteger(_USR_DESIRED_OPENGL_MINOR,desiredOpenGlMinor,"recommended to keep -1.");
	c.addInteger(_USR_OFFSCREEN_CONTEXT_TYPE,offscreenContextType,"recommended to keep -1 (-1=default, 0=Qt offscreen, 1=QGLWidget visible, 2=QGLWidget invisible).");
	c.addInteger(_USR_FBO_TYPE,fboType,"recommended to keep -1 (-1=default, 0=native, 1=QOpenGLFramebufferObject).");
	c.addBoolean(_USR_FORCE_FBO_VIA_EXT,forceFboViaExt,"recommended to keep false.");
	c.addBoolean(_USR_OGL_COMPATIBILITY_TWEAK_1,oglCompatibilityTweak1,"recommended to keep false since it causes small memory leaks.");
	c.addInteger(_USR_VISION_SENSORS_USE_GUI_WINDOWED,visionSensorsUseGuiThread_windowed,"recommended to keep -1 (-1=default, 0=GUI when not otherwise possible, 1=always GUI).");
	c.addInteger(_USR_VISION_SENSORS_USE_GUI_HEADLESS,visionSensorsUseGuiThread_headless,"recommended to keep -1 (-1=default, 0=GUI when not otherwise possible, 1=always GUI).");
	c.addBoolean(_USR_USE_GLFINISH,useGlFinish,"recommended to keep false. Graphic card dependent.");
	c.addBoolean(_USR_USE_GLFINISH_VISION_SENSORS,useGlFinish_visionSensors,"recommended to keep false. Graphic card dependent.");
	c.addInteger(_USR_VSYNC,vsync,"recommended to keep at 0. Graphic card dependent.");
	c.addBoolean(_USR_DEBUG_OPENGL,debugOpenGl,"");
	c.addFloat(_USR_STEREO_DIST,stereoDist,"0=no stereo, otherwise the intra occular distance (0.0635 for the human eyes).");
	c.addRandomLine("");
	c.addRandomLine("");


	c.addRandomLine("// Visual");
	c.addRandomLine("// =================================================");
	c.addFloat(_USR_RETINA_SCALING_1_X,retinaScaling1X,"0=automatic scaling");
	c.addFloat(_USR_RETINA_SCALING_1_Y,retinaScaling1Y,"0=automatic scaling");
	c.addFloat(_USR_RETINA_SCALING_2_X,retinaScaling2X,"0=automatic scaling");
	c.addFloat(_USR_RETINA_SCALING_2_Y,retinaScaling2Y,"0=automatic scaling");
	c.addFloat(_USR_RETINA_SCALING_3_X,retinaScaling3X,"0=automatic scaling");
	c.addFloat(_USR_RETINA_SCALING_3_Y,retinaScaling3Y,"0=automatic scaling");
	c.addInteger(_USR_RENDERING_SURFACE_VERTICAL_SHIFT,renderingSurfaceVShift,"");
	c.addInteger(_USR_RENDERING_SURFACE_VERTICAL_RESIZE,renderingSurfaceVResize,"");
	c.addString(_USR_SCRIPT_EDITOR_FONT,scriptEditorFont,"empty=default.");
	c.addInteger(_USR_SCRIPT_EDITOR_FONT_SIZE,scriptEditorFontSize,"-1=default.");
	c.addString(_USR_AUX_CONSOLE_FONT,auxConsoleFont,"empty=default");
	c.addInteger(_USR_AUX_CONSOLE_FONT_SIZE,auxConsoleFontSize,"-1=default.");
	c.addBoolean(_USR_DISPLAY_WORLD_REF,displayWorldReference,"");
	c.addBoolean(_USR_ANTIALIASING,antiAliasing,"");
	c.addBoolean(_USR_DISPLAY_BOUNDING_BOXES,displayBoundingBoxeWhenObjectSelected,"");
	c.addInteger(_USR_GUI_FONT_SIZE_WIN,guiFontSize_Win,"");
	c.addInteger(_USR_GUI_FONT_SIZE_MAC,guiFontSize_Mac,"");
	c.addInteger(_USR_GUI_FONT_SIZE_LINUX,guiFontSize_Linux,"");
	c.addBoolean(_USR_ALLOW_TRANSPARENT_DIALOGS,allowTransparentDialogs,"");
	c.addFloat(_USR_DIALOG_TRANSPARENCY_FACTOR,dialogTransparencyFactor,"");
	c.addBoolean(_USR_STATUSBAR_INITIALLY_VISIBLE,statusbarInitiallyVisible,"");
	c.addRandomLine("");
	c.addRandomLine("");


	c.addRandomLine("// Directories");
	c.addRandomLine("// =================================================");
	c.addString(_USR_DIRECTORY_FOR_SCENES,defaultDirectoryForScenes,"absolute path, e.g. d:/myScenes (or leave empty for default path)");
	c.addString(_USR_DIRECTORY_FOR_MODELS,defaultDirectoryForModels,"absolute path, e.g. d:/myModels (or leave empty for default path)");
	c.addString(_USR_DIRECTORY_FOR_CAD,defaultDirectoryForCadFiles,"absolute path, e.g. d:/myCadFiles (or leave empty for default path)");
	c.addString(_USR_DIRECTORY_FOR_MISC,defaultDirectoryForMiscFiles,"absolute path, e.g. d:/myMiscFiles (or leave empty for default path)");
	c.addRandomLine("");
	c.addRandomLine("");


	c.addRandomLine("// Serialization");
	c.addRandomLine("// =================================================");
	c.addInteger(_USR_AUTO_SAVE_DELAY,autoSaveDelay,"in minutes. 0 to disable.");
	c.addBoolean(_USR_DO_NOT_WRITE_PERSISTENT_DATA,doNotWritePersistentData,"");
	c.addBoolean(_USR_COMPRESS_FILES,compressFiles,"");
	c.addBoolean(_USR_APPROXIMATED_NORMALS,saveApproxNormals,"");
	c.addBoolean(_USR_PACK_INDICES,packIndices,"");
	c.addInteger(_USR_FILE_DIALOGS_NATIVE,fileDialogs,"recommended to keep -1 (-1=default, 0=native dialogs, 1=Qt dialogs).");
	c.addInteger(_USR_BULLET_SERIALIZATION_BUFFER,bulletSerializationBuffer,"");
	c.addRandomLine("");
	c.addRandomLine("");


	c.addRandomLine("// Undo/Redo");
	c.addRandomLine("// =================================================");
	c.addBoolean(_USR_UNDO_REDO_ENABLED,_undoRedoEnabled,"");
	c.addInteger(_USR_UNDO_REDO_LEVEL_COUNT,undoRedoLevelCount,"");
	c.addInteger(_USR_UNDO_REDO_MAX_BUFFER_SIZE,undoRedoMaxBufferSize,"");
	c.addInteger(_USR_TIME_FOR_UNDO_REDO_TOO_LONG_WARNING,timeInMsForUndoRedoTooLongWarning,"");
	c.addRandomLine("");
	c.addRandomLine("");


	c.addRandomLine("// Messaging");
	c.addRandomLine("// =================================================");
	c.addBoolean(_USR_DO_NOT_SHOW_OPENGL_SETTINGS_MESSAGE,doNotShowOpenglSettingsMessage,"");
	c.addBoolean(_USR_DO_NOT_SHOW_CRASH_RECOVERY_MESSAGE,doNotShowCrashRecoveryMessage,"");
	c.addBoolean(_USR_DO_NOT_SHOW_UPDATE_CHECK_MESSAGE,doNotShowUpdateCheckMessage,"");
	c.addRandomLine("");
	c.addRandomLine("");


	c.addRandomLine("// Compatibility");
	c.addRandomLine("// =================================================");
	c.addBoolean(_USR_CORRECT_COLORS_AND_LIGHTS,correctColorsAndLights,"");
	c.addBoolean(_USR_FORCE_DEPRECATED_FILEFORMAT_LOAD,forceDeprecatedFileformatLoad,"");
	c.addInteger(_USR_APPLICATION_TWEAK1,applicationTweak1,"recommended to keep -1 (1=non modal file dialogs)");
	c.addInteger(_USR_APPLICATION_TWEAK2,applicationTweak2,"recommended to keep -1 (1=UI processes partial messages while waiting for read/write lock)");
	c.addInteger(_USR_APPLICATION_TWEAK3,applicationTweak3,"recommended to keep -1 (1=UI processes full messages in certain situations)");
	c.addRandomLine("");
	c.addRandomLine("");




	c.addRandomLine("// Various");
	c.addRandomLine("// =================================================");
	c.addFloat(_USR_MOUSE_WHEEL_ZOOM_FACTOR,mouseWheelZoomFactor,"");
	c.addFloat(_USR_DYNAMIC_ACTIVITY_RANGE,dynamicActivityRange,"");
	c.addBoolean(_USR_DEGREE_UNIT,gv::degrees,"");
	c.addBoolean(_USR_METER_UNIT,gv::meters,"");
	c.addBoolean(_USR_SECOND_UNIT,gv::seconds,"");
	c.addFloat(_USR_TRANSLATION_STEP_SIZE,_translationStepSize,"");
	c.addFloat(_USR_ROTATION_STEP_SIZE,_rotationStepSize*radToDeg,"");
	c.addInteger(_USR_WORK_THREAD_COUNT,workThreadCount,"-1, 0-256. (0 to disable, -1 for automatic setting (i.e. thread cnt=core cnt)).");
	c.addInteger(_USR_PROCESSOR_CORE_AFFINITY,CThreadPool::getProcessorCoreAffinity(),"recommended to keep 0 (-1:os default, 0:all threads on same core, m: affinity mask (bit1=core1, bit2=core2, etc.))");
	c.addInteger(_USR_FREE_SERVER_PORT_START,freeServerPortStart,"");
	c.addInteger(_USR_FREE_SERVER_PORT_RANGE,freeServerPortRange,"");
	c.addBoolean(_USR_USE_EXTERNAL_SCRIPT_EDITOR,_useExternalScriptEditorToSave,"");
	c.addString(_USR_EXTERNAL_SCRIPT_EDITOR,externalScriptEditorToSave,"");
	c.addInteger(_USR_ABORT_SCRIPT_EXECUTION_BUTTON,abortScriptExecutionButton,"in seconds. Zero to disable.");
	c.addInteger(_USR_TRIANGLE_COUNT_IN_OBB,triCountInOBB,"");
	c.addBoolean(_USR_REMOVE_IDENTICAL_VERTICES,identicalVerticesCheck,"");
	c.addFloat(_USR_IDENTICAL_VERTICES_TOLERANCE,identicalVerticesTolerance,"");
	c.addBoolean(_USR_REMOVE_IDENTICAL_TRIANGLES,identicalTrianglesCheck,"");
	c.addBoolean(_USR_TRIANGLE_WINDING_CHECK,identicalTrianglesWindingCheck,"");


	std::string filenameAndPath(VVarious::splitPath_path(VVarious::getModulePathAndFile())+VREP_SLASH+V_REP_SYSTEM_DIRECTORY_NAME+VREP_SLASH+USER_SETTINGS_FILENAME);
	c.writeConfiguration(filenameAndPath.c_str());
}

void CUserSettings::loadUserSettings()
{
	CConfReaderAndWriter c;

	// The CDirectoryPaths object might not yet be set-up
	std::string filenameAndPath(VVarious::splitPath_path(VVarious::getModulePathAndFile())+VREP_SLASH+V_REP_SYSTEM_DIRECTORY_NAME+VREP_SLASH+USER_SETTINGS_FILENAME);

	// Following call might fail. So it is important to later on always check if we could effectively read the value (if that value is not set to default)
	c.readConfiguration(filenameAndPath.c_str());

	// Debugging section:
	// *****************************
	c.getBoolean(_USR_ALWAYS_SHOW_CONSOLE,alwaysShowConsole);
	bool dummyBool=false;
	if (c.getBoolean(_USR_DEBUG_GUI_SIM_THREAD_SYNC_ACTIVITY,dummyBool))
		CSimAndUiThreadSync::setShowActivityInConsole(dummyBool);
	if (c.getBoolean(_USR_DEBUG_GUI_SIM_THREAD_SYNC_FAILS_ACTIVITY,dummyBool))
		CSimAndUiThreadSync::setShowLockFailsActivityInConsole(dummyBool);
	int dummyInt=0;
	if (c.getBoolean(_USR_DEBUG_INTERNAL_FUNCTION_ACCESS,dummyBool))
	{
		if (dummyBool)
			dummyInt+=1;
	}
	if (c.getBoolean(_USR_DEBUG_C_API_ACCESS,dummyBool))
	{
		if (dummyBool)
			dummyInt+=2;
	}
	if (c.getBoolean(_USR_DEBUG_LUA_API_ACCESS,dummyBool))
	{
		if (dummyBool)
			dummyInt+=4;
	}
	CFuncDebug::setDebugMask(dummyInt);
	if (c.getBoolean(_USR_DEBUG_THREAD_SWITCHES,dummyBool))
		CThreadPool::setShowThreadSwitches(dummyBool);
	if (c.getBoolean(_USR_DEBUG_EASYLOCK_ACTIVITY,dummyBool))
		CEasyLock::setShowActivity(dummyBool);
	if (c.getBoolean(_USR_DEBUG_TO_FILE,dummyBool))
		CDebugLogFile::setDebugToFile(dummyBool);


	// Rendering section:
	// *****************************
	c.getInteger(_USR_IDLE_FPS,_idleFps);
	setIdleFps(_idleFps);
	c.getInteger(_USR_THREADED_RENDERING_DURING_SIMULATION,threadedRenderingDuringSimulation);
	c.getInteger(_USR_DESIRED_OPENGL_MAJOR,desiredOpenGlMajor);
	c.getInteger(_USR_DESIRED_OPENGL_MINOR,desiredOpenGlMinor);
	c.getInteger(_USR_OFFSCREEN_CONTEXT_TYPE,offscreenContextType);
	c.getInteger(_USR_FBO_TYPE,fboType);
	c.getBoolean(_USR_FORCE_FBO_VIA_EXT,forceFboViaExt);
	c.getBoolean(_USR_OGL_COMPATIBILITY_TWEAK_1,oglCompatibilityTweak1);
	c.getInteger(_USR_VISION_SENSORS_USE_GUI_WINDOWED,visionSensorsUseGuiThread_windowed);
	c.getInteger(_USR_VISION_SENSORS_USE_GUI_HEADLESS,visionSensorsUseGuiThread_headless);
	c.getBoolean(_USR_USE_GLFINISH,useGlFinish);
	c.getBoolean(_USR_USE_GLFINISH_VISION_SENSORS,useGlFinish_visionSensors);
	c.getInteger(_USR_VSYNC,vsync);
	c.getBoolean(_USR_DEBUG_OPENGL,debugOpenGl);
	c.getFloat(_USR_STEREO_DIST,stereoDist);


	// Visual section:
	// *****************************
	c.getFloat(_USR_RETINA_SCALING_1_X,retinaScaling1X);
	c.getFloat(_USR_RETINA_SCALING_1_Y,retinaScaling1Y);
	c.getFloat(_USR_RETINA_SCALING_2_X,retinaScaling2X);
	c.getFloat(_USR_RETINA_SCALING_2_Y,retinaScaling2Y);
	c.getFloat(_USR_RETINA_SCALING_3_X,retinaScaling3X);
	c.getFloat(_USR_RETINA_SCALING_3_Y,retinaScaling3Y);
	c.getInteger(_USR_RENDERING_SURFACE_VERTICAL_SHIFT,renderingSurfaceVShift);
	c.getInteger(_USR_RENDERING_SURFACE_VERTICAL_RESIZE,renderingSurfaceVResize);
	c.getString(_USR_SCRIPT_EDITOR_FONT,scriptEditorFont);
	c.getInteger(_USR_SCRIPT_EDITOR_FONT_SIZE,scriptEditorFontSize);
	c.getString(_USR_AUX_CONSOLE_FONT,auxConsoleFont);
	c.getInteger(_USR_AUX_CONSOLE_FONT_SIZE,auxConsoleFontSize);
	c.getBoolean(_USR_DISPLAY_WORLD_REF,displayWorldReference);
	c.getBoolean(_USR_ANTIALIASING,antiAliasing);
	c.getBoolean(_USR_DISPLAY_BOUNDING_BOXES,displayBoundingBoxeWhenObjectSelected);
	c.getInteger(_USR_GUI_FONT_SIZE_WIN,guiFontSize_Win);
	c.getInteger(_USR_GUI_FONT_SIZE_MAC,guiFontSize_Mac);
	c.getInteger(_USR_GUI_FONT_SIZE_LINUX,guiFontSize_Linux);
	c.getBoolean(_USR_ALLOW_TRANSPARENT_DIALOGS,allowTransparentDialogs);
	c.getFloat(_USR_DIALOG_TRANSPARENCY_FACTOR,dialogTransparencyFactor);
	c.getBoolean(_USR_STATUSBAR_INITIALLY_VISIBLE,statusbarInitiallyVisible);


	// Directories section:
	// *****************************
	c.getString(_USR_DIRECTORY_FOR_SCENES,defaultDirectoryForScenes);
	c.getString(_USR_DIRECTORY_FOR_MODELS,defaultDirectoryForModels);
	c.getString(_USR_DIRECTORY_FOR_CAD,defaultDirectoryForCadFiles);
	c.getString(_USR_DIRECTORY_FOR_MISC,defaultDirectoryForMiscFiles);


	// Serialization section:
	// *****************************
	c.getInteger(_USR_AUTO_SAVE_DELAY,autoSaveDelay);
	c.getBoolean(_USR_DO_NOT_WRITE_PERSISTENT_DATA,doNotWritePersistentData);
	c.getBoolean(_USR_COMPRESS_FILES,compressFiles);
	c.getBoolean(_USR_APPROXIMATED_NORMALS,saveApproxNormals);
	c.getBoolean(_USR_PACK_INDICES,packIndices);
	c.getInteger(_USR_FILE_DIALOGS_NATIVE,fileDialogs);
	c.getInteger(_USR_BULLET_SERIALIZATION_BUFFER,bulletSerializationBuffer);


	// Undo/redo section:
	// *****************************
	c.getBoolean(_USR_UNDO_REDO_ENABLED,_undoRedoEnabled);
	c.getInteger(_USR_UNDO_REDO_LEVEL_COUNT,undoRedoLevelCount);
	undoRedoLevelCount=tt::getLimitedInt(3,50,undoRedoLevelCount);
	c.getInteger(_USR_UNDO_REDO_MAX_BUFFER_SIZE,undoRedoMaxBufferSize);
	undoRedoMaxBufferSize=tt::getLimitedInt(2000000,800000000,undoRedoMaxBufferSize);
	c.getInteger(_USR_TIME_FOR_UNDO_REDO_TOO_LONG_WARNING,timeInMsForUndoRedoTooLongWarning);


	// Messaging section:
	// *****************************
	c.getBoolean(_USR_DO_NOT_SHOW_OPENGL_SETTINGS_MESSAGE,doNotShowOpenglSettingsMessage);
	c.getBoolean(_USR_DO_NOT_SHOW_CRASH_RECOVERY_MESSAGE,doNotShowCrashRecoveryMessage);
	c.getBoolean(_USR_DO_NOT_SHOW_UPDATE_CHECK_MESSAGE,doNotShowUpdateCheckMessage);


	// Compatibility section:
	// *****************************
	c.getBoolean(_USR_CORRECT_COLORS_AND_LIGHTS,correctColorsAndLights);
	c.getBoolean(_USR_FORCE_DEPRECATED_FILEFORMAT_LOAD,forceDeprecatedFileformatLoad);
	c.getInteger(_USR_APPLICATION_TWEAK1,applicationTweak1);
	c.getInteger(_USR_APPLICATION_TWEAK2,applicationTweak2);
	c.getInteger(_USR_APPLICATION_TWEAK3,applicationTweak3);




	// Various section:
	// *****************************
	c.getFloat(_USR_MOUSE_WHEEL_ZOOM_FACTOR,mouseWheelZoomFactor);
	c.getFloat(_USR_DYNAMIC_ACTIVITY_RANGE,dynamicActivityRange);
	bool dummy=true;
	if (c.getBoolean(_USR_DEGREE_UNIT,dummy))
	{
		if (dummy)
			gv::setToDegrees();
		else
			gv::setToRadians();
	}
	if (c.getBoolean(_USR_METER_UNIT,dummy))
	{
		if (dummy)
			gv::setToMeters();
		else
			gv::setToMilimeters();
	}
	if (c.getBoolean(_USR_SECOND_UNIT,dummy))
	{
		if (dummy)
			gv::setToSeconds();
		else
			gv::setToMiliseconds();
	}
	if (c.getFloat(_USR_TRANSLATION_STEP_SIZE,_translationStepSize))
		setTranslationStepSize(_translationStepSize);
	if (c.getFloat(_USR_ROTATION_STEP_SIZE,_rotationStepSize))
		setRotationStepSize(_rotationStepSize*degToRad);
	c.getInteger(_USR_WORK_THREAD_COUNT,workThreadCount);
	int processorCoreAffinity=0;
	if (c.getInteger(_USR_PROCESSOR_CORE_AFFINITY,processorCoreAffinity))
		CThreadPool::setProcessorCoreAffinity(processorCoreAffinity);
	c.getInteger(_USR_FREE_SERVER_PORT_START,freeServerPortStart);
	CIloIlo::nextfreeServerPortToUse=freeServerPortStart;
	c.getInteger(_USR_FREE_SERVER_PORT_RANGE,freeServerPortRange);
	c.getBoolean(_USR_USE_EXTERNAL_SCRIPT_EDITOR,_useExternalScriptEditor);
	_useExternalScriptEditorToSave=_useExternalScriptEditor;
	c.getString(_USR_EXTERNAL_SCRIPT_EDITOR,externalScriptEditor);
	externalScriptEditorToSave=externalScriptEditor;
	c.getInteger(_USR_ABORT_SCRIPT_EXECUTION_BUTTON,abortScriptExecutionButton);
	c.getInteger(_USR_TRIANGLE_COUNT_IN_OBB,triCountInOBB);
	c.getBoolean(_USR_REMOVE_IDENTICAL_VERTICES,identicalVerticesCheck);
	c.getFloat(_USR_IDENTICAL_VERTICES_TOLERANCE,identicalVerticesTolerance);
	c.getBoolean(_USR_REMOVE_IDENTICAL_TRIANGLES,identicalTrianglesCheck);
	c.getBoolean(_USR_TRIANGLE_WINDING_CHECK,identicalTrianglesWindingCheck);
	c.getBoolean(_USR_FORCE_BUG_FIX_REL_30002,forceBugFix_rel30002);
}

