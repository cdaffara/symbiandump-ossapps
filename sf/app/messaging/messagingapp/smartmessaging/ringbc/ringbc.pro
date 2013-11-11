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
# Description: RingBc dll pro file.
#

TEMPLATE = lib
TARGET = ringbc


DEFINES += RINGBC_DLL

DEPENDPATH += . \
    inc \
    src
    
INCLUDEPATH += .
INCLUDEPATH += ../../../inc


symbian:
{
TARGET.CAPABILITY = ALL -TCB
TARGET.UID3 = 0x2002E698
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
SYMBIAN_PLATFORMS = WINSCW ARMV5
TARGET.EPOCALLOWDLLDATA = 1
}


CONFIG += hb

symbian {

BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/ringbc.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(ringbc.iby)" 

}

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/ringbc.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/ringbc.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

HEADERS += inc/ringbc.h \
		    inc/ringbc_p.h \
		   inc/ringbctoneconverter.h 
					
SOURCES += src/ringbc.cpp \
		   src/ringbc_p.cpp \
		   src/ringbctoneconverter.cpp 

LIBS += -lplatformenv \
        -lxqutils \
        -lefsrv
		     


symbian:MMP_RULES += SMPSAFE
