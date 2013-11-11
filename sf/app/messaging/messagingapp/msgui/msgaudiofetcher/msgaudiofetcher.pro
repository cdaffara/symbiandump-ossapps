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
TARGET = msgaudiofetcher
DEFINES += BUILD_MSGAUDIOFETCHER_DLL

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += ../inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += hb

TARGET.UID3 = 0x2001FE67
TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

# Platforms
SYMBIAN_PLATFORMS = WINSCW \
    ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/msgaudiofetcher.iby CORE_APP_LAYER_IBY_EXPORT_PATH(msgaudiofetcher.iby)"

# Input
HEADERS += msgaudioselectionengine.h \
           msgaudiofetchermodel.h \
           ../inc/msgaudiofetcherdialog.h \
           msgaudiopreview.h
           
SOURCES += msgaudioselectionengine.cpp \
           msgaudiofetchermodel.cpp \
           msgaudiofetcherdialog.cpp \
           msgaudiopreview.cpp

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/msgaudiofetcher.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/msgaudiofetcher.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

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
