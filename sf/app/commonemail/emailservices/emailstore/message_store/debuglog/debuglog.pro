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
TARGET = debuglog
DEPENDPATH += . inc src
INCLUDEPATH += .

HEADERS += inc/DebugLog.h \
           inc/DebugLogConst.h \
           inc/DebugLogMacros.h \
           inc/GlobalDebugLoggingOptions.h
SOURCES += src/DebugLog.cpp

LIBS += -leuser \
		-lefsrv \
		-lsysutil
		
symbian*: { 
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x2001FDA1
	
	defBlock = \      
	  "$${LITERAL_HASH}if defined(MARM)" \
	  "DEFFILE  eabi/debuglog.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/debuglog.def" \
      "$${LITERAL_HASH}endif"
	
    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE
}	

win32 {
	DESTDIR = ../../../.../bin
}		