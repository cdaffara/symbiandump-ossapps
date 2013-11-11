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
# Description: Project file for application monitor utility.
#

TEMPLATE = lib
CONFIG += hb qt
TARGET = mpapplicationmonitor
symbian: { 
    TARGET.UID3 = 0x10207C98
    MMP_RULES += "DEFFILE mpapplicationmonitor.def"
    defFilePath = .
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
}
DEFINES += BUILD_MPAPPLICATIONMONITOR

INCLUDEPATH += . \
      inc \
      ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
     
LIBS += -lws32 \
        -lapgrfx \
        -lavkon

# Input
HEADERS += ../../inc/mpapplicationmonitor.h \
           inc/mpapplicationmonitor_p.h

SOURCES += src/mpapplicationmonitor.cpp \
           src/mpapplicationmonitor_p.cpp

