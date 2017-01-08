// This file is part of the REMOTE API
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The REMOTE API is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The REMOTE API is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE REMOTE API IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the REMOTE API.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include <urbi/uobject.hh>

extern "C" {
#include "extApi.h"
}


using namespace urbi;
class vrep: public urbi::UObject // Must inherit from UObject.
{ 
    public: 
        // The class must have a single const  ructor taking a string. 
       vrep (const std::string &s):UObject(s) {
            UBindFunction(vrep ,simxGetJointPosition);
            UBindFunction(vrep ,simxSetJointPosition);
            UBindFunction(vrep ,simxGetJointMatrix);
            UBindFunction(vrep ,simxSetSphericalJointMatrix);
            UBindFunction(vrep ,simxSetJointTargetVelocity);
            UBindFunction(vrep ,simxSetJointTargetPosition);
            UBindFunction(vrep ,simxJointGetForce);
            UBindFunction(vrep ,simxGetJointForce);
            UBindFunction(vrep ,simxSetJointForce);
            UBindFunction(vrep ,simxReadForceSensor);
            UBindFunction(vrep ,simxBreakForceSensor);
            UBindFunction(vrep ,simxReadVisionSensor);
            UBindFunction(vrep ,simxGetObjectHandle);
            UBindFunction(vrep ,simxGetVisionSensorImage);
            UBindFunction(vrep ,simxSetVisionSensorImage);
            UBindFunction(vrep ,simxGetVisionSensorDepthBuffer);
            UBindFunction(vrep ,simxGetObjectChild);
            UBindFunction(vrep ,simxGetObjectParent);
            UBindFunction(vrep ,simxReadProximitySensor);
            UBindFunction(vrep ,simxLoadModel);
            UBindFunction(vrep ,simxLoadUI);
            UBindFunction(vrep ,simxLoadScene);
            UBindFunction(vrep ,simxStartSimulation);
            UBindFunction(vrep ,simxPauseSimulation);
            UBindFunction(vrep ,simxStopSimulation);
            UBindFunction(vrep ,simxGetUIHandle);
            UBindFunction(vrep ,simxGetUISlider);
            UBindFunction(vrep ,simxSetUISlider);
            UBindFunction(vrep ,simxGetUIEventButton);
            UBindFunction(vrep ,simxGetUIButtonProperty);
            UBindFunction(vrep ,simxSetUIButtonProperty);
            UBindFunction(vrep ,simxAddStatusbarMessage);
            UBindFunction(vrep ,simxAuxiliaryConsoleOpen);
            UBindFunction(vrep ,simxAuxiliaryConsoleClose);
            UBindFunction(vrep ,simxAuxiliaryConsolePrint);
            UBindFunction(vrep ,simxAuxiliaryConsoleShow);
            UBindFunction(vrep ,simxGetObjectOrientation);
            UBindFunction(vrep ,simxGetObjectPosition);
            UBindFunction(vrep ,simxSetObjectOrientation);
            UBindFunction(vrep ,simxSetObjectPosition);
            UBindFunction(vrep ,simxSetObjectParent);
            UBindFunction(vrep ,simxSetUIButtonLabel);
            UBindFunction(vrep ,simxGetLastErrors);
            UBindFunction(vrep ,simxGetArrayParameter);
            UBindFunction(vrep ,simxSetArrayParameter);            
            UBindFunction(vrep ,simxGetBooleanParameter);
            UBindFunction(vrep ,simxSetBooleanParameter);
            UBindFunction(vrep ,simxGetIntegerParameter);
            UBindFunction(vrep ,simxSetIntegerParameter);
            UBindFunction(vrep ,simxGetFloatingParameter);
            UBindFunction(vrep ,simxSetFloatingParameter);
            UBindFunction(vrep ,simxGetStringParameter);
            UBindFunction(vrep ,simxGetCollisionHandle);
            UBindFunction(vrep ,simxGetDistanceHandle);
            UBindFunction(vrep ,simxReadCollision);
            UBindFunction(vrep ,simxReadDistance);
            UBindFunction(vrep ,simxRemoveObject);
            UBindFunction(vrep ,simxRemoveModel);
            UBindFunction(vrep ,simxRemoveUI);
            UBindFunction(vrep ,simxCloseScene);
            UBindFunction(vrep ,simxGetObjects);
            UBindFunction(vrep ,simxDisplayDialog);
            UBindFunction(vrep ,simxEndDialog);
            UBindFunction(vrep ,simxGetDialogInput);
            UBindFunction(vrep ,simxGetDialogResult);
            UBindFunction(vrep ,simxCopyPasteObjects);
            UBindFunction(vrep ,simxGetObjectSelection);
            UBindFunction(vrep ,simxSetObjectSelection);
            UBindFunction(vrep ,simxClearFloatSignal);
            UBindFunction(vrep ,simxClearIntegerSignal);
            UBindFunction(vrep ,simxClearStringSignal);
            UBindFunction(vrep ,simxGetFloatSignal);
            UBindFunction(vrep ,simxGetIntegerSignal);
            UBindFunction(vrep ,simxGetStringSignal);
            UBindFunction(vrep ,simxSetFloatSignal);
            UBindFunction(vrep ,simxSetIntegerSignal);
            UBindFunction(vrep ,simxSetStringSignal);
            UBindFunction(vrep ,simxAppendStringSignal);
            UBindFunction(vrep ,simxWriteStringStream);
            UBindFunction(vrep ,simxGetObjectFloatParameter);
            UBindFunction(vrep ,simxSetObjectFloatParameter);
            UBindFunction(vrep ,simxGetObjectIntParameter);
            UBindFunction(vrep ,simxSetObjectIntParameter);
            UBindFunction(vrep ,simxGetModelProperty);
            UBindFunction(vrep ,simxSetModelProperty);
            UBindFunction(vrep ,simxStart);
            UBindFunction(vrep ,simxFinish);
            UBindFunction(vrep ,simxGetPingTime);
            UBindFunction(vrep ,simxGetLastCmdTime);
            UBindFunction(vrep ,simxSynchronousTrigger);
            UBindFunction(vrep ,simxSynchronous);
            UBindFunction(vrep ,simxPauseCommunication);
            UBindFunction(vrep ,simxGetInMessageInfo);
            UBindFunction(vrep ,simxGetOutMessageInfo);
            UBindFunction(vrep ,simxGetConnectionId);
            UBindFunction(vrep ,simxTransferFile);
            UBindFunction(vrep ,simxEraseFile);
            UBindFunction(vrep ,simxGetAndClearStringSignal);
            UBindFunction(vrep ,simxReadStringStream);
            UBindFunction(vrep ,simxQuery);
            UBindFunction(vrep ,simxGetObjectGroupData);
            UBindFunction(vrep ,simxGetObjectVelocity);
            UBindFunction(vrep ,simxPackInts);
            UBindFunction(vrep ,simxUnpackInts);
            UBindFunction(vrep ,simxPackFloats);
            UBindFunction(vrep ,simxUnpackFloats);
        }
        UList simxGetJointPosition (simxInt clientID,simxInt jointHandle, simxInt operationMode) {
            UList result;
            simxFloat position;
            result.push_back(::simxGetJointPosition(clientID,jointHandle, &position, operationMode));
            result.push_back(position);
            return result;
        }

        simxInt simxSetJointPosition (simxInt clientID,simxInt jointHandle, ufloat position, simxInt operationMode) {
            return ::simxSetJointPosition(clientID,jointHandle, simxFloat(position), operationMode);
        }

        UList simxGetJointMatrix (simxInt clientID,simxInt jointHandle, simxInt operationMode) {
            int i;
            UList       matrix;
            UList result;
            simxFloat   c_matrix[12];

            result.push_back(::simxGetJointMatrix(clientID,jointHandle, c_matrix, operationMode));
            for (i=0; i<12; i++) {
                matrix.push_back((ufloat)c_matrix[i]);
            }
            result.push_back(matrix);
            return result;
        }

        UList simxSetSphericalJointMatrix (simxInt clientID,simxInt jointHandle, UList matrix, simxInt operationMode) {
            int i;
            UList result;
            simxFloat   c_matrix[12];

            for (i=0; i<12; i++) {
                c_matrix[i] = simxFloat((ufloat)matrix[i]);
            }
            result.push_back(::simxSetSphericalJointMatrix(clientID,jointHandle, c_matrix, operationMode));
            return result;
        }

        simxInt simxSetJointTargetVelocity (simxInt clientID,simxInt jointHandle, ufloat targetVelocity, simxInt operationMode) {
            return ::simxSetJointTargetVelocity(clientID,jointHandle, simxFloat(targetVelocity), operationMode);
        }

        simxInt simxSetJointTargetPosition (simxInt clientID,simxInt jointHandle, ufloat targetPosition, simxInt operationMode) {
            return ::simxSetJointTargetPosition(clientID,jointHandle, simxFloat(targetPosition), operationMode);
        }

        UList simxJointGetForce (simxInt clientID,simxInt jointHandle, simxInt operationMode) {
            UList result;
            simxFloat force;
            result.push_back(::simxGetJointForce(clientID,jointHandle, &force, operationMode));
            result.push_back(force);
            return result;
        }

        UList simxGetJointForce (simxInt clientID,simxInt jointHandle, simxInt operationMode) {
            UList result;
            simxFloat force;
            result.push_back(::simxGetJointForce(clientID,jointHandle, &force, operationMode));
            result.push_back(force);
            return result;
        }

        simxInt simxSetJointForce (simxInt clientID,simxInt jointHandle, ufloat force, simxInt operationMode) {
            return ::simxSetJointForce(clientID,jointHandle, simxFloat(force), operationMode);
        }

        UList simxReadForceSensor (simxInt clientID,simxInt forceSensorHandle, simxInt operationMode) {
            int i;
            simxUChar state;
            UList result;            
            UList forceVector;
            UList torqueVector;

            simxFloat c_forceVector[3];
            simxFloat c_torqueVector[3];

            result.push_back(::simxReadForceSensor(clientID,forceSensorHandle, &state, c_forceVector, c_torqueVector, operationMode));

            for (i=0; i<3; i++) {
                forceVector.push_back((ufloat)c_forceVector[i]);
                torqueVector.push_back((ufloat)c_torqueVector[i]);
            }
            result.push_back((int) state);
            result.push_back(forceVector);
            result.push_back(torqueVector);
            return result;
        }

        simxInt simxBreakForceSensor (simxInt clientID,simxInt forceSensorHandle, simxInt operationMode) {
            return ::simxBreakForceSensor(clientID,forceSensorHandle, operationMode);
        }

        UList simxReadVisionSensor (simxInt clientID,simxInt sensorHandle, simxInt operationMode) {
            int i,x,ret, offset;
            UList values;
            UList result;
            simxUChar    detectionState;
            simxFloat   *auxValues;
            simxInt     *auxValuesCount;

            ret = ::simxReadVisionSensor(clientID,sensorHandle, &detectionState, &auxValues, &auxValuesCount, operationMode);
            if (ret==0) {
                offset = 0;
                for (i=0; i<auxValuesCount[0]; i++) {
					UList packet;

                    for (x=0; x<auxValuesCount[i+1]; x++) {
                        packet.push_back(auxValues[offset+x]);
                    }
					values.push_back(packet);
                    offset += auxValuesCount[i+1];
                }
	            simxReleaseBuffer((simxUChar*)auxValues);
		        simxReleaseBuffer((simxUChar*)auxValuesCount);
            }

            result.push_back(ret);
            result.push_back((int) detectionState);
            result.push_back(values);

            return result;
        }

        UList simxGetObjectHandle (simxInt clientID,const simxChar* objectName, simxInt operationMode) {
            UList result;
            simxInt handle;
            result.push_back(::simxGetObjectHandle(clientID,objectName, &handle, operationMode));
            result.push_back(handle);
            return result;
        }

        UList simxGetVisionSensorImage (simxInt clientID,simxInt sensorHandle, int options, simxInt operationMode) {
            int ret;
            simxUChar *c_image;
            simxInt resolution[2];
			simxInt bpp=3;
			if (options&1)
				bpp=1; // greyscale

            UImage image;
            UList result;

            ret = ::simxGetVisionSensorImage(clientID,sensorHandle, resolution, &c_image, (simxUChar)options, operationMode);
			if (ret==0)
			{
	            image.data   = (unsigned char*)c_image;
	            image.width  = resolution[0];
		        image.height = resolution[1];
				image.size = resolution[0]*resolution[1]*bpp;
				if (options&1)
					image.imageFormat = IMAGE_GREY8;
				else
					image.imageFormat = IMAGE_RGB;
			}
			else
			{
	            image.data=NULL;
	            image.width=0;
		        image.height=0;
				image.size=0;
				image.imageFormat=IMAGE_UNKNOWN;
			}
            result.push_back(ret);
            result.push_back(image);
            return result;
        }

        simxInt simxSetVisionSensorImage (simxInt clientID,simxInt sensorHandle, UImage image, simxInt operationMode) {
			simxInt options=0;
			simxInt bpp=3;
			if (image.imageFormat == IMAGE_GREY8)
			{
				options|=1;
				bpp=1;
			}
			else
			{
				if (image.imageFormat != IMAGE_RGB)
					return simx_return_local_error_flag;

			}

            return ::simxSetVisionSensorImage(clientID,sensorHandle,(simxUChar*) image.data, image.width*image.height*bpp, (simxUChar) options, operationMode);
        }

        UList simxGetVisionSensorDepthBuffer (simxInt clientID,simxInt sensorHandle, simxInt operationMode) {
            int ret;
            simxFloat *c_buffer;
            simxInt resolution[2];
            
            UImage image;
//            UList buffer;
//            UList reso;
            UList result;

            ret = ::simxGetVisionSensorDepthBuffer(clientID,sensorHandle, resolution, &c_buffer, operationMode);
			if (ret==0)
			{
	            image.data   = (unsigned char*)c_buffer;
	            image.width  = resolution[0];
		        image.height = resolution[1];
				image.size = resolution[0]*resolution[1]*4;
				image.imageFormat = IMAGE_UNKNOWN;
			}
			else
			{
	            image.data=NULL;
	            image.width=0;
		        image.height=0;
				image.size=0;
				image.imageFormat=IMAGE_UNKNOWN;
			}
            result.push_back(ret);
            result.push_back(image);
/*
			if (ret == 0) {
                for (i=0; i<resolution[0]*resolution[1]; i++) {
                    buffer.push_back(c_buffer[i]);
                }
				reso.push_back(resolution[0]);
				reso.push_back(resolution[1]);
            }
            result.push_back(ret);
            result.push_back(reso);
            result.push_back(buffer);
			*/
            return result;
        }

        UList simxGetObjectChild (simxInt clientID,simxInt parentObjectHandle, simxInt childIndex, simxInt operationMode) {
            UList result;
            simxInt childObjectHandle;
            result.push_back(::simxGetObjectChild(clientID,parentObjectHandle, childIndex, &childObjectHandle, operationMode));
            result.push_back(childObjectHandle);
            return result;
        }

        UList simxGetObjectParent (simxInt clientID,simxInt childObjectHandle, simxInt operationMode) {
            UList result;
            simxInt parentObjectHandle;
            result.push_back(::simxGetObjectParent(clientID,childObjectHandle, &parentObjectHandle, operationMode));
            result.push_back(parentObjectHandle);
            return result;
        }

        UList simxReadProximitySensor (simxInt clientID,simxInt sensorHandle, simxInt operationMode) {
            int i, ret;
            simxUChar  detectionState;
            simxFloat c_detectedPoint[3];
            simxInt detectedObjectHandle;
            simxFloat c_detectedSurfaceNormalVector[3];

            UList result;
            UList detectedPoint;
            UList detectedSurfaceNormalVector;

            ret = ::simxReadProximitySensor(clientID,sensorHandle, &detectionState, c_detectedPoint, &detectedObjectHandle, c_detectedSurfaceNormalVector, operationMode);
            if (ret == 0) {
                for (i=0; i<3; i++) {
                    detectedPoint.push_back(c_detectedPoint[i]);
                    detectedSurfaceNormalVector.push_back(c_detectedSurfaceNormalVector[i]);
                }
            }

            result.push_back(ret);
            result.push_back((int) detectionState);
            result.push_back(detectedPoint);
            result.push_back(detectedObjectHandle);
            result.push_back(detectedSurfaceNormalVector);
            return result;
        }

        UList simxLoadModel (simxInt clientID,const simxChar* modelPathAndName, int options, simxInt operationMode) {
            UList result;
            simxInt baseHandle;
            result.push_back(::simxLoadModel(clientID,modelPathAndName, (simxUChar) options, &baseHandle, operationMode));
            result.push_back(baseHandle);
            return result;
        }

        UList simxLoadUI (simxInt clientID,const simxChar* uiPathAndName, int options, simxInt operationMode) {
            int i, ret;
            simxInt count;
            simxInt *c_uiHandles;
            
            UList uiHandles;
            UList result;

            ret = ::simxLoadUI(clientID,uiPathAndName, (simxUChar) options, &count, &c_uiHandles, operationMode);
            if (ret == 0) {
                for (i=0; i<count; i++) {
                    uiHandles.push_back(c_uiHandles[i]);
                }
				simxReleaseBuffer((simxUChar*)c_uiHandles);
            }
            result.push_back(ret);
            result.push_back(uiHandles);
            return result;
        }

        simxInt simxLoadScene (simxInt clientID,const simxChar* scenePathAndName, int options, simxInt operationMode) {
            return ::simxLoadScene(clientID,scenePathAndName, (simxUChar) options, operationMode);
        }

        simxInt simxStartSimulation (simxInt clientID,simxInt operationMode) {
            return ::simxStartSimulation(clientID,operationMode);
        }

        simxInt simxPauseSimulation (simxInt clientID,simxInt operationMode) {
            return ::simxPauseSimulation(clientID,operationMode);
        }

        simxInt simxStopSimulation (simxInt clientID,simxInt operationMode) {
            return ::simxStopSimulation(clientID,operationMode);
        }

        UList simxGetUIHandle (simxInt clientID,const simxChar* uiName, simxInt operationMode) {
            UList result;
            simxInt handle;
            result.push_back(::simxGetUIHandle(clientID,uiName, &handle, operationMode));
            result.push_back(handle);
            return result;
        }

        UList simxGetUISlider (simxInt clientID,simxInt uiHandle, simxInt uiButtonID, simxInt operationMode) {
            UList result;
            simxInt position;
            result.push_back(::simxGetUISlider(clientID,uiHandle, uiButtonID, &position, operationMode));
            result.push_back(position);
            return result;
        }

        simxInt simxSetUISlider (simxInt clientID,simxInt uiHandle, simxInt uiButtonID, simxInt position, simxInt operationMode) {
            return ::simxSetUISlider(clientID,uiHandle, uiButtonID, position, operationMode);
        }

        UList simxGetUIEventButton (simxInt clientID,simxInt uiHandle, simxInt operationMode) {
            int i, ret;
            simxInt c_auxValues[2];
            simxInt uiEventButtonID;

            UList auxValues;
            UList result;

            ret = ::simxGetUIEventButton(clientID,uiHandle, &uiEventButtonID, c_auxValues, operationMode);
            if (ret == 0) {
                for (i=0; i<2; i++) {
                    auxValues.push_back(c_auxValues[i]);
                }
            }

            result.push_back(ret);
            result.push_back(uiEventButtonID);
            result.push_back(auxValues);
            return result;
        }

        UList simxGetUIButtonProperty (simxInt clientID,simxInt uiHandle, simxInt uiButtonID, simxInt operationMode) {
            simxInt prop;
            UList result;

            result.push_back(::simxGetUIButtonProperty(clientID,uiHandle, uiButtonID, &prop, operationMode));
            result.push_back(prop);
            return result;
        }

        simxInt simxSetUIButtonProperty (simxInt clientID,simxInt uiHandle, simxInt uiButtonID, simxInt prop, simxInt operationMode) {
            return ::simxSetUIButtonProperty(clientID,uiHandle, uiButtonID, prop, operationMode);
        }

        simxInt simxAddStatusbarMessage (simxInt clientID,const simxChar* message, simxInt operationMode) {
            return ::simxAddStatusbarMessage(clientID,message, operationMode);
        }

        UList simxAuxiliaryConsoleOpen (simxInt clientID,const simxChar* title, simxInt maxLines, simxInt mode, UList position, UList size,
                UList textColor, UList backgroundColor, simxInt operationMode) {
            int i;

			simxInt* cc_size=NULL;
			simxInt* cc_pos=NULL;
			simxFloat* cc_textCol=NULL;
			simxFloat* cc_backCol=NULL;
            simxInt c_size[2];
            simxInt c_position[2];
            simxFloat c_textColor[3];
            simxFloat c_backgroundColor[3];

			if (position.size()>=2)
			{
	            for (i=0; i<2; i++) {
	             c_position[i] = position[i];
				}
				cc_pos=c_position;
			}

			if (size.size()>=2)
			{
	            for (i=0; i<2; i++) {
	             c_size[i] = size[i];
				}
				cc_size=c_size;
			}

			if (textColor.size()>=3)
			{
	            for (i=0; i<3; i++) {
	             c_textColor[i] = simxFloat((ufloat)textColor[i]);
				}
				cc_textCol=c_textColor;
			}

			if (backgroundColor.size()>=3)
			{
	            for (i=0; i<3; i++) {
	             c_backgroundColor[i] = simxFloat((ufloat)backgroundColor[i]);
				}
				cc_backCol=c_backgroundColor;
			}

			simxInt consoleHandle;

            UList result;

            result.push_back(::simxAuxiliaryConsoleOpen(clientID,title, maxLines, mode, cc_pos, cc_size, cc_textCol, cc_backCol, &consoleHandle, operationMode));
            result.push_back(consoleHandle);
            return result;
        }

        simxInt simxAuxiliaryConsoleClose (simxInt clientID,simxInt consoleHandle, simxInt operationMode) {
            return ::simxAuxiliaryConsoleClose(clientID,consoleHandle, operationMode);
        }

        simxInt simxAuxiliaryConsolePrint (simxInt clientID,simxInt consoleHandle, const simxChar* txt, simxInt operationMode) {
			simxChar* cc_txt=NULL;
			if (strlen(txt)!=0)
				cc_txt=(simxChar*)txt;
            return ::simxAuxiliaryConsolePrint(clientID,consoleHandle, cc_txt, operationMode);
        }

        simxInt simxAuxiliaryConsoleShow (simxInt clientID,simxInt consoleHandle, int showState, simxInt operationMode) {
            return ::simxAuxiliaryConsoleShow(clientID,consoleHandle, (simxUChar) showState, operationMode);
        }

        UList simxGetObjectOrientation (simxInt clientID,simxInt objectHandle, simxInt relativeToObjectHandle, simxInt operationMode) {
            int ret, i;
            simxFloat c_eulerAngles[3];

            UList eulerAngles;
            UList result;

            ret = ::simxGetObjectOrientation(clientID,objectHandle, relativeToObjectHandle, c_eulerAngles, operationMode);
            if (ret == 0) {
                for (i=0; i<3; i++) {
                    eulerAngles.push_back(c_eulerAngles[i]);
                }
            }

            result.push_back(ret);
            result.push_back(eulerAngles);
            return result;
        }

        UList simxGetObjectPosition (simxInt clientID,simxInt objectHandle, simxInt relativeToObjectHandle, simxInt operationMode) {
            int ret, i;
            simxFloat c_position[3];

            UList position;
            UList result;

            ret = ::simxGetObjectPosition(clientID,objectHandle, relativeToObjectHandle, c_position, operationMode);
            if (ret == 0) {
                for (i=0; i<3; i++) {
                    position.push_back(c_position[i]);
                }
            }

            result.push_back(ret);
            result.push_back(position);
            return result;
        }

        simxInt simxSetObjectOrientation (simxInt clientID,simxInt objectHandle, simxInt relativeToObjectHandle, UList eulerAngles, simxInt operationMode) {
            int i;
            simxFloat c_eulerAngles[3];           
            for (i=0; i<3; i++) {
                c_eulerAngles[i] = simxFloat((ufloat) eulerAngles[i]);
            }
            return ::simxSetObjectOrientation(clientID,objectHandle, relativeToObjectHandle, c_eulerAngles, operationMode);
        }

        simxInt simxSetObjectPosition (simxInt clientID,simxInt objectHandle, simxInt relativeToObjectHandle, UList position, simxInt operationMode) {
            int i;
            simxFloat c_position[3];           
            for (i=0; i<3; i++) {
                c_position[i] = simxFloat((ufloat) position[i]);
            }

            return  ::simxSetObjectPosition(clientID,objectHandle, relativeToObjectHandle, c_position, operationMode);
        }

        simxInt simxSetObjectParent (simxInt clientID,simxInt objectHandle, simxInt parentObject, int keepInPlace, simxInt operationMode) {
            return ::simxSetObjectParent(clientID,objectHandle, parentObject, (simxUChar) keepInPlace, operationMode);
        }

        simxInt simxSetUIButtonLabel (simxInt clientID,simxInt uiHandle, simxInt uiButtonID, const simxChar* upStateLabel, const simxChar* downStateLabel, simxInt operationMode) {
            return ::simxSetUIButtonLabel(clientID,uiHandle, uiButtonID, upStateLabel, downStateLabel, operationMode);
        }

        UList simxGetLastErrors (simxInt clientID,simxInt operationMode) {
            int ret, i;
            simxInt errorCnt;
            simxChar *c_errorStrings;

            UList errorStrings;
            UList result;

            ret = ::simxGetLastErrors(clientID,&errorCnt, &c_errorStrings, operationMode);
            if (ret == 0) {
                for (i=0; i<errorCnt; i++) {
                    std::string err_str(strdup(c_errorStrings));
                    errorStrings.push_back(err_str);
                    c_errorStrings += strlen(c_errorStrings) + 1; // one for NULL byte
                }
            }

            result.push_back(ret);
            result.push_back(errorStrings);
            return result;
        }

        UList simxGetArrayParameter (simxInt clientID,simxInt paramIdentifier, simxInt operationMode) {
            int i, ret;
            UList paramValues;
            UList result;            
            simxFloat c_paramValues[3];

            ret = ::simxGetArrayParameter(clientID,paramIdentifier, c_paramValues, operationMode);
            if (ret == 0) {
                for (i=0; i<3; i++) {
                    paramValues.push_back(c_paramValues[i]);
                }
            }

            result.push_back(ret);
            result.push_back(paramValues);
            return result;
        }

        simxInt simxSetArrayParameter (simxInt clientID,simxInt paramIdentifier, UList paramValues, simxInt operationMode) {
            int i;
            simxFloat c_paramValues[3];

            for (i=0; i<3; i++) {
                c_paramValues[i] = simxFloat((ufloat) paramValues[i]);
            }
            return ::simxSetArrayParameter(clientID,paramIdentifier, c_paramValues, operationMode);
        }

        UList simxGetBooleanParameter (simxInt clientID,simxInt paramIdentifier, simxInt operationMode) {
            UList result;
            simxUChar paramValue;
            result.push_back(::simxGetBooleanParameter(clientID,paramIdentifier, &paramValue, operationMode));
            result.push_back((int) paramValue);
            return result;
        }

        simxInt simxSetBooleanParameter (simxInt clientID,simxInt paramIdentifier, simxInt paramValue, simxInt operationMode) {
            return ::simxSetBooleanParameter(clientID,paramIdentifier, (simxUChar) paramValue, operationMode);
        }

        UList simxGetIntegerParameter (simxInt clientID,simxInt paramIdentifier, simxInt operationMode) {
            UList result;
            simxInt paramValue;
            result.push_back(::simxGetIntegerParameter(clientID,paramIdentifier, &paramValue, operationMode));
            result.push_back(paramValue);
            return result;
        }

        simxInt simxSetIntegerParameter (simxInt clientID,simxInt paramIdentifier, simxInt paramValue, simxInt operationMode) {
            return ::simxSetIntegerParameter(clientID,paramIdentifier, paramValue, operationMode);
        }

        UList simxGetFloatingParameter (simxInt clientID,simxInt paramIdentifier, simxInt operationMode) {
            UList result;
            simxFloat paramValue;
            result.push_back(::simxGetFloatingParameter(clientID,paramIdentifier, &paramValue, operationMode));
            result.push_back(paramValue);
            return result;
        }

        simxInt simxSetFloatingParameter (simxInt clientID,simxInt paramIdentifier, ufloat paramValue, simxInt operationMode) {
            return ::simxSetFloatingParameter(clientID,paramIdentifier, (float) paramValue, operationMode);
        }

        UList simxGetStringParameter (simxInt clientID,simxInt paramIdentifier, simxInt operationMode) {
            UList result;
			int ret;
            simxChar *paramValue;
			ret=::simxGetStringParameter(clientID,paramIdentifier, &paramValue, operationMode);
            result.push_back(ret);
			if (ret==0)
	            result.push_back(paramValue);
			else
	            result.push_back("");
            return result;
        }

        UList simxGetCollisionHandle (simxInt clientID,const simxChar* collisionObjectName, simxInt operationMode) {
            UList result;
            simxInt handle;
            result.push_back(::simxGetCollisionHandle(clientID,collisionObjectName, &handle, operationMode));
            result.push_back(handle);
            return result;
        }

        UList simxGetDistanceHandle (simxInt clientID,const simxChar* distanceObjectName, simxInt operationMode) {
            UList result;
            simxInt handle;
            result.push_back(::simxGetDistanceHandle(clientID,distanceObjectName, &handle, operationMode));
            result.push_back(handle);
            return result;
        }

        UList simxReadCollision (simxInt clientID,simxInt collisionObjectHandle, simxInt operationMode) {
            UList result;
            simxUChar collisionState;
            result.push_back(::simxReadCollision(clientID,collisionObjectHandle, &collisionState, operationMode));
            result.push_back(collisionState);
            return result;
        }

        UList simxReadDistance (simxInt clientID,simxInt distanceObjectHandle, simxInt operationMode) {
            UList result;
            simxFloat minimumDistance;
            result.push_back(::simxReadDistance(clientID,distanceObjectHandle, &minimumDistance, operationMode));
            result.push_back(minimumDistance);
            return result;
        }

        simxInt simxRemoveObject (simxInt clientID,simxInt objectHandle, simxInt operationMode) {
            return ::simxRemoveObject(clientID,objectHandle, operationMode);
        }

        simxInt simxRemoveModel (simxInt clientID,simxInt objectHandle, simxInt operationMode) {
            return ::simxRemoveModel(clientID,objectHandle, operationMode);
        }

        simxInt simxRemoveUI (simxInt clientID,simxInt uiHandle, simxInt operationMode) {
            return ::simxRemoveUI(clientID,uiHandle, operationMode);
        }

        simxInt simxCloseScene (simxInt clientID,simxInt operationMode) {
            return ::simxCloseScene(clientID,operationMode);
        }

        UList simxGetObjects (simxInt clientID,simxInt objectType, simxInt operationMode) {
            UList result;
            UList objectHandles;

            int ret, i;
            simxInt objectCount;
            simxInt *c_objectHandles;
            
            ret = ::simxGetObjects(clientID,objectType, &objectCount, &c_objectHandles, operationMode);
            if (ret == 0) {
                for (i=0; i<objectCount; i++) {
                    objectHandles.push_back(c_objectHandles[i]);
                }
            }

            result.push_back(ret);
            result.push_back(objectHandles);
            return result;
        }

        UList simxDisplayDialog (simxInt clientID,const simxChar* titleText, const simxChar* mainText, simxInt dialogType, 
                const simxChar* initialText, UList tileColors, UList dialogColors, simxInt operationMode) {
            int ret,i;
            simxInt uiHandle;
            simxInt dialogHandle;
            simxFloat c_titleColors[6];
            simxFloat c_dialogColors[6];
            simxFloat* cc_titleColors=NULL;
            simxFloat* cc_dialogColors=NULL;

            UList result;

			if (tileColors.size()>=6)
			{
				for (i=0; i<6; i++)
					c_titleColors[i]=simxFloat((ufloat)tileColors[i]);
				cc_titleColors=c_titleColors;
			}

			if (dialogColors.size()>=6)
			{
				for (i=0; i<6; i++)
					c_dialogColors[i]=simxFloat((ufloat)dialogColors[i]);
				cc_dialogColors=c_dialogColors;
			}

            ret = ::simxDisplayDialog(clientID,titleText, mainText, dialogType, initialText, cc_titleColors, cc_dialogColors, &dialogHandle, &uiHandle, operationMode);
            result.push_back(ret);
            result.push_back(dialogHandle);
            result.push_back(uiHandle);
            return result;
        }

        simxInt simxEndDialog (simxInt clientID,simxInt dialogHandle, simxInt operationMode) {
            return ::simxEndDialog(clientID,dialogHandle, operationMode);
        }

        UList simxGetDialogInput (simxInt clientID,simxInt dialogHandle, simxInt operationMode) {
            UList result;
			int ret;
            simxChar *inputText;
			ret=::simxGetDialogInput(clientID,dialogHandle, &inputText, operationMode);
            result.push_back(ret);
			if (ret==0)
	            result.push_back(inputText);
			else
				result.push_back("");
            return result;
        }

        UList simxGetDialogResult (simxInt clientID,simxInt dialogHandle, simxInt operationMode) {
            UList result;
            simxInt c_result;
            result.push_back(::simxGetDialogResult(clientID,dialogHandle, &c_result, operationMode));
            result.push_back(c_result);
            return result;
        }

        UList simxCopyPasteObjects (simxInt clientID,UList objectHandles,simxInt operationMode) {
            int ret,i;
            simxInt newObjectCount;
            simxInt *c_newObjectHandles;
            simxInt *c_objectHandles;

            UList result;
            UList newObjectHandles;

            c_objectHandles = (simxInt*) malloc(sizeof(simxInt)*objectHandles.size());
            for (i=0; i<int(objectHandles.size()); i++) {
                c_objectHandles[i] = (simxInt)objectHandles[i];
            }

            ret = ::simxCopyPasteObjects(clientID,c_objectHandles, objectHandles.size(), &c_newObjectHandles, &newObjectCount, operationMode);
            if (ret == 0) {
                for (i=0; i<newObjectCount; i++) {
                    newObjectHandles.push_back(c_newObjectHandles[i]);
                }
            }
            result.push_back(ret);
            result.push_back(newObjectHandles);
            free(c_objectHandles);
            return result;
        }

        UList simxGetObjectSelection (simxInt clientID,simxInt operationMode) {
            int i, ret;
            simxInt objectCount;
            simxInt *c_objectHandles;
            
            UList objectHandles;
            UList result;

            ret = ::simxGetObjectSelection(clientID,&c_objectHandles, &objectCount, operationMode);
            if (ret == 0) {
                for (i=0; i<objectCount; i++) {
                    objectHandles.push_back(c_objectHandles[i]);                         
                }
            }

            result.push_back(ret);
            result.push_back(objectHandles);
            return result;
        }

        simxInt simxSetObjectSelection (simxInt clientID,UList objectHandles, simxInt operationMode) {
            int i,ret;
            simxInt *c_objectHandles;
            c_objectHandles = (simxInt*) malloc(sizeof(simxInt) * objectHandles.size());

            for (i=0; i<int(objectHandles.size()); i++) {
                c_objectHandles[i] = objectHandles[i];
            }

            ret = ::simxSetObjectSelection(clientID,c_objectHandles, objectHandles.size(), operationMode);
            free(c_objectHandles);
            return ret;
        }

        simxInt simxClearFloatSignal (simxInt clientID,const simxChar* signalName, simxInt operationMode) {
            return  ::simxClearFloatSignal(clientID,signalName, operationMode);
        }

        simxInt simxClearIntegerSignal (simxInt clientID,const simxChar* signalName, simxInt operationMode) {
            return ::simxClearIntegerSignal(clientID,signalName, operationMode);
        }

        simxInt simxClearStringSignal (simxInt clientID,const simxChar* signalName, simxInt operationMode) {
            return ::simxClearStringSignal(clientID,signalName, operationMode);
        }

        UList simxGetFloatSignal (simxInt clientID,const simxChar* signalName, simxInt operationMode) {
            UList result;
            simxFloat signalValue;

            result.push_back(::simxGetFloatSignal(clientID,signalName, &signalValue, operationMode));
            result.push_back(signalValue);
            return result;
        }

        UList simxGetIntegerSignal (simxInt clientID,const simxChar* signalName, simxInt operationMode) {
            UList result;
            simxInt signalValue;

            result.push_back(::simxGetIntegerSignal(clientID,signalName, &signalValue, operationMode));
            result.push_back(signalValue);
            return result;
        }

        UList simxGetStringSignal (simxInt clientID,const simxChar* signalName, simxInt operationMode) {
            UList result;
			int ret;
            simxUChar *signalValue;
            simxInt signalLength;
			ret=::simxGetStringSignal(clientID,signalName, &signalValue, &signalLength, operationMode);
            result.push_back(ret);
			if (ret==0)
			{
				std::string s((char*)signalValue,signalLength);
				UValue val(s);
				result.push_back(val);
			}
			else
	            result.push_back("");
            return result;
        }

        UList simxGetAndClearStringSignal (simxInt clientID,const simxChar* signalName, simxInt operationMode) {
            UList result;
			int ret;
            simxUChar *signalValue;
            simxInt signalLength;
			ret=::simxGetAndClearStringSignal(clientID,signalName, &signalValue, &signalLength, operationMode);
            result.push_back(ret);
			if (ret==0)
			{
				std::string s((char*)signalValue,signalLength);
				UValue val(s);
				result.push_back(val);
			}
			else
	            result.push_back("");
            return result;
        }

        UList simxReadStringStream (simxInt clientID,const simxChar* signalName, simxInt operationMode) {
            UList result;
			int ret;
            simxUChar *signalValue;
            simxInt signalLength;
			ret=::simxReadStringStream(clientID,signalName, &signalValue, &signalLength, operationMode);
            result.push_back(ret);
			if (ret==0)
			{
				std::string s((char*)signalValue,signalLength);
				UValue val(s);
				result.push_back(val);
			}
			else
	            result.push_back("");
            return result;
        }

        simxInt simxSetFloatSignal (simxInt clientID,const simxChar* signalName, ufloat signalValue, simxInt operationMode) {
            return ::simxSetFloatSignal(clientID,signalName, simxFloat(signalValue), operationMode);
        }

        simxInt simxSetIntegerSignal (simxInt clientID,const simxChar* signalName, simxInt signalValue, simxInt operationMode) {
            return ::simxSetIntegerSignal(clientID,signalName, signalValue, operationMode);
        }

        simxInt simxSetStringSignal (simxInt clientID,const simxChar* signalName, UValue signalValue, simxInt operationMode) {
			simxInt ret=simx_return_local_error_flag;
			if (signalValue.type==DATA_STRING)
			{
				ret=::simxSetStringSignal(clientID,signalName,(const simxUChar*)signalValue.stringValue->c_str(), signalValue.stringValue->length(), operationMode);
			}
			return ret;
        }

        simxInt simxAppendStringSignal (simxInt clientID,const simxChar* signalName, UValue signalValue, simxInt operationMode) {
			simxInt ret=simx_return_local_error_flag;
			if (signalValue.type==DATA_STRING)
			{
				ret=::simxAppendStringSignal(clientID,signalName,(const simxUChar*)signalValue.stringValue->c_str(), signalValue.stringValue->length(), operationMode);
			}
			return ret;
        }

        simxInt simxWriteStringStream (simxInt clientID,const simxChar* signalName, UValue signalValue, simxInt operationMode) {
			simxInt ret=simx_return_local_error_flag;
			if (signalValue.type==DATA_STRING)
			{
				ret=::simxWriteStringStream(clientID,signalName,(const simxUChar*)signalValue.stringValue->c_str(), signalValue.stringValue->length(), operationMode);
			}
			return ret;
        }

        UList simxGetObjectFloatParameter (simxInt clientID,simxInt objectHandle, simxInt parameterID, simxInt operationMode) {
            UList result;
            simxFloat parameterValue;
            result.push_back(::simxGetObjectFloatParameter(clientID,objectHandle, parameterID, &parameterValue, operationMode));
            result.push_back(parameterValue);
            return result;
        }

        simxInt simxSetObjectFloatParameter (simxInt clientID,simxInt objectHandle, simxInt parameterID, ufloat parameterValue, simxInt operationMode) {
            return ::simxSetObjectFloatParameter(clientID,objectHandle, parameterID, simxFloat(parameterValue), operationMode);
        }

        UList simxGetObjectIntParameter (simxInt clientID,simxInt objectHandle, simxInt parameterID, simxInt operationMode) {
            UList result;
            simxInt parameterValue;
            result.push_back(::simxGetObjectIntParameter(clientID,objectHandle, parameterID, &parameterValue, operationMode));
            result.push_back(parameterValue);
            return result;
        }

        simxInt simxSetObjectIntParameter (simxInt clientID,simxInt objectHandle, simxInt parameterID, simxInt parameterValue, simxInt operationMode) {
            return ::simxSetObjectIntParameter(clientID,objectHandle, parameterID, parameterValue, operationMode);
        }

        UList simxGetModelProperty (simxInt clientID,simxInt objectHandle, simxInt operationMode) {
            UList result;
            simxInt prop;
            result.push_back(::simxGetModelProperty(clientID,objectHandle, &prop, operationMode));
            result.push_back(prop);
            return result;
        }

        simxInt simxSetModelProperty (simxInt clientID,simxInt objectHandle, simxInt prop, simxInt operationMode) {
            return ::simxSetModelProperty(clientID,objectHandle, prop, operationMode);
        }

        simxInt simxStart (const simxChar* connectionAddress, simxInt connectionPort, int waitUntilConnected, int doNotReconnectOnceDisconnected,simxInt timeOutInMs,simxInt commThreadCycleInMs) {
            return ::simxStart(connectionAddress, connectionPort,(simxUChar) waitUntilConnected, (simxUChar) doNotReconnectOnceDisconnected,timeOutInMs,commThreadCycleInMs);
        }

        simxVoid simxFinish (simxInt clientID) {
            ::simxFinish(clientID);
        }

        UList simxGetPingTime (simxInt clientID) {
            UList result;
            simxInt pingTime;
            result.push_back(::simxGetPingTime(clientID,&pingTime));
            result.push_back(pingTime);
            return result;
        }

        simxInt simxGetLastCmdTime (simxInt clientID) {
            return ::simxGetLastCmdTime(clientID);
        }

        simxInt simxSynchronousTrigger (simxInt clientID) {
            return ::simxSynchronousTrigger(clientID);
        }

        simxInt simxSynchronous (simxInt clientID,int enable) {
            return ::simxSynchronous(clientID,(simxUChar) enable);
        }

        simxInt simxPauseCommunication (simxInt clientID,int enable) {
            return ::simxPauseCommunication(clientID,(simxUChar) enable);
        }

        UList simxGetInMessageInfo (simxInt clientID,simxInt infoType) {
            UList result;
            simxInt info;
            result.push_back(::simxGetInMessageInfo(clientID,infoType, &info));
            result.push_back(info);
            return result;
        }

        UList simxGetOutMessageInfo (simxInt clientID,simxInt infoType) {
            UList result;
            simxInt info;
            result.push_back(::simxGetOutMessageInfo(clientID,infoType, &info));
            result.push_back(info);
            return result;
        }

        simxInt simxGetConnectionId (simxInt clientID) {
            return ::simxGetConnectionId(clientID);
        }

        simxInt simxTransferFile (simxInt clientID,const simxChar* filePathAndName, const simxChar* fileName_serverSide, simxInt timeOut, simxInt operationMode) {
            return ::simxTransferFile(clientID,filePathAndName, fileName_serverSide, timeOut, operationMode);
        }

        simxInt simxEraseFile (simxInt clientID,const simxChar* fileName_serverSide, simxInt operationMode) {
            return ::simxEraseFile(clientID,fileName_serverSide, operationMode);
        }


        UList simxQuery (simxInt clientID,const simxChar* signalName, UValue signalValue, const simxChar* retSignalName, simxInt timeOutInMs) {
            UList result;
			int ret=simx_return_local_error_flag;
            simxUChar *retSignalValue;
	        simxInt retSignalLength;
			if (signalValue.type==DATA_STRING)
			{
				ret=::simxQuery(clientID,signalName,(const simxUChar*)signalValue.stringValue->c_str(), signalValue.stringValue->length(), retSignalName, &retSignalValue, &retSignalLength, timeOutInMs);
	            result.push_back(ret);
			}
			if (ret==0)
			{
				std::string s((char*)retSignalValue,retSignalLength);
				UValue val(s);
				result.push_back(val);
			}
			else
	            result.push_back("");
			return result;
        }
        UList simxGetObjectGroupData (simxInt clientID,simxInt objectType,simxInt dataType,simxInt operationMode) {
			int theHandleCount;
			int theIntDataCount;
			int theFloatDataCount;
			int theStringDataCount;
			int* theHandles;
			int* theInts;
			float* theFloats;
			char* theStrings;
			int ret=::simxGetObjectGroupData(clientID,objectType,dataType,&theHandleCount,&theHandles,&theIntDataCount,&theInts,&theFloatDataCount,&theFloats,&theStringDataCount,&theStrings,operationMode);
			UList result;
			result.push_back(ret);
            if (ret == 0) 
			{
				UList handles;
				UList intData;
	            UList floatData;
				UList stringData;

                for (int i=0; i<theHandleCount; i++) 
					handles.push_back(theHandles[i]);
				result.push_back(handles);

                for (int i=0; i<theIntDataCount; i++) 
					intData.push_back(theInts[i]);
				result.push_back(intData);
				
                for (int i=0; i<theFloatDataCount; i++) 
					floatData.push_back((ufloat)theFloats[i]);
				result.push_back(floatData);
				
				for (int i=0; i<theStringDataCount; i++) 
				{
                    std::string _str(strdup(theStrings));
                    stringData.push_back(_str);
                    theStrings += strlen(theStrings) + 1; // one for NULL byte
                }
				result.push_back(stringData);
            }
			return result;
        }

        UList simxGetObjectVelocity (simxInt clientID,simxInt objectHandle, simxInt operationMode) {
            int i;
            UList result;            
            UList linearVelocity;
            UList angularVelocity;

            simxFloat c_linearVelocity[3];
            simxFloat c_angularVelocity[3];

            result.push_back(::simxGetObjectVelocity(clientID,objectHandle, c_linearVelocity, c_angularVelocity, operationMode));

            for (i=0; i<3; i++) {
                linearVelocity.push_back((ufloat)c_linearVelocity[i]);
                angularVelocity.push_back((ufloat)c_angularVelocity[i]);
            }
            result.push_back(linearVelocity);
            result.push_back(angularVelocity);
            return result;
        }

        UValue simxPackInts (UList intList) {
			std::string s;
			for (int i=0;i<int(intList.size());i++)
			{
				int v=intList[i];
				char* vp=(char*)(&v);
				s+=vp[0];
				s+=vp[1];
				s+=vp[2];
				s+=vp[3];
			}
			UValue str(s);
			return(str);
        }

        UList simxUnpackInts (UValue string) {
			UList retList;
			const char* p=string.stringValue->c_str();
			for (int i=0;i<int(string.stringValue->length())/4;i++)
			{
				int v=((int*)(p+4*i))[0];
				retList.push_back(v);
			}
			return(retList);
        }

        UValue simxPackFloats (UList floatList) {
			std::string s;
			for (int i=0;i<int(floatList.size());i++)
			{
				float v=(ufloat)floatList[i];
				char* vp=(char*)(&v);
				s+=vp[0];
				s+=vp[1];
				s+=vp[2];
				s+=vp[3];
			}
			UValue str(s);
			return(str);
        }

        UList simxUnpackFloats (UValue string) {
			UList retList;
			const char* p=string.stringValue->c_str();
			for (int i=0;i<int(string.stringValue->length())/4;i++)
			{
				float v=((float*)(p+4*i))[0];
				retList.push_back(v);
			}
			return(retList);
        }
};

// Register the class to the Urbi kernel. 
UStart (vrep);


