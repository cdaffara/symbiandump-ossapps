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
TARGET = cpsilenceindicatorplugin
CONFIG += plugin
CONFIG += hb

# directories
DEPENDPATH += .

HEADERS += src/cpsilenceindicatorplugin.h
SOURCES += src/cpsilenceindicatorplugin.cpp
symbian*::LIBS += -HbCore \
    -HbWidgets
symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ProtServ SwEvent TrustedUI ReadDeviceData

    TARGET.UID3 = 0x2002C39F
    SYMBIAN_PLATFORMS = WINSCW \
        ARMV5
    LIBS += -lprofileeng
                         
    BLD_INF_RULES.prj_exports += "rom/cpsilenceindicatorplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(cpsilenceindicatorplugin.iby)"
    pluginstub.sources = cpsilenceindicatorplugin.dll
    pluginstub.path = /resource/plugins/indicators
    DEPLOYMENT += pluginstub
}

