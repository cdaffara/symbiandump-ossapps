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
# Description: Project for messaging history.
#
TEMPLATE = lib
TARGET = msghistory
DEPENDPATH += . inc src
INCLUDEPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += MSGHISTORY_DLL

# UID3
TARGET.UID3 = 0x20026F9E

# Capability
TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.EPOCALLOWDLLDATA = 1

CONFIG += hb

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/msghistory.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(msghistory.iby)"

# Input
HEADERS += ../../../msg_plat/messaging_history_api/inc/msghistory.h \
				inc/msghistoryimpl.h \
				inc/msghistoryprivate.h


# Input
SOURCES += src/msghistory.cpp \
				 src/msghistoryimpl.cpp \
				 src/msghistoryprivate.cpp \
				 src/msgitem.cpp

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/msghistory.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/msghistory.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

LIBS += -lcsserverclientapi \
        -lcsutils \
        -lxqutils


symbian:MMP_RULES += SMPSAFE
