#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

TEMPLATE = lib
TARGET = cpprofilewrapper

CONFIG += debug_and_release 

win32|mac {
    !contains(QT_CONFIG,debug)|!contains(QT_CONFIG,release) {
        CONFIG -= debug_and_release debug release
        contains(QT_CONFIG,debug):  CONFIG+=debug
        contains(QT_CONFIG,release):CONFIG+=release
    }
}

CONFIG(debug, debug|release) {
    SUBDIRPART = debug
} else {
    SUBDIRPART = release
}

win32 { 
    DESTDIR = C:/ControlPanel/$$SUBDIRPART/bin
    OBJECTS_DIR = $$PWD/../$$SUBDIRPART/tmp/$$TARGET
}

# Add the output dirs to the link path too
LIBS += -L$$DESTDIR

MOC_DIR = moc
OBJECT_DIR = obj
RCC_DIR = rcc

DEFINES += PROFILEWRAPPER_FREEZE


HEADERS += ../inc/cpprofilemodel.h \
           ../inc/cpprofilewrappermacro.h \
           ../inc/cpprofilemonitor.h \
           src/cpprofilemodel_p.h \
           src/cpprofilemonitor_p.h 
SOURCES += src/cpprofilemodel.cpp \
		   src/cpprofilemonitor.cpp
CONFIG += hb
win32 {
    INCLUDEPATH += $$PWD/src
    INCLUDEPATH += ../inc
    SOURCES += src/cpprofilemodel_win_p.cpp
}

symbian {
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE \
    		   $$APP_LAYER_SYSTEMINCLUDE \
	           $$MOC_DIR
    INCLUDEPATH += $$MW_LAYER_PLATFORM_EXPORT_PATH(cplogger)

    SOURCES += src/cpprofilemodel_p.cpp \
			   src/cpprofilemonitor_p.cpp

    LIBS += -lprofileeng  \ 
            -lProfileEngine \
            -lcentralrepository \
            -lcharconv \
            -lcplogger \  # For cplogger
            -lxqutils
    TARGET.CAPABILITY = All -TCB 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20025FE6
}




symbian:MMP_RULES += SMPSAFE
