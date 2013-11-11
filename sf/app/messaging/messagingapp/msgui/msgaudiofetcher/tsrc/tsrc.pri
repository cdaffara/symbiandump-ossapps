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
# Description:
#
#

TEMPLATE = app
TARGET.CAPABILITY  = All -TCB
DEFINES += AUDIO_FETCHER_UNIT_TEST
DEPENDPATH += .
DEPENDPATH += inc
DEPENDPATH += src

DEPENDPATH += ../../inc
DEPENDPATH += ../../src
DEPENDPATH += ../../../inc


# Platforms
SYMBIAN_PLATFORMS = WINSCW \
    ARMV5

# msgui inc
#INCLUDEPATH += ../../../inc

# msgui tsrc
INCLUDEPATH += ../../../tsrc/shared

# messaging inc 
#INCLUDEPATH += ../../../../../inc

#INCLUDEPATH += ../../../msguiutils/inc
#INCLUDEPATH += ../../../../msgutils/unieditorutils/editorgenutils/inc
#INCLUDEPATH += ../../../appengine/inc
#INCLUDEPATH += ../../../../smartmessaging/ringbc/inc
#INCLUDEPATH += ../../../../msgsettings/settingsview/inc
#INCLUDEPATH += ../../../../msgutils/unidatamodel/univcaldataplugin/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

QT += testlib
QT -= gui
CONFIG += hb
CONFIG += symbian_test


#HEADERS
HEADERS += msgaudioselectionengine.h \
           msgaudiofetchermodel.h \
           ../inc/msgaudiofetcherdialog.h \
           msgaudiopreview.h

#SOURCES           
SOURCES += msgaudioselectionengine.cpp \
           msgaudiofetchermodel.cpp \
           msgaudiofetcherdialog.cpp \
           msgaudiopreview.cpp


# Libs
LIBS += -lxqutils \
        -lmdeclient \
        -lcentralrepository \
        -lProfileEng \
        -lmediaclientaudio \
        -lDrmAudioPlayUtility \
        -lDRMCommon \
        -lDrmRights \
        -lDrmHelper \
        -ldrmutility \
        -lapmime \
        -lecom \
        -lxqservice \
        -lapgrfx


symbian:MMP_RULES += SMPSAFE