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
#

TARGET = unieditorpluginloader

TEMPLATE = lib

CONFIG += dll
DEFINES += UNIEDITORPLUGINLOADER_DLL
DEFINES += BUILD_DLL_EDITOR_PLUGIN


DEPENDPATH += . \
    inc \
    src

QT -= gui

INCLUDEPATH += .
INCLUDEPATH += inc
INCLUDEPATH += ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Capability
TARGET.UID3 = 0x2001FE71
TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/unieditorpluginloader.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(unieditorpluginloader.iby)"

SOURCES += src/unieditorpluginloader.cpp

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/unieditorpluginloader.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/unieditorpluginloader.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock


symbian:MMP_RULES += SMPSAFE
