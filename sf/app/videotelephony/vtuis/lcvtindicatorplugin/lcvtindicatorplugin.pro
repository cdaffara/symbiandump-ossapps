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
CONFIG += plugin hb

# directories
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ./inc

# Input
HEADERS += inc/lcvtindicatorplugin.h
SOURCES += src/lcvtindicatorplugin.cpp

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x200315EB
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    LIBS += -lxqservice

    :BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
    :BLD_INF_RULES.prj_exports += "rom/lcvtindicatorplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(lcvtindicatorplugin.iby)"
    
    pluginstub.sources = lcvtindicatorplugin.dll
    pluginstub.path = /resource/plugins/indicators
    DEPLOYMENT += pluginstub
}
