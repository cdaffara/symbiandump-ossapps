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

TEMPLATE = lib
TARGET = nmindicatorplugin
CONFIG += plugin
CONFIG += hb

MOC_DIR = moc

DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../inc

HEADERS += inc/nmindicatorplugin.h \
           inc/nmindicator.h \
           inc/nmsendingindicator.h \
           inc/nmsyncindicator.h \
           inc/nmunreadindicator.h
SOURCES += src/nmindicatorplugin.cpp \
           src/nmindicator.cpp \
           src/nmsendingindicator.cpp \
           src/nmsyncindicator.cpp \
           src/nmunreadindicator.cpp

symbian {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002E69F

    pluginstub.sources = nmindicatorplugin.dll
    pluginstub.path = /resource/plugins/indicators
    DEPLOYMENT += pluginstub

    LIBS += -leuser
    LIBS += -llibc
    LIBS += -lxqservice
    
    MMP_RULES += SMPSAFE
}

BLD_INF_RULES.prj_exports += \
  "$${LITERAL_HASH}include <platform_paths.hrh>" \
  "rom/nmindicatorplugin.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(nmindicatorplugin.iby)"

