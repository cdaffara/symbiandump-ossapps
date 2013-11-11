#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: project file for radioenginewrapper's unit tests
#
TEMPLATE = app
TARGET = t_radioenginewrapper
symbian: { 
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.SID = 0x2002EAD8  # Tried  0x101FF976 but failed
    MMP_RULES += SMPSAFE    
}

CONFIG += qtestlib \
          Hb \
          symbian_test  

DEPENDPATH += . \
    inc \
    src
INCLUDEPATH += . \
    ../../../inc \
    ../../../common \
    ../inc \
    ../commoninc \
    ../../../../inc \
    /sf/app/radio/radioengine/utils/Stub/inc \
    /sf/app/radio/radioengine/utils/api \
    /sf/app/radio/radioengine/utils/inc \
    /sf/app/radio/radioengine/settings/api \
    /sf/mw/mmmw/inc \
    /sf/mw/mmmw/mmserv/inc \
    /sf/mw/mmmw/mmserv/radioutility/inc \
    /sf/mw/mmmw/mmserv/radioutility/radio_utility/src \
    /sf/mw/mmmw/mmserv/radioutility/radioserver/inc
CONFIG += qtestlib
symbian:LIBS += -lfmradioenginewrapper_stub
symbian:LIBS += -lfmradioengineutils_stub
symbian:LIBS += -lRadioSession_Stub
symbian:LIBS += -leuser
symbian:LIBS += -lcentralrepository
symbian:LIBS += -lflogger
HEADERS += inc/t_radioenginewrapper.h
HEADERS += inc/t_schedulerstartandstoptimer.h
SOURCES += src/t_radioenginewrapper.cpp
SOURCES += src/t_schedulerstartandstoptimer.cpp