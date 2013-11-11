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
# Description: Project file for Music Services.
#

TEMPLATE = lib
CONFIG += hb
TARGET = musicservices
symbian: { 
    TARGET.UID3 = 0x10207C69
    MMP_RULES += "DEFFILE musicservices.def" SMPSAFE
    defFilePath = .
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
}
DEFINES += BUILD_MUSICSERVICES_LIB

INCLUDEPATH += . \
               inc \
    	       ../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lxqservice \
        -lxqserviceutil

# Input
HEADERS += inc/musicservices.h \
           inc/getmusicservice.h \
           inc/playmusicservice.h 

SOURCES += src/musicservices.cpp \
           src/getmusicservice.cpp \
           src/playmusicservice.cpp

