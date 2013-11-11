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

TARGET = unidatamodelloader

TEMPLATE = lib

CONFIG += dll
DEFINES += UNIDATAMODELLOADER_DLL
DEFINES += BUILD_DLL_DATA_MODEL



DEPENDPATH += . \
    inc \
    src

QT -= gui

INCLUDEPATH += .
INCLUDEPATH += inc
INCLUDEPATH += ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Capability
TARGET.UID3 = 0x2001FE62
TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/unidatamodelloader.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(unidatamodelloader.iby)"

MMP_RULES += "SOURCEPATH ."

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/unidatamodelloader.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/unidatamodelloader.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

SOURCES += src/unidatamodelloader.cpp
    
LIBS += -lQtCore



symbian:MMP_RULES += SMPSAFE
