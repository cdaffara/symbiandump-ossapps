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

symbian:TARGET.UID3 = 0x10207C68
    
TEMPLATE = lib
CONFIG += hb qt
TARGET = mpsettingsmanager
symbian: { 
    TARGET.UID3 = 0x10207C68
    MMP_RULES += "DEFFILE mpsettingsmanager.def" SMPSAFE
    defFilePath = .
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    BLD_INF_RULES.prj_exports += \
        "$${LITERAL_HASH}include <platform_paths.hrh>" \
        "conf/musicplayer.confml                   APP_LAYER_CONFML(musicplayer.confml)" \
        "conf/musicplayer_10207C92.crml        APP_LAYER_CRML(musicplayer_10207C92.crml)"
}
DEFINES += BUILD_MPSETTINGSMANAGER

INCLUDEPATH += . \
               inc \
               ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lxqsettingsmanager.dll

# Input
HEADERS += ../../inc/mpsettingsmanager.h 

SOURCES += src/mpsettingsmanager.cpp

