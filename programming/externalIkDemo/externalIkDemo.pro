# Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
# marc@coppeliarobotics.com
# www.coppeliarobotics.com
# 
# -------------------------------------------------------------------
# THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
# AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
# DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
# MISUSING THIS SOFTWARE.
# 
# You are free to use/modify/distribute this file for whatever purpose!
# -------------------------------------------------------------------
#
# This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

QT -= core
QT -= gui

TARGET = externalIkDemo
TEMPLATE = app

DEFINES -= UNICODE
CONFIG   += console
CONFIG   -= app_bundle

DEFINES += NON_MATLAB_PARSING
DEFINES += MAX_EXT_API_CONNECTIONS=255

*-msvc* {
	QMAKE_CXXFLAGS += -O2
	QMAKE_CXXFLAGS += -W3
}
*-g++* {
	QMAKE_CXXFLAGS += -O3
	QMAKE_CXXFLAGS += -Wall
	QMAKE_CXXFLAGS += -Wno-unused-parameter
	QMAKE_CXXFLAGS += -Wno-strict-aliasing
	QMAKE_CXXFLAGS += -Wno-empty-body
	QMAKE_CXXFLAGS += -Wno-write-strings

	QMAKE_CXXFLAGS += -Wno-unused-but-set-variable
	QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
	QMAKE_CXXFLAGS += -Wno-narrowing

	QMAKE_CFLAGS += -O3
	QMAKE_CFLAGS += -Wall
	QMAKE_CFLAGS += -Wno-strict-aliasing
	QMAKE_CFLAGS += -Wno-unused-parameter
	QMAKE_CFLAGS += -Wno-unused-but-set-variable
	QMAKE_CFLAGS += -Wno-unused-local-typedefs
}

win32 {
	LIBS += -lwinmm
	LIBS += -lWs2_32
	DEFINES += WIN_VREP
	INCLUDEPATH += "c:/boost_1_54_0"
}

macx {
    DEFINES += MAC_VREP
    INCLUDEPATH += "/usr/local/include"
}

unix:!macx {
    DEFINES += LIN_VREP
    INCLUDEPATH += "../../boost_1_49_0" #probably not needed when installing boost with sudo apt-get install libboost-all-dev
}

INCLUDEPATH += "../include"
INCLUDEPATH += "../remoteApi"
INCLUDEPATH += "../externalIk"

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

HEADERS += \
    ../remoteApi/extApiPlatform.h \
    ../remoteApi/extApiInternal.h \
    ../remoteApi/extApi.h \
    ../externalIk/VPoint.h \
    ../externalIk/Vector.h \
    ../externalIk/Tt.h \
    ../externalIk/Registerediks.h \
    ../externalIk/ObjCont.h \
    ../externalIk/MyMath.h \
    ../externalIk/MMatrix.h \
    ../externalIk/mathDefines.h \
    ../externalIk/MainCont.h \
    ../externalIk/jointObject.h \
    ../externalIk/ikRoutine.h \
    ../externalIk/ikGroup.h \
    ../externalIk/ikEl.h \
    ../externalIk/Global.h \
    ../externalIk/extIkSer.h \
    ../externalIk/extIk.h \
    ../externalIk/Dummy.h \
    ../externalIk/Ct.h \
    ../externalIk/7Vector.h \
    ../externalIk/6X6Matrix.h \
    ../externalIk/6Vector.h \
    ../externalIk/4X4Matrix.h \
    ../externalIk/4X4FullMatrix.h \
    ../externalIk/4Vector.h \
    ../externalIk/3X3Matrix.h \
    ../externalIk/3Vector.h \
    ../externalIk/3DObject.h

SOURCES += \
    ../remoteApi/extApiPlatform.c \
    ../remoteApi/extApi.c \
    ../externalIk/Vector.cpp \
    ../externalIk/Tt.cpp \
    ../externalIk/Registerediks.cpp \
    ../externalIk/ObjCont.cpp \
    ../externalIk/MyMath.cpp \
    ../externalIk/MMatrix.cpp \
    ../externalIk/MainCont.cpp \
    ../externalIk/jointObject.cpp \
    ../externalIk/ikRoutine.cpp \
    ../externalIk/ikGroup.cpp \
    ../externalIk/ikEl.cpp \
    ../externalIk/extIkSer.cpp \
    ../externalIk/extIk.cpp \
    ../externalIk/Dummy.cpp \
    ../externalIk/Ct.cpp \
    ../externalIk/7Vector.cpp \
    ../externalIk/6X6Matrix.cpp \
    ../externalIk/6Vector.cpp \
    ../externalIk/4X4Matrix.cpp \
    ../externalIk/4X4FullMatrix.cpp \
    ../externalIk/4Vector.cpp \
    ../externalIk/3X3Matrix.cpp \
    ../externalIk/3Vector.cpp \
    ../externalIk/3DObject.cpp \
    externalIkDemo.cpp


















