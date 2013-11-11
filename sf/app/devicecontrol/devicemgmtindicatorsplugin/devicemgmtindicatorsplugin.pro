# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:
TEMPLATE = lib
TARGET = dmindicatorsplugin
CONFIG += plugin
CONFIG += hb
INCLUDEPATH += .
DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
HEADERS += inc/dmpronoteindicators.h \
    inc/dmindicatorsplugin.h \
    inc/dmindicator.h
    
SOURCES += src/dmpronoteindicators.cpp \
    src/dmindicatorsplugin.cpp \
    src/dmindicator.cpp \
    src/tarmtrustindicator.cpp


symbian {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x20030009

    pluginstub.sources = dmindicatorsplugin.dll
    pluginstub.path = /resource/plugins/indicators
    DEPLOYMENT += pluginstub
    }
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/dmindicatorsplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(dmindicatorsplugin.iby)" \
    "resources/message1.svg /epoc32/winscw/c/resource/devicemanagement/message1.svg" \
    "resources/qtg_small_smiley_kissing.svg /epoc32/winscw/c/resource/devicemanagement/qtg_small_smiley_kissing.svg"
LIBS += -lxqservice \
 		 -lcsserverclientapi \
 		 -lapgrfx \
 		 -lcone  
