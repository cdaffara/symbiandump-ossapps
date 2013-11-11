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
TARGET = convergedmessageutils

INCLUDEPATH += .
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
SYMBIAN_PLATFORMS = WINSCW ARMV5

HEADERS += \
        inc/convergedmessageimpl.h \
        inc/convergedmessageaddressimpl.h \
        ../../../inc/convergedmessage.h \
        ../../../inc/convergedmessageaddress.h \
        ../../../inc/convergedmessageid.h \        
        ../../../inc/convergedmessageattachment.h 				
	

SOURCES += \
		src/convergedmessage.cpp \
        src/convergedmessageimpl.cpp \
        src/convergedmessageaddress.cpp \
        src/convergedmessageaddressimpl.cpp \
        src/convergedmessageid.cpp \
        src/convergedmessageattachment.cpp 
        
DEFINES += BUILD_MSG_UTILS_DLL

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/convergedmessageutils.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(convergedmessageutils.iby)"

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/convergedmessageutils.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/convergedmessageutils.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

symbian {
	TARGET.UID3 =  0x20024319
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	TARGET.EPOCSTACKSIZE = 0x8000
	TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
	TARGET.EPOCALLOWDLLDATA = 1
}



symbian:MMP_RULES += SMPSAFE
