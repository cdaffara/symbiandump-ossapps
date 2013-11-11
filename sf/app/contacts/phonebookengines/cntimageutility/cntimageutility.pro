#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).


#
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
TARGET = cntimageutility
MOC_DIR = moc
CONFIG += hb
DEFINES += CNTIMAGEUTILITY_LIBRARY

TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x2002B3F5

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += inc

HEADERS += inc/cntimageutilityglobal.h \
           inc/cntimageutility.h

SOURCES += src/cntimageutility.cpp

LIBS += -lplatformenv -lefsrv
defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" \
		"DEFFILE  ../eabi/cntimageutility.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE  ../bwins/cntimageutility.def" \
	"$${LITERAL_HASH}endif"
MMP_RULES += defBlock
symbian:MMP_RULES += SMPSAFE