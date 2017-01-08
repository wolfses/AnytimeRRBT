# This file is part of the REMOTE API
# 
# Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
# marc@coppeliarobotics.com
# www.coppeliarobotics.com
# 
# The REMOTE API is licensed under the terms of GNU GPL:
# 
# -------------------------------------------------------------------
# The REMOTE API is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# THE REMOTE API IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
# AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
# DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
# MISUSING THIS SOFTWARE.
# 
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with the REMOTE API.  If not, see <http://www.gnu.org/licenses/>.
# -------------------------------------------------------------------
#
# This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

QT -= core
QT -= gui

TARGET = remoteApiJava
TEMPLATE = lib

DEFINES -= UNICODE
DEFINES += QT_COMPIL
DEFINES += NON_MATLAB_PARSING
DEFINES += MAX_EXT_API_CONNECTIONS=255
DEFINES += _Included_extApiJava

CONFIG += shared
INCLUDEPATH += "../../include"
INCLUDEPATH += "../../remoteApi"

win32 {
	INCLUDEPATH += "C:/Program Files/Java/jdk1.7.0_40/include"
	INCLUDEPATH += "C:/Program Files/Java/jdk1.7.0_40/include/win32"
}

macx {
		INCLUDEPATH += "/System/Library/Frameworks/javaVM.framework/Headers"
}

unix:!macx {
		INCLUDEPATH += "/usr/lib/jvm/java-7-openjdk-i386/include"
		INCLUDEPATH += "/usr/lib/jvm/java-7-openjdk-amd64/include"
}

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
}

macx {
}

unix:!macx {
}

SOURCES += \
    extApiJavaQt.cpp \
    ../../remoteApi/extApiPlatform.c \

HEADERS += \
    extApiJava.h \
    ../../remoteApi/extApi.h \
    ../../remoteApi/extApiPlatform.h \
    ../../remoteApi/extApiInternal.h \

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

