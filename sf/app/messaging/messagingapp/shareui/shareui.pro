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
# Description: Project file for file share services.
#

TEMPLATE = lib
TARGET = shareui
DEFINES += BUILD_SHAREUI_DLL
DEFINES += __SHAREUI_MIME_HANDLING__

DEPENDPATH += inc src
INCLUDEPATH += inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
   
QT += core
CONFIG += hb

TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.UID3 = 0x2002DD00
TARGET.EPOCHEAPSIZE = 0x20000 0x25000

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "sis/shareui_stub.sis   /epoc32/data/z/system/install/shareui_stub.sis" \
     "rom/shareui.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(shareui.iby)" \
      "rom/shareuiresources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(shareuiresources.iby)" \
     
defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/shareui.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/shareui.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

HEADERS += shareuiprivate.h 
    
SOURCES += shareui.cpp \
    	   shareuiprivate.cpp
    	   
TRANSLATIONS = share.ts

LIBS += -lxqservice \
        -lxqserviceutil


symbian:MMP_RULES += SMPSAFE
