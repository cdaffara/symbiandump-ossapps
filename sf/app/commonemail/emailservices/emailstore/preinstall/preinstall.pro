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

TEMPLATE = app
TARGET = EmailStorePreInstall
DEPENDPATH += . inc src
INCLUDEPATH += . \
				../../../inc


HEADERS += inc/EmailStorePreInstall.h \
		inc/ProcessTimer.h
		
SOURCES += src/EmailStorePreInstall.cpp \
		src/ProcessTimer.cpp
		
LIBS += -leuser \
		-lefsrv \
		-lws32 \
		-lapparc \
		-lapgrfx \
		-lbafl \
		-lDebugLog
		
symbian*: { 
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID2 = 0
    TARGET.UID3 = 0x20021190
    MMP_RULES += "EPOCPROCESSPRIORITY background"
    MMP_RULES += EXPORTUNFROZEN
    MMP_RULES += SMPSAFE
}	

win32 {
	DESTDIR = ../../../bin
}		
