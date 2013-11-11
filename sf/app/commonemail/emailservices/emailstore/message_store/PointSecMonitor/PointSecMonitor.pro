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
TARGET = ImsPointSecMonitor
DEPENDPATH += . Inc Src
INCLUDEPATH += . \
				../../inc \
				../../../inc \
				../../../../inc


HEADERS += Inc/ImsPointsecMonitor.h \
		Inc/ImsPointsecObserver.h
		
SOURCES += Src/ImsPointsecMonitor.cpp

LIBS += -leuser \
		-lDebugLog
		
symbian*: { 
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x2001FDA5
    
	defBlock = \      
	  "$${LITERAL_HASH}if defined(MARM)" \
	  "DEFFILE  eabi/imspointsecmonitor.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/imspointsecmonitor.def" \
      "$${LITERAL_HASH}endif"
	
    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE
}	

win32 {
	DESTDIR = ../../../../bin
}		

