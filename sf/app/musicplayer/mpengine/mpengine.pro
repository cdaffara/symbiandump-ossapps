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
# Description: Project file for Music Player Engine.
#

TEMPLATE = lib
CONFIG += hb
TARGET = mpengine
symbian: { 
    TARGET.UID3 = 0x10207C93
    MMP_RULES += "DEFFILE mpengine.def" SMPSAFE
    defFilePath = .
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    BLD_INF_RULES.prj_exports += \
        "resources/nullsound.mp3 z:/system/data/nullsound.mp3"
}
DEFINES += BUILD_MPENGINE_LIB

INCLUDEPATH += . \
               inc \
               ../inc \
               ../mpserviceplugins/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor \
        -lmpxplaybackutility \
        -lmpxcollectionutility \
        -lmpxharvesterutility \
        -lmpxcommon \
        -lmpxcollectionhelper \
        -lmpsettingsmanager \
        -lmpdata \
        -lxqserviceutil \
        -laudioequalizerutility \
        -lmediaclientaudio \
        -lefsrv \
        -lmpapplicationmonitor

# Input
HEADERS += ../inc/mpenginefactory.h \
           ../inc/mpengine.h \
           ../inc/mpsongscanner.h \
           inc/mpmpxharvesterframeworkwrapper.h \
           inc/mpmpxharvesterframeworkwrapper_p.h \
           inc/mpmpxplaybackframeworkwrapper.h \
           inc/mpmpxembeddedplaybackhelper.h \
           inc/mpmpxplaybackframeworkwrapper_p.h \
           inc/mpmpxcollectionframeworkwrapper.h \
           inc/mpmpxisolatedcollectionhelper.h \
           inc/mpmpxcollectionframeworkwrapper_p.h \
           inc/mpaudioeffectsframeworkwrapper.h \
           inc/mpaudioeffectsframeworkwrapper_p.h \
           inc/mpequalizerframeworkwrapper.h \
           inc/mpequalizerframeworkwrapper_p.h

SOURCES += src/mpenginefactory.cpp \
           src/mpengine.cpp \
           src/mpsongscanner.cpp \
           src/mpmpxharvesterframeworkwrapper.cpp \
           src/mpmpxharvesterframeworkwrapper_p.cpp \
           src/mpmpxplaybackframeworkwrapper.cpp \
           src/mpmpxembeddedplaybackhelper.cpp \
           src/mpmpxplaybackframeworkwrapper_p.cpp \
           src/mpmpxcollectionframeworkwrapper.cpp \
           src/mpmpxisolatedcollectionhelper.cpp \
           src/mpmpxcollectionframeworkwrapper_p.cpp \
           src/mpaudioeffectsframeworkwrapper.cpp \
           src/mpaudioeffectsframeworkwrapper_p.cpp \
           src/mpequalizerframeworkwrapper.cpp \
           src/mpequalizerframeworkwrapper_p.cpp

