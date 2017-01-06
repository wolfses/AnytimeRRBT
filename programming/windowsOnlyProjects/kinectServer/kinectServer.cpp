/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/

//############################################################################
//############################################################################
// THE ORIGINAL SOURCE CODE OF THIS FILE WAS TAKEN FROM THE "NiUserTracker"
// PROJECT AND MODIFIED BY ERIC ROHMER ON 24/6/2011.
// MODIFIED, ADDED OR REMOVED PARTS ARE CLEARLY MARKED AS SUCH.
// THE SOURCE CODE AND THE EXECUTABLE OF THE "kinectServer" PROJECT
// IS DISTRIBUTED FREE OF CHARGE
//############################################################################
//############################################################################



//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include "SceneDrawer.h"

// ADDED FOLLOWING 4 LINES:
#include "inConnection.h"
#include <time.h>
#pragma comment (lib, "openNI.lib") 
#pragma comment(lib,"winmm.lib")


//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
xn::Context g_Context;
xn::DepthGenerator g_DepthGenerator;
xn::UserGenerator g_UserGenerator;

XnBool g_bNeedPose = FALSE;
XnChar g_strPose[20] = "";
XnBool g_bDrawBackground = TRUE;
XnBool g_bDrawPixels = TRUE;
XnBool g_bDrawSkeleton = TRUE;
XnBool g_bPrintID = TRUE;
XnBool g_bPrintState = TRUE;

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif

#define GL_WIN_SIZE_X 720
#define GL_WIN_SIZE_Y 480

XnBool g_bPause = false;
XnBool g_bRecord = false;

XnBool g_bQuit = false;

// ADDED FOLLOWING 8 LINES:
bool _commThreadLaunched=false;
bool _commThreadEndRequest=false;
std::string _connectionPortOrPipe;
float _skeletonData[16*4]; // hand left (x,y,z,confidence), hand right (x,y,z,confidence), elbow ..., shoulder..., hip..., knee..., ankle..., foot..., torso (x,y,z,confidence), head (x,y,z,confidence)
CRITICAL_SECTION _dataCriticalSection;
clock_t Begin, End;
int elapTicks;
#define SENDFREQUENCY 50

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

// ADDED FOLLOWING FUNCTION:
DWORD WINAPI _commThread(LPVOID lpParam)
{
	_commThreadLaunched=true;
	printf("Communication thread started.\n");
	int portNb=20000;
	bool socketConnectionDesired=true;
	if (_connectionPortOrPipe.length()!=0)
	{
		portNb=atoi(_connectionPortOrPipe.c_str());
		socketConnectionDesired=(portNb!=0);
	}
	CInConnection* connection;
	if (socketConnectionDesired)
	{ // using sockets
		printf("Trying socket communication on port %i\n",portNb);
		connection=new CInConnection(portNb);
	}
	else
	{ // using pipes
		printf("Trying pipe communication on pipe %s\n",_connectionPortOrPipe.c_str());
		connection=new CInConnection(_connectionPortOrPipe.c_str());
	}

	printf("Connecting to client...\n");
	if (connection->connectToClient())
	{
		printf("Connected to client.\n");
		while (!_commThreadEndRequest)
		{
			int dataSize;
			char* data=connection->receiveData(dataSize);
			if (dataSize>0)
			{ // We received data. The server ALWAYS replies!
				// Use the data here
				delete[] data; // destroy the buffer
				// Reply now with the correct data:
				EnterCriticalSection(&_dataCriticalSection);
				bool result=connection->replyToReceivedData((char*)_skeletonData,16*4*sizeof(float));
				LeaveCriticalSection(&_dataCriticalSection);
				if (!result)
				{
					printf("Failed to send reply.\n");
					break;
				}
			}
			if (dataSize==-1)
				break; // error
		}
	}
	else
		printf("Failed to connect to client.\n");
	_commThreadLaunched=false;
	delete connection;
	_commThreadEndRequest=false; // indicate that we left the thread if we made the request from outside
	printf("Communication thread ended.\n");
	return(0);
}

// ADDED FOLLOWING FUNCTION:
void prepareSkeletalData(XnUserID player) 
{

	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return;
	}
	XnSkeletonJointPosition leftHand,rightHand,leftElbow,rightElbow,leftShoulder,rightShoulder,torso,leftHip,rightHip,leftKnee,rightKnee,leftAnkle,rightAnkle,leftFoot,rightFoot,head;
	
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_LEFT_HAND,leftHand);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_RIGHT_HAND,rightHand);
  
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_LEFT_ELBOW,leftElbow);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_RIGHT_ELBOW,rightElbow);
  
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_LEFT_SHOULDER,leftShoulder);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_RIGHT_SHOULDER,rightShoulder);
  
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_LEFT_HIP,leftHip);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_RIGHT_HIP,rightHip);

	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_LEFT_KNEE,leftKnee);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_RIGHT_KNEE,rightKnee);

	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_LEFT_ANKLE,leftAnkle);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_RIGHT_ANKLE,rightAnkle);
  
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_LEFT_FOOT,leftFoot);
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_RIGHT_FOOT,rightFoot);

	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_TORSO,torso);

	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,XN_SKEL_HEAD,head);

	EnterCriticalSection(&_dataCriticalSection);

	_skeletonData[0]=leftHand.position.X;
	_skeletonData[1]=leftHand.position.Y;
	_skeletonData[2]=leftHand.position.Z;
	_skeletonData[3]=leftHand.fConfidence;
	_skeletonData[4]=rightHand.position.X;
	_skeletonData[5]=rightHand.position.Y;
	_skeletonData[6]=rightHand.position.Z;
	_skeletonData[7]=rightHand.fConfidence;

	_skeletonData[8]=leftElbow.position.X;
	_skeletonData[9]=leftElbow.position.Y;
	_skeletonData[10]=leftElbow.position.Z;
	_skeletonData[11]=leftElbow.fConfidence;
	_skeletonData[12]=rightElbow.position.X;
	_skeletonData[13]=rightElbow.position.Y;
	_skeletonData[14]=rightElbow.position.Z;
	_skeletonData[15]=rightElbow.fConfidence;

	_skeletonData[16]=leftShoulder.position.X;
	_skeletonData[17]=leftShoulder.position.Y;
	_skeletonData[18]=leftShoulder.position.Z;
	_skeletonData[19]=leftShoulder.fConfidence;
	_skeletonData[20]=rightShoulder.position.X;
	_skeletonData[21]=rightShoulder.position.Y;
	_skeletonData[22]=rightShoulder.position.Z;
	_skeletonData[23]=rightShoulder.fConfidence;

	_skeletonData[24]=leftHip.position.X;
	_skeletonData[25]=leftHip.position.Y;
	_skeletonData[26]=leftHip.position.Z;
	_skeletonData[27]=leftHip.fConfidence;
	_skeletonData[28]=rightHip.position.X;
	_skeletonData[29]=rightHip.position.Y;
	_skeletonData[30]=rightHip.position.Z;
	_skeletonData[31]=rightHip.fConfidence;

	_skeletonData[32]=leftKnee.position.X;
	_skeletonData[33]=leftKnee.position.Y;
	_skeletonData[34]=leftKnee.position.Z;
	_skeletonData[35]=leftKnee.fConfidence;
	_skeletonData[36]=rightKnee.position.X;
	_skeletonData[37]=rightKnee.position.Y;
	_skeletonData[38]=rightKnee.position.Z;
	_skeletonData[39]=rightKnee.fConfidence;

	_skeletonData[40]=leftAnkle.position.X;
	_skeletonData[41]=leftAnkle.position.Y;
	_skeletonData[42]=leftAnkle.position.Z;
	_skeletonData[43]=leftAnkle.fConfidence;
	_skeletonData[44]=rightAnkle.position.X;
	_skeletonData[45]=rightAnkle.position.Y;
	_skeletonData[46]=rightAnkle.position.Z;
	_skeletonData[47]=rightAnkle.fConfidence;

	_skeletonData[48]=leftFoot.position.X;
	_skeletonData[49]=leftFoot.position.Y;
	_skeletonData[50]=leftFoot.position.Z;
	_skeletonData[51]=leftFoot.fConfidence;
	_skeletonData[52]=rightFoot.position.X;
	_skeletonData[53]=rightFoot.position.Y;
	_skeletonData[54]=rightFoot.position.Z;
	_skeletonData[55]=rightFoot.fConfidence;

	_skeletonData[56]=torso.position.X;
	_skeletonData[57]=torso.position.Y;
	_skeletonData[58]=torso.position.Z;
	_skeletonData[59]=torso.fConfidence;

	_skeletonData[60]=head.position.X;
	_skeletonData[61]=head.position.Y;
	_skeletonData[62]=head.position.Z;
	_skeletonData[63]=head.fConfidence;

	LeaveCriticalSection(&_dataCriticalSection);
}

void CleanupExit()
{
	g_Context.Shutdown();

	exit (1);
}

// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	printf("New User %d\n", nId);
	// New user found
	if (g_bNeedPose)
	{
		g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
	}
	else
	{
		g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	}
}
// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	printf("Lost user %d\n", nId);
}
// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	printf("Pose %s detected for user %d\n", strPose, nId);
	g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}
// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
	printf("Calibration started for user %d\n", nId);
}
// Callback: Finished calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
	if (bSuccess)
	{
		// Calibration succeeded
		printf("Calibration complete, start tracking user %d\n", nId);
		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		printf("Calibration failed for user %d\n", nId);
		if (g_bNeedPose)
		{
			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		}
		else
		{
			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

// ADDED FOLLOWING FUNCTION:
XnUserID FirstTrackingUser(xn::UserGenerator userGenerator)
{
	XnUserID aUsers[10];
	XnUInt16 nUsers=10;
	userGenerator.GetUsers(aUsers,nUsers);
	for (int i=0;i<nUsers;++i)
	{
		if (userGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
			return aUsers[i];
	}
	return NULL;
}

// this function is called each frame
void glutDisplay (void)
{

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup the OpenGL viewpoint
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	xn::SceneMetaData sceneMD;
	xn::DepthMetaData depthMD;
	g_DepthGenerator.GetMetaData(depthMD);
	glOrtho(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);

	glDisable(GL_TEXTURE_2D);

	if (!g_bPause)
	{
		// Read next available data
		g_Context.WaitAndUpdateAll();
	}

		// Process the data
		g_DepthGenerator.GetMetaData(depthMD);
		g_UserGenerator.GetUserPixels(0, sceneMD);
		DrawDepthMap(depthMD, sceneMD);

	glutSwapBuffers();

	// ADDED FOLLOWING 9 LINES:
	XnUserID trackingUser=FirstTrackingUser(g_UserGenerator);
	End=clock()*CLK_TCK;
	elapTicks=End-Begin;
	int dt=elapTicks/1000;
	if (dt>SENDFREQUENCY)
	{
		prepareSkeletalData(trackingUser);
		Begin=clock()*CLK_TCK;
	}
}

void glutIdle (void)
{
	// MODIFIED FOLLOWING LINES (ADDED SECOND PART)
	if (g_bQuit||(!_commThreadLaunched)) {
		CleanupExit();
	}

	// Display the frame
	glutPostRedisplay();
}

void glutKeyboard (unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		CleanupExit();
	case 'b':
		// Draw background?
		g_bDrawBackground = !g_bDrawBackground;
		break;
	case 'x':
		// Draw pixels at all?
		g_bDrawPixels = !g_bDrawPixels;
		break;
	case 's':
		// Draw Skeleton?
		g_bDrawSkeleton = !g_bDrawSkeleton;
		break;
	case 'i':
		// Print label?
		g_bPrintID = !g_bPrintID;
		break;
	case 'l':
		// Print ID & state as label, or only ID?
		g_bPrintState = !g_bPrintState;
		break;
	case'p':
		g_bPause = !g_bPause;
		break;
	}
}
void glInit (int * pargc, char ** argv)
{
	glutInit(pargc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow ("Prime Sense User Tracker Viewer");
	//glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

// MODIFIED FOLLOWING LINE:
#define SAMPLE_XML_PATH "SamplesConfig.xml" // "../../../Data/SamplesConfig.xml"

#define CHECK_RC(nRetVal, what)										\
	if (nRetVal != XN_STATUS_OK)									\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));\
		return nRetVal;												\
	}

int main(int argc, char **argv)
{
	// ADDED FOLLOWING 6 LINES:
	InitializeCriticalSection(&_dataCriticalSection);
	if (argc>1)
		_connectionPortOrPipe=argv[1];
	CreateThread(NULL,0,_commThread,NULL,THREAD_PRIORITY_NORMAL,NULL);
	while (!_commThreadLaunched)
		Sleep(2);


	XnStatus nRetVal = XN_STATUS_OK;

	// MODIFIED FOLLOWING SECTION (COMMENTED OUT CODE):
	//if (argc > 1)
	//{
	//	nRetVal = g_Context.Init();
	//	CHECK_RC(nRetVal, "Init");
	//	nRetVal = g_Context.OpenFileRecording(argv[1]);
	//	if (nRetVal != XN_STATUS_OK)
	//	{
	//		printf("Can't open recording %s: %s\n", argv[1], xnGetStatusString(nRetVal));
	//		return 1;
	//	}
	//}
	//else
	//{
		xn::EnumerationErrors errors;
		nRetVal = g_Context.InitFromXmlFile(SAMPLE_XML_PATH, &errors);
		if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
		{
			XnChar strError[1024];
			errors.ToString(strError, 1024);
			printf("%s\n", strError);
			return (nRetVal);
		}
		else if (nRetVal != XN_STATUS_OK)
		{
			printf("Open failed: %s\n", xnGetStatusString(nRetVal));
			return (nRetVal);
		}
	//}

	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	CHECK_RC(nRetVal, "Find depth generator");
	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
	if (nRetVal != XN_STATUS_OK)
	{
		nRetVal = g_UserGenerator.Create(g_Context);
		CHECK_RC(nRetVal, "Find user generator");
	}

	XnCallbackHandle hUserCallbacks, hCalibrationCallbacks, hPoseCallbacks;
	if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
	{
		printf("Supplied user generator doesn't support skeleton\n");
		return 1;
	}
	g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
	g_UserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(UserCalibration_CalibrationStart, UserCalibration_CalibrationEnd, NULL, hCalibrationCallbacks);

	if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
	{
		g_bNeedPose = TRUE;
		if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
		{
			printf("Pose required, but not supported\n");
			return 1;
		}
		g_UserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(UserPose_PoseDetected, NULL, NULL, hPoseCallbacks);
		g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
	}

	g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

	nRetVal = g_Context.StartGeneratingAll();
	CHECK_RC(nRetVal, "StartGenerating");

	// ADDED FOLLOWING LINE:
	Begin=clock()*CLK_TCK;

	glInit(&argc, argv);
	glutMainLoop();

}
