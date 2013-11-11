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
TARGET = MessageStoreExe
DEPENDPATH += . src
INCLUDEPATH += . \
			../common/inc \
			../server/inc \
			../../../../inc

SOURCES += src/MessageStoreExe.cpp

LIBS += -leuser \
		-lMessageStoreServer
		
symbian*: { 
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_SERVER
    TARGET.UID2 = 0
    TARGET.UID3 = 0x2001FDA4
    TARGET.EPOCHEAPSIZE = 0x400 \ 
    					0x200000
    MMP_RULES += EXPORTUNFROZEN
    MMP_RULES += SMPSAFE
}	

win32 {
	DESTDIR = ../../../../bin
}		