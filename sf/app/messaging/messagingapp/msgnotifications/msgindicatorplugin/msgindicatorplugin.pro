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
TARGET = msgindicatorplugin
CONFIG += plugin
CONFIG += hb

INCLUDEPATH += .
INCLUDEPATH += ../inc
DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../../inc



HEADERS += inc/msgindicatorplugin.h \
		   inc/msgindicator.h \
		   inc/msgindicator_p.h 
		   
SOURCES += src/msgindicatorplugin.cpp \
		   src/msgindicator.cpp \
		   src/msgindicator_p.cpp

TRANSLATIONS = messaging.ts		   

symbian {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2001FE6C

    pluginstub.sources = msgindicatorplugin.dll
    pluginstub.path = /resource/plugins/indicators
    DEPLOYMENT += pluginstub
}

BLD_INF_RULES.prj_exports += \
  "$${LITERAL_HASH}include <platform_paths.hrh>" \
  "rom/msgindicatorplugin.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(msgindicatorplugin.iby)" \
  "resources/message.svg             			/epoc32/winscw/c/resource/messaging/message.svg"
  
 LIBS += -lxqservice \
 		 -lcsserverclientapi \
         -lcsutils \
         -lxqutils


symbian:MMP_RULES += SMPSAFE
