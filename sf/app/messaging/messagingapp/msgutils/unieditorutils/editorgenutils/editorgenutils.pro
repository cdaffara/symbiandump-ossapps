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

TEMPLATE = lib
TARGET = editorgenutils
DEPENDPATH += . inc src

INCLUDEPATH += .
INCLUDEPATH += ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_MUIU_UITLS_DLL
#QMAKE_CXXFLAGS.ARMCC -= --no_hide_all

# UID3
TARGET.UID3 = 0x2001FE70

# Capability
TARGET.CAPABILITY = CAP_GENERAL_DLL

TARGET.EPOCALLOWDLLDATA = 1

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/editorgenutils.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(editorgenutils.iby)"

# Input
HEADERS += inc/MuiuOperationWait.h \
    inc/UniEditorGenUtils.h
    
SOURCES += src/MuiuOperationWait.cpp \
    src/UniEditorGenUtils.cpp

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/editorgenutils.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/editorgenutils.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

LIBS += -leuser \
    -lCentralRepository \
    -lconvergedmessageutils \
    -lsssettings \
    -lMsgMedia \
    -lapmime \
    -lxqutils \
    -lavkon \
    -lefsrv 



symbian:MMP_RULES += SMPSAFE
