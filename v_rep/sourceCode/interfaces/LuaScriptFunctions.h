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

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}

#include "VMutex.h"
#include <string>
#include <vector>

enum {lua_arg_empty,lua_arg_nil,lua_arg_number,lua_arg_bool,lua_arg_string,lua_arg_function,lua_arg_userdata,lua_arg_table};


lua_State* initializeNewLuaState(const char* scriptSuffixNumberString);
void registerNewLuaFunctions(lua_State* L);
void prepareNewLuaVariables(lua_State* L,const char* scriptSuffixNumberString);
void setNewLuaVariable(lua_State* L,const char* name,int identifier);

void pushCorrectTypeOntoLuaStack(lua_State* L,const std::string& txt);
int getCorrectType(const std::string& txt);

void registerCustomLuaFunctions(lua_State* L);
void registerCustomLuaVariables(lua_State* L);

bool readCustomFunctionDataFromStack(lua_State* L,int ind,int dataType,
									 std::vector<char>& inBoolVector,
									 std::vector<int>& inIntVector,
									 std::vector<float>& inFloatVector,
									 std::vector<std::string>& inStringVector,
									 std::vector<std::string>& inCharVector,
									std::vector<int>& inInfoVector);
void writeCustomFunctionDataOntoStack(lua_State* L,int dataType,int dataSize,
									  unsigned char* boolData,int& boolDataPos,
									  int* intData,int& intDataPos,
									  float* floatData,int& floatDataPos,
									  char* stringData,int& stringDataPos,
									  char* charData,int& charDataPos);



void getFloatsFromTable(lua_State* L,int tablePos,int floatCount,float* arrayField);
void getDoublesFromTable(lua_State* L,int tablePos,int doubleCount,double* arrayField);
bool getIntsFromTable(lua_State* L,int tablePos,int intCount,int* arrayField);
void getBoolsFromTable(lua_State* L,int tablePos,int boolCount,char* arrayField);
void pushFloatTableOntoStack(lua_State* L,int floatCount,const float* arrayField);
void pushDoubleTableOntoStack(lua_State* L,int doubleCount,const double* arrayField);
void pushIntTableOntoStack(lua_State* L,int intCount,const int* arrayField);
int luaToInt(lua_State* L,int pos);
float luaToFloat(lua_State* L,int pos);
bool luaToBool(lua_State* L,int pos);

bool isDashFree(const std::string& functionName,const std::string& name);
bool suffixAdjustStringIfNeeded(const std::string& functionName,bool outputError,lua_State* L,std::string& name);
int getCurrentScriptID(lua_State* L);

void getScriptTree_mainOr(lua_State* L,bool selfIncluded,std::vector<int>& scriptHandles);
void getScriptChain(lua_State* L,bool selfIncluded,bool mainIncluded,std::vector<int>& scriptHandles);

void outputCallErrorMessage(const char* functionName,const char* message,bool outputError);
void outputCallWarningMessage(const char* functionName,const char* message);

void memorizeLocation(lua_State* L);
int getLocationIndex(VTHREAD_ID_TYPE threadID);
void forgetLocation();
std::string getLocationString();

// Input argument checking:
bool checkInputArguments(lua_State* L,std::string& functionName,bool outputErrors,
						 int type1=lua_arg_empty,int type1Cnt_zeroIfNotTable=-1,
						 int type2=lua_arg_empty,int type2Cnt_zeroIfNotTable=-1,
						 int type3=lua_arg_empty,int type3Cnt_zeroIfNotTable=-1,
						 int type4=lua_arg_empty,int type4Cnt_zeroIfNotTable=-1,
						 int type5=lua_arg_empty,int type5Cnt_zeroIfNotTable=-1,
						 int type6=lua_arg_empty,int type6Cnt_zeroIfNotTable=-1,
						 int type7=lua_arg_empty,int type7Cnt_zeroIfNotTable=-1,
						 int type8=lua_arg_empty,int type8Cnt_zeroIfNotTable=-1,
						 int type9=lua_arg_empty,int type9Cnt_zeroIfNotTable=-1,
						 int type10=lua_arg_empty,int type10Cnt_zeroIfNotTable=-1,
						 int type11=lua_arg_empty,int type11Cnt_zeroIfNotTable=-1);
int checkOneGeneralInputArgument(lua_State* L,int index,std::string& functionName,
						   int type,int cnt_orZeroIfNotTable,bool optional,bool nilInsteadOfTypeAndCountAllowed,bool outputErrors);
bool checkOneInputArgument(lua_State* L,int index,std::string& functionName,int type,bool outputErrors);


void luaHookFunction(lua_State* L,lua_Debug* ar);
void moduleCommonPart(lua_State* L,int action);



int _simHandleChildScript(lua_State* L);
int _simHandleSensingChildScripts(lua_State* L);
int _simGetScriptName(lua_State* L);

int _simGetObjectAssociatedWithScript(lua_State* L);
int _simGetScriptAssociatedWithObject(lua_State* L);

int _simGenericFunctionHandler(lua_State* L);
int _simGetScriptExecutionCount(lua_State* L);
int _simIsScriptExecutionThreaded(lua_State* L);
int _simIsScriptRunningInThread(lua_State* L);
int _simOpenModule(lua_State* L);
int _simCloseModule(lua_State* L);
int _simHandleModule(lua_State* L);
int _simBoolOr16(lua_State* L);
int _simBoolAnd16(lua_State* L);
int _simBoolXor16(lua_State* L);

int _simHandleDynamics(lua_State* L);
int _simHandleIkGroup(lua_State* L);
int _simCheckIkGroup(lua_State* L);
int _simHandleCollision(lua_State* L);
int _simReadCollision(lua_State* L);
int _simHandleDistance(lua_State* L);
int _simReadDistance(lua_State* L);
int _simHandleProximitySensor(lua_State* L);
int _simReadProximitySensor(lua_State* L);
int _simHandleMill(lua_State* L);
int _simResetCollision(lua_State* L);
int _simResetDistance(lua_State* L);
int _simResetProximitySensor(lua_State* L);
int _simResetMill(lua_State* L);
int _simCheckProximitySensor(lua_State* L);
int _simCheckProximitySensorEx(lua_State* L);
int _simCheckProximitySensorEx2(lua_State* L);
int _simGetObjectHandle(lua_State* L);
int _simGetScriptHandle(lua_State* L);
int _simGetCollectionHandle(lua_State* L);
int _simRemoveCollection(lua_State* L);
int _simGetObjectPosition(lua_State* L);
int _simGetObjectOrientation(lua_State* L);
int _simSetObjectPosition(lua_State* L);
int _simSetObjectOrientation(lua_State* L);
int _simGetJointPosition(lua_State* L);
int _simSetJointPosition(lua_State* L);
int _simSetJointTargetPosition(lua_State* L);
int _simGetJointTargetPosition(lua_State* L);
int _simSetJointForce(lua_State* L);
int _simGetPathPosition(lua_State* L);
int _simSetPathPosition(lua_State* L);
int _simGetPathLength(lua_State* L);
int _simSetJointTargetVelocity(lua_State* L);
int _simGetJointTargetVelocity(lua_State* L);
int _simSetPathTargetNominalVelocity(lua_State* L);
int _simGetUIHandle(lua_State* L);
int _simGetUIProperty(lua_State* L);
int _simSetUIProperty(lua_State* L);
int _simGetUIEventButton(lua_State* L);
int _simGetUIButtonProperty(lua_State* L);
int _simSetUIButtonProperty(lua_State* L);
int _simGetUIButtonSize(lua_State* L);
int _simSetUIButtonLabel(lua_State* L);
int _simGetUIButtonLabel(lua_State* L);
int _simSetUISlider(lua_State* L);
int _simGetUISlider(lua_State* L);
int _simGetObjectName(lua_State* L);
int _simGetCollectionName(lua_State* L);
int _simSetObjectName(lua_State* L);
int _simSetCollectionName(lua_State* L);
int _simRemoveObject(lua_State* L);
int _simSetUIButtonColor(lua_State* L);
int _simGetSimulationTime(lua_State* L);
int _simGetSimulationState(lua_State* L);
int _simGetSystemTime(lua_State* L);
int _simGetSystemTimeInMilliseconds(lua_State* L); // deprecated
int _simGetSystemTimeInMs(lua_State* L);
int _simCheckCollision(lua_State* L);
int _simCheckCollisionEx(lua_State* L);
int _simCheckDistance(lua_State* L);
int _simGetObjectConfiguration(lua_State* L);
int _simSetObjectConfiguration(lua_State* L);
int _simGetConfigurationTree(lua_State* L);
int _simSetConfigurationTree(lua_State* L);
int _simHandleMechanism(lua_State* L);
int _simGetSimulationTimeStep(lua_State* L);
int _simGetSimulatorMessage(lua_State* L);
int _simResetPath(lua_State* L);
int _simHandlePath(lua_State* L);

int _simResetJoint(lua_State* L);
int _simHandleJoint(lua_State* L);
int _simResetTracing(lua_State* L);
int _simHandleTracing(lua_State* L);

int _simResetGraph(lua_State* L);
int _simHandleGraph(lua_State* L);
int _simAddStatusbarMessage(lua_State* L);
int _simGetLastError(lua_State* L);
int _simGetObjects(lua_State* L);
int _simRefreshDialogs(lua_State* L);
int _simGetModuleName(lua_State* L);
int _simGetIkGroupHandle(lua_State* L);
int _simGetCollisionHandle(lua_State* L);
int _simGetDistanceHandle(lua_State* L);
int _simGetScriptSimulationParameter(lua_State* L);
int _simSetScriptSimulationParameter(lua_State* L);
int _simDisplayDialog(lua_State* L);
int _simGetDialogResult(lua_State* L);
int _simGetDialogInput(lua_State* L);
int _simEndDialog(lua_State* L);
int _simStopSimulation(lua_State* L);
int _simPauseSimulation(lua_State* L);
int _simStartSimulation(lua_State* L);

int _simGetObjectMatrix(lua_State* L);
int _simSetObjectMatrix(lua_State* L);
int _simGetJointMatrix(lua_State* L);
int _simSetSphericalJointMatrix(lua_State* L);

int _simBuildIdentityMatrix(lua_State* L);
int _simCopyMatrix(lua_State* L);
int _simBuildMatrix(lua_State* L);
int _simGetEulerAnglesFromMatrix(lua_State* L);
int _simGetInvertedMatrix(lua_State* L);
int _simMultiplyMatrices(lua_State* L);
int _simInterpolateMatrices(lua_State* L);
int _simMultiplyVector(lua_State* L);

int _simGetObjectParent(lua_State* L);
int _simSetObjectParent(lua_State* L);
int _simGetObjectChild(lua_State* L);
int _simGetObjectType(lua_State* L);
int _simGetJointType(lua_State* L);

int _simSetBooleanParameter(lua_State* L);
int _simGetBooleanParameter(lua_State* L);
int _simSetIntegerParameter(lua_State* L);
int _simGetIntegerParameter(lua_State* L);
int _simSetFloatingParameter(lua_State* L);
int _simGetFloatingParameter(lua_State* L);
int _simSetStringParameter(lua_State* L);
int _simGetStringParameter(lua_State* L);
int _simSetArrayParameter(lua_State* L);
int _simGetArrayParameter(lua_State* L);

int _simCreateUIButtonArray(lua_State* L);
int _simSetUIButtonArrayColor(lua_State* L);
int _simDeleteUIButtonArray(lua_State* L);

int _simGetJointInterval(lua_State* L);
int _simSetJointInterval(lua_State* L);

int _simLoadScene(lua_State* L);
int _simSaveScene(lua_State* L);
int _simLoadModel(lua_State* L);
int _simSaveModel(lua_State* L);
int _simLoadUI(lua_State* L);
int _simSaveUI(lua_State* L);
int _simRemoveUI(lua_State* L);


int _simIsObjectInSelection(lua_State* L);
int _simAddObjectToSelection(lua_State* L);
int _simRemoveObjectFromSelection(lua_State* L);
int _simGetObjectSelectionSize(lua_State* L);
int _simGetObjectLastSelection(lua_State* L);
int _simGetObjectSelection(lua_State* L);

int _simGetRealTimeSimulation(lua_State* L);

int _simCreateUI(lua_State* L);
int _simCreateUIButton(lua_State* L);
int _simLockInterface(lua_State* L);

int _simGetMechanismHandle(lua_State* L);
int _simGetPathPlanningHandle(lua_State* L);
int _simSearchPath(lua_State* L);
int _simInitializePathSearch(lua_State* L);
int _simPerformPathSearchStep(lua_State* L);

int _simSetNavigationMode(lua_State* L);
int _simGetNavigationMode(lua_State* L);
int _simSetPage(lua_State* L);
int _simGetPage(lua_State* L);

int _simReleaseScriptRawBuffer(lua_State* L);

int _simCopyPasteSelectedObjects(lua_State* L);
int _simDeleteSelectedObjects(lua_State* L);
int _simScaleSelectedObjects(lua_State* L);

int _simGetObjectUniqueIdentifier(lua_State* L);

int _simGetNameSuffix(lua_State* L);
int _simSetNameSuffix(lua_State* L);

int _simSetThreadAutomaticSwitch(lua_State* L);
int _simSetThreadSwitchTiming(lua_State* L);
int _simSetThreadResumeLocation(lua_State* L);
int _simResumeThreads(lua_State* L);
int _simSwitchThread(lua_State* L);

int _simSendData(lua_State* L);
int _simReceiveData(lua_State* L);

int _simPackInts(lua_State* L);
int _simPackFloats(lua_State* L);
int _simUnpackInts(lua_State* L);
int _simUnpackFloats(lua_State* L);

int _simGetVelocity(lua_State* L);
int _simGetObjectVelocity(lua_State* L);
int _simAddForceAndTorque(lua_State* L);
int _simAddForce(lua_State* L);

int _simSetExplicitHandling(lua_State* L);
int _simGetExplicitHandling(lua_State* L);

int _simGetLinkDummy(lua_State* L);
int _simSetLinkDummy(lua_State* L);

int _simSetGraphUserData(lua_State* L);

int _simSetUIPosition(lua_State* L);
int _simGetUIPosition(lua_State* L);

int _simAddDrawingObject(lua_State* L);
int _simRemoveDrawingObject(lua_State* L);
int _simAddDrawingObjectItem(lua_State* L);

int _simAddParticleObject(lua_State* L);
int _simRemoveParticleObject(lua_State* L);
int _simAddParticleObjectItem(lua_State* L);

int _simSerialOpen(lua_State* L);
int _simSerialClose(lua_State* L);
int _simSerialSend(lua_State* L);
int _simSerialRead(lua_State* L);
int _simSerialCheck(lua_State* L);

int _simSerialPortOpen(lua_State* L);
int _simSerialPortClose(lua_State* L);
int _simSerialPortSend(lua_State* L);
int _simSerialPortRead(lua_State* L);

int _simGetObjectSizeFactor(lua_State* L);

int _simResetMilling(lua_State* L);
int _simApplyMilling(lua_State* L);


int _simSetIntegerSignal(lua_State* L);
int _simGetIntegerSignal(lua_State* L);
int _simClearIntegerSignal(lua_State* L);
int _simSetFloatSignal(lua_State* L);
int _simGetFloatSignal(lua_State* L);
int _simClearFloatSignal(lua_State* L);
int _simSetStringSignal(lua_State* L);
int _simGetStringSignal(lua_State* L);
int _simClearStringSignal(lua_State* L);
int _simGetSignalName(lua_State* L);
int _simWaitForSignal(lua_State* L);

int _simPersistentDataWrite(lua_State* L);
int _simPersistentDataRead(lua_State* L);

int _simSetObjectProperty(lua_State* L);
int _simGetObjectProperty(lua_State* L);

int _simSetObjectSpecialProperty(lua_State* L);
int _simGetObjectSpecialProperty(lua_State* L);

int _simSetModelProperty(lua_State* L);
int _simGetModelProperty(lua_State* L);

int _simMoveToPosition(lua_State* L);
int _simMoveToObject(lua_State* L);
int _simFollowPath(lua_State* L);
int _simMoveToJointPositions(lua_State* L);

int _simWait(lua_State* L);
int _simDelegateChildScriptExecution(lua_State* L);

int _simGetDataOnPath(lua_State* L);
int _simGetPositionOnPath(lua_State* L);
int _simGetOrientationOnPath(lua_State* L);
int _simGetClosestPositionOnPath(lua_State* L);

int _simReadForceSensor(lua_State* L);
int _simBreakForceSensor(lua_State* L);

int _simGetShapeVertex(lua_State* L);
int _simGetShapeTriangle(lua_State* L);

int _simGetLightParameters(lua_State* L);
int _simSetLightParameters(lua_State* L);

int _simHandleVarious(lua_State* L);

int _simSetShapeColor(lua_State* L);
int _simGetShapeColor(lua_State* L);
int _simResetDynamicObject(lua_State* L);
int _simSetJointMode(lua_State* L);
int _simGetJointMode(lua_State* L);

int _simGetContactInfo(lua_State* L);
int _simSetThreadIsFree(lua_State* L);

int _simTubeOpen(lua_State* L);
int _simTubeClose(lua_State* L);
int _simTubeWrite(lua_State* L);
int _simTubeRead(lua_State* L);
int _simTubeStatus(lua_State* L);

int _simAuxiliaryConsoleOpen(lua_State* L);
int _simAuxiliaryConsoleClose(lua_State* L);
int _simAuxiliaryConsolePrint(lua_State* L);
int _simAuxiliaryConsoleShow(lua_State* L);

int _simImportShape(lua_State* L);
int _simImportMesh(lua_State* L);
int _simExportMesh(lua_State* L);
int _simCreateMeshShape(lua_State* L);
int _simGetShapeMesh(lua_State* L);
int _simCreatePureShape(lua_State* L);
int _simCreateHeightfieldShape(lua_State* L);

int _simAddBanner(lua_State* L);
int _simRemoveBanner(lua_State* L);

int _simCreateJoint(lua_State* L);
int _simCreateDummy(lua_State* L);
int _simCreateProximitySensor(lua_State* L);
int _simCreatePath(lua_State* L);
int _simInsertPathCtrlPoints(lua_State* L);
int _simCutPathCtrlPoints(lua_State* L);
int _simGetIkGroupMatrix(lua_State* L);
int _simCreateForceSensor(lua_State* L);
int _simCreateVisionSensor(lua_State* L);

int _simFloatingViewAdd(lua_State* L);
int _simFloatingViewRemove(lua_State* L);
int _simAdjustView(lua_State* L);
int _simCameraFitToView(lua_State* L);

// ***** Following 2 are DEPRECATED:
int _simGetInstanceIndex(lua_State* L);
int _simGetVisibleInstanceIndex(lua_State* L);
// *****


int _simGetObjectIntParameter(lua_State* L);
int _simSetObjectIntParameter(lua_State* L);
int _simGetObjectFloatParameter(lua_State* L);
int _simSetObjectFloatParameter(lua_State* L);
int _simGetObjectStringParameter(lua_State* L);
int _simSetObjectStringParameter(lua_State* L);

int _simGetRotationAxis(lua_State* L);
int _simRotateAroundAxis(lua_State* L);

int _simLaunchExecutable(lua_State* L);

int _simGetJointForce(lua_State* L);
int _simJointGetForce(lua_State* L);

int _simSetIkGroupProperties(lua_State* L);
int _simSetIkElementProperties(lua_State* L);


int _simAddSceneCustomData(lua_State* L);
int _simGetSceneCustomData(lua_State* L);
int _simAddObjectCustomData(lua_State* L);
int _simGetObjectCustomData(lua_State* L);

int _simIsHandleValid(lua_State* L);


int _simHandleVisionSensor(lua_State* L);
int _simReadVisionSensor(lua_State* L);
int _simResetVisionSensor(lua_State* L);
int _simGetVisionSensorResolution(lua_State* L);
int _simGetVisionSensorImage(lua_State* L);
int _simSetVisionSensorImage(lua_State* L);
int _simGetVisionSensorDepthBuffer(lua_State* L);
int _simCheckVisionSensor(lua_State* L);
int _simCheckVisionSensorEx(lua_State* L);

int _simRMLPosition(lua_State* L); // deprecated since 23/5/2014
int _simRMLVelocity(lua_State* L); // deprecated since 23/5/2014
int _simRMLPos(lua_State* L);
int _simRMLVel(lua_State* L);
int _simRMLStep(lua_State* L);
int _simRMLRemove(lua_State* L);

int _simRMLMoveToPosition(lua_State* L);
int _simRMLMoveToJointPositions(lua_State* L);

int _simGetObjectQuaternion(lua_State* L);
int _simSetObjectQuaternion(lua_State* L);

int _simSetShapeMassAndInertia(lua_State* L);
int _simGetShapeMassAndInertia(lua_State* L);

int _simGroupShapes(lua_State* L);
int _simUngroupShape(lua_State* L);
int _simConvexDecompose(lua_State* L);

int _simGetMotionPlanningHandle(lua_State* L);
int _simGetMpConfigForTipPose(lua_State* L);
int _simFindMpPath(lua_State* L);
int _simSimplifyMpPath(lua_State* L);
int _simFindIkPath(lua_State* L);
int _simGetMpConfigTransition(lua_State* L);

int _simAddGhost(lua_State* L);
int _simModifyGhost(lua_State* L);

int _simQuitSimulator(lua_State* L);
int _simGetThreadId(lua_State* L);

int _simGetMaterialId(lua_State* L);
int _simSetShapeMaterial(lua_State* L);
int _simGetShapeMaterial(lua_State* L);

int _simGetTextureId(lua_State* L);
int _simReadTexture(lua_State* L);
int _simWriteTexture(lua_State* L);
int _simCreateTexture(lua_State* L);

int _simWriteCustomDataBlock(lua_State* L);
int _simReadCustomDataBlock(lua_State* L);

int _simAddPointCloud(lua_State* L);
int _simModifyPointCloud(lua_State* L);

int _simGetShapeGeomInfo(lua_State* L);


int _simBuildMatrixQ(lua_State* L);
int _simGetQuaternionFromMatrix(lua_State* L);
int _simFileDialog(lua_State* L);
int _simMsgBox(lua_State* L);

int _simEnableWorkThreads(lua_State* L);
int _simWaitForWorkThreads(lua_State* L);


int _simTest(lua_State* L);



struct SLuaCommands
{
	std::string name;
	lua_CFunction func;
	std::string callTip;
};
struct SLuaVariables
{
	std::string name;
	int val;
};

static SLuaCommands simLuaCommands[]= 
{
	{"simHandleChildScript",_simHandleChildScript,"number executedScriptCount=simHandleChildScript(sim_handle_all/sim_handle_all_except_explicit,...<objects to be passed>)\n<returned values from child script>=simHandleChildScript(number scriptHandle,...<objects to be passed>)"},
	{"simHandleSensingChildScripts",_simHandleSensingChildScripts,"number executedScriptCount=simHandleSensingChildScripts()"},
	{"simGetScriptName",_simGetScriptName,"string scriptName=simGetScriptName(number scriptHandle)"},

	{"simGetObjectAssociatedWithScript",_simGetObjectAssociatedWithScript,"number objectHandle=simGetObjectAssociatedWithScript(number scriptHandle)"},
	{"simGetScriptAssociatedWithObject",_simGetScriptAssociatedWithObject,"number scriptHandle=simGetScriptAssociatedWithObject(number objectHandle)"},


	{"simGetScriptExecutionCount",_simGetScriptExecutionCount,"number executionCount=simGetScriptExecutionCount()"},
	{"simIsScriptExecutionThreaded",_simIsScriptExecutionThreaded,"number result=simIsScriptExecutionThreaded()"},
	{"simIsScriptRunningInThread",_simIsScriptRunningInThread,"number result=simIsScriptRunningInThread()"},
	{"simOpenModule",_simOpenModule,"number result=simOpenModule(sim_handle_all)\nnumber result=simOpenModule(string moduleName)"},
	{"simCloseModule",_simCloseModule,"number result=simCloseModule(sim_handle_all)\nnumber result=simCloseModule(string moduleName)"},
	{"simHandleModule",_simHandleModule,"number result=simHandleModule(sim_handle_all)\nnumber result=simHandleModule(string moduleName)"},
	{"simBoolOr16",_simBoolOr16,"number result=simBoolOr16(number value1,number value2)"},
	{"simBoolAnd16",_simBoolAnd16,"number result=simBoolAnd16(number value1,number value2)"},
	{"simBoolXor16",_simBoolXor16,"number result=simBoolXor16(number value1,number value2)"},

	{"simHandleDynamics",_simHandleDynamics,"number result=simHandleDynamics(number deltaTime)"},
	{"simHandleIkGroup",_simHandleIkGroup,"number performedIkGroupCalculationCount=simHandleIkGroup(number ikGroupHandle)"},
	{"simCheckIkGroup",_simCheckIkGroup,"number ikCalculationResult,table jointValues=simCheckIkGroup(number ikGroupHandle,table jointHandles)"},
	{"simHandleCollision",_simHandleCollision,"number collisionCount=simHandleCollision(number collisionObjectHandle)"},
	{"simReadCollision",_simReadCollision,"number collisionState=simReadCollision(number collisionObjectHandle)"},
	{"simHandleDistance",_simHandleDistance,"number result,number smallestDistance=simHandleDistance(number distanceObjectHandle)"},
	{"simReadDistance",_simReadDistance,"number result,number smallestDistance=simReadDistance(number distanceObjectHandle)"},
	{"simHandleProximitySensor",_simHandleProximitySensor,"number result,number distance,table_3 detectedPoint,number detectedObjectHandle,table_3 normalVector=simHandleProximitySensor(number sensorHandle)"},
	{"simReadProximitySensor",_simReadProximitySensor,"number result,number distance,table_3 detectedPoint,number detectedObjectHandle,table_3 normalVector=simReadProximitySensor(number sensorHandle)"},
	{"simHandleMill",_simHandleMill,"number cutCount,table_2 removedSurfaceAndVolume=simHandleMill(number millHandle)"},
	{"simResetCollision",_simResetCollision,"number result=simResetCollision(number collisionObjectHandle)"},
	{"simResetDistance",_simResetDistance,"number result=simResetDistance(number distanceObjectHandle)"},
	{"simResetProximitySensor",_simResetProximitySensor,"number result=simResetProximitySensor(number objectHandle)"},
	{"simResetMill",_simResetMill,"number result=simResetMill(number objectHandle)"},
	{"simCheckProximitySensor",_simCheckProximitySensor,"number result,number distance,table_3 detectedPoint=simCheckProximitySensor(number sensorHandle,number entityHandle)"},
	{"simCheckProximitySensorEx",_simCheckProximitySensorEx,"number result,number distance,table_3 detectedPoint,number detectedObjectHandle,table_3 normalVector=\nsimCheckProximitySensorEx(number sensorHandle,number entityHandle,number mode,number threshold,number maxAngle)"},
	{"simCheckProximitySensorEx2",_simCheckProximitySensorEx2,"number result,number distance,table_3 detectedPoint,table_3 normalVector=\nsimCheckProximitySensorEx2(number sensorHandle,table vertices,number itemType,number itemCount,number mode,number threshold,number maxAngle)"},

	{"simGetNameSuffix",_simGetNameSuffix,"number suffix,string name=simGetNameSuffix(string nameWithSuffix)"},
	{"simSetNameSuffix",_simSetNameSuffix,"number result=simSetNameSuffix(number nameSuffixNumber)"}, 
	
	{"simGetObjectHandle",_simGetObjectHandle,"number objectHandle=simGetObjectHandle(string objectName)"},

	{"simGetScriptHandle",_simGetScriptHandle,"number scriptHandle=simGetScriptHandle(string scriptName=nil)"},
	{"simGetCollectionHandle",_simGetCollectionHandle,"number collectionHandle=simGetCollectionHandle(string collectionName)"},
	{"simRemoveCollection",_simRemoveCollection,"number result=simRemoveCollection(number collectionHandle)"},
	{"simGetObjectPosition",_simGetObjectPosition,"table_3 position=simGetObjectPosition(number objectHandle,number relativeToObjectHandle)"},
	{"simGetObjectOrientation",_simGetObjectOrientation,"table_3 eulerAngles=simGetObjectOrientation(number objectHandle,number relativeToObjectHandle)"},
	{"simSetObjectPosition",_simSetObjectPosition,"number result=simSetObjectPosition(number objectHandle,number relativeToObjectHandle,table_3 position)"},
	{"simSetObjectOrientation",_simSetObjectOrientation,"number result=simSetObjectOrientation(number objectHandle,number relativeToObjectHandle,table_3 eulerAngles)"},
	{"simGetJointPosition",_simGetJointPosition,"number position=simGetJointPosition(number objectHandle)"},
	{"simSetJointPosition",_simSetJointPosition,"number result=simSetJointPosition(number objectHandle,number position)"},
	{"simSetJointTargetPosition",_simSetJointTargetPosition,"number result=simSetJointTargetPosition(number objectHandle,number targetPosition)"},
	{"simGetJointTargetPosition",_simGetJointTargetPosition,"number result,number targetPosition=simGetJointTargetPosition(number objectHandle)"},
	{"simSetJointForce",_simSetJointForce,"number result=simSetJointForce(number objectHandle,number forceOrTorque)"},
	{"simGetPathPosition",_simGetPathPosition,"number position=simGetPathPosition(number objectHandle)"},
	{"simSetPathPosition",_simSetPathPosition,"number result=simSetPathPosition(number objectHandle,number position)"},
	{"simGetPathLength",_simGetPathLength,"number length=simGetPathLength(number objectHandle)"},
	{"simSetJointTargetVelocity",_simSetJointTargetVelocity,"number result=simSetJointTargetVelocity(number objectHandle,number targetVelocity)"},
	{"simGetJointTargetVelocity",_simGetJointTargetVelocity,"number targetVelocity=simGetJointTargetVelocity(number objectHandle)"},
	{"simSetPathTargetNominalVelocity",_simSetPathTargetNominalVelocity,"number result=simSetPathTargetNominalVelocity(number objectHandle,number targetVelocity)"},
	{"simGetObjectName",_simGetObjectName,"string objectName=simGetObjectName(number objectHandle)"},
	{"simGetCollectionName",_simGetCollectionName,"string collectionName=simGetCollectionName(number collectionHandle)"},
	{"simRemoveObject",_simRemoveObject,"number result=simRemoveObject(number objectHandle)"},
	{"simGetSimulationTime",_simGetSimulationTime,"number simulationTime=simGetSimulationTime()"},
	{"simGetSimulationState",_simGetSimulationState,"number simulationState=simGetSimulationState()"},
	{"simGetSystemTime",_simGetSystemTime,"number systemTime=simGetSystemTime()"},
	{"simGetSystemTimeInMs",_simGetSystemTimeInMs,"number systemTimeOrTimeDiff=simGetSystemTimeInMs(number previousTime)"},
	{"simCheckCollision",_simCheckCollision,"number result=simCheckCollision(number entity1Handle,number entity2Handle)"},
	{"simCheckCollisionEx",_simCheckCollisionEx,"number segmentCount,table segmentData=simCheckCollisionEx(number entity1Handle,number entity2Handle)"},
	{"simCheckDistance",_simCheckDistance,"number result,table_7 distanceData=simCheckDistance(number entity1Handle,number entity2Handle,number threshold)"},
	{"simGetObjectConfiguration",_simGetObjectConfiguration,"number rawBufferHandle=simGetObjectConfiguration(number objectHandle)"},
	{"simSetObjectConfiguration",_simSetObjectConfiguration,"number result=simSetObjectConfiguration(number rawBufferHandle)"},
	{"simGetConfigurationTree",_simGetConfigurationTree,"number rawBufferHandle=simGetConfigurationTree(number objectHandle)"},
	{"simSetConfigurationTree",_simSetConfigurationTree,"number result=simSetConfigurationTree(number rawBufferHandle)"},
	{"simHandleMechanism",_simHandleMechanism,"number result=simHandleMechanism(number mechanismHandle)"},
	{"simGetSimulationTimeStep",_simGetSimulationTimeStep,"number timeStep=simGetSimulationTimeStep()"},
	{"simGetSimulatorMessage",_simGetSimulatorMessage,"number messageID,table_4 auxiliaryData=simGetSimulatorMessage()"},
	{"simResetPath",_simResetPath,"number result=simResetPath(number objectHandle)"},
	{"simHandlePath",_simHandlePath,"number result=simHandlePath(number objectHandle,number deltaTime)"},
	{"simResetJoint",_simResetJoint,"number result=simResetJoint(number objectHandle)"},
	{"simHandleJoint",_simHandleJoint,"number result=simHandleJoint(number objectHandle,number deltaTime)"},
	{"simResetTracing",_simResetTracing,"This function is deprecated"},
	{"simHandleTracing",_simHandleTracing,"This function is deprecated"},
	{"simResetGraph",_simResetGraph,"number result=simResetGraph(number objectHandle)"},
	{"simHandleGraph",_simHandleGraph,"number result=simHandleGraph(number objectHandle,number simulationTime)"},
	{"simAddStatusbarMessage",_simAddStatusbarMessage,"number result=simAddStatusbarMessage(string message)"},
	{"simGetLastError",_simGetLastError,"string lastError=simGetLastError()"},
	{"simGetObjects",_simGetObjects,"number objectHandle=simGetObjects(number index,number objectType)"},
	{"simRefreshDialogs",_simRefreshDialogs,"number result=simRefreshDialogs(number refreshDegree)"},
	{"simGetModuleName",_simGetModuleName,"string moduleName,number version=simGetModuleName(number index)"},
	{"simGetIkGroupHandle",_simGetIkGroupHandle,"number ikGroupHandle=simGetIkGroupHandle(string ikGroupName)"},
	{"simGetCollisionHandle",_simGetCollisionHandle,"number collisionObjectHandle=simGetCollisionHandle(string collisionObjectName)"},
	{"simGetDistanceHandle",_simGetDistanceHandle,"number distanceObjectHandle=simGetDistanceHandle(string distanceObjectName)"},
	{"simGetMechanismHandle",_simGetMechanismHandle,"number mechanismHandle=simGetMechanismHandle(string mechanismName)"},
	{"simGetPathPlanningHandle",_simGetPathPlanningHandle,"number pathPlanningObjectHandle=simGetPathPlanningHandle(string pathPlanningObjectName)"},
	{"simSearchPath",_simSearchPath,"number result=simSearchPath(number pathPlanningObjectHandle,number maximumSearchTime,number subTimeStep)"},

	{"simInitializePathSearch",_simInitializePathSearch,"number temporaryPathSearchObjectHandle=simInitializePathSearch(number pathPlanningObjectHandle,number maximumSearchTime,number searchTimeStep)"},
	{"simPerformPathSearchStep",_simPerformPathSearchStep,"number result=simPerformPathSearchStep(number temporaryPathSearchObjectHandle,boolean abortSearch)"},

	{"simGetScriptSimulationParameter",_simGetScriptSimulationParameter,"boolean/number/string parameterValue=simGetScriptSimulationParameter(number scriptHandle,string parameterName,boolean forceStringReturn=false)\ntable parameterValues,table scriptHandles=simGetScriptSimulationParameter(number targetScripts,string parameterName,boolean forceStringReturn=false)"},
	{"simSetScriptSimulationParameter",_simSetScriptSimulationParameter,"number setCount=simSetScriptSimulationParameter(number scriptHandle,string parameterName,string parameterValue)"},
	{"simDisplayDialog",_simDisplayDialog,"number genericDlgHandle,number UIHandle=simDisplayDialog(string title,string mainText,number style,boolean modal,string initTxt,table_6 titleColors,table_6 dlgColors)"},
	{"simGetDialogResult",_simGetDialogResult,"number result=simGetDialogResult(number genericDlgHandle)"},
	{"simGetDialogInput",_simGetDialogInput,"string input=simGetDialogInput(number genericDlgHandle)"},
	{"simEndDialog",_simEndDialog,"number result=simEndDialog(number genericDlgHandle)"},
	{"simStopSimulation",_simStopSimulation,"number result=simStopSimulation()"},
	{"simPauseSimulation",_simPauseSimulation,"number result=simPauseSimulation()"},
	{"simStartSimulation",_simStartSimulation,"number result=simStartSimulation()"},
	{"simGetObjectMatrix",_simGetObjectMatrix,"table_12 matrix=simGetObjectMatrix(number objectHandle,number relativeToObjectHandle)"},
	{"simSetObjectMatrix",_simSetObjectMatrix,"number result=simSetObjectMatrix(number objectHandle,number relativeToObjectHandle,table_12 matrix)"},
	{"simGetJointMatrix",_simGetJointMatrix,"table_12 matrix=simGetJointMatrix(number objectHandle)"},
	{"simSetSphericalJointMatrix",_simSetSphericalJointMatrix,"number result=simSetSphericalJointMatrix(number objectHandle,table_12 matrix"},
	{"simBuildIdentityMatrix",_simBuildIdentityMatrix,"table_12 matrix=simBuildIdentityMatrix()"},
	{"simCopyMatrix",_simCopyMatrix,"table_12 matrix=simCopyMatrix(table_12 matrixToCopy)"},
	{"simBuildMatrix",_simBuildMatrix,"table_12 matrix=simBuildMatrix(table_3 position,table_3 eulerAngles)"},
	{"simGetEulerAnglesFromMatrix",_simGetEulerAnglesFromMatrix,"table_3 eulerAngles=simGetEulerAnglesFromMatrix(table_12 matrix)"},
	{"simGetInvertedMatrix",_simGetInvertedMatrix,"table_12 invertexMatrix=simGetInvertedMatrix(table_12 matrix)"},
	{"simMultiplyMatrices",_simMultiplyMatrices,"table_12 resultMatrix=simMultiplyMatrices(table_12 matrixIn1,table_12 matrixIn2)"},
	{"simInterpolateMatrices",_simInterpolateMatrices,"table_12 resultMatrix=simInterpolateMatrices(table_12 matrixIn1,table_12 matrixIn2,number interpolFactor)"},

	{"simMultiplyVector",_simMultiplyVector,"table_3 resultVector=simMultiplyVector(table_12 matrix,table_3 vector)"},
	{"simGetObjectChild",_simGetObjectChild,"number childObjectHandle=simGetObjectChild(number objectHandle,number index)"},
	{"simGetObjectParent",_simGetObjectParent,"number parentObjectHandle=simGetObjectParent(number objectHandle)"},
	{"simSetObjectParent",_simSetObjectParent,"number result=simSetObjectParent(number objectHandle,number parentObjectHandle,boolean keepInPlace)"},
	{"simGetObjectType",_simGetObjectType,"number objectType=simGetObjectType(number objectHandle)"},
	{"simGetJointType",_simGetJointType,"number jointType=simGetJointType(number objectHandle)"},
	{"simSetBooleanParameter",_simSetBooleanParameter,"number result=simSetBooleanParameter(number parameter,boolean boolState)"},
	{"simGetBooleanParameter",_simGetBooleanParameter,"boolean boolState=simGetBooleanParameter(number parameter)"},
	{"simSetIntegerParameter",_simSetIntegerParameter,"number result=simSetIntegerParameter(number parameter,number intState)"},
	{"simGetIntegerParameter",_simGetIntegerParameter,"number intState=simGetIntegerParameter(number parameter)"},
	{"simSetFloatingParameter",_simSetFloatingParameter,"number result=simSetFloatingParameter(number parameter,number floatState)"},
	{"simGetFloatingParameter",_simGetFloatingParameter,"number floatState=simGetFloatingParameter(number parameter)"},

	{"simSetStringParameter",_simSetStringParameter,"number result=simSetStringParameter(number parameter,string stringState)"},
	{"simGetStringParameter",_simGetStringParameter,"string stringState=simGetStringParameter(number parameter)"},

	{"simSetArrayParameter",_simSetArrayParameter,"number result=simSetArrayParameter(number parameter,table arrayOfValues)"},
	{"simGetArrayParameter",_simGetArrayParameter,"table arrayOfValues=simGetArrayParameter(number parameter)"},


	{"simSetObjectName",_simSetObjectName,"number result=simSetObjectName(number objectHandle,string objectName)"},
	{"simSetCollectionName",_simSetCollectionName,"number result=simSetCollectionName(number collectionHandle,string collectionName)"},
	{"simGetJointInterval",_simGetJointInterval,"boolean cyclic,table_2 interval=simGetJointInterval(number objectHandle)"},
	{"simSetJointInterval",_simSetJointInterval,"number result=simSetJointInterval(number objectHandle,boolean cyclic,table_2 interval)"},
	{"simLoadScene",_simLoadScene,"number result=simLoadScene(string filename)"},
	{"simSaveScene",_simSaveScene,"number result=simSaveScene(string filename)"},
	{"simLoadModel",_simLoadModel,"number result=simLoadModel(string filename)"},
	{"simSaveModel",_simSaveModel,"number result=simSaveModel(number modelBaseHandle,string filename)"},
	{"simLoadUI",_simLoadUI,"table handles=simLoadUI(string filename)"},
	{"simSaveUI",_simSaveUI,"number result=simSaveUI(table uiHandles,string filename)"},

	{"simIsObjectInSelection",_simIsObjectInSelection,"number selectionState=simIsObjectInSelection(number objectHandle)"},
	{"simAddObjectToSelection",_simAddObjectToSelection,"number result=simAddObjectToSelection(number what,number objectHandle)\nnumber result=simAddObjectToSelection(table objectHandles)"},
	{"simRemoveObjectFromSelection",_simRemoveObjectFromSelection,"number result=simRemoveObjectFromSelection(number what,number objectHandle)\nnumber result=simRemoveObjectFromSelection(table objectHandles)"},
	{"simGetObjectSelectionSize",_simGetObjectSelectionSize,"number selectionSize=simGetObjectSelectionSize()"},
	{"simGetObjectLastSelection",_simGetObjectLastSelection,"number objectHandle=simGetObjectLastSelection()"},
	{"simGetObjectSelection",_simGetObjectSelection,"table selectedObjectHandles=simGetObjectSelection()"},
	{"simGetRealTimeSimulation",_simGetRealTimeSimulation,"number result=simGetRealTimeSimulation()"},
	{"simSetNavigationMode",_simSetNavigationMode,"number result=simSetNavigationMode(number navigationMode)"},
	{"simGetNavigationMode",_simGetNavigationMode,"number navigationMode=simGetNavigationMode()"},
	{"simSetPage",_simSetPage,"number result=simSetPage(number pageIndex)"},
	{"simGetPage",_simGetPage,"number pageIndex=simGetPage()"},
	{"simReleaseScriptRawBuffer",_simReleaseScriptRawBuffer,"number result=simReleaseScriptRawBuffer(number scriptHandle,number bufferHandle)"},

	{"simCopyPasteSelectedObjects",_simCopyPasteSelectedObjects,"number result=simCopyPasteSelectedObjects()"},
	{"simDeleteSelectedObjects",_simDeleteSelectedObjects,"number result=simDeleteSelectedObjects()"},
	{"simScaleSelectedObjects",_simScaleSelectedObjects,"number result=simScaleSelectedObjects(number scalingFactor,boolean scalePositionsToo)"},

	{"simGetObjectUniqueIdentifier",_simGetObjectUniqueIdentifier,"number uniqueIdentifier=simGetObjectUniqueIdentifier(number objectHandle)\ntable uniqueIdentifiers=simGetObjectUniqueIdentifier(sim_handle_all)"},

	{"simSetThreadAutomaticSwitch",_simSetThreadAutomaticSwitch,"number result=simSetThreadAutomaticSwitch(boolean automaticSwitch)"},
	{"simSetThreadSwitchTiming",_simSetThreadSwitchTiming,"number result=simSetThreadSwitchTiming(number deltaTimeInMilliseconds)"},
	{"simSetThreadResumeLocation",_simSetThreadResumeLocation,"number result=simSetThreadResumeLocation(number location,number order)"},
	{"simResumeThreads",_simResumeThreads,"number count=simResumeThreads(number location)"},
	{"simSwitchThread",_simSwitchThread,"number result=simSwitchThread()"},

	{"simSendData",_simSendData,"number result=simSendData(number targetID,number dataHeader,string dataName,string data,number antennaHandle=sim_handle_self,number actionRadius=100,number emissionAngle1=3.1415,number emissionAngle2=6.283,number persistence=0)"},
	{"simReceiveData",_simReceiveData,"string data,number senderID,number dataHeader,string dataName=simReceiveData(number dataHeader=-1,string dataName=nil,number antennaHandle=sim_handle_self,number index=-1)"},

	{"simPackInts",_simPackInts,"string data=simPackInts(table integerNumbers)"},
	{"simPackFloats",_simPackFloats,"string data=simPackFloats(table floatingNumbers)"},
	{"simUnpackInts",_simUnpackInts,"table integerNumbers=simUnpackInts(string data,number startIndex=0,number count=0)"},
	{"simUnpackFloats",_simUnpackFloats,"table floatingNumbers=simUnpackFloats(string data,number startIndex=0,number count=0)"},

	{"simGetVelocity",_simGetVelocity,"table_3 linearVelocity,table_3 angularVelocity=simGetVelocity(number shapeHandle)"},
	{"simGetObjectVelocity",_simGetObjectVelocity,"table_3 linearVelocity,table_3 angularVelocity=simGetObjectVelocity(number objectHandle)"},
	{"simAddForceAndTorque",_simAddForceAndTorque,"number result=simAddForceAndTorque(number shapeHandle,table_3 force=nil,table_3 torque=nil)"},
	{"simAddForce",_simAddForce,"number result=simAddForce(number shapeHandle,table_3 position,table_3 force)"},
	
	{"simSetExplicitHandling",_simSetExplicitHandling,"number result=simSetExplicitHandling(number generalObjectHandle,number explicitHandlingFlags)"},
	{"simGetExplicitHandling",_simGetExplicitHandling,"number explicitHandlingFlags=simGetExplicitHandling(number generalObjectHandle)"},

	{"simSetGraphUserData",_simSetGraphUserData,"number result=simSetGraphUserData(number graphHandle,string dataStreamName,number data)"},

	{"simAddDrawingObject",_simAddDrawingObject,"number drawingObjectHandle=simAddDrawingObject(number objectType,number size,number duplicateTolerance,number parentObjectHandle,number maxItemCount,table_3 ambient=nil,table_3 diffuse=nil,table_3 specular=nil,table_3 emission=nil)"},
	{"simRemoveDrawingObject",_simRemoveDrawingObject,"number result=simRemoveDrawingObject(number drawingObjectHandle)"},
	{"simAddDrawingObjectItem",_simAddDrawingObjectItem,"number result=simAddDrawingObjectItem(number drawingObjectHandle,table itemData)"},

	{"simAddParticleObject",_simAddParticleObject,"number particleObjectHandle=simAddParticleObject(number objectType,number size,number density,table params,number lifeTime,number maxItemCount,table_3 ambient=nil,table_3 diffuse=nil,table_3 specular=nil,table_3 emission=nil)"},
	{"simRemoveParticleObject",_simRemoveParticleObject,"number result=simRemoveParticleObject(number particleObjectHandle)"},
	{"simAddParticleObjectItem",_simAddParticleObjectItem,"number result=simAddParticleObjectItem(number particleObjectHandle,table itemData)"},


	{"simGetObjectSizeFactor",_simGetObjectSizeFactor,"number sizeFactor=simGetObjectSizeFactor(number ObjectHandle)"},


	{"simResetMilling",_simResetMilling,"number result=simResetMilling(number objectHandle)"},
	{"simApplyMilling",_simApplyMilling,"number result=simApplyMilling(number objectHandle)"},



	{"simSetIntegerSignal",_simSetIntegerSignal,"number result=simSetIntegerSignal(string signalName,number signalValue)"},
	{"simGetIntegerSignal",_simGetIntegerSignal,"number signalValue=simGetIntegerSignal(string signalName)"},
	{"simClearIntegerSignal",_simClearIntegerSignal,"number clearCount=simClearIntegerSignal(string signalName)"},

	{"simSetFloatSignal",_simSetFloatSignal,"number result=simSetFloatSignal(string signalName,number signalValue)"},
	{"simGetFloatSignal",_simGetFloatSignal,"number signalValue=simGetFloatSignal(string signalName)"},
	{"simClearFloatSignal",_simClearFloatSignal,"number clearCount=simClearFloatSignal(string signalName)"},

	{"simSetStringSignal",_simSetStringSignal,"number result=simSetStringSignal(string signalName,string signalValue)"},
	{"simGetStringSignal",_simGetStringSignal,"string signalValue=simGetStringSignal(string signalName)"},
	{"simClearStringSignal",_simClearStringSignal,"number clearCount=simClearStringSignal(string signalName)"},

	{"simGetSignalName",_simGetSignalName,"string signalName=simGetSignalName(number signalIndex,number signalType)"},
	{"simWaitForSignal",_simWaitForSignal,"number/string signalValue=simWaitForSignal(string signalName)"},

	{"simPersistentDataWrite",_simPersistentDataWrite,"number result=simPersistentDataWrite(string dataName,string dataValue,number options=0)"},
	{"simPersistentDataRead",_simPersistentDataRead,"string dataValue=simPersistentDataRead(string dataName)"},


	{"simSetObjectProperty",_simSetObjectProperty,"number result=simSetObjectProperty(number objectHandle,number property)"},
	{"simGetObjectProperty",_simGetObjectProperty,"number property=simGetObjectProperty(number objectHandle)"},

	{"simSetObjectSpecialProperty",_simSetObjectSpecialProperty,"number result=simSetObjectSpecialProperty(number objectHandle,number property)"},
	{"simGetObjectSpecialProperty",_simGetObjectSpecialProperty,"number property=simGetObjectSpecialProperty(number objectHandle)"},

	{"simSetModelProperty",_simSetModelProperty,"number result=simSetModelProperty(number objectHandle,number property)"},
	{"simGetModelProperty",_simGetModelProperty,"number property=simGetModelProperty(number objectHandle)"},

	{"simMoveToPosition",_simMoveToPosition,"number deltaTimeLeft=simMoveToPosition(number objectHandle,number relativeToObjectHandle,table_3 position,table_3 orientation,number velocity,number accel,table_2 distCalcMethod)"},
	{"simMoveToObject",_simMoveToObject,"number deltaTimeLeft=simMoveToObject(number objectHandle,number targetObjectHandle,number positionAndOrOrientation,number relativeDistanceOnPath,number velocity,number accel)"},
	{"simFollowPath",_simFollowPath,"number deltaTimeLeft=simFollowPath(number objectHandle,number pathHandle,number positionAndOrOrientation,number relativeDistanceOnPath,number velocity,number accel)"},
	{"simMoveToJointPositions",_simMoveToJointPositions,"number deltaTimeLeft=simMoveToJointPositions(table jointHandles,table jointPositions,number/table velocity,number/table accel,number angleToLinearCoeff)"},

	{"simWait",_simWait,"number deltaTimeLeft=simWait(number deltaTime,boolean simulationTime=true)"},
	{"simDelegateChildScriptExecution",_simDelegateChildScriptExecution,"number result=simDelegateChildScriptExecution()"},

	{"simGetDataOnPath",_simGetDataOnPath,"number auxFlags,table_4 auxChannels=simGetDataOnPath(number pathHandle,number relativeDistance)"},
	{"simGetPositionOnPath",_simGetPositionOnPath,"table_3 position=simGetPositionOnPath(number pathHandle,number relativeDistance)"},
	{"simGetOrientationOnPath",_simGetOrientationOnPath,"table_3 eulerAngles=simGetOrientationOnPath(number pathHandle,number relativeDistance)"},
	{"simGetClosestPositionOnPath",_simGetClosestPositionOnPath,"number positionOnPath=simGetClosestPositionOnPath(number pathHandle,table_3 absolutePosition)"},

	{"simReadForceSensor",_simReadForceSensor,"number result,table_3 forceVector,table_3 torqueVector=simReadForceSensor(number objectHandle)"},
	{"simBreakForceSensor",_simBreakForceSensor,"number result=simBreakForceSensor(number objectHandle)"},

	{"simGetShapeVertex",_simGetShapeVertex,"DEPRECATED. Use simGetShapeMesh instead"},//"number result,table_3 relativePosition=simGetShapeVertex(number shapeHandle,number groupElementIndex,number vertexIndex)"},
	{"simGetShapeTriangle",_simGetShapeTriangle,"DEPRECATED. Use simGetShapeMesh instead"},//"number result,table_3 vertexIndices,table_9 triangleNormals=simGetShapeTriangle(number shapeHandle,number groupElementIndex,number triangleIndex)"},

	{"simGetLightParameters",_simGetLightParameters,"number state,table_3 ambientPart,table_3 diffusePart,table_3 specular=simGetLightParameters(number lightHandle)"},
	{"simSetLightParameters",_simSetLightParameters,"number result=simSetLightParameters(number lightHandle,number state,table_3 ambientPart,table_3 diffusePart,table_3 specular)"},

	{"simHandleVarious",_simHandleVarious,"number result=simHandleVarious()"},

	{"simGetLinkDummy",_simGetLinkDummy,"number linkDummyHandle=simGetLinkDummy(number dummyHandle)"},
	{"simSetLinkDummy",_simSetLinkDummy,"number result=simSetLinkDummy(number dummyHandle,number linkDummyHandle)"},


	{"simSetShapeColor",_simSetShapeColor,"number result=simSetShapeColor(number shapeHandle,string colorName,number colorComponent,table_3 rgbData)"},
	{"simGetShapeColor",_simGetShapeColor,"number result,table_3 rgbData=simGetShapeColor(number shapeHandle,string colorName,number colorComponent)"},
	{"simResetDynamicObject",_simResetDynamicObject,"number result=simResetDynamicObject(number objectHandle)"},
	{"simSetJointMode",_simSetJointMode,"number result=simSetJointMode(number jointHandle,number jointMode,number options)"},
	{"simGetJointMode",_simGetJointMode,"number jointMode,number options=simGetJointMode(number jointHandle)"},

	{"simSerialOpen",_simSerialOpen,"number portHandle=simSerialOpen(string portString,number baudRate)"},
	{"simSerialClose",_simSerialClose,"number result=simSerialClose(number portHandle)"},
	{"simSerialSend",_simSerialSend,"number charsSent=simSerialSend(number portHandle,string data)"},
	{"simSerialRead",_simSerialRead,"string data=simSerialRead(number portHandle,number dataLengthToRead,boolean blockingOperation,string closingString='',number timeout=0)"},
	{"simSerialCheck",_simSerialCheck,"number byteCount=simSerialCheck(number portHandle)"},

	{"simGetContactInfo",_simGetContactInfo,"table_2 collidingObjects,table_3 collisionPoint,table_3 reactionForce=simGetContactInfo(number dynamicPass,number objectHandle,number index)"},
	{"simSetThreadIsFree",_simSetThreadIsFree,"number result=simSetThreadIsFree(boolean freeMode)"},

	{"simTubeOpen",_simTubeOpen,"number tubeHandle=simTubeOpen(number dataHeader,string dataName,number readBufferSize)"},
	{"simTubeClose",_simTubeClose,"number result=simTubeClose(number tubeHandle)"},
	{"simTubeWrite",_simTubeWrite,"number result=simTubeWrite(number tubeHandle,string data)"},
	{"simTubeRead",_simTubeRead,"string data=simTubeRead(number tubeHandle,boolean blockingOperation=false)"},
	{"simTubeStatus",_simTubeStatus,"number status,number readPacketsCount,number writePacketsCount=simTubeStatus(number tubeHandle)"},

	{"simAuxiliaryConsoleOpen",_simAuxiliaryConsoleOpen,"number consoleHandle=simAuxiliaryConsoleOpen(string title,number maxLines,number mode,table_2 position=nil,table_2 size=nil,table_3 textColor=nil,table_3 backgroundColor=nil)"},
	{"simAuxiliaryConsoleClose",_simAuxiliaryConsoleClose,"number result=simAuxiliaryConsoleClose(number consoleHandle)"},
	{"simAuxiliaryConsolePrint",_simAuxiliaryConsolePrint,"number result=simAuxiliaryConsolePrint(number consoleHandle,string text)"},
	{"simAuxiliaryConsoleShow",_simAuxiliaryConsoleShow,"number result=simAuxiliaryConsoleShow(number consoleHandle,Boolean showState)"},


	{"simImportShape",_simImportShape,"number shapeHandle=simImportShape(number fileformat,string pathAndFilename,number options,number identicalVerticeTolerance,number scalingFactor)"},
	{"simImportMesh",_simImportMesh,"table_of_table vertices,table_of_table indices,nil,table names=simImportMesh(number fileformat,string pathAndFilename,number options,number identicalVerticeTolerance,number scalingFactor)"},
	{"simExportMesh",_simExportMesh,"number result=simExportMesh(number fileformat,string pathAndFilename,number options,number scalingFactor,table_of_table vertices,table_of_table indices,nil,table names)"},
	{"simCreateMeshShape",_simCreateMeshShape,"number objectHandle=simCreateMeshShape(number options,number shadingAngle,table vertices,table indices)"},
	{"simGetShapeMesh",_simGetShapeMesh,"table vertices,table indices,table normals=simGetShapeMesh(number shapeHandle)"},
	{"simCreatePureShape",_simCreatePureShape,"number objectHandle=simCreatePureShape(number primitiveType,number options,table_3 sizes,number mass,table_2 precision=nil)"},
	{"simCreateHeightfieldShape",_simCreateHeightfieldShape,"number objectHandle=simCreateHeightfieldShape(number options,number shadingAngle,number xPointCount,number yPointCount,number xSize,table heights)"},


	{"simAddBanner",_simAddBanner,"number bannerID=simAddBanner(string label,number size,number options,table_6 positionAndEulerAngles=nil,number parentObjectHandle=nil,table_12 labelColors=nil,table_12 backgroundColors=nil)"},
	{"simRemoveBanner",_simRemoveBanner,"number result=simRemoveBanner(number bannerID)"},

	{"simCreateJoint",_simCreateJoint,"number jointHandle=simCreateJoint(number jointType,number jointMode,number options,table_2 sizes=nil,table_12 colorA=nil,table_12 colorB=nil)"},
	{"simCreateDummy",_simCreateDummy,"number dummyHandle=simCreateDummy(number size,table_12 color=nil)"},
	{"simCreateProximitySensor",_simCreateProximitySensor,"number sensorHandle=simCreateProximitySensor(number sensorType,number subType,number options,table_8 intParams,table_15 floatParams,table_48 color=nil)"},
	{"simCreatePath",_simCreatePath,"number pathHandle=simCreatePath(number attributes,table_3 intParams=nil,table_3 floatParams=nil,table_12 color=nil)"},
	{"simCreateForceSensor",_simCreateForceSensor,"number sensorHandle=simCreateForceSensor(number options,table_5 intParams,table_5 floatParams,table_24 color=nil)"},
	{"simCreateVisionSensor",_simCreateVisionSensor,"number sensorHandle=simCreateVisionSensor(number options,table_4 intParams,table_11 floatParams,table_48 color=nil)"},

	{"simInsertPathCtrlPoints",_simInsertPathCtrlPoints,"number result=simInsertPathCtrlPoints(number pathHandle,number options,number startIndex,number ptCnt,table ptData)"},
	{"simCutPathCtrlPoints",_simCutPathCtrlPoints,"number result=simCutPathCtrlPoints(number pathHandle,number startIndex,number ptCnt)"},
	{"simGetIkGroupMatrix",_simGetIkGroupMatrix,"table matrix,table_2 matrixSize=simGetIkGroupMatrix(number ikGroupHandle,number options)"},




	{"simFloatingViewAdd",_simFloatingViewAdd,"number floatingViewHandle=simFloatingViewAdd(number posX,number posY,number sizeX,number sizeY,number options)"},
	{"simFloatingViewRemove",_simFloatingViewRemove,"number result=simFloatingViewRemove(number floatingViewHandle)"},
	{"simAdjustView",_simAdjustView,"number result=simAdjustView(number viewHandleOrIndex,number associatedViewableObjectHandle,number options,string viewLabel=nil)"},
	{"simCameraFitToView",_simCameraFitToView,"number result=simCameraFitToView(number viewHandleOrIndex,table objectHandles=nil,simInt options=0,simFloat scaling=1)"},



	{"simGetInstanceIndex",_simGetInstanceIndex,"DEPRECATED. Returns 0."},//"number index=simGetInstanceIndex()"},
	{"simGetVisibleInstanceIndex",_simGetVisibleInstanceIndex,"DEPRECATED. Returns 0."},//"number index=simGetVisibleInstanceIndex()"},


	{"simGetObjectIntParameter",_simGetObjectIntParameter,"number result,number parameter=simGetObjectIntParameter(number objectHandle,number parameterID)"},
	{"simSetObjectIntParameter",_simSetObjectIntParameter,"number result=simSetObjectIntParameter(number objectHandle,number parameterID,number parameter)"},

	{"simGetObjectFloatParameter",_simGetObjectFloatParameter,"number result,number parameter=simGetObjectFloatParameter(number objectHandle,number parameterID)"},
	{"simSetObjectFloatParameter",_simSetObjectFloatParameter,"number result=simSetObjectFloatParameter(number objectHandle,number parameterID,number parameter)"},

	{"simGetObjectStringParameter",_simGetObjectStringParameter,"string parameter=simGetObjectStringParameter(number objectHandle,number parameterID)"},
	{"simSetObjectStringParameter",_simSetObjectStringParameter,"number result=simSetObjectStringParameter(number objectHandle,number parameterID,string parameter)"},

	{"simGetRotationAxis",_simGetRotationAxis,"table_3 axis,number angle=simGetRotationAxis(table_12 matrixStart,table_12 matrixGoal)"},
	{"simRotateAroundAxis",_simRotateAroundAxis,"table_12 matrixOut=simRotateAroundAxis(table_12 matrixIn,table_3 axis,table_3 axisPos,number angle)"},

	{"simLaunchExecutable",_simLaunchExecutable,"number result=simLaunchExecutable(string filename,string parameters='',number showStatus=1)"},

	{"simGetJointForce",_simGetJointForce,"number forceOrTorque=simGetJointForce(number jointHandle)"},


	{"simSetIkGroupProperties",_simSetIkGroupProperties,"number result=simSetIkGroupProperties(number ikGroupHandle,number resolutionMethod,number maxIterations,number damping)"},
	{"simSetIkElementProperties",_simSetIkElementProperties,"number result=simSetIkElementProperties(number ikGroupHandle,number tipDummyHandle,number constraints,table_2 precision=nil,table_2 weight=nil)"},

	{"simAddSceneCustomData",_simAddSceneCustomData,"number result=simAddSceneCustomData(number header,string data)"},
	{"simGetSceneCustomData",_simGetSceneCustomData,"string data=simGetSceneCustomData(number header)"},
	{"simAddObjectCustomData",_simAddObjectCustomData,"number result=simAddObjectCustomData(number objectHandle,number header,string data)"},
	{"simGetObjectCustomData",_simGetObjectCustomData,"string data=simGetObjectCustomData(number objectHandle,number header)"},

	{"simIsHandleValid",_simIsHandleValid,"number result=simIsHandleValid(number generalObjectHandle,number generalObjectType=-1)"},

	{"simGetObjectQuaternion",_simGetObjectQuaternion,"table_4 quaternion=simGetObjectQuaternion(number objectHandle,number relativeToObjectHandle)"},
	{"simSetObjectQuaternion",_simSetObjectQuaternion,"number result=simSetObjectQuaternion(number objectHandle,number relativeToObjectHandle,table_4 quaternion)"},

	{"simSetShapeMassAndInertia",_simSetShapeMassAndInertia,"number result=simSetShapeMassAndInertia(number shapeHandle,number mass,table_9 inertiaMatrix,table_3 centerOfMass,table_12 transformation=nil)"},
	{"simGetShapeMassAndInertia",_simGetShapeMassAndInertia,"number mass,table_9 inertiaMatrix,table_3 centerOfMass=simGetShapeMassAndInertia(number shapeHandle,table_12 transformation=nil)"},

	{"simGroupShapes",_simGroupShapes,"number shapeHandle=simGroupShapes(table shapeHandles)"},
	{"simUngroupShape",_simUngroupShape,"table simpleShapeHandles=simUngroupShape(number shapeHandle)"},
	{"simConvexDecompose",_simConvexDecompose,"number shapeHandle=simConvexDecompose(number shapeHandle,number options,table_4 intParams,table_3 floatParams)"},

	{"simGetMotionPlanningHandle",_simGetMotionPlanningHandle,"number motionPlanningObjectHandle=simGetMotionPlanningHandle(string motionPlanningObjectName)"},
	{"simGetMpConfigForTipPose",_simGetMpConfigForTipPose,"number trialsBeforeSuccess,table jointPositions=simGetMpConfigForTipPose(number motionPlanningObjectHandle,number options,\nnumber closeNodesDistance,number trialCount,table_7 tipPose,number maxTimeInMs=0,table referenceConfigs=nil,\ntable jointWeights=nil,table jointBehaviour=nil,number correctionPasses=0)"},
	{"simFindMpPath",_simFindMpPath,"table path,table confSpaceLengths,table_3 tipPositions,table_4 tipQuaternions,table cartesianSpaceLengths=simFindMpPath(\nnumber motionPlanningObjectHandle,table startConfig,table goalConfig,number options,number stepSize,\nnumber maxTimeInMs=0,table auxIntParams=nil,table auxFloatParams=nil)"},
	{"simSimplifyMpPath",_simSimplifyMpPath,"table path,table confSpaceLengths,table_3 tipPositions,table_4 tipQuaternions,table cartesianSpaceLengths=simSimplifyMpPath(\nnumber motionPlanningObjectHandle,table path,number options,number stepSize,\nnumber increment=1,number maxTimeInMs=0,table auxIntParams=nil,table auxFloatParams=nil)"},
	{"simFindIkPath",_simFindIkPath,"table path,table confSpaceLengths,table_3 tipPositions,table_4 tipQuaternions,table cartesianSpaceLengths=simFindIkPath(\nnumber motionPlanningObjectHandle,table startConfig,table goalPose,number options,number stepSize,\ntable auxIntParams=nil,table auxFloatParams=nil)"},
	{"simGetMpConfigTransition",_simGetMpConfigTransition,"table path,table confSpaceLengths,table_3 tipPositions,table_4 tipQuaternions,table cartesianSpaceLengths=simGetMpConfigTransition(\nnumber motionPlanningObjectHandle,table startConfig,table goalConfig,number options,table select,number calcStepSize,\nnumber maxOutStepSize,table wayPoints=nil)"},


	{"simAddGhost",_simAddGhost,"number ghostId=simAddGhost(number ghostGroup,number objectHandle,number options,number startTime,number endTime,table_12 color=nil)"},
	{"simModifyGhost",_simModifyGhost,"number result=simModifyGhost(number ghostGroup,number ghostId,number operation,number floatValue,number options=nil,number optionsMask=nil,table colorOrTransformation=nil)"},

	{"simQuitSimulator",_simQuitSimulator,"simQuitSimulator(boolean doNotDisplayMessages)"},
	{"simGetThreadId",_simGetThreadId,"number threadId=simGetThreadId()"},

	{"simGetMaterialId",_simGetMaterialId,"number materialId=simGetMaterialId(string materialName)"},
	{"simSetShapeMaterial",_simSetShapeMaterial,"number result=simSetShapeMaterial(number shapeHandle,number materialId)"},
	{"simGetShapeMaterial",_simGetShapeMaterial,"number materialId=simGetShapeMaterial(number shapeHandle)"},

	{"simGetTextureId",_simGetTextureId,"number textureId,table_2 resolution=simGetTextureId(string textureName)"},
	{"simReadTexture",_simReadTexture,"string textureData=simReadTexture(number textureId,number options,number posX=0,number posY=0,number sizeX=0,number sizeY=0)"},
	{"simWriteTexture",_simWriteTexture,"number result=simWriteTexture(number textureId,number options,string textureData,number posX=0,number posY=0,number sizeX=0,number sizeY=0,number interpol=0)"},
	{"simCreateTexture",_simCreateTexture,"number shapeHandle,number textureId,table_2 resolution=simCreateTexture(string fileName,number options,table_2 planeSizes=nil,table_2 scalingUV=nil,table_2 xy_g=nil,number fixedResolution=0,table_2 resolution=nil}"},

	{"simWriteCustomDataBlock",_simWriteCustomDataBlock,"number result=simWriteCustomDataBlock(number objectHandle,string dataName,string data)"},
	{"simReadCustomDataBlock",_simReadCustomDataBlock,"string data=simReadCustomDataBlock(number objectHandle,string dataName)"},


	{"simAddPointCloud",_simAddPointCloud,"number pointCloudHandle=simAddPointCloud(number pageMask,number layerMask,number objectHandle,number options,number pointSize,table pointCoordinates,table_12 defaultColors=nil,table pointColors=nil,table pointNormals=nil)"},
	{"simModifyPointCloud",_simModifyPointCloud,"number result=simModifyPointCloud(number pointCloudHandle,number operation)"},
	{"simGetShapeGeomInfo",_simGetShapeGeomInfo,"number result,number pureType,table_4 dimensions=simGetShapeGeomInfo(number shapeHandle)"},


	// Following deprecated:
	{"simGetSystemTimeInMilliseconds",_simGetSystemTimeInMilliseconds,"Deprecated. See simGetSystemTimeInMs instead."},
	{"simLockInterface",_simLockInterface,"Deprecated. Has no effect anymore."},
	{"simJointGetForce",_simJointGetForce,"Deprecated. See simGetJointForce instead."},

	// Following for backward compatibility (nov-dec 2011):
	// ********************************************************
	{"simGet2DElementHandle",_simGetUIHandle,"Deprecated. see simGetUIHandle instead."},
	{"simGet2DElementProperty",_simGetUIProperty,"Deprecated. see simGetUIProperty instead."},
	{"simGet2DElementEventButton",_simGetUIEventButton,"Deprecated. see simGetUIEventButton instead."},
	{"simSet2DElementProperty",_simSetUIProperty,"Deprecated. see simSetUIProperty instead."},
	{"simGet2DElementButtonProperty",_simGetUIButtonProperty,"Deprecated. see simGetUIButtonProperty instead."},
	{"simSet2DElementButtonProperty",_simSetUIButtonProperty,"Deprecated. see simSetUIButtonProperty instead."},
	{"simGet2DElementButtonSize",_simGetUIButtonSize,"Deprecated. see simGetUIButtonSize instead."},
	{"simSet2DElementButtonLabel",_simSetUIButtonLabel,"Deprecated. see simSetUIButtonLabel instead."},
	{"simGet2DElementButtonLabel",_simGetUIButtonLabel,"Deprecated. see simGetUIButtonLabel instead."},
	{"simSet2DElementSlider",_simSetUISlider,"Deprecated. see simSetUISlider instead."},
	{"simGet2DElementSlider",_simGetUISlider,"Deprecated. see simGetUISlider instead."},
	{"simSet2DElementButtonColor",_simSetUIButtonColor,"Deprecated. see simSetUIButtonColor instead."},
	{"simCreate2DElementButtonArray",_simCreateUIButtonArray,"Deprecated. see simCreateUIButtonArray instead."},
	{"simSet2DElementButtonArrayColor",_simSetUIButtonArrayColor,"Deprecated. see simSetUIButtonArrayColor instead."},
	{"simDelete2DElementButtonArray",_simDeleteUIButtonArray,"Deprecated. see simDeleteUIButtonArray instead."},
	{"simRemove2DElement",_simRemoveUI,"Deprecated. see simRemoveUI instead."},
	{"simCreate2DElement",_simCreateUI,"Deprecated. see simCreateUI instead."},
	{"simCreate2DElementButton",_simCreateUIButton,"Deprecated. see simCreateUIButton instead."},
	{"simGet2DElementPosition",_simGetUIPosition,"Deprecated. see simGetUIPosition instead."},
	{"simSet2DElementPosition",_simSetUIPosition,"Deprecated. see simSetUIPosition instead."},
	// ********************************************************

	// Following for backward compatibility (10/04/2012):
	// ********************************************************
	{"simSerialPortOpen",_simSerialPortOpen,"Deprecated. See simSerialOpen instead."},
	{"simSerialPortClose",_simSerialPortClose,"Deprecated. See simSerialClose instead."},
	{"simSerialPortSend",_simSerialPortSend,"Deprecated. See simSerialSend instead."},
	{"simSerialPortRead",_simSerialPortRead,"Deprecated. See simSerialRead instead."},
	// ********************************************************


	// Following for backward compatibility:
	// ********************************************************
	{"simHandleRenderingSensor",_simHandleVisionSensor,"Deprecated. see simHandleVisionSensor instead."},
	{"simReadRenderingSensor",_simReadVisionSensor,"Deprecated. see simReadVisionSensor instead."},
	{"simResetRenderingSensor",_simResetVisionSensor,"Deprecated. see simResetVisionSensor instead."},
	{"simGetRenderingSensorResolution",_simGetVisionSensorResolution,"Deprecated. see simGetVisionSensorResolution instead."},
	{"simGetRenderingSensorImage",_simGetVisionSensorImage,"Deprecated. see simGetVisionSensorImage instead."},
	{"simSetRenderingSensorImage",_simSetVisionSensorImage,"Deprecated. see simSetVisionSensorImage instead."},
	{"simGetRenderingSensorDepthBuffer",_simGetVisionSensorDepthBuffer,"Deprecated. see simGetVisionSensorDepthBuffer instead."},
	{"simCheckRenderingSensor",_simCheckVisionSensor,"Deprecated. see simCheckVisionSensor instead."},
	{"simCheckRenderingSensorEx",_simCheckVisionSensorEx,"Deprecated. see simCheckVisionSensorEx instead."},
	// ********************************************************


	// Following for backward compatibility (deprecated since 23/5/2014):
	// ********************************************************
	{"simRMLPosition",_simRMLPosition,"Deprecated. See simRMLPos instead."},
	{"simRMLVelocity",_simRMLVelocity,"Deprecated. See simRMLVel instead."},
	// ********************************************************

	{"simHandleVisionSensor",_simHandleVisionSensor,"number detectionCount,table auxiliaryValuesPacket1,table auxiliaryValuesPacket2,etc.=simHandleVisionSensor(number sensorHandle)"},
	{"simReadVisionSensor",_simReadVisionSensor,"number result,table auxiliaryValues=simReadVisionSensor(number sensorHandle)"},
	{"simResetVisionSensor",_simResetVisionSensor,"number result=simResetVisionSensor(number sensorHandle)"},
	{"simGetVisionSensorResolution",_simGetVisionSensorResolution,"table_2 resolution=simGetVisionSensorResolution(number sensorHandle)"},
	{"simGetVisionSensorImage",_simGetVisionSensorImage,"table/string imageBuffer=simGetVisionSensorImage(number sensorHandle,number posX=0,number posY=0,number sizeX=0,number sizeY=0,number returnType=0)"},
	{"simSetVisionSensorImage",_simSetVisionSensorImage,"number result=simSetVisionSensorImage(number sensorHandle,table imageBuffer)\nnumber result=simSetVisionSensorImage(number sensorHandle,string imageBuffer)"},
	{"simGetVisionSensorDepthBuffer",_simGetVisionSensorDepthBuffer,"table depthBuffer=simGetVisionSensorDepthBuffer(number sensorHandle,number posX=0,number posY=0,number sizeX=0,number sizeY=0)"},
	{"simCheckVisionSensor",_simCheckVisionSensor,"number result,table auxiliaryValuesPacket1,table auxiliaryValuesPacket2,etc.=simCheckVisionSensor(number sensorHandle,number entityHandle)"},
	{"simCheckVisionSensorEx",_simCheckVisionSensorEx,"table buffer=simCheckVisionSensorEx(number sensorHandle,number entityHandle,boolean returnImage)"},


	{"simGetUIHandle",_simGetUIHandle,"number uiHandle=simGetUIHandle(string uiName)"},
	{"simGetUIProperty",_simGetUIProperty,"number property=simGetUIProperty(number uiHandle)"},
	{"simGetUIEventButton",_simGetUIEventButton,"number buttonHandle, table_2 auxiliaryValues=simGetUIEventButton(number uiHandle)"},
	{"simSetUIProperty",_simSetUIProperty,"number result=simSetUIProperty(number uiHandle,number uiProperty)"},
	{"simGetUIButtonProperty",_simGetUIButtonProperty,"number property=simGetUIButtonProperty(number uiHandle,number buttonHandle)"},
	{"simSetUIButtonProperty",_simSetUIButtonProperty,"number result=simSetUIButtonProperty(number uiHandle,number buttonHandle,number buttonProperty)"},
	{"simGetUIButtonSize",_simGetUIButtonSize,"table_2 size=simGetUIButtonSize(number uiHandle,number buttonHandle)"},
	{"simSetUIButtonLabel",_simSetUIButtonLabel,"number result=simSetUIButtonLabel(number uiHandle,number buttonHandle,string upLabel=nil,string downLabel=nil)"},
	{"simGetUIButtonLabel",_simGetUIButtonLabel,"string label=simGetUIButtonLabel(number uiHandle,number buttonHandle)"},
	{"simSetUISlider",_simSetUISlider,"number result=simSetUISlider(number uiHandle,number buttonHandle,number position)"},
	{"simGetUISlider",_simGetUISlider,"number position=simGetUISlider(number uiHandle,number buttonHandle)"},
	{"simSetUIButtonColor",_simSetUIButtonColor,"number result=simSetUIButtonColor(number uiHandle,number buttonHandle,table_3 upColor=nil,table_3 downColor=nil,table_3 labelColor=nil)"},
	{"simCreateUIButtonArray",_simCreateUIButtonArray,"number result=simCreateUIButtonArray(number uiHandle,number buttonHandle)"},
	{"simSetUIButtonArrayColor",_simSetUIButtonArrayColor,"number result=simSetUIButtonArrayColor(number uiHandle,number buttonHandle,table_2 position,table_3 color)"},
	{"simDeleteUIButtonArray",_simDeleteUIButtonArray,"number result=simDeleteUIButtonArray(number uiHandle,number buttonHandle)"},
	{"simRemoveUI",_simRemoveUI,"number result=simRemoveUI(number uiHandle)"},
	{"simCreateUI",_simCreateUI,"number uiHandle,table buttonHandles=simCreateUI(string uiName,number menuAttributes,table_2 clientSize,table_2 cellSize)"},
	{"simCreateUIButton",_simCreateUIButton,"number buttonHandle=simCreateUIButton(number uiHandle,table_2 position,table_2 size,number buttonProperty)"},
	{"simGetUIPosition",_simGetUIPosition,"table_2 position=simGetUIPosition(number uiHandle)"},
	{"simSetUIPosition",_simSetUIPosition,"number result=simSetUIPosition(number uiHandle,table_2 position)"},

	{"simRMLPos",_simRMLPos,"number handle=simRMLPos(number dofs,number smallestTimeStep,number flags,table currentPosVelAccel,table maxVelAccelJerk,table selection,table targetPosVel)"},
	{"simRMLVel",_simRMLVel,"number handle=simRMLVel(number dofs,number smallestTimeStep,number flags,table currentPosVelAccel,table maxAccelJerk,table selection,table targetVel)"},
	{"simRMLStep",_simRMLStep,"number result,table newPosVelAccel,number synchronizationTime=simRMLStep(number handle,number timeStep)"},
	{"simRMLRemove",_simRMLRemove,"number result=simRMLRemove(number handle)"},

	{"simRMLMoveToPosition",_simRMLMoveToPosition,"number result,table_3 newPos,table_4 newQuaternion,table_4 newVel,table_4 newAccel,number timeLeft=simRMLMoveToPosition(number objectHandle,number relativeToObjectHandle,number flags,\ntable_4 currentVel,table_4 currentAccel,table_4 maxVel,table_4 maxAccel,table_4 maxJerk,table_3 targetPosition,table_4 targetQuaternion,table_4 targetVel)"},
	{"simRMLMoveToJointPositions",_simRMLMoveToJointPositions,"number result,table newPos,table newVel,table newAccel,number timeLeft=simRMLMoveToJointPositions(table jointHandles,number flags,\ntable currentVel,table currentAccel,table maxVel,table maxAccel,table maxJerk,table targetPos,table targetVel,table direction=nil)"},

	{"simBuildMatrixQ",_simBuildMatrixQ,"table_12 matrix=simBuildMatrixQ(table_3 position,table_4 quaternion)"},
	{"simGetQuaternionFromMatrix",_simGetQuaternionFromMatrix,"table_4 quaternion=simGetQuaternionFromMatrix(table_12 matrix)"},
	{"simFileDialog",_simFileDialog,"string pathAndName=simFileDialog(number mode,string title,string startPath,string initName,string extName,string ext)"},
	{"simMsgBox",_simMsgBox,"number returnValue=simMsgBox(number dlgType,number buttons,string title,string message)"},

	{"simEnableWorkThreads",_simEnableWorkThreads,"number effectiveThreadCnt=simEnableWorkThreads(number threadCnt)"},
	{"simWaitForWorkThreads",_simWaitForWorkThreads,"simWaitForWorkThreads()"},


	{"simTest",_simTest,"test function - shouldn't be used"},

	{"",NULL,""}
};


static SLuaVariables simLuaVariables[]= 
{
	// 3D object types (main types):
	{"sim_object_shape_type",sim_object_shape_type},
	{"sim_object_joint_type",sim_object_joint_type},
	{"sim_object_graph_type",sim_object_graph_type},
	{"sim_object_camera_type",sim_object_camera_type},
	{"sim_object_dummy_type",sim_object_dummy_type},
	{"sim_object_proximitysensor_type",sim_object_proximitysensor_type},
	{"sim_object_path_type",sim_object_path_type},
	{"sim_object_renderingsensor_type",sim_object_visionsensor_type},
	{"sim_object_visionsensor_type",sim_object_visionsensor_type},
	{"sim_object_volume_type",sim_object_volume_type},
	{"sim_object_mill_type",sim_object_mill_type},
	{"sim_object_forcesensor_type",sim_object_forcesensor_type},
	{"sim_object_light_type",sim_object_light_type},
	{"sim_object_mirror_type",sim_object_mirror_type},

	// 3D object sub-types:
	{"sim_light_omnidirectional_subtype",sim_light_omnidirectional_subtype},
	{"sim_light_spot_subtype",sim_light_spot_subtype},
	{"sim_light_directional_subtype",sim_light_directional_subtype},
	{"sim_joint_revolute_subtype",sim_joint_revolute_subtype},
	{"sim_joint_prismatic_subtype",sim_joint_prismatic_subtype},
	{"sim_joint_spherical_subtype",sim_joint_spherical_subtype},
	{"sim_shape_simpleshape_subtype",sim_shape_simpleshape_subtype},
	{"sim_shape_multishape_subtype",sim_shape_multishape_subtype},
	{"sim_proximitysensor_pyramid_subtype",sim_proximitysensor_pyramid_subtype},
	{"sim_proximitysensor_cylinder_subtype",sim_proximitysensor_cylinder_subtype},
	{"sim_proximitysensor_disc_subtype",sim_proximitysensor_disc_subtype},
	{"sim_proximitysensor_cone_subtype",sim_proximitysensor_cone_subtype},
	{"sim_proximitysensor_ray_subtype",sim_proximitysensor_ray_subtype},
	{"sim_mill_pyramid_subtype",sim_mill_pyramid_subtype},
	{"sim_mill_cylinder_subtype",sim_mill_cylinder_subtype},
	{"sim_mill_disc_subtype",sim_mill_disc_subtype},
	{"sim_mill_cone_subtype",sim_mill_cone_subtype},
	{"sim_object_no_subtype",sim_object_no_subtype},

	// Other object types:
	{"sim_appobj_object_type",sim_appobj_object_type},
	{"sim_appobj_collision_type",sim_appobj_collision_type},
	{"sim_appobj_distance_type",sim_appobj_distance_type},
	{"sim_appobj_simulation_type",sim_appobj_simulation_type},
	{"sim_appobj_ik_type",sim_appobj_ik_type},
	{"sim_appobj_constraintsolver_type",sim_appobj_constraintsolver_type},
	{"sim_appobj_collection_type",sim_appobj_collection_type},
	{"sim_appobj_2delement_type",sim_appobj_ui_type},  // for backward compatibility
	{"sim_appobj_ui_type",sim_appobj_ui_type},
	{"sim_appobj_script_type",sim_appobj_script_type},
	{"sim_appobj_pathplanning_type",sim_appobj_pathplanning_type},
	{"sim_appobj_texture_type",sim_appobj_texture_type},
	{"sim_appobj_motionplanning_type",sim_appobj_motionplanning_type},

	// IK calculation methods:
	{"sim_ik_pseudo_inverse_method",sim_ik_pseudo_inverse_method},
	{"sim_ik_damped_least_squares_method",sim_ik_damped_least_squares_method},
	{"sim_ik_jacobian_transpose_method",sim_ik_jacobian_transpose_method},

	// IK constraints:
	{"sim_ik_x_constraint",sim_ik_x_constraint},
	{"sim_ik_y_constraint",sim_ik_y_constraint},
	{"sim_ik_z_constraint",sim_ik_z_constraint},
	{"sim_ik_alpha_beta_constraint",sim_ik_alpha_beta_constraint},
	{"sim_ik_gamma_constraint",sim_ik_gamma_constraint},
	{"sim_ik_avoidance_constraint",sim_ik_avoidance_constraint},

	// IK calculation results:
	{"sim_ikresult_not_performed",sim_ikresult_not_performed},
	{"sim_ikresult_success",sim_ikresult_success},
	{"sim_ikresult_fail",sim_ikresult_fail},

	// Simulation messages:
	{"sim_message_ui_button_state_change",sim_message_ui_button_state_change},
	{"sim_message_model_loaded",sim_message_model_loaded},
	{"sim_message_scene_loaded",sim_message_scene_loaded},
	{"sim_message_object_selection_changed",sim_message_object_selection_changed},
	{"sim_message_keypress",sim_message_keypress},
	{"sim_message_bannerclicked",sim_message_bannerclicked},
	{"sim_message_prox_sensor_select_down",sim_message_prox_sensor_select_down},
	{"sim_message_prox_sensor_select_up",sim_message_prox_sensor_select_up},
	{"sim_message_pick_select_down",sim_message_pick_select_down},
	
	// Button properties:
	{"sim_buttonproperty_button",sim_buttonproperty_button},
	{"sim_buttonproperty_label",sim_buttonproperty_label},
	{"sim_buttonproperty_editbox",sim_buttonproperty_editbox},
	{"sim_buttonproperty_slider",sim_buttonproperty_slider},
	{"sim_buttonproperty_staydown",sim_buttonproperty_staydown},
	{"sim_buttonproperty_enabled",sim_buttonproperty_enabled},
	{"sim_buttonproperty_borderless",sim_buttonproperty_borderless},
	{"sim_buttonproperty_horizontallycentered",sim_buttonproperty_horizontallycentered},
	{"sim_buttonproperty_ignoremouse",sim_buttonproperty_ignoremouse},
	{"sim_buttonproperty_isdown",sim_buttonproperty_isdown},
	{"sim_buttonproperty_transparent",sim_buttonproperty_transparent},
	{"sim_buttonproperty_nobackgroundcolor",sim_buttonproperty_nobackgroundcolor},
	{"sim_buttonproperty_rollupaction",sim_buttonproperty_rollupaction},
	{"sim_buttonproperty_closeaction",sim_buttonproperty_closeaction},
	{"sim_buttonproperty_verticallycentered",sim_buttonproperty_verticallycentered},
	{"sim_buttonproperty_downupevent",sim_buttonproperty_downupevent},
	

	// UI properties:
	{"sim_ui_property_visible",sim_ui_property_visible},
	{"sim_ui_property_visibleduringsimulationonly",sim_ui_property_visibleduringsimulationonly},
	{"sim_ui_property_moveable",sim_ui_property_moveable},
	{"sim_ui_property_relativetoleftborder",sim_ui_property_relativetoleftborder},
	{"sim_ui_property_relativetotopborder",sim_ui_property_relativetotopborder},
	{"sim_ui_property_fixedwidthfont",sim_ui_property_fixedwidthfont},
	{"sim_ui_property_systemblock",sim_ui_property_systemblock},
	{"sim_ui_property_settocenter",sim_ui_property_settocenter},
	{"sim_ui_property_rolledup",sim_ui_property_rolledup},
	{"sim_ui_property_selectassociatedobject",sim_ui_property_selectassociatedobject},
	{"sim_ui_property_visiblewhenobjectselected",sim_ui_property_visiblewhenobjectselected},
	{"sim_ui_property_systemblockcanmovetofront",sim_ui_property_systemblockcanmovetofront},
	{"sim_ui_property_pauseactive",sim_ui_property_pauseactive},

	// for backward compatibility:
	{"sim_2delement_property_visible",sim_ui_property_visible},
	{"sim_2delement_property_visibleduringsimulationonly",sim_ui_property_visibleduringsimulationonly},
	{"sim_2delement_property_moveable",sim_ui_property_moveable},
	{"sim_2delement_property_relativetoleftborder",sim_ui_property_relativetoleftborder},
	{"sim_2delement_property_relativetotopborder",sim_ui_property_relativetotopborder},
	{"sim_2delement_property_fixedwidthfont",sim_ui_property_fixedwidthfont},
	{"sim_2delement_property_systemblock",sim_ui_property_systemblock},
	{"sim_2delement_property_settocenter",sim_ui_property_settocenter},
	{"sim_2delement_property_rolledup",sim_ui_property_rolledup},
	{"sim_2delement_property_selectassociatedobject",sim_ui_property_selectassociatedobject},
	{"sim_2delement_property_visiblewhenobjectselected",sim_ui_property_visiblewhenobjectselected},

	// object properties. Combine with the | operator
	{"sim_objectproperty_collapsed",sim_objectproperty_collapsed},
	{"sim_objectproperty_selectable",sim_objectproperty_selectable},
	{"sim_objectproperty_selectmodelbaseinstead",sim_objectproperty_selectmodelbaseinstead},
	{"sim_objectproperty_dontshowasinsidemodel",sim_objectproperty_dontshowasinsidemodel},
	{"sim_objectproperty_canupdatedna",sim_objectproperty_canupdatedna},
	{"sim_objectproperty_selectinvisible",sim_objectproperty_selectinvisible},
	{"sim_objectproperty_depthinvisible",sim_objectproperty_depthinvisible},

	// Simulation status:
	{"sim_simulation_stopped",sim_simulation_stopped},
	{"sim_simulation_paused",sim_simulation_paused},
	{"sim_simulation_advancing",sim_simulation_advancing},
	{"sim_simulation_advancing_firstafterstop",sim_simulation_advancing_firstafterstop},
	{"sim_simulation_advancing_running",sim_simulation_advancing_running},
	{"sim_simulation_advancing_lastbeforepause",sim_simulation_advancing_lastbeforepause},
	{"sim_simulation_advancing_firstafterpause",sim_simulation_advancing_firstafterpause},
	{"sim_simulation_advancing_abouttostop",sim_simulation_advancing_abouttostop},
	{"sim_simulation_advancing_lastbeforestop",sim_simulation_advancing_lastbeforestop},

	// Script execution result:
	{"sim_script_no_error",sim_script_no_error},
	{"sim_script_main_script_nonexistent",sim_script_main_script_nonexistent},
	{"sim_script_main_not_called",sim_script_main_script_not_called},
	{"sim_script_reentrance_error",sim_script_reentrance_error},
	{"sim_script_lua_error",sim_script_lua_error},
	{"sim_script_call_error",sim_script_call_error},

	// API call error messages
	{"sim_api_errormessage_ignore",sim_api_errormessage_ignore},
	{"sim_api_errormessage_report",sim_api_errormessage_report},
	{"sim_api_errormessage_output",sim_api_errormessage_output},

	// special arguments for some functions:
	{"sim_handle_all",sim_handle_all},
	{"sim_handle_all_except_explicit",sim_handle_all_except_explicit},
	{"sim_handle_self",sim_handle_self},
	{"sim_handle_main_script",sim_handle_main_script},
	{"sim_handle_tree",sim_handle_tree},
	{"sim_handle_chain",sim_handle_chain},
	{"sim_handle_single",sim_handle_single},
	{"sim_handle_default",sim_handle_default},
	{"sim_handle_all_except_self",sim_handle_all_except_self},
	{"sim_handle_parent",sim_handle_parent},
	{"sim_handle_scene",sim_handle_scene},

	// General object main properties:
	{"sim_objectspecialproperty_collidable",sim_objectspecialproperty_collidable},
	{"sim_objectspecialproperty_measurable",sim_objectspecialproperty_measurable},
	{"sim_objectspecialproperty_detectable_ultrasonic",sim_objectspecialproperty_detectable_ultrasonic},
	{"sim_objectspecialproperty_detectable_infrared",sim_objectspecialproperty_detectable_infrared},
	{"sim_objectspecialproperty_detectable_laser",sim_objectspecialproperty_detectable_laser},
	{"sim_objectspecialproperty_detectable_inductive",sim_objectspecialproperty_detectable_inductive},
	{"sim_objectspecialproperty_detectable_capacitive",sim_objectspecialproperty_detectable_capacitive},
	{"sim_objectspecialproperty_renderable",sim_objectspecialproperty_renderable},
	{"sim_objectspecialproperty_detectable_all",sim_objectspecialproperty_detectable_all},
	{"sim_objectspecialproperty_cuttable",sim_objectspecialproperty_cuttable},
	{"sim_objectspecialproperty_pathplanning_ignored",sim_objectspecialproperty_pathplanning_ignored},

	// Model override properties:
	{"sim_modelproperty_not_collidable",sim_modelproperty_not_collidable},
	{"sim_modelproperty_not_measurable",sim_modelproperty_not_measurable},
	{"sim_modelproperty_not_renderable",sim_modelproperty_not_renderable},
	{"sim_modelproperty_not_detectable",sim_modelproperty_not_detectable},
	{"sim_modelproperty_not_cuttable",sim_modelproperty_not_cuttable},
	{"sim_modelproperty_not_dynamic",sim_modelproperty_not_dynamic},
	{"sim_modelproperty_not_respondable",sim_modelproperty_not_respondable},
	{"sim_modelproperty_not_reset",sim_modelproperty_not_reset},
	{"sim_modelproperty_not_visible",sim_modelproperty_not_visible},
	{"sim_modelproperty_not_model",sim_modelproperty_not_model},
	
	// Generic dialog styles:
	{"sim_dlgstyle_message",sim_dlgstyle_message},
	{"sim_dlgstyle_input",sim_dlgstyle_input},
	{"sim_dlgstyle_ok",sim_dlgstyle_ok},
	{"sim_dlgstyle_ok_cancel",sim_dlgstyle_ok_cancel},
	{"sim_dlgstyle_yes_no",sim_dlgstyle_yes_no},
	{"sim_dlgstyle_dont_center",sim_dlgstyle_dont_center},

	// Generic dialog return value:
	{"sim_dlgret_still_open",sim_dlgret_still_open},
	{"sim_dlgret_ok",sim_dlgret_ok},
	{"sim_dlgret_cancel",sim_dlgret_cancel},
	{"sim_dlgret_yes",sim_dlgret_yes},
	{"sim_dlgret_no",sim_dlgret_no},

	// Path properties:	
	{"sim_pathproperty_show_line",sim_pathproperty_show_line},
	{"sim_pathproperty_show_orientation",sim_pathproperty_show_orientation},
	{"sim_pathproperty_closed_path",sim_pathproperty_closed_path},
	{"sim_pathproperty_automatic_orientation",sim_pathproperty_automatic_orientation},
	{"sim_pathproperty_invert_velocity",sim_pathproperty_invert_velocity},
	{"sim_pathproperty_infinite_acceleration",sim_pathproperty_infinite_acceleration},
	{"sim_pathproperty_flat_path",sim_pathproperty_flat_path},
	{"sim_pathproperty_show_position",sim_pathproperty_show_position},
	{"sim_pathproperty_auto_velocity_profile_translation",sim_pathproperty_auto_velocity_profile_translation},
	{"sim_pathproperty_auto_velocity_profile_rotation",sim_pathproperty_auto_velocity_profile_rotation},
	{"sim_pathproperty_endpoints_at_zero",sim_pathproperty_endpoints_at_zero},
	{"sim_pathproperty_keep_x_up",sim_pathproperty_keep_x_up},

	// distance calculation methods:
	{"sim_distcalcmethod_dl",sim_distcalcmethod_dl},
	{"sim_distcalcmethod_dac",sim_distcalcmethod_dac},
	{"sim_distcalcmethod_max_dl_dac",sim_distcalcmethod_max_dl_dac},
	{"sim_distcalcmethod_dl_and_dac",sim_distcalcmethod_dl_and_dac},
	{"sim_distcalcmethod_sqrt_dl2_and_dac2",sim_distcalcmethod_sqrt_dl2_and_dac2},
	{"sim_distcalcmethod_dl_if_nonzero",sim_distcalcmethod_dl_if_nonzero},
	{"sim_distcalcmethod_dac_if_nonzero",sim_distcalcmethod_dac_if_nonzero},

	// Boolean parameters:	
	{"sim_boolparam_hierarchy_visible",sim_boolparam_hierarchy_visible},
	{"sim_boolparam_console_visible",sim_boolparam_console_visible},
	{"sim_boolparam_collision_handling_enabled",sim_boolparam_collision_handling_enabled},
	{"sim_boolparam_distance_handling_enabled",sim_boolparam_distance_handling_enabled},
	{"sim_boolparam_ik_handling_enabled",sim_boolparam_ik_handling_enabled},
	{"sim_boolparam_gcs_handling_enabled",sim_boolparam_gcs_handling_enabled},
	{"sim_boolparam_dynamics_handling_enabled",sim_boolparam_dynamics_handling_enabled},
	{"sim_boolparam_joint_motion_handling_enabled",sim_boolparam_joint_motion_handling_enabled},
	{"sim_boolparam_path_motion_handling_enabled",sim_boolparam_path_motion_handling_enabled},
	{"sim_boolparam_proximity_sensor_handling_enabled",sim_boolparam_proximity_sensor_handling_enabled},
	{"sim_boolparam_vision_sensor_handling_enabled",sim_boolparam_vision_sensor_handling_enabled},
	{"sim_boolparam_rendering_sensor_handling_enabled",sim_boolparam_vision_sensor_handling_enabled},
	{"sim_boolparam_mill_handling_enabled",sim_boolparam_mill_handling_enabled},
	{"sim_boolparam_browser_visible",sim_boolparam_browser_visible},
	{"sim_boolparam_scene_and_model_load_messages",sim_boolparam_scene_and_model_load_messages},
	{"sim_boolparam_shape_textures_are_visible",sim_boolparam_shape_textures_are_visible},
	{"sim_boolparam_display_enabled",sim_boolparam_display_enabled},
	{"sim_boolparam_infotext_visible",sim_boolparam_infotext_visible},
	{"sim_boolparam_statustext_open",sim_boolparam_statustext_open},
	{"sim_boolparam_fog_enabled",sim_boolparam_fog_enabled},
	{"sim_boolparam_rml2_available",sim_boolparam_rml2_available},
	{"sim_boolparam_rml4_available",sim_boolparam_rml4_available},
	{"sim_boolparam_mirrors_enabled",sim_boolparam_mirrors_enabled},
	{"sim_boolparam_aux_clip_planes_enabled",sim_boolparam_aux_clip_planes_enabled},
	{"sim_boolparam_full_model_copy_from_api",sim_boolparam_full_model_copy_from_api},
	{"sim_boolparam_realtime_simulation",sim_boolparam_realtime_simulation},
	{"sim_boolparam_use_glfinish_cmd",sim_boolparam_use_glfinish_cmd},
	{"sim_boolparam_force_show_wireless_emission",sim_boolparam_force_show_wireless_emission},
	{"sim_boolparam_force_show_wireless_reception",sim_boolparam_force_show_wireless_reception},
	{"sim_boolparam_video_recording_triggered",sim_boolparam_video_recording_triggered},
	{"sim_boolparam_threaded_rendering_enabled",sim_boolparam_threaded_rendering_enabled},
	{"sim_boolparam_fullscreen",sim_boolparam_fullscreen},
	{"sim_boolparam_headless",sim_boolparam_headless},

	// Integer parameters:	
	{"sim_intparam_error_report_mode",sim_intparam_error_report_mode},
	{"sim_intparam_program_version",sim_intparam_program_version},
	{"sim_intparam_compilation_version",sim_intparam_compilation_version},
	{"sim_intparam_current_page",sim_intparam_current_page},
	{"sim_intparam_flymode_camera_handle",sim_intparam_flymode_camera_handle},
	{"sim_intparam_dynamic_step_divider",sim_intparam_dynamic_step_divider},
	{"sim_intparam_dynamic_engine",sim_intparam_dynamic_engine},
	{"sim_intparam_server_port_start",sim_intparam_server_port_start},
	{"sim_intparam_server_port_range",sim_intparam_server_port_range},
	{"sim_intparam_server_port_next",sim_intparam_server_port_next},
	{"sim_intparam_visible_layers",sim_intparam_visible_layers},
	{"sim_intparam_infotext_style",sim_intparam_infotext_style},
	{"sim_intparam_settings",sim_intparam_settings},
	{"sim_intparam_qt_version",sim_intparam_qt_version},
	{"sim_intparam_event_flags_read",sim_intparam_event_flags_read},
	{"sim_intparam_event_flags_read_clear",sim_intparam_event_flags_read_clear},
	{"sim_intparam_platform",sim_intparam_platform},
	{"sim_intparam_scene_unique_id",sim_intparam_scene_unique_id},
	{"sim_intparam_edit_mode_type",sim_intparam_edit_mode_type},
	{"sim_intparam_work_thread_count",sim_intparam_work_thread_count},
	{"sim_intparam_mouse_x",sim_intparam_mouse_x},
	{"sim_intparam_mouse_y",sim_intparam_mouse_y},
	{"sim_intparam_core_count",sim_intparam_core_count},
	{"sim_intparam_work_thread_calc_time_ms",sim_intparam_work_thread_calc_time_ms},
	{"sim_intparam_idle_fps",sim_intparam_idle_fps},
	{"sim_intparam_prox_sensor_select_down",sim_intparam_prox_sensor_select_down},
	{"sim_intparam_prox_sensor_select_up",sim_intparam_prox_sensor_select_up},
	{"sim_intparam_stop_request_counter",sim_intparam_stop_request_counter},
	{"sim_intparam_program_revision",sim_intparam_program_revision},
	{"sim_intparam_mouse_buttons",sim_intparam_mouse_buttons},
	

	// Float parameters:
	{"sim_floatparam_rand",sim_floatparam_rand},
	{"sim_floatparam_simulation_time_step",sim_floatparam_simulation_time_step},
	{"sim_floatparam_stereo_distance",sim_floatparam_stereo_distance},
	

	// Array parameters:
	{"sim_arrayparam_gravity",sim_arrayparam_gravity},
	{"sim_arrayparam_fog",sim_arrayparam_fog},
	{"sim_arrayparam_fog_color",sim_arrayparam_fog_color},
	{"sim_arrayparam_background_color1",sim_arrayparam_background_color1},
	{"sim_arrayparam_background_color2",sim_arrayparam_background_color2},

	// String parameters:
	{"sim_stringparam_application_path",sim_stringparam_application_path},
	{"sim_stringparam_video_filename",sim_stringparam_video_filename},
	{"sim_stringparam_app_arg1",sim_stringparam_app_arg1},
	{"sim_stringparam_app_arg2",sim_stringparam_app_arg2},
	{"sim_stringparam_app_arg3",sim_stringparam_app_arg3},
	{"sim_stringparam_app_arg4",sim_stringparam_app_arg4},
	{"sim_stringparam_app_arg5",sim_stringparam_app_arg5},
	{"sim_stringparam_app_arg6",sim_stringparam_app_arg6},
	{"sim_stringparam_app_arg7",sim_stringparam_app_arg7},
	{"sim_stringparam_app_arg8",sim_stringparam_app_arg8},
	{"sim_stringparam_app_arg9",sim_stringparam_app_arg9},
	{"sim_stringparam_scene_path_and_name",sim_stringparam_scene_path_and_name},

	// UI menu attributes:
	{"sim_ui_menu_title",sim_ui_menu_title},
	{"sim_ui_menu_minimize",sim_ui_menu_minimize},
	{"sim_ui_menu_close",sim_ui_menu_close},

	// for backward compatibility:
	{"sim_2delement_menu_title",sim_ui_menu_title},
	{"sim_2delement_menu_minimize",sim_ui_menu_minimize},
	{"sim_2delement_menu_close",sim_ui_menu_close},

	// Navigation and selection modes with the mouse:
	{"sim_navigation_passive",sim_navigation_passive},
	{"sim_navigation_camerashift",sim_navigation_camerashift},
	{"sim_navigation_camerarotate",sim_navigation_camerarotate},
	{"sim_navigation_camerazoom",sim_navigation_camerazoom},
	{"sim_navigation_cameratilt",sim_navigation_cameratilt},
	{"sim_navigation_cameraangle",sim_navigation_cameraangle},
	{"sim_navigation_camerafly",sim_navigation_camerafly},
	{"sim_navigation_objectshift",sim_navigation_objectshift},
	{"sim_navigation_objectrotate",sim_navigation_objectrotate},
	{"sim_navigation_createpathpoint",sim_navigation_createpathpoint},
	{"sim_navigation_clickselection",sim_navigation_clickselection},
	{"sim_navigation_ctrlselection",sim_navigation_ctrlselection},
	{"sim_navigation_shiftselection",sim_navigation_shiftselection},
	{"sim_navigation_camerazoomwheel",sim_navigation_camerazoomwheel},
	{"sim_navigation_camerarotaterightbutton",sim_navigation_camerarotaterightbutton},

	// drawing objects
	{"sim_drawing_points",sim_drawing_points},
	{"sim_drawing_lines",sim_drawing_lines},
	{"sim_drawing_triangles",sim_drawing_triangles},
	{"sim_drawing_trianglepoints",sim_drawing_trianglepoints},
	{"sim_drawing_quadpoints",sim_drawing_quadpoints},
	{"sim_drawing_discpoints",sim_drawing_discpoints},
	{"sim_drawing_cubepoints",sim_drawing_cubepoints},
	{"sim_drawing_spherepoints",sim_drawing_spherepoints},
	{"sim_drawing_itemcolors",sim_drawing_itemcolors},
	{"sim_drawing_vertexcolors",sim_drawing_vertexcolors},
	{"sim_drawing_itemsizes",sim_drawing_itemsizes},
	{"sim_drawing_backfaceculling",sim_drawing_backfaceculling},
	{"sim_drawing_wireframe",sim_drawing_wireframe},
	{"sim_drawing_painttag",sim_drawing_painttag},
	{"sim_drawing_followparentvisibility",sim_drawing_followparentvisibility},
	{"sim_drawing_cyclic",sim_drawing_cyclic},
	{"sim_drawing_50percenttransparency",sim_drawing_50percenttransparency},
	{"sim_drawing_25percenttransparency",sim_drawing_25percenttransparency},
	{"sim_drawing_12percenttransparency",sim_drawing_12percenttransparency},
	{"sim_drawing_emissioncolor",sim_drawing_emissioncolor},
	{"sim_drawing_facingcamera",sim_drawing_facingcamera},
	{"sim_drawing_overlay",sim_drawing_overlay},
	{"sim_drawing_itemtransparency",sim_drawing_itemtransparency},
	
	// Banners
	{"sim_banner_left",sim_banner_left},
	{"sim_banner_right",sim_banner_right},
	{"sim_banner_nobackground",sim_banner_nobackground},
	{"sim_banner_overlay",sim_banner_overlay},
	{"sim_banner_followparentvisibility",sim_banner_followparentvisibility},
	{"sim_banner_clickselectsparent",sim_banner_clickselectsparent},
	{"sim_banner_clicktriggersevent",sim_banner_clicktriggersevent},
	{"sim_banner_facingcamera",sim_banner_facingcamera},
	{"sim_banner_fullyfacingcamera",sim_banner_fullyfacingcamera},
	{"sim_banner_backfaceculling",sim_banner_backfaceculling},
	{"sim_banner_keepsamesize",sim_banner_keepsamesize},
	{"sim_banner_bitmapfont",sim_banner_bitmapfont},
	
	// particle objects
	{"sim_particle_points1",sim_particle_points1},
	{"sim_particle_points2",sim_particle_points2},
	{"sim_particle_points4",sim_particle_points4},
	{"sim_particle_roughspheres",sim_particle_roughspheres},
	{"sim_particle_spheres",sim_particle_spheres},
	{"sim_particle_respondable1to4",sim_particle_respondable1to4},
	{"sim_particle_respondable5to8",sim_particle_respondable5to8},
	{"sim_particle_particlerespondable",sim_particle_particlerespondable},
	{"sim_particle_ignoresgravity",sim_particle_ignoresgravity},
	{"sim_particle_invisible",sim_particle_invisible},
	{"sim_particle_painttag",sim_particle_painttag},
	{"sim_particle_itemsizes",sim_particle_itemsizes},
	{"sim_particle_itemdensities",sim_particle_itemdensities},
	{"sim_particle_itemcolors",sim_particle_itemcolors},
	{"sim_particle_cyclic",sim_particle_cyclic},
	{"sim_particle_emissioncolor",sim_particle_emissioncolor},
	{"sim_particle_water",sim_particle_water},

	// joint modes
	{"sim_jointmode_passive",sim_jointmode_passive},
	{"sim_jointmode_motion",sim_jointmode_motion},
	{"sim_jointmode_ik",sim_jointmode_ik},
	{"sim_jointmode_ikdependent",sim_jointmode_ikdependent},
	{"sim_jointmode_dependent",sim_jointmode_dependent},
	{"sim_jointmode_force",sim_jointmode_force},

	// file dialog styles
	{"sim_filedlg_type_load",sim_filedlg_type_load},
	{"sim_filedlg_type_save",sim_filedlg_type_save},

	// message box styles
	{"sim_msgbox_type_info",sim_msgbox_type_info},
	{"sim_msgbox_type_question",sim_msgbox_type_question},
	{"sim_msgbox_type_warning",sim_msgbox_type_warning},
	{"sim_msgbox_type_critical",sim_msgbox_type_critical},

	// message box buttons
	{"sim_msgbox_buttons_ok",sim_msgbox_buttons_ok},
	{"sim_msgbox_buttons_yesno",sim_msgbox_buttons_yesno},
	{"sim_msgbox_buttons_yesnocancel",sim_msgbox_buttons_yesnocancel},
	{"sim_msgbox_buttons_okcancel",sim_msgbox_buttons_okcancel},

	// message box return values
	{"sim_msgbox_return_cancel",sim_msgbox_return_cancel},
	{"sim_msgbox_return_no",sim_msgbox_return_no},
	{"sim_msgbox_return_yes",sim_msgbox_return_yes},
	{"sim_msgbox_return_ok",sim_msgbox_return_ok},
	{"sim_msgbox_return_error",sim_msgbox_return_error},

	// physics engine
	{"sim_physics_bullet",sim_physics_bullet},
	{"sim_physics_ode",sim_physics_ode},
	{"sim_physics_vortex",sim_physics_vortex},

	// pure primitives type
	{"sim_pure_primitive_none",sim_pure_primitive_none},
	{"sim_pure_primitive_plane",sim_pure_primitive_plane},
	{"sim_pure_primitive_disc",sim_pure_primitive_disc},
	{"sim_pure_primitive_cuboid",sim_pure_primitive_cuboid},
	{"sim_pure_primitive_spheroid",sim_pure_primitive_spheroid},
	{"sim_pure_primitive_cylinder",sim_pure_primitive_cylinder},
	{"sim_pure_primitive_cone",sim_pure_primitive_cone},
	{"sim_pure_primitive_heightfield",sim_pure_primitive_heightfield},


	{"",-1}
};




