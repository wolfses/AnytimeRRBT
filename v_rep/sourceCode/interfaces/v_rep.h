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

#if !defined(V_REP_INCLUDED_)
#define V_REP_INCLUDED_

#include "v_repConst.h"
#include "v_repTypes.h"

#ifndef VREP_DLLEXPORT
	#ifdef WIN_VREP
		#define VREP_DLLEXPORT extern "C" __declspec(dllexport)
	#endif
	#ifdef MAC_VREP
		#define VREP_DLLEXPORT extern "C" __attribute__((visibility("default")))
	#endif
	#ifdef LIN_VREP
		#define VREP_DLLEXPORT extern "C" __attribute__((visibility("default")))
	#endif
#endif

VREP_DLLEXPORT simInt simRunSimulator(const simChar* applicationName,simInt options,simVoid(*initCallBack)(),simVoid(*loopCallBack)(),simVoid(*deinitCallBack)());
VREP_DLLEXPORT simVoid* simGetMainWindow(simInt type);
VREP_DLLEXPORT simChar* simGetLastError();
VREP_DLLEXPORT simInt simSetBooleanParameter(simInt parameter,simBool boolState);
VREP_DLLEXPORT simInt simGetBooleanParameter(simInt parameter);
VREP_DLLEXPORT simInt simSetIntegerParameter(simInt parameter,simInt intState);
VREP_DLLEXPORT simInt simGetIntegerParameter(simInt parameter,simInt* intState);
VREP_DLLEXPORT simInt simSetFloatingParameter(simInt parameter,simFloat floatState);
VREP_DLLEXPORT simInt simGetFloatingParameter(simInt parameter,simFloat* floatState);
VREP_DLLEXPORT simInt simSetStringParameter(simInt parameter,const simChar* str);
VREP_DLLEXPORT simChar* simGetStringParameter(simInt parameter);
VREP_DLLEXPORT simInt simGetObjectHandle(const simChar* objectName);
VREP_DLLEXPORT simInt simRemoveObject(simInt objectHandle);
VREP_DLLEXPORT simChar* simGetObjectName(simInt objectHandle);
VREP_DLLEXPORT simInt simGetObjects(simInt index,simInt objectType);
VREP_DLLEXPORT simInt simSetObjectName(simInt objectHandle,const simChar* objectName);
VREP_DLLEXPORT simInt simGetCollectionHandle(const simChar* collectionName);
VREP_DLLEXPORT simInt simRemoveCollection(simInt collectionHandle);
VREP_DLLEXPORT simChar* simGetCollectionName(simInt collectionHandle);
VREP_DLLEXPORT simInt simSetCollectionName(simInt collectionHandle,const simChar* collectionName);
VREP_DLLEXPORT simInt simGetObjectMatrix(simInt objectHandle,simInt relativeToObjectHandle,simFloat* matrix);
VREP_DLLEXPORT simInt simSetObjectMatrix(simInt objectHandle,simInt relativeToObjectHandle,const simFloat* matrix);
VREP_DLLEXPORT simInt simGetObjectPosition(simInt objectHandle,simInt relativeToObjectHandle,simFloat* position);
VREP_DLLEXPORT simInt simSetObjectPosition(simInt objectHandle,simInt relativeToObjectHandle,const simFloat* position);
VREP_DLLEXPORT simInt simGetObjectOrientation(simInt objectHandle,simInt relativeToObjectHandle,simFloat* eulerAngles);
VREP_DLLEXPORT simInt simGetObjectQuaternion(simInt objectHandle,simInt relativeToObjectHandle,simFloat* quaternion);
VREP_DLLEXPORT simInt simSetObjectQuaternion(simInt objectHandle,simInt relativeToObjectHandle,const simFloat* quaternion);
VREP_DLLEXPORT simInt simSetObjectOrientation(simInt objectHandle,simInt relativeToObjectHandle,const simFloat* eulerAngles);
VREP_DLLEXPORT simInt simGetJointPosition(simInt objectHandle,simFloat* position);
VREP_DLLEXPORT simInt simSetJointPosition(simInt objectHandle,simFloat position);
VREP_DLLEXPORT simInt simSetJointTargetPosition(simInt objectHandle,simFloat targetPosition);
VREP_DLLEXPORT simInt simGetJointTargetPosition(simInt objectHandle,simFloat* targetPosition);
VREP_DLLEXPORT simInt simSetJointForce(simInt objectHandle,simFloat forceOrTorque);
VREP_DLLEXPORT simInt simGetPathPosition(simInt objectHandle,simFloat* position);
VREP_DLLEXPORT simInt simSetPathPosition(simInt objectHandle,simFloat position);
VREP_DLLEXPORT simInt simGetPathLength(simInt objectHandle,simFloat* length);
VREP_DLLEXPORT simInt simGetJointMatrix(simInt objectHandle,simFloat* matrix);
VREP_DLLEXPORT simInt simSetSphericalJointMatrix(simInt objectHandle,const simFloat* matrix);
VREP_DLLEXPORT simInt simGetJointInterval(simInt objectHandle,simBool* cyclic,simFloat* interval);
VREP_DLLEXPORT simInt simSetJointInterval(simInt objectHandle,simBool cyclic,const simFloat* interval);
VREP_DLLEXPORT simInt simGetObjectParent(simInt objectHandle);
VREP_DLLEXPORT simInt simGetObjectChild(simInt objectHandle,simInt index);
VREP_DLLEXPORT simInt simSetObjectParent(simInt objectHandle,simInt parentObjectHandle,simBool keepInPlace);
VREP_DLLEXPORT simInt simGetObjectType(simInt objectHandle);
VREP_DLLEXPORT simInt simGetJointType(simInt objectHandle);
VREP_DLLEXPORT simInt simBuildIdentityMatrix(simFloat* matrix);
VREP_DLLEXPORT simInt simCopyMatrix(const simFloat* matrixIn,simFloat* matrixOut);
VREP_DLLEXPORT simInt simBuildMatrix(const simFloat* position,const simFloat* eulerAngles,simFloat* matrix);
VREP_DLLEXPORT simInt simBuildMatrixQ(const simFloat* position,const simFloat* quaternion,simFloat* matrix);
VREP_DLLEXPORT simInt simGetEulerAnglesFromMatrix(const simFloat* matrix,simFloat* eulerAngles);
VREP_DLLEXPORT simInt simGetQuaternionFromMatrix(const simFloat* matrix,simFloat* quaternion);
VREP_DLLEXPORT simInt simInvertMatrix(simFloat* matrix);
VREP_DLLEXPORT simInt simMultiplyMatrices(const simFloat* matrixIn1,const simFloat* matrixIn2,simFloat* matrixOut);
VREP_DLLEXPORT simInt simInterpolateMatrices(const simFloat* matrixIn1,const simFloat* matrixIn2,simFloat interpolFactor,simFloat* matrixOut);
VREP_DLLEXPORT simInt simTransformVector(const simFloat* matrix,simFloat* vect);
VREP_DLLEXPORT simInt simReservedCommand(simInt v,simInt w);
VREP_DLLEXPORT simFloat simGetSimulationTime();
VREP_DLLEXPORT simInt simGetSimulationState();
VREP_DLLEXPORT simFloat simGetSystemTime();
VREP_DLLEXPORT simInt simGetSystemTimeInMilliseconds(); // deprecated
VREP_DLLEXPORT simUInt simGetSystemTimeInMs(simInt previousTime);
VREP_DLLEXPORT simInt simLoadScene(const simChar* filename);
VREP_DLLEXPORT simInt simCloseScene();
VREP_DLLEXPORT simInt simSaveScene(const simChar* filename);
VREP_DLLEXPORT simInt simLoadModel(const simChar* filename);
VREP_DLLEXPORT simInt simSaveModel(simInt baseOfModelHandle,const simChar* filename);
VREP_DLLEXPORT simInt simLoadUI(const simChar* filename,simInt maxCount,simInt* uiHandles);
VREP_DLLEXPORT simInt simSaveUI(simInt count,const simInt* uiHandles,const simChar* filename);
VREP_DLLEXPORT simInt simAddStatusbarMessage(const simChar* message);
VREP_DLLEXPORT simChar* simGetSimulatorMessage(simInt* messageID,simInt* auxiliaryData,simInt* returnedDataSize);
VREP_DLLEXPORT simInt simAddModuleMenuEntry(const simChar* entryLabel,simInt itemCount,simInt* itemHandles);
VREP_DLLEXPORT simInt simSetModuleMenuItemState(simInt itemHandle,simInt state,const simChar* label);
VREP_DLLEXPORT simInt simDoesFileExist(const simChar* filename);
VREP_DLLEXPORT simInt simIsObjectInSelection(simInt objectHandle);
VREP_DLLEXPORT simInt simAddObjectToSelection(simInt what,simInt objectHandle);
VREP_DLLEXPORT simInt simRemoveObjectFromSelection(simInt what,simInt objectHandle);
VREP_DLLEXPORT simInt simGetObjectSelectionSize();
VREP_DLLEXPORT simInt simGetObjectLastSelection();
VREP_DLLEXPORT simInt simGetObjectSelection(simInt* objectHandles);
VREP_DLLEXPORT simInt simSearchPath(simInt pathPlanningObjectHandle,simFloat maximumSearchTime);
VREP_DLLEXPORT simInt simInitializePathSearch(simInt pathPlanningObjectHandle,simFloat maximumSearchTime,simFloat searchTimeStep);
VREP_DLLEXPORT simInt simPerformPathSearchStep(simInt temporaryPathSearchObject,simBool abortSearch);
VREP_DLLEXPORT simInt simHandleCollision(simInt collisionObjectHandle);
VREP_DLLEXPORT simInt simReadCollision(simInt collisionObjectHandle);
VREP_DLLEXPORT simInt simHandleDistance(simInt distanceObjectHandle,simFloat* smallestDistance);
VREP_DLLEXPORT simInt simReadDistance(simInt distanceObjectHandle,simFloat* smallestDistance);
VREP_DLLEXPORT simInt simHandleProximitySensor(simInt sensorHandle,simFloat* detectedPoint,simInt* detectedObjectHandle,simFloat* normalVector);
VREP_DLLEXPORT simInt simReadProximitySensor(simInt sensorHandle,simFloat* detectedPoint,simInt* detectedObjectHandle,simFloat* normalVector);
VREP_DLLEXPORT simInt simHandleMill(simInt millHandle,simFloat* removedSurfaceAndVolume);
VREP_DLLEXPORT simInt simHandleIkGroup(simInt ikGroupHandle);
VREP_DLLEXPORT simInt simCheckIkGroup(simInt ikGroupHandle,simInt jointCnt,const simInt* jointHandles,simFloat* jointValues,simVoid* reserved);
VREP_DLLEXPORT simInt simHandleDynamics(simFloat deltaTime);
VREP_DLLEXPORT simInt simGetMechanismHandle(const simChar* mechanismName);
VREP_DLLEXPORT simInt simGetPathPlanningHandle(const simChar* pathPlanningObjectName);
VREP_DLLEXPORT simInt simGetMotionPlanningHandle(const simChar* motionPlanningObjectName);
VREP_DLLEXPORT simInt simGetMpConfigForTipPose(simInt motionPlanningObjectHandle,simInt options,simFloat closeNodesDistance,simInt trialCount,const simFloat* tipPose,simInt maxTimeInMs,simFloat* outputJointPositions,const simFloat* referenceConfigs,simInt referenceConfigCount,const simFloat* jointWeights,const simInt* jointBehaviour,simInt correctionPasses);
VREP_DLLEXPORT simFloat* simFindMpPath(simInt motionPlanningObjectHandle,const simFloat* startConfig,const simFloat* goalConfig,simInt options,simFloat stepSize,simInt* outputConfigsCnt,simInt maxTimeInMs,simFloat* reserved,const simInt* auxIntParams,const simFloat* auxFloatParams);
VREP_DLLEXPORT simFloat* simSimplifyMpPath(simInt motionPlanningObjectHandle,const simFloat* pathBuffer,simInt configCnt,simInt options,simFloat stepSize,simInt increment,simInt* outputConfigsCnt,simInt maxTimeInMs,simFloat* reserved,const simInt* auxIntParams,const simFloat* auxFloatParams);
VREP_DLLEXPORT simFloat* simFindIkPath(simInt motionPlanningObjectHandle,const simFloat* startConfig,const simFloat* goalPose,simInt options,simFloat stepSize,simInt* outputConfigsCnt,simFloat* reserved,const simInt* auxIntParams,const simFloat* auxFloatParams);
VREP_DLLEXPORT simFloat* simGetMpConfigTransition(simInt motionPlanningObjectHandle,const simFloat* startConfig,const simFloat* goalConfig,simInt options,const simInt* select,simFloat calcStepSize,simFloat maxOutStepSize,simInt wayPointCnt,const simFloat* wayPoints,simInt* outputConfigsCnt,const simInt* auxIntParams,const simFloat* auxFloatParams);
VREP_DLLEXPORT simInt simHandleMechanism(simInt mechanismHandle);
VREP_DLLEXPORT simInt simGetScriptHandle(const simChar* scriptName);
VREP_DLLEXPORT simInt simSetScriptText(simInt scriptHandle,const simChar* scriptText);
VREP_DLLEXPORT simChar* simGetScriptText(simInt scriptHandle);
VREP_DLLEXPORT simInt simGetScriptProperty(simInt scriptHandle,simInt* scriptProperty,simInt* associatedObjectHandle);
VREP_DLLEXPORT simInt simAssociateScriptWithObject(simInt scriptHandle,simInt associatedObjectHandle);
VREP_DLLEXPORT simInt simGetScript(simInt index);
VREP_DLLEXPORT simInt simGetScriptAssociatedWithObject(simInt objectHandle);
VREP_DLLEXPORT simInt simGetObjectAssociatedWithScript(simInt scriptHandle);
VREP_DLLEXPORT simChar* simGetScriptName(simInt scriptHandle);
VREP_DLLEXPORT simInt simHandleMainScript();
VREP_DLLEXPORT simInt simResetScript(simInt scriptHandle);
VREP_DLLEXPORT simInt simAddScript(simInt scriptProperty);
VREP_DLLEXPORT simInt simRemoveScript(simInt scriptHandle);
VREP_DLLEXPORT simInt simRefreshDialogs(simInt refreshDegree);
VREP_DLLEXPORT simInt simGetCollisionHandle(const simChar* collisionObjectName);
VREP_DLLEXPORT simInt simGetDistanceHandle(const simChar* distanceObjectName);
VREP_DLLEXPORT simInt simGetIkGroupHandle(const simChar* ikGroupName);
VREP_DLLEXPORT simInt simResetCollision(simInt collisionObjectHandle);
VREP_DLLEXPORT simInt simResetDistance(simInt distanceObjectHandle);
VREP_DLLEXPORT simInt simResetProximitySensor(simInt sensorHandle);
VREP_DLLEXPORT simInt simResetMill(simInt millHandle);
VREP_DLLEXPORT simInt simCheckProximitySensor(simInt sensorHandle,simInt entityHandle,simFloat* detectedPoint);
VREP_DLLEXPORT simInt simCheckProximitySensorEx(simInt sensorHandle,simInt entityHandle,simInt detectionMode,simFloat detectionThreshold,simFloat maxAngle,simFloat* detectedPoint,simInt* detectedObjectHandle,simFloat* normalVector);
VREP_DLLEXPORT simInt simCheckProximitySensorEx2(simInt sensorHandle,simFloat* vertexPointer,simInt itemType,simInt itemCount,simInt detectionMode,simFloat detectionThreshold,simFloat maxAngle,simFloat* detectedPoint,simFloat* normalVector);
VREP_DLLEXPORT simInt simAddSceneCustomData(simInt header,const simChar* data,simInt dataLength);
VREP_DLLEXPORT simInt simGetSceneCustomDataLength(simInt header);
VREP_DLLEXPORT simInt simGetSceneCustomData(simInt header,simChar* data);
VREP_DLLEXPORT simInt simAddObjectCustomData(simInt objectHandle,simInt header,const simChar* data,simInt dataLength);
VREP_DLLEXPORT simInt simGetObjectCustomDataLength(simInt objectHandle,simInt header);
VREP_DLLEXPORT simInt simGetObjectCustomData(simInt objectHandle,simInt header,simChar* data);
VREP_DLLEXPORT simChar* simCreateBuffer(simInt size);
VREP_DLLEXPORT simInt simReleaseBuffer(simChar* buffer);
VREP_DLLEXPORT simInt simCheckCollision(simInt entity1Handle,simInt entity2Handle);
VREP_DLLEXPORT simInt simCheckCollisionEx(simInt entity1Handle,simInt entity2Handle,simFloat** intersectionSegments);
VREP_DLLEXPORT simInt simCheckDistance(simInt entity1Handle,simInt entity2Handle,simFloat threshold,simFloat* distanceData);
VREP_DLLEXPORT simChar* simGetObjectConfiguration(simInt objectHandle);
VREP_DLLEXPORT simInt simSetObjectConfiguration(const simChar* data);
VREP_DLLEXPORT simChar* simGetConfigurationTree(simInt objectHandle);
VREP_DLLEXPORT simInt simSetConfigurationTree(const simChar* data);
VREP_DLLEXPORT simInt simSetSimulationTimeStep(simFloat timeStep);
VREP_DLLEXPORT simFloat simGetSimulationTimeStep();
VREP_DLLEXPORT simInt simGetRealTimeSimulation();
VREP_DLLEXPORT simInt simIsRealTimeSimulationStepNeeded();
VREP_DLLEXPORT simInt simAdjustRealTimeTimer(simInt instanceIndex,simFloat deltaTime);
VREP_DLLEXPORT simInt simGetSimulationPassesPerRenderingPass();
VREP_DLLEXPORT simInt simAdvanceSimulationByOneStep();
VREP_DLLEXPORT simInt simStartSimulation();
VREP_DLLEXPORT simInt simStopSimulation();
VREP_DLLEXPORT simInt simPauseSimulation();
VREP_DLLEXPORT simInt simLoadModule(const simChar* filenameAndPath,const simChar* pluginName);
VREP_DLLEXPORT simInt simUnloadModule(simInt pluginhandle);
VREP_DLLEXPORT simVoid* simSendModuleMessage(simInt message,simInt* auxiliaryData,simVoid* customData,simInt* replyData);
VREP_DLLEXPORT simVoid* simBroadcastMessage(simInt* auxiliaryData,simVoid* customData,simInt* replyData);
VREP_DLLEXPORT simChar* simGetModuleName(simInt index,simUChar* moduleVersion);
VREP_DLLEXPORT simChar* simGetScriptSimulationParameter(simInt scriptHandle,const simChar* parameterName,simInt* parameterLength);
VREP_DLLEXPORT simInt simSetScriptSimulationParameter(simInt scriptHandle,const simChar* parameterName,const simChar* parameterValue,simInt parameterLength);
VREP_DLLEXPORT simInt simFloatingViewAdd(simFloat posX,simFloat posY,simFloat sizeX,simFloat sizeY,simInt options);
VREP_DLLEXPORT simInt simFloatingViewRemove(simInt floatingViewHandle);
VREP_DLLEXPORT simInt simAdjustView(simInt viewHandleOrIndex,simInt associatedViewableObjectHandle,simInt options,const simChar* viewLabel);
VREP_DLLEXPORT simInt simResetPath(simInt pathHandle);
VREP_DLLEXPORT simInt simHandlePath(simInt pathHandle,simFloat deltaTime);
VREP_DLLEXPORT simInt simResetJoint(simInt jointHandle);
VREP_DLLEXPORT simInt simHandleJoint(simInt jointHandle,simFloat deltaTime);
VREP_DLLEXPORT simInt simSetLastError(const simChar* funcName,const simChar* errorMessage);
VREP_DLLEXPORT simInt simHandleGraph(simInt graphHandle,simFloat simulationTime);
VREP_DLLEXPORT simInt simResetGraph(simInt graphHandle);
VREP_DLLEXPORT simInt simSetNavigationMode(simInt navigationMode);
VREP_DLLEXPORT simInt simGetNavigationMode();
VREP_DLLEXPORT simInt simSetPage(simInt index);
VREP_DLLEXPORT simInt simGetPage();
VREP_DLLEXPORT simInt simDisplayDialog(const simChar* titleText,const simChar* mainText,simInt dialogType,const simChar* initialText,const simFloat* titleColors,const simFloat* dialogColors,simInt* elementHandle);
VREP_DLLEXPORT simInt simGetDialogResult(simInt genericDialogHandle);
VREP_DLLEXPORT simChar* simGetDialogInput(simInt genericDialogHandle);
VREP_DLLEXPORT simInt simEndDialog(simInt genericDialogHandle);
VREP_DLLEXPORT simInt simRegisterCustomLuaFunction(const simChar* funcName,const simChar* callTips,const simInt* inputArgumentTypes,simVoid(*callBack)(struct SLuaCallBack* p));
VREP_DLLEXPORT simInt simRegisterCustomLuaVariable(const simChar* varName,const simChar* varValue);
VREP_DLLEXPORT simInt simSetJointTargetVelocity(simInt objectHandle,simFloat targetVelocity);
VREP_DLLEXPORT simInt simGetJointTargetVelocity(simInt objectHandle,simFloat* targetVelocity);
VREP_DLLEXPORT simInt simSetPathTargetNominalVelocity(simInt objectHandle,simFloat targetNominalVelocity);
VREP_DLLEXPORT simInt simLockInterface(simBool locked);
VREP_DLLEXPORT simChar* simGetScriptRawBuffer(simInt scriptHandle,simInt bufferHandle);
VREP_DLLEXPORT simInt simSetScriptRawBuffer(simInt scriptHandle,const simChar* buffer,simInt bufferSize);
VREP_DLLEXPORT simInt simReleaseScriptRawBuffer(simInt scriptHandle,simInt bufferHandle);
VREP_DLLEXPORT simInt simCopyPasteSelectedObjects();
VREP_DLLEXPORT simInt simScaleSelectedObjects(simFloat scalingFactor,simBool scalePositionsToo);
VREP_DLLEXPORT simInt simDeleteSelectedObjects();
VREP_DLLEXPORT simInt simGetObjectUniqueIdentifier(simInt objectHandle,simInt* uniqueIdentifier);
VREP_DLLEXPORT simInt simGetNameSuffix(const simChar* name);
VREP_DLLEXPORT simInt simSendData(simInt targetID,simInt dataHeader,const simChar* dataName,const simChar* data,simInt dataLength,simInt antennaHandle,simFloat actionRadius,simFloat emissionAngle1,simFloat emissionAngle2,simFloat persistence);
VREP_DLLEXPORT simChar* simReceiveData(simInt dataHeader,const simChar* dataName,simInt antennaHandle,simInt index,simInt* dataLength,simInt* senderID,simInt* dataHeaderR,simChar** dataNameR);
VREP_DLLEXPORT simInt simSetGraphUserData(simInt graphHandle,const simChar* dataStreamName,simFloat data);
VREP_DLLEXPORT simInt simSetNameSuffix(simInt nameSuffixNumber);
VREP_DLLEXPORT simInt simAddDrawingObject(simInt objectType,simFloat size,simFloat duplicateTolerance,simInt parentObjectHandle,simInt maxItemCount,const simFloat* ambient,const simFloat* diffuse,const simFloat* specular,const simFloat* emission);
VREP_DLLEXPORT simInt simRemoveDrawingObject(simInt objectHandle);
VREP_DLLEXPORT simInt simAddDrawingObjectItem(simInt objectHandle,const simFloat* itemData);
VREP_DLLEXPORT simInt simAddParticleObject(simInt objectType,simFloat size,simFloat density,const simVoid* params,simFloat lifeTime,simInt maxItemCount,const simFloat* ambient,const simFloat* diffuse,const simFloat* specular,const simFloat* emission);
VREP_DLLEXPORT simInt simRemoveParticleObject(simInt objectHandle);
VREP_DLLEXPORT simInt simAddParticleObjectItem(simInt objectHandle,const simFloat* itemData);
VREP_DLLEXPORT simFloat simGetObjectSizeFactor(simInt objectHandle);
VREP_DLLEXPORT simInt simAnnounceSceneContentChange();
VREP_DLLEXPORT simInt simResetMilling(simInt objectHandle);
VREP_DLLEXPORT simInt simApplyMilling(simInt objectHandle);
VREP_DLLEXPORT simInt simSetIntegerSignal(const simChar* signalName,simInt signalValue);
VREP_DLLEXPORT simInt simGetIntegerSignal(const simChar* signalName,simInt* signalValue);
VREP_DLLEXPORT simInt simClearIntegerSignal(const simChar* signalName);
VREP_DLLEXPORT simInt simSetFloatSignal(const simChar* signalName,simFloat signalValue);
VREP_DLLEXPORT simInt simGetFloatSignal(const simChar* signalName,simFloat* signalValue);
VREP_DLLEXPORT simInt simClearFloatSignal(const simChar* signalName);
VREP_DLLEXPORT simInt simSetStringSignal(const simChar* signalName,const simChar* signalValue,simInt stringLength);
VREP_DLLEXPORT simChar* simGetStringSignal(const simChar* signalName,simInt* stringLength);
VREP_DLLEXPORT simInt simClearStringSignal(const simChar* signalName);
VREP_DLLEXPORT simChar* simGetSignalName(simInt signalIndex,simInt signalType);
VREP_DLLEXPORT simInt simSetObjectProperty(simInt objectHandle,simInt prop);
VREP_DLLEXPORT simInt simGetObjectProperty(simInt objectHandle);
VREP_DLLEXPORT simInt simSetObjectSpecialProperty(simInt objectHandle,simInt prop);
VREP_DLLEXPORT simInt simGetObjectSpecialProperty(simInt objectHandle);
VREP_DLLEXPORT simInt simGetPositionOnPath(simInt pathHandle,simFloat relativeDistance,simFloat* position);
VREP_DLLEXPORT simInt simGetDataOnPath(simInt pathHandle,simFloat relativeDistance,simInt dataType,simInt* intData,simFloat* floatData);
VREP_DLLEXPORT simInt simGetOrientationOnPath(simInt pathHandle,simFloat relativeDistance,simFloat* eulerAngles);
VREP_DLLEXPORT simInt simGetClosestPositionOnPath(simInt pathHandle,simFloat* absolutePosition,simFloat* pathPosition);
VREP_DLLEXPORT simInt simReadForceSensor(simInt objectHandle,simFloat* forceVector,simFloat* torqueVector);
VREP_DLLEXPORT simInt simBreakForceSensor(simInt objectHandle);
VREP_DLLEXPORT simInt simGetShapeVertex(simInt shapeHandle,simInt groupElementIndex,simInt vertexIndex,simFloat* relativePosition);
VREP_DLLEXPORT simInt simGetShapeTriangle(simInt shapeHandle,simInt groupElementIndex,simInt triangleIndex,simInt* vertexIndices,simFloat* triangleNormals);
VREP_DLLEXPORT simInt simSetLightParameters(simInt objectHandle,simInt state,const simFloat* ambientPart,const simFloat* diffusePart,const simFloat* specularPart);
VREP_DLLEXPORT simInt simGetLightParameters(simInt objectHandle,simFloat* ambientPart,simFloat* diffusePart,simFloat* specularPart);
VREP_DLLEXPORT simInt simHandleVarious();
VREP_DLLEXPORT simInt simGetVelocity(simInt shapeHandle,simFloat* linearVelocity,simFloat* angularVelocity);
VREP_DLLEXPORT simInt simGetObjectVelocity(simInt objectHandle,simFloat* linearVelocity,simFloat* angularVelocity);
VREP_DLLEXPORT simInt simAddForceAndTorque(simInt shapeHandle,const simFloat* force,const simFloat* torque);
VREP_DLLEXPORT simInt simAddForce(simInt shapeHandle,const simFloat* position,const simFloat* force);
VREP_DLLEXPORT simInt simSetExplicitHandling(simInt generalObjectHandle,int explicitFlags);
VREP_DLLEXPORT simInt simGetExplicitHandling(simInt generalObjectHandle);
VREP_DLLEXPORT simInt simGetLinkDummy(simInt dummyHandle);
VREP_DLLEXPORT simInt simSetLinkDummy(simInt dummyHandle,simInt linkedDummyHandle);
VREP_DLLEXPORT simInt simSetModelProperty(simInt objectHandle,simInt modelProperty);
VREP_DLLEXPORT simInt simGetModelProperty(simInt objectHandle);
VREP_DLLEXPORT simInt simSetShapeColor(simInt shapeHandle,const simChar* colorName,simInt colorComponent,const simFloat* rgbData);
VREP_DLLEXPORT simInt simGetShapeColor(simInt shapeHandle,const simChar* colorName,simInt colorComponent,simFloat* rgbData);
VREP_DLLEXPORT simInt simResetDynamicObject(simInt objectHandle);
VREP_DLLEXPORT simInt simSetJointMode(simInt jointHandle,simInt jointMode,simInt options);
VREP_DLLEXPORT simInt simGetJointMode(simInt jointHandle,simInt* options);
VREP_DLLEXPORT simInt simSerialOpen(const simChar* portString,simInt baudRate,simVoid* reserved1,simVoid* reserved2);
VREP_DLLEXPORT simInt simSerialClose(simInt portHandle);
VREP_DLLEXPORT simInt simSerialSend(simInt portHandle,const simChar* data,simInt dataLength);
VREP_DLLEXPORT simInt simSerialRead(simInt portHandle,simChar* buffer,simInt dataLengthToRead);
VREP_DLLEXPORT simInt simSerialCheck(simInt portHandle);
VREP_DLLEXPORT simInt simGetContactInfo(simInt dynamicPass,simInt objectHandle,simInt index,simInt* objectHandles,simFloat* contactInfo);
VREP_DLLEXPORT simInt simSetThreadIsFree(simBool freeMode);
VREP_DLLEXPORT simInt simTubeOpen(simInt dataHeader,const simChar* dataName,simInt readBufferSize,simBool notUsedButKeepZero);
VREP_DLLEXPORT simInt simTubeClose(simInt tubeHandle);
VREP_DLLEXPORT simInt simTubeWrite(simInt tubeHandle,const simChar* data,simInt dataLength);
VREP_DLLEXPORT simChar* simTubeRead(simInt tubeHandle,simInt* dataLength);
VREP_DLLEXPORT simInt simTubeStatus(simInt tubeHandle,simInt* readPacketsCount,simInt* writePacketsCount);
VREP_DLLEXPORT simInt simAuxiliaryConsoleOpen(const simChar* title,simInt maxLines,simInt mode,const simInt* position,const simInt* size,const simFloat* textColor,const simFloat* backgroundColor);
VREP_DLLEXPORT simInt simAuxiliaryConsoleClose(simInt consoleHandle);
VREP_DLLEXPORT simInt simAuxiliaryConsoleShow(simInt consoleHandle,simBool showState);
VREP_DLLEXPORT simInt simAuxiliaryConsolePrint(simInt consoleHandle,const simChar* text);
VREP_DLLEXPORT simInt simImportShape(simInt fileformat,const simChar* pathAndFilename,simInt options,simFloat identicalVerticeTolerance,simFloat scalingFactor);
VREP_DLLEXPORT simInt simImportMesh(simInt fileformat,const simChar* pathAndFilename,simInt options,simFloat identicalVerticeTolerance,simFloat scalingFactor,simFloat*** vertices,simInt** verticesSizes,simInt*** indices,simInt** indicesSizes,simFloat*** reserved,simChar*** names);
VREP_DLLEXPORT simInt simExportMesh(simInt fileformat,const simChar* pathAndFilename,simInt options,simFloat scalingFactor,simInt elementCount,simFloat** vertices,const simInt* verticesSizes,simInt** indices,const simInt* indicesSizes,simFloat** reserved,simChar** names);
VREP_DLLEXPORT simInt simCreateMeshShape(simInt options,simFloat shadingAngle,const simFloat* vertices,simInt verticesSize,const simInt* indices,simInt indicesSize,simFloat* reserved);
VREP_DLLEXPORT simInt simCreatePureShape(simInt primitiveType,simInt options,const simFloat* sizes,simFloat mass,const simInt* precision);
VREP_DLLEXPORT simInt simCreateHeightfieldShape(simInt options,simFloat shadingAngle,simInt xPointCount,simInt yPointCount,simFloat xSize,const simFloat* heights);
VREP_DLLEXPORT simInt simGetShapeMesh(simInt shapeHandle,simFloat** vertices,simInt* verticesSize,simInt** indices,simInt* indicesSize,simFloat** normals);
VREP_DLLEXPORT simInt simAddBanner(const simChar* label,simFloat size,simInt options,const simFloat* positionAndEulerAngles,simInt parentObjectHandle,const simFloat* labelColors,const simFloat* backgroundColors);
VREP_DLLEXPORT simInt simRemoveBanner(simInt bannerID);
VREP_DLLEXPORT simInt simCreateJoint(simInt jointType,simInt jointMode,simInt options,const simFloat* sizes,const simFloat* colorA,const simFloat* colorB);
VREP_DLLEXPORT simInt simRegisterContactCallback(simInt(*callBack)(simInt,simInt,simInt,simInt*,simFloat*));
VREP_DLLEXPORT simInt simGetObjectIntParameter(simInt objectHandle,simInt parameterID,simInt* parameter);
VREP_DLLEXPORT simInt simSetObjectIntParameter(simInt objectHandle,simInt parameterID,simInt parameter);
VREP_DLLEXPORT simInt simGetObjectFloatParameter(simInt objectHandle,simInt parameterID,simFloat* parameter);
VREP_DLLEXPORT simInt simSetObjectFloatParameter(simInt objectHandle,simInt parameterID,simFloat parameter);
VREP_DLLEXPORT simChar* simGetObjectStringParameter(simInt objectHandle,simInt parameterID,simInt* parameterLength);
VREP_DLLEXPORT simInt simSetObjectStringParameter(simInt objectHandle,simInt parameterID,simChar* parameter,simInt parameterLength);
VREP_DLLEXPORT simInt simSetSimulationPassesPerRenderingPass(simInt p);
VREP_DLLEXPORT simInt simGetRotationAxis(const simFloat* matrixStart,const simFloat* matrixGoal,simFloat* axis,simFloat* angle);
VREP_DLLEXPORT simInt simRotateAroundAxis(const simFloat* matrixIn,const simFloat* axis,const simFloat* axisPos,simFloat angle,simFloat* matrixOut);
VREP_DLLEXPORT simInt simGetJointForce(simInt jointHandle,simFloat* forceOrTorque);
VREP_DLLEXPORT simInt simSetArrayParameter(simInt parameter,const simVoid* arrayOfValues);
VREP_DLLEXPORT simInt simGetArrayParameter(simInt parameter,simVoid* arrayOfValues);
VREP_DLLEXPORT simInt simSetIkGroupProperties(simInt ikGroupHandle,simInt resolutionMethod,simInt maxIterations,simFloat damping,void* reserved);
VREP_DLLEXPORT simInt simSetIkElementProperties(simInt ikGroupHandle,simInt tipDummyHandle,simInt constraints,const simFloat* precision,const simFloat* weight,void* reserved);
VREP_DLLEXPORT simInt simCameraFitToView(simInt viewHandleOrIndex,simInt objectCount,const simInt* objectHandles,simInt options,simFloat scaling);
VREP_DLLEXPORT simInt simPersistentDataWrite(const simChar* dataName,const simChar* dataValue,simInt dataLength,simInt options);
VREP_DLLEXPORT simChar* simPersistentDataRead(const simChar* dataName,simInt* dataLength);
VREP_DLLEXPORT simInt simIsHandleValid(simInt generalObjectHandle,simInt generalObjectType);
VREP_DLLEXPORT simInt simHandleVisionSensor(simInt visionSensorHandle,simFloat** auxValues,simInt** auxValuesCount);
VREP_DLLEXPORT simInt simReadVisionSensor(simInt visionSensorHandle,simFloat** auxValues,simInt** auxValuesCount);
VREP_DLLEXPORT simInt simResetVisionSensor(simInt visionSensorHandle);
VREP_DLLEXPORT simInt simCheckVisionSensor(simInt visionSensorHandle,simInt entityHandle,simFloat** auxValues,simInt** auxValuesCount);
VREP_DLLEXPORT simFloat* simCheckVisionSensorEx(simInt visionSensorHandle,simInt entityHandle,simBool returnImage);
VREP_DLLEXPORT simInt simGetVisionSensorResolution(simInt visionSensorHandle,simInt* resolution);
VREP_DLLEXPORT simFloat* simGetVisionSensorImage(simInt visionSensorHandle);
VREP_DLLEXPORT simInt simSetVisionSensorImage(simInt visionSensorHandle,const simFloat* image);
VREP_DLLEXPORT simFloat* simGetVisionSensorDepthBuffer(simInt visionSensorHandle);
VREP_DLLEXPORT simInt simCreateUI(const simChar* uiName,simInt menuAttributes,const simInt* clientSize,const simInt* cellSize,simInt* buttonHandles);
VREP_DLLEXPORT simInt simCreateUIButton(simInt uiHandle,const simInt* position,const simInt* size,simInt buttonProperty);
VREP_DLLEXPORT simInt simGetUIHandle(const simChar* uiName);
VREP_DLLEXPORT simInt simGetUIProperty(simInt uiHandle);
VREP_DLLEXPORT simInt simGetUIEventButton(simInt uiHandle,simInt* auxiliaryValues);
VREP_DLLEXPORT simInt simSetUIProperty(simInt uiHandle,simInt elementProperty);
VREP_DLLEXPORT simInt simGetUIButtonProperty(simInt uiHandle,simInt buttonHandle);
VREP_DLLEXPORT simInt simSetUIButtonProperty(simInt uiHandle,simInt buttonHandle,simInt buttonProperty);
VREP_DLLEXPORT simInt simGetUIButtonSize(simInt uiHandle,simInt buttonHandle,simInt* size);
VREP_DLLEXPORT simInt simSetUIButtonLabel(simInt uiHandle,simInt buttonHandle,const simChar* upStateLabel,const simChar* downStateLabel);
VREP_DLLEXPORT simChar* simGetUIButtonLabel(simInt uiHandle,simInt buttonHandle);
VREP_DLLEXPORT simInt simSetUISlider(simInt uiHandle,simInt buttonHandle,simInt position);
VREP_DLLEXPORT simInt simGetUISlider(simInt uiHandle,simInt buttonHandle);
VREP_DLLEXPORT simInt simSetUIButtonColor(simInt uiHandle,simInt buttonHandle,const simFloat* upStateColor,const simFloat* downStateColor,const simFloat* labelColor);
VREP_DLLEXPORT simInt simSetUIButtonTexture(simInt uiHandle,simInt buttonHandle,const simInt* size,const simChar* textureData);
VREP_DLLEXPORT simInt simCreateUIButtonArray(simInt uiHandle,simInt buttonHandle);
VREP_DLLEXPORT simInt simSetUIButtonArrayColor(simInt uiHandle,simInt buttonHandle,const simInt* position,const simFloat* color);
VREP_DLLEXPORT simInt simDeleteUIButtonArray(simInt uiHandle,simInt buttonHandle);
VREP_DLLEXPORT simInt simRemoveUI(simInt uiHandle);
VREP_DLLEXPORT simInt simSetUIPosition(simInt uiHandle,const simInt* position);
VREP_DLLEXPORT simInt simGetUIPosition(simInt uiHandle,simInt* position);
VREP_DLLEXPORT simInt simRMLPosition(simInt dofs,simDouble timeStep,simInt flags,const simDouble* currentPosVelAccel,const simDouble* maxVelAccelJerk,const simBool* selection,const simDouble* targetPosVel,simDouble* newPosVelAccel,simVoid* auxData);
VREP_DLLEXPORT simInt simRMLVelocity(simInt dofs,simDouble timeStep,simInt flags,const simDouble* currentPosVelAccel,const simDouble* maxAccelJerk,const simBool* selection,const simDouble* targetVel,simDouble* newPosVelAccel,simVoid* auxData);
VREP_DLLEXPORT simInt simRMLPos(simInt dofs,simDouble smallestTimeStep,simInt flags,const simDouble* currentPosVelAccel,const simDouble* maxVelAccelJerk,const simBool* selection,const simDouble* targetPosVel,simVoid* auxData);
VREP_DLLEXPORT simInt simRMLVel(simInt dofs,simDouble smallestTimeStep,simInt flags,const simDouble* currentPosVelAccel,const simDouble* maxAccelJerk,const simBool* selection,const simDouble* targetVel,simVoid* auxData);
VREP_DLLEXPORT simInt simRMLStep(simInt handle,simDouble timeStep,simDouble* newPosVelAccel,simVoid* auxData,simVoid* reserved);
VREP_DLLEXPORT simInt simRMLRemove(simInt handle);
VREP_DLLEXPORT simChar* simFileDialog(simInt mode,const simChar* title,const simChar* startPath,const simChar* initName,const simChar* extName,const simChar* ext);
VREP_DLLEXPORT simInt simMsgBox(simInt dlgType,simInt buttons,const simChar* title,const simChar* message);
VREP_DLLEXPORT simInt simCreateDummy(simFloat size,const simFloat* color);
VREP_DLLEXPORT simInt simSetShapeMassAndInertia(simInt shapeHandle,simFloat mass,const simFloat* inertiaMatrix,const simFloat* centerOfMass,const simFloat* transformation);
VREP_DLLEXPORT simInt simGetShapeMassAndInertia(simInt shapeHandle,simFloat* mass,simFloat* inertiaMatrix,simFloat* centerOfMass,const simFloat* transformation);
VREP_DLLEXPORT simInt simGroupShapes(const simInt* shapeHandles,simInt shapeCount);
VREP_DLLEXPORT simInt* simUngroupShape(simInt shapeHandle,simInt* shapeCount);
VREP_DLLEXPORT simInt simCreateProximitySensor(simInt sensorType,simInt subType,simInt options,const simInt* intParams,const simFloat* floatParams,const simFloat* color);
VREP_DLLEXPORT simInt simCreateForceSensor(simInt options,const simInt* intParams,const simFloat* floatParams,const simFloat* color);
VREP_DLLEXPORT simInt simCreateVisionSensor(simInt options,const simInt* intParams,const simFloat* floatParams,const simFloat* color);
VREP_DLLEXPORT simInt simConvexDecompose(simInt shapeHandle,simInt options,const simInt* intParams,const simFloat* floatParams);
VREP_DLLEXPORT simInt simCreatePath(simInt attributes,const simInt* intParams,const simFloat* floatParams,const simFloat* color);
VREP_DLLEXPORT simInt simInsertPathCtrlPoints(simInt pathHandle,simInt options,simInt startIndex,simInt ptCnt,const simVoid* ptData);
VREP_DLLEXPORT simInt simCutPathCtrlPoints(simInt pathHandle,simInt startIndex,simInt ptCnt);
VREP_DLLEXPORT simFloat* simGetIkGroupMatrix(simInt ikGroupHandle,simInt options,simInt* matrixSize);
VREP_DLLEXPORT simInt simRegisterJointCtrlCallback(simInt(*callBack)(simInt,simInt,simInt,const simInt*,const simFloat*,simFloat*));
VREP_DLLEXPORT simInt simAddGhost(simInt ghostGroup,simInt objectHandle,simInt options,simFloat startTime,simFloat endTime,const simFloat* color);
VREP_DLLEXPORT simInt simModifyGhost(simInt ghostGroup,simInt ghostId,simInt operation,simFloat floatValue,simInt options,simInt optionsMask,const simFloat* colorOrTransformation);
VREP_DLLEXPORT simVoid simQuitSimulator(simBool doNotDisplayMessages);
VREP_DLLEXPORT simInt simGetThreadId();
VREP_DLLEXPORT simInt simLockResources(simInt lockType,simInt reserved);
VREP_DLLEXPORT simInt simUnlockResources(simInt lockHandle);
VREP_DLLEXPORT simInt simEnableEventCallback(simInt eventCallbackType,const simChar* plugin,simInt reserved);
VREP_DLLEXPORT simInt simGetMaterialId(const simChar* materialName);
VREP_DLLEXPORT simInt simSetShapeMaterial(simInt shapeHandle,simInt materialId);
VREP_DLLEXPORT simInt simGetShapeMaterial(simInt shapeHandle);
VREP_DLLEXPORT simInt simGetTextureId(const simChar* textureName,simInt* resolution);
VREP_DLLEXPORT simChar* simReadTexture(simInt textureId,simInt options,simInt posX,simInt posY,simInt sizeX,simInt sizeY);
VREP_DLLEXPORT simInt simWriteTexture(simInt textureId,simInt options,const simChar* data,simInt posX,simInt posY,simInt sizeX,simInt sizeY,simFloat interpol);
VREP_DLLEXPORT simInt simCreateTexture(const simChar* fileName,simInt options,const simFloat* planeSizes,const simFloat* scalingUV,const simFloat* xy_g,simInt fixedResolution,simInt* textureId,simInt* resolution,const simVoid* reserved);
VREP_DLLEXPORT simInt simWriteCustomDataBlock(simInt objectHandle,const simChar* dataName,const simChar* data,simInt dataSize);
VREP_DLLEXPORT simChar* simReadCustomDataBlock(simInt objectHandle,const simChar* dataName,simInt* dataSize);
VREP_DLLEXPORT simInt simAddPointCloud(simInt pageMask,simInt layerMask,simInt objectHandle,simInt options,simFloat pointSize,simInt ptCnt,const simFloat* pointCoordinates,const simChar* defaultColors,const simChar* pointColors,const simFloat* pointNormals);
VREP_DLLEXPORT simInt simModifyPointCloud(simInt pointCloudHandle,simInt operation,const simInt* intParam,const simFloat* floatParam);
VREP_DLLEXPORT simInt simGetShapeGeomInfo(simInt shapeHandle,simInt* intData,simFloat* floatData,simVoid* reserved);


VREP_DLLEXPORT simInt _simGetContactCallbackCount();
VREP_DLLEXPORT const void* _simGetContactCallback(simInt index);
VREP_DLLEXPORT simVoid _simSetDynamicSimulationIconCode(simVoid* object,simInt code);
VREP_DLLEXPORT simVoid _simSetDynamicObjectFlagForVisualization(simVoid* object,simInt flag);
VREP_DLLEXPORT simInt _simGetObjectListSize(simInt objType);
VREP_DLLEXPORT const simVoid* _simGetObjectFromIndex(simInt objType,simInt index);
VREP_DLLEXPORT simInt _simGetObjectID(const simVoid* object);
VREP_DLLEXPORT simInt _simGetObjectType(const simVoid* object);
VREP_DLLEXPORT const simVoid** _simGetObjectChildren(const simVoid* object,simInt* count);
VREP_DLLEXPORT const simVoid* _simGetGeomProxyFromShape(const simVoid* shape);
VREP_DLLEXPORT const simVoid* _simGetParentObject(const simVoid* object);
VREP_DLLEXPORT const simVoid* _simGetObject(int objID);
VREP_DLLEXPORT simVoid _simGetObjectLocalTransformation(const simVoid* object,simFloat* pos,simFloat* quat,simBool excludeFirstJointTransformation);
VREP_DLLEXPORT simVoid _simSetObjectLocalTransformation(simVoid* object,const simFloat* pos,const simFloat* quat);
VREP_DLLEXPORT simVoid _simSetObjectCumulativeTransformation(simVoid* object,const simFloat* pos,const simFloat* quat,simBool keepChildrenInPlace);
VREP_DLLEXPORT simVoid _simGetObjectCumulativeTransformation(const simVoid* object,simFloat* pos,simFloat* quat,simBool excludeFirstJointTransformation);
VREP_DLLEXPORT simBool _simIsShapeDynamicallyStatic(const simVoid* shape);
VREP_DLLEXPORT simInt _simGetTreeDynamicProperty(const simVoid* object);
VREP_DLLEXPORT simInt _simGetDummyLinkType(const simVoid* dummy,simInt* linkedDummyID);
VREP_DLLEXPORT simInt _simGetJointMode(const simVoid* joint);
VREP_DLLEXPORT simBool _simIsJointInHybridOperation(const simVoid* joint);
VREP_DLLEXPORT simVoid _simDisableDynamicTreeForManipulation(const simVoid* object,simBool disableFlag);
VREP_DLLEXPORT simBool _simIsShapeDynamicallyRespondable(const simVoid* shape);
VREP_DLLEXPORT simInt _simGetDynamicCollisionMask(const simVoid* shape);
VREP_DLLEXPORT const simVoid* _simGetLastParentForLocalGlobalCollidable(const simVoid* shape);
VREP_DLLEXPORT simVoid _simSetShapeIsStaticAndNotRespondableButDynamicTag(const simVoid* shape,simBool tag);
VREP_DLLEXPORT simBool _simGetShapeIsStaticAndNotRespondableButDynamicTag(const simVoid* shape);
VREP_DLLEXPORT simVoid _simSetJointPosition(const simVoid* joint,simFloat pos);
VREP_DLLEXPORT simFloat _simGetJointPosition(const simVoid* joint);
VREP_DLLEXPORT simVoid _simSetDynamicMotorPositionControlTargetPosition(const simVoid* joint,simFloat pos);
VREP_DLLEXPORT simVoid _simGetInitialDynamicVelocity(const simVoid* shape,simFloat* vel);
VREP_DLLEXPORT simVoid _simSetInitialDynamicVelocity(simVoid* shape,const simFloat* vel);
VREP_DLLEXPORT simVoid _simGetInitialDynamicAngVelocity(const simVoid* shape,simFloat* angularVel);
VREP_DLLEXPORT simVoid _simSetInitialDynamicAngVelocity(simVoid* shape,const simFloat* angularVel);
VREP_DLLEXPORT simBool _simGetStartSleeping(const simVoid* shape);
VREP_DLLEXPORT simBool _simGetWasPutToSleepOnce(const simVoid* shape);
VREP_DLLEXPORT simBool _simGetDynamicsFullRefreshFlag(const simVoid* object);
VREP_DLLEXPORT simVoid _simSetDynamicsFullRefreshFlag(const simVoid* object,simBool flag);
VREP_DLLEXPORT simVoid _simSetGeomProxyDynamicsFullRefreshFlag(simVoid* geomData,simBool flag);
VREP_DLLEXPORT simBool _simGetGeomProxyDynamicsFullRefreshFlag(const simVoid* geomData);
VREP_DLLEXPORT simBool _simGetParentFollowsDynamic(const simVoid* shape);
VREP_DLLEXPORT simVoid _simSetShapeDynamicVelocity(simVoid* shape,const simFloat* linear,const simFloat* angular);
VREP_DLLEXPORT simVoid _simGetAdditionalForceAndTorque(const simVoid* shape,simFloat* force,simFloat* torque);
VREP_DLLEXPORT simVoid _simClearAdditionalForceAndTorque(const simVoid* shape);
VREP_DLLEXPORT simBool _simGetJointPositionInterval(const simVoid* joint,simFloat* minValue,simFloat* rangeValue);
VREP_DLLEXPORT simInt _simGetJointType(const simVoid* joint);
VREP_DLLEXPORT simVoid _simGetJointOdeParameters(const simVoid* joint,simFloat* stopERP,simFloat* stopCFM,simFloat* bounce,simFloat* fudge,simFloat* normalCFM);
VREP_DLLEXPORT simVoid _simGetJointBulletParameters(const simVoid* joint,simFloat* stopERP,simFloat* stopCFM,simFloat* normalCFM);
VREP_DLLEXPORT simBool _simIsForceSensorBroken(const simVoid* forceSensor);
VREP_DLLEXPORT simVoid _simGetDynamicForceSensorLocalTransformationPart2(const simVoid* forceSensor,simFloat* pos,simFloat* quat);
VREP_DLLEXPORT simBool _simIsDynamicMotorEnabled(const simVoid* joint);
VREP_DLLEXPORT simBool _simIsDynamicMotorPositionCtrlEnabled(const simVoid* joint);
VREP_DLLEXPORT simBool _simIsDynamicMotorTorqueModulationEnabled(const simVoid* joint);
VREP_DLLEXPORT simVoid _simGetMotorPid(const simVoid* joint,simFloat* pParam,simFloat* iParam,simFloat* dParam);
VREP_DLLEXPORT simFloat _simGetDynamicMotorTargetPosition(const simVoid* joint);
VREP_DLLEXPORT simFloat _simGetDynamicMotorTargetVelocity(const simVoid* joint);
VREP_DLLEXPORT simFloat _simGetDynamicMotorMaxForce(const simVoid* joint);
VREP_DLLEXPORT simFloat _simGetDynamicMotorUpperLimitVelocity(const simVoid* joint);
VREP_DLLEXPORT simVoid _simSetDynamicMotorReflectedPositionFromDynamicEngine(simVoid* joint,simFloat pos);
VREP_DLLEXPORT simVoid _simSetJointSphericalTransformation(simVoid* joint,const simFloat* quat);
VREP_DLLEXPORT simVoid _simAddForceSensorCumulativeForcesAndTorques(simVoid* forceSensor,const simFloat* force,const simFloat* torque,int totalPassesCount);
VREP_DLLEXPORT simVoid _simAddJointCumulativeForcesOrTorques(simVoid* joint,float forceOrTorque,int totalPassesCount);
VREP_DLLEXPORT simVoid _simSetDynamicJointLocalTransformationPart2(simVoid* joint,const simFloat* pos,const simFloat* quat);
VREP_DLLEXPORT simVoid _simSetDynamicForceSensorLocalTransformationPart2(simVoid* forceSensor,const simFloat* pos,const simFloat* quat);
VREP_DLLEXPORT simVoid _simSetDynamicJointLocalTransformationPart2IsValid(simVoid* joint,simBool valid);
VREP_DLLEXPORT simVoid _simSetDynamicForceSensorLocalTransformationPart2IsValid(simVoid* forceSensor,simBool valid);
VREP_DLLEXPORT const simVoid* _simGetGeomWrapFromGeomProxy(const simVoid* geomData);
VREP_DLLEXPORT simVoid _simGetLocalInertiaFrame(const simVoid* geomInfo,simFloat* pos,simFloat* quat);
VREP_DLLEXPORT simInt _simGetPurePrimitiveType(const simVoid* geomInfo);
VREP_DLLEXPORT simBool _simIsGeomWrapGeometric(const simVoid* geomInfo);
VREP_DLLEXPORT simBool _simIsGeomWrapConvex(const simVoid* geomInfo);
VREP_DLLEXPORT simInt _simGetGeometricCount(const simVoid* geomInfo);
VREP_DLLEXPORT simVoid _simGetAllGeometrics(const simVoid* geomInfo,simVoid** allGeometrics);
VREP_DLLEXPORT simVoid _simGetPurePrimitiveSizes(const simVoid* geometric,simFloat* sizes);
VREP_DLLEXPORT simVoid _simMakeDynamicAnnouncement(int announceType);
VREP_DLLEXPORT simVoid _simGetVerticesLocalFrame(const simVoid* geometric,simFloat* pos,simFloat* quat);
VREP_DLLEXPORT const simFloat* _simGetHeightfieldData(const simVoid* geometric,simInt* xCount,simInt* yCount,simFloat* minHeight,simFloat* maxHeight);
VREP_DLLEXPORT simVoid _simGetCumulativeMeshes(const simVoid* geomInfo,simFloat** vertices,simInt* verticesSize,simInt** indices,simInt* indicesSize);
VREP_DLLEXPORT simVoid _simGetOdeMaxContactFrictionCFMandERP(const simVoid* geomInfo,simInt* maxContacts,simFloat* friction,simFloat* cfm,simFloat* erp);
VREP_DLLEXPORT simBool _simGetBulletCollisionMargin(const simVoid* geomInfo,simFloat* margin,simInt* otherProp);
VREP_DLLEXPORT simBool _simGetBulletStickyContact(const simVoid* geomInfo);
VREP_DLLEXPORT simFloat _simGetBulletRestitution(const simVoid* geomInfo);
VREP_DLLEXPORT simFloat _simGetMass(const simVoid* geomInfo);
VREP_DLLEXPORT simVoid _simGetPrincipalMomentOfInertia(const simVoid* geomInfo,simFloat* inertia);
VREP_DLLEXPORT simVoid _simGetDamping(const simVoid* geomInfo,simFloat* linDamping,simFloat* angDamping);
VREP_DLLEXPORT simFloat _simGetFriction(const simVoid* geomInfo);
VREP_DLLEXPORT simVoid _simGetGravity(simFloat* gravity);
VREP_DLLEXPORT simInt _simGetTimeDiffInMs(simInt previousTime);
VREP_DLLEXPORT simBool _simDoEntitiesCollide(simInt entity1ID,simInt entity2ID,simInt* cacheBuffer,simBool overrideCollidableFlagIfShape1,simBool overrideCollidableFlagIfShape2,simBool pathOrMotionPlanningRoutineCalling);
VREP_DLLEXPORT simBool _simGetDistanceBetweenEntitiesIfSmaller(simInt entity1ID,simInt entity2ID,simFloat* distance,simFloat* ray,simInt* cacheBuffer,simBool overrideMeasurableFlagIfNonCollection1,simBool overrideMeasurableFlagIfNonCollection2,simBool pathPlanningRoutineCalling);
VREP_DLLEXPORT simInt _simHandleJointControl(const simVoid* joint,simInt auxV,const simInt* inputValuesInt,const simFloat* inputValuesFloat,simFloat* outputValues);
VREP_DLLEXPORT simInt _simHandleCustomContact(simInt objHandle1,simInt objHandle2,simInt engine,simInt* dataInt,simFloat* dataFloat);
VREP_DLLEXPORT const simVoid* _simGetIkGroupObject(int ikGroupID);
VREP_DLLEXPORT simInt _simMpHandleIkGroupObject(const simVoid* ikGroup);
VREP_DLLEXPORT simFloat _simGetPureHollowScaling(const simVoid* geometric);
VREP_DLLEXPORT simVoid _simGetVortexParameters(const simVoid* object,simInt version,simFloat* floatParams,simInt* intParams);
VREP_DLLEXPORT simInt _simGetJointCallbackCallOrder(const simVoid* joint);


// Deprecated
VREP_DLLEXPORT simInt simSerialPortOpen(simInt portNumber,simInt baudRate,simVoid* reserved1,simVoid* reserved2);
VREP_DLLEXPORT simInt simSerialPortClose(simInt portNumber);
VREP_DLLEXPORT simInt simSerialPortSend(simInt portNumber,const simChar* data,simInt dataLength);
VREP_DLLEXPORT simInt simSerialPortRead(simInt portNumber,simChar* buffer,simInt dataLengthToRead);
VREP_DLLEXPORT simInt simJointGetForce(simInt jointHandle,simFloat* forceOrTorque);

#endif // !defined(V_REP_INCLUDED_)
