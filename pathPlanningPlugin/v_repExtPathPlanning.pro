# This file is part of the PATH PLANNING PLUGIN for V-REP
# 
# Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
# marc@coppeliarobotics.com
# www.coppeliarobotics.com
# 
# The PATH PLANNING PLUGIN is licensed under the terms of EITHER:
#   1. PATH PLANNING PLUGIN commercial license (contact us for details)
#   2. PATH PLANNING PLUGIN educational license (see below)
# 
# PATH PLANNING PLUGIN educational license:
# -------------------------------------------------------------------
# The PATH PLANNING PLUGIN educational license applies only to EDUCATIONAL
# ENTITIES composed by following people and institutions:
# 
# 1. Hobbyists, students, teachers and professors
# 2. Schools and universities
# 
# EDUCATIONAL ENTITIES do NOT include companies, research institutions,
# non-profit organisations, foundations, etc.
# 
# An EDUCATIONAL ENTITY may use, modify, compile and distribute the
# modified/unmodified PATH PLANNING PLUGIN under following conditions:
#  
# 1. Distribution should be free of charge.
# 2. Distribution should be to EDUCATIONAL ENTITIES only.
# 3. Usage should be non-commercial.
# 4. Altered source versions must be plainly marked as such and distributed
#    along with any compiled code.
# 5. When using the PATH PLANNING PLUGIN in conjunction with V-REP, the "EDU"
#    watermark in the V-REP scene view should not be removed.
# 6. The origin of the PATH PLANNING PLUGIN must not be misrepresented. you must
#    not claim that you wrote the original software.
# 
# THE PATH PLANNING PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR
# IMPLIED WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
# AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
# DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
# MISUSING THIS SOFTWARE.
# -------------------------------------------------------------------
#
# This file was automatically created for V-REP release V3.1.2 on June 16th 2014

TARGET = v_repExtPathPlanning
TEMPLATE = lib

DEFINES -= UNICODE

CONFIG += shared

*-msvc* {
        QMAKE_CXXFLAGS += -O
	QMAKE_CXXFLAGS += -W3
}
*-g++* {
        QMAKE_CXXFLAGS += -O
	QMAKE_CXXFLAGS += -Wall
	QMAKE_CXXFLAGS += -Wno-unused-parameter
	QMAKE_CXXFLAGS += -Wno-strict-aliasing
	QMAKE_CXXFLAGS += -Wno-empty-body
	QMAKE_CXXFLAGS += -Wno-write-strings

	# Best would be to have a switch based on compiler version, but apparently that doesnt exist. So we use the Qt version..
	greaterThan(QT_MAJOR_VERSION,4): QMAKE_CXXFLAGS += -Wno-unused-but-set-variable
	greaterThan(QT_MAJOR_VERSION,4): QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
	greaterThan(QT_MAJOR_VERSION,4): QMAKE_CXXFLAGS += -Wno-narrowing

        QMAKE_CFLAGS += -O
	QMAKE_CFLAGS += -Wall
	QMAKE_CFLAGS += -Wno-strict-aliasing
	QMAKE_CFLAGS += -Wno-unused-parameter
	QMAKE_CFLAGS += -Wno-unused-but-set-variable
	QMAKE_CFLAGS += -Wno-unused-local-typedefs
}

win32 {
	DEFINES += WIN_VREP
}

macx {
	DEFINES += MAC_VREP
}

unix:!macx {
	DEFINES += LIN_VREP
}

DEFINES += COMPILING_EXTERNAL_PATHPLANNING_DLL
#DEFINES += QT_FRAMEWORK # when using this, make sure you use the exact same compiler and Qt version as the main V-REP library!


INCLUDEPATH += "sourceCode"
INCLUDEPATH += "sourceCode/pathPlanning"
INCLUDEPATH += "sourceCode/motionPlanning"
INCLUDEPATH += "sourceCode/VelocityObstacle"
INCLUDEPATH += "../v_rep/sourceCode/interfaces"
INCLUDEPATH += "../programming/v_repMath"
INCLUDEPATH += "../programming/include"
INCLUDEPATH += "../programming/common"
INCLUDEPATH += "/opt/local/include"
INCLUDEPATH += "/usr/local/include/eigen3"
INCLUDEPATH += "/Users/yhc/Documents/sglab_vrep/ompl-1.0.0-Source/src"

HEADERS += ../v_rep/sourceCode/interfaces/pathPlanningInterface.h \
	../v_rep/sourceCode/interfaces/dummyClasses.h \
        ../programming/include/v_repLib.h \
	../programming/v_repMath/3Vector.h \
	../programming/v_repMath/4Vector.h \
	../programming/v_repMath/7Vector.h \
	../programming/v_repMath/3X3Matrix.h \
	../programming/v_repMath/4X4Matrix.h \
	../programming/v_repMath/MyMath.h \
    sourceCode/pathPlanning/MeasurementRegion.h \
    sourceCode/pathPlanning/Obstacles.h \
    sourceCode/pathPlanning/Region.h \
    sourceCode/pathPlanning/StaticData.h \
    sourceCode/pathPlanning/Belief.h \
    sourceCode/pathPlanning/DynamicBbstacle.h \
    sourceCode/pathPlanning/Beacon.h \
    sourceCode/pathPlanning/Robot.h \
    sourceCode/VelocityObstacle/Agent.h \
    sourceCode/VelocityObstacle/Definitions.h \
    sourceCode/VelocityObstacle/KdTree.h \
    sourceCode/VelocityObstacle/Obstacle.h \
    sourceCode/VelocityObstacle/RVO.h \
    sourceCode/VelocityObstacle/RVOSimulator.h \
    sourceCode/VelocityObstacle/Vector2.h
	
HEADERS += sourceCode/pathPlanning/HolonomicPathNode.h \
	sourceCode/pathPlanning/HolonomicPathPlanning.h \
	sourceCode/pathPlanning/NonHolonomicPathNode.h \
	sourceCode/pathPlanning/NonHolonomicPathPlanning.h \
	sourceCode/pathPlanning/PathPlanning.h \
	sourceCode/pathPlanning/DubinPath.h \
        sourceCode/pathPlanning/ShootingPath.h \
        sourceCode/motionPlanning/mpObject.h \
	sourceCode/motionPlanning/mpPhase1Node.h \
	sourceCode/motionPlanning/mpPhase2Node.h \
	sourceCode/v_repExtPathPlanning.h \
	

SOURCES += ../v_rep/sourceCode/interfaces/pathPlanningInterface.cpp \
	../programming/common/v_repLib.cpp \
	../programming/v_repMath/3Vector.cpp \
	../programming/v_repMath/4Vector.cpp \
	../programming/v_repMath/7Vector.cpp \
	../programming/v_repMath/3X3Matrix.cpp \
	../programming/v_repMath/4X4Matrix.cpp \
	../programming/v_repMath/MyMath.cpp \
    sourceCode/pathPlanning/MeasurementRegion.cpp \
    sourceCode/pathPlanning/Obstacles.cpp \
    sourceCode/pathPlanning/Region.cpp \
    sourceCode/pathPlanning/StaticData.cpp \
    sourceCode/pathPlanning/Belief.cpp \
    sourceCode/pathPlanning/DynamicBbstacle.cpp \
    sourceCode/pathPlanning/Beacon.cpp \
    sourceCode/pathPlanning/Robot.cpp \
    sourceCode/VelocityObstacle/Agent.cpp \
    sourceCode/VelocityObstacle/KdTree.cpp \
    sourceCode/VelocityObstacle/Obstacle.cpp \
    sourceCode/VelocityObstacle/RVOSimulator.cpp

SOURCES += sourceCode/pathPlanning/HolonomicPathNode.cpp \
	sourceCode/pathPlanning/HolonomicPathPlanning.cpp \
	sourceCode/pathPlanning/NonHolonomicPathNode.cpp \
	sourceCode/pathPlanning/NonHolonomicPathPlanning.cpp \
	sourceCode/pathPlanning/PathPlanning.cpp \
        sourceCode/pathPlanning/DubinPath.cpp \
        sourceCode/pathPlanning/ShootingPath.cpp \
	sourceCode/motionPlanning/mpObject.cpp \
	sourceCode/motionPlanning/mpPhase1Node.cpp \
	sourceCode/motionPlanning/mpPhase2Node.cpp \
	sourceCode/v_repExtPathPlanning.cpp \
	
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
