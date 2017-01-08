// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include "v_repExtSimpleFilter.h"
#include "v_repLib.h"
#include "simplefilterdialog.h"
#include "simpleFilterGlobal.h"
#include <iostream>

#ifdef _WIN32
	#include <direct.h>
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
	#include <unistd.h>
	#include <string.h>
	#define _stricmp(x,y) strcasecmp(x,y)
#endif

const int filterCount=2; // this plugin exports 2 filters
int filterID[filterCount]={-1,0}; // Filters with negative IDs won't have a dialog or special triggering conditions (negative IDs for simple filters!)
// Header ID (DEVELOPER_DATA_HEADER), filterIDs and parameter values of a filter are serialized!! (don't change at will!)
int nextFilterEnum=0; // used during enumeration
char* filterName[filterCount]={"SimpleExampleFilter1: add noise","SimpleExampleFilter2: black squares"}; // filter names

LIBRARY vrepLib;

VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt)
{ // This is called just once, at the start of V-REP.

	// Dynamically load and bind V-REP functions:
	 // ******************************************
	 // 1. Figure out this plugin's directory:
	 char curDirAndFile[1024];
 #ifdef _WIN32
	 _getcwd(curDirAndFile, sizeof(curDirAndFile));
 #elif defined (__linux) || defined (__APPLE__)
	 getcwd(curDirAndFile, sizeof(curDirAndFile));
 #endif
	 std::string currentDirAndPath(curDirAndFile);
	 // 2. Append the V-REP library's name:
	 std::string temp(currentDirAndPath);
 #ifdef _WIN32
	 temp+="/v_rep.dll";
 #elif defined (__linux)
	 temp+="/libv_rep.so";
 #elif defined (__APPLE__)
	 temp+="/libv_rep.dylib";
 #endif /* __linux || __APPLE__ */
	 // 3. Load the V-REP library:
	 vrepLib=loadVrepLibrary(temp.c_str());
	 if (vrepLib==NULL)
	 {
		 std::cout << "Error, could not find or correctly load the V-REP library. Cannot start 'SimpleFilter' plugin.\n";
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 if (getVrepProcAddresses(vrepLib)==0)
	 {
		 std::cout << "Error, could not find all required functions in the V-REP library. Cannot start 'SimpleFilter' plugin.\n";
		 unloadVrepLibrary(vrepLib);
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 // ******************************************

	 // Check the version of V-REP:
	 // ******************************************
	 int vrepVer;
	 simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
	 if (vrepVer<20604) // if V-REP version is smaller than 2.06.04
	 {
		 std::cout << "Sorry, your V-REP copy is somewhat old. Cannot start 'SimpleFilter' plugin.\n";
		 unloadVrepLibrary(vrepLib);
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 // ******************************************


	return(5);	// initialization went fine, return the version number of this plugin!
				// version 1 was for V-REP 2.5.11 or earlier
				// version 2 was for V-REP 2.5.12 or earlier
				// version 3 was for V-REP versions before V-REP 2.6.7
				// version 4 is the Qt version
				// version 5 is since 10/1/2014
}

VREP_DLLEXPORT void v_repEnd()
{ // This is called just once, at the end of V-REP
	unloadVrepLibrary(vrepLib); // release the library
}

VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{ // This is called quite often. Just watch out for messages/events you want to handle

	// This function should not generate any error messages:
	int errorModeSaved;
	simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
	simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);

	void* retVal=NULL;

	if (message==sim_message_eventcallback_imagefilter_enumreset)
		nextFilterEnum=0; // Reset the enumeration procedure for these filters

	if (message==sim_message_eventcallback_imagefilter_enumerate)
	{ // Filter enumeration procedure. Here we inform V-REP of all filters that this plugin has
		if (nextFilterEnum<filterCount)
		{ // This plugin has more filters to 'register' to V-REP!
			replyData[0]=DEVELOPER_DATA_HEADER;
			replyData[1]=filterID[nextFilterEnum];
			char* retValTmp=simCreateBuffer(int(strlen(filterName[nextFilterEnum])+1));
			strcpy(retValTmp,filterName[nextFilterEnum]);
			retVal=(void*)retValTmp;
			nextFilterEnum++;
		}
	}

	if (message==sim_message_eventcallback_imagefilter_adjustparams)
	{ // V-REP calls this when the user wants to adjust one filter provided by a plugin
		if (auxiliaryData[0]==DEVELOPER_DATA_HEADER)
		{ // yes the filter is from this plugin!
			if (auxiliaryData[1]==0)
			{ // Filter with ID 0 (the only one that can be adjusted (filters with negative IDs cannot be adjusted))
				QWidget* pMainWindow = (QWidget*)simGetMainWindow(1);
				CSimpleFilterDialog filterDlg(pMainWindow);
				filterDlg.sizeValue=10;		// default value
				filterDlg.thresholdValue=0.5f; // default value
				unsigned char filterVersion=0;
				if ((customData!=NULL)&&(auxiliaryData[2]!=0))
					filterVersion=((unsigned char*)customData)[0];
				if (filterVersion==1)
				{ // this is the first version of the filter (different from this plugin's version!)
					unsigned char* filterParameters=((unsigned char*)customData)+1;
					filterDlg.sizeValue=((int*)filterParameters)[0];			// previously set value
					filterDlg.thresholdValue=((float*)filterParameters)[1];			// previously set value
				}

				// Prepare the filter parameters that will be returned (1+4+4 bytes):
				unsigned char* filterParametersToReturn=(unsigned char*)simCreateBuffer(1+sizeof(int)+sizeof(float)); // is automatically released by V-REP upon callback return
				filterParametersToReturn[0]=1; // current filter version
				((int*)(filterParametersToReturn+1))[0]=filterDlg.sizeValue; // size value
				((float*)(filterParametersToReturn+1))[1]=filterDlg.thresholdValue; // threshold value

				filterDlg.refresh();
				if (filterDlg.exec()!=QDialog::Rejected) // display the filter parameter dialog
				{ // update filter parameters only if we clicked "OK":
					((int*)(filterParametersToReturn+1))[0]=filterDlg.sizeValue;
					((float*)(filterParametersToReturn+1))[1]=filterDlg.thresholdValue;
				}
				retVal=(void*)filterParametersToReturn;
				replyData[0]=1+sizeof(int)+sizeof(float); // the size of the data that we return
			}
		}
	}

	if (message==sim_message_eventcallback_imagefilter_process)
	{ // V-REP calls this when a plugin filter should be applied
		if (auxiliaryData[0]==DEVELOPER_DATA_HEADER)
		{ // yes the filter is in this plugin (the filter is identified by a header ID (in auxiliaryData[0]) and a filter ID (in auxiliaryData[1]))
			int res[2]={auxiliaryData[2],auxiliaryData[3]}; // size of the images/buffers/depth maps
			void** ptrs=(void**)customData;
			float* inputImage=(float*)ptrs[0]; // original image from the vision sensor
			float* inputDepth=(float*)ptrs[1]; // original depth map from the vision sensor
			float* workImage=(float*)ptrs[2]; // work image
			float* buffer1=(float*)ptrs[3]; // buffer 1 image
			float* buffer2=(float*)ptrs[4]; // buffer 2 image
			float* outputImage=(float*)ptrs[5]; // output image
			unsigned char* params=(unsigned char*)ptrs[6];
			if (auxiliaryData[1]==0)
			{ // We have filter ID 0, we apply it:
				int sizeValue=10;			// default parameter
				float thresholdValue=0.5f;	// default parameter
				if (params!=NULL)
				{ // We use previously set values (those values are stored in the vision sensor and are also serialized!):
					// For this sensor we have 3 values: 1 byte for the filter version (not filter ID!), 1 int for the size value and one flot for the threshold value
					if (params[0]==1)
					{ // for now we have only one filter version
						sizeValue=((int*)(params+1))[0];  // size values
						thresholdValue=((float*)(params+1))[1];  // threshold value
					}
				}
				// We apply the filter:
				for (int i=0;i<res[0];i++)
				{
					for (int j=0;j<res[1];j++)
					{
						if (((i/sizeValue)&1)+((j/sizeValue)&1)==2)
						{
							workImage[3*(i+j*res[0])+0]=0.0f;
							workImage[3*(i+j*res[0])+1]=0.0f;
							workImage[3*(i+j*res[0])+2]=0.0f;
						}
					}
				}
				// We trigger the sensor:
				float average=0.0f;
				for (int i=0;i<res[0]*res[1];i++)
					average+=workImage[3*i+0]+workImage[3*i+1]+workImage[3*i+2];
				average/=res[0]*res[1]*3;
				if (average>thresholdValue)
					replyData[0]=1; // Triggers!
				else
					replyData[0]=0; // Doesn't trigger
			}
			if (auxiliaryData[1]==-1)
			{ // We have filter ID -1
				// We apply the filter (noise):
				for (int i=0;i<res[0]*res[1];i++)
				{
					workImage[3*i+0]*=0.8f+(rand()/(float)RAND_MAX)*0.4f;
					workImage[3*i+1]*=0.8f+(rand()/(float)RAND_MAX)*0.4f;
					workImage[3*i+2]*=0.8f+(rand()/(float)RAND_MAX)*0.4f;
					if (workImage[3*i+0]>1.0f)
						workImage[3*i+0]=1.0f;
					if (workImage[3*i+1]>1.0f)
						workImage[3*i+1]=1.0f;
					if (workImage[3*i+2]>1.0f)
						workImage[3*i+2]=1.0f;
				}
			}
			// We return auxiliary information that resulted from the image processing (that could be a vector, a direction, or other filter specific data)
			// That auxiliary information (as well as the trigger state) are retrieved with simHandleVisionSensor
			// For now we simply return two values: 42.0 and 99.0:
			replyData[1]=2; // the number of floats we return
			retVal=simCreateBuffer(sizeof(float)*2); // is automatically released by V-REP upon callback return
			((float*)retVal)[0]=42.0f;
			((float*)retVal)[1]=99.0f;
		}
	}


	// You can add more messages to handle here

	simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
	return(retVal);
}
