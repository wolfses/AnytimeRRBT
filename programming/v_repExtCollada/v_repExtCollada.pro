# This file is part of the COLLADA PLUGIN for V-REP
# 
# Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
# marc@coppeliarobotics.com
# www.coppeliarobotics.com
# 
# The COLLADA PLUGIN is licensed under the terms of GNU GPL:
# 
# -------------------------------------------------------------------
# The COLLADA PLUGIN is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# THE COLLADA PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
# AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
# DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
# MISUSING THIS SOFTWARE.
#  
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with the COLLADA PLUGIN.  If not, see <http://www.gnu.org/licenses/>.
# -------------------------------------------------------------------
#
# This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

TARGET = v_repExtCollada
TEMPLATE = lib

DEFINES -= UNICODE
CONFIG += shared
greaterThan(QT_MAJOR_VERSION,4): QT += widgets

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

INCLUDEPATH += "../include"
INCLUDEPATH += "../v_repMath"
INCLUDEPATH += "tinyxml2"

win32 {
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

SOURCES += \
    COLLADAExporter.cpp \
    COLLADAImporter.cpp \
    mat4.cpp \
    Matrix.cpp \
    Mesh.cpp \
    Polygon.cpp \
    PolygonGroup.cpp \
    Rotate.cpp \
    Scale.cpp \
    SceneNode.cpp \
    Source.cpp \
    StringHelper.cpp \
    Translate.cpp \
    TriangleGroup.cpp \
    VisualScene.cpp \
    XMLHelper.cpp \
    colladadialog.cpp \
    tinyxml2/tinyxml2.cpp \
	v_repExtCollada.cpp \
    ../v_repMath/3Vector.cpp \
    ../v_repMath/3X3Matrix.cpp \
    ../v_repMath/4Vector.cpp \
    ../v_repMath/4X4FullMatrix.cpp \
    ../v_repMath/4X4Matrix.cpp \
    ../v_repMath/7Vector.cpp \
    ../v_repMath/MyMath.cpp \
    ../common/v_repLib.cpp \

HEADERS +=\
    COLLADAExporter.h \
    COLLADAImporter.h \
    Hash.h \
    mat4.h \
    Material.h \
    Matrix.h \
    Mesh.h \
    Polygon.h \
    PolygonGroup.h \
    Rotate.h \
    Scale.h \
    SceneNode.h \
    Source.h \
    StringHelper.h \
    Transformation.h \
    Translate.h \
    TriangleGroup.h \
    vec3.h \
    VisualScene.h \
    XMLHelper.h \
    colladadialog.h \
    tinyxml2/tinyxml2.h \
	v_repExtCollada.h \
    ../v_repMath/3Vector.h \
    ../v_repMath/3X3Matrix.h \
    ../v_repMath/4Vector.h \
    ../v_repMath/4X4FullMatrix.h \
    ../v_repMath/4X4Matrix.h \
    ../v_repMath/7Vector.h \
    ../v_repMath/MyMath.h \
    ../include/v_repLib.h \

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

FORMS += \
    colladadialog.ui


















