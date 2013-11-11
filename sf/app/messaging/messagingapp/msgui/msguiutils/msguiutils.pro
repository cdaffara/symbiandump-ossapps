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
TARGET = msguiutils
DEFINES += BUILD_MSGUI_UTILS_DLL

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../msgutils/unieditorutils/editorgenutils/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += hb

TARGET.UID3 = 0x2001FE69


TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

# Platforms
SYMBIAN_PLATFORMS = WINSCW \
    ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/msguiutils.iby CORE_APP_LAYER_IBY_EXPORT_PATH(msguiutils.iby)"


defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/msguiutils.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/msguiutils.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

# Input
HEADERS += msgsendutil.h \
           mmsconformancecheck.h \
           msgmediautil.h \
           msgcontactsutil.h \
           msgservicelaunchutil.h \
           msgservicelaunchutilprivate.h
           
SOURCES += msgsendutil.cpp \
           mmsconformancecheck.cpp \
           msgmediautil.cpp \
           msgcontactsutil.cpp \
           msgservicelaunchutil.cpp \
           msgservicelaunchutilprivate.cpp
           
    
# Libs
LIBS += -lxqservice \
    -lunieditorpluginloader \
    -leditorgenutils \
    -lconvergedmessageutils \
    -lcentralrepository \
    -lcommonengine \
    -lDRMHelper \
    -lmsgmedia \
    -lapmime \
    -lunidatamodelloader \
    -lxqutils \
    -lefsrv \
    -lxqserviceutil \
    -lmsgs \
    -lapgrfx
    



symbian:MMP_RULES += SMPSAFE
